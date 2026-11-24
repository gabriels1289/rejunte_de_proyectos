//------------------------------------------------------------------------------
// Módulo principal
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "msp430x14x.h"
#include "support.h"
#include "cs8900.h"
#include "enc28j60.h"
#include "tcpip.h"
#include "cashless.h"
#include "flash.h"
#include "mgmt.h"
#include "display.h"
#include "ks0108.h"
#include "card.h"
#include "fram.h"
#include "dacom.h"
#include "serial.h"

unsigned char vgmProto;   // SAS - DACOM - GAMMA - EIBE

const char hexa[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

EventMsgType bufEventos;  // buffer para almacenar los eventos obtenidos de la máquina y preparar el mensaje

unsigned char nSecEventMsg;  // Nro de secuencia del mensaje de eventos

const unsigned char vgmAddr = 0x01;  // dirección de la máquina

// comandos para los Long Polls usados para consultar a la máquina
// 2do campo: tamaño de la respuesta
// 3er campo: offset dentro del buffer de transmisión

const LongPollType longPoll[TOTAL_POLLS] =
{ {0x10,4,2},   // send total cancelled credit meter
  {0x17,4,6},   // send games lost meter
  {0x18,4,10},  // send games since last power up and door closure
  {0x1A,4,14},  // send current credit meters
  {0x1C,32,18}, // send meters
  {0x1F,20,50}, // send Game Machine ID and information
  {0x2A,4,70},  // send true coin in
  {0x2B,4,74},  // send true coin out
  {0x31,4,78},  // send $1.00 bills in meter
  {0x32,4,82},  // send $2.00 bills in meter
  {0x33,4,86},  // send $5.00 bills in meter
  {0x34,4,90},  // send $10.00 bills in meter
  {0x35,4,94},  // send $20.00 bills in meter
  {0x36,4,98},  // send $50.00 bills in meter
  {0x37,4,102}, // send $100.00 bills in meter
  {0x54,20,106} // send SAS version and game machine serial number
};

static unsigned char nPoll;     // Índice para recorrer el arreglo de long polls (de 0 a TOTAL_POLLS-1)

static unsigned char nextPoll;  // próximo tipo de poll a efectuar: GP o LP (o SINCx o CASH)

volatile unsigned int pollingTimer;  // Timer para controlar el pedido de eventos y long polls
unsigned int pollingPeriod;          // Período entre pedido de eventos y long polls
static unsigned int vgmResponseTime; // Tiempo de respuesta a un comando GP o LP
static unsigned int aftResponseTime; // Tiempo de respuesta para los comandos AFT

LongPollMsgType longPollBuffer;      // Buffer para almacenar las respuestas a los long polls
volatile unsigned int waitLongPollTimer;// timer para esperar la respuesta a un long poll (100 ms)

volatile unsigned int waitEventTimer;   // timer para esperar la respuesta a un pedido de eventos (20 ms)
volatile unsigned int waitEventAckTimer;// timer para esperar el ACK a un mensaje de eventos (100 ms)
unsigned int eventMsgRetry;             // flag que indica que el mensaje de eventos es una retransmisión

volatile unsigned int sendMetersTimer; // timer para controlar el período entre envío de contadores
unsigned int sendMetersPeriod;         // período entre envíos de contadores (en decenas de ms) - Default = 1 minuto

unsigned int sendHelloPeriod;        // período entre envio de Hellos
volatile unsigned int sendHelloTimer;// timer para controlar el envío de mensajes de Hello
unsigned int serverAlive;            // flag que indica si el servidor está vivo. Se activa al recibir la
                                     // respuesta al Hello -con fecha y hora-

volatile unsigned int delayTimer;    // Timer para retardos

volatile unsigned int blinkingLedTimer; // Timer para el parpadeo de led de test

volatile unsigned int WDTimer;       // Timer auxiliar para el manejo del watchdog. Se lo
                                     // resetea en el loop principal del programa. El WDT se
                                     // resetea en la ISR del timer. Si WDTimer supera
                                     // los 10 seg, no se resetea más el WDT para que el micro
                                     // se reinicie

FechaHoraType tiempo;                // almacena aaaa/mm/dd hh:mm:ss

static unsigned char rx_byte;        // para almacenar el último byte recibido por la UART0

volatile unsigned int rxByteTimer;   // tiempo desde el último byte recibido

unsigned char rx_buffer[128];        // buffer para almacenar las respuestas recibidas por la UART0
volatile unsigned char rx_buflen;    // cantidad de bytes almacenados en rx_buffer

unsigned char * const tx_buffer=ARP_ICMP_TX_BUF;// puntero al buffer para almacenar el mensaje a enviar
                                     // a la VGM por la UART0, o al display LCD, etc. 
                                     // Obs.: usa el espacio de memoria correspondiente al
                                     // buffer de transmisión de datagramas ARP o ICMP, por lo
                                     // que no debe usarse simultáneamente para ambas funciones

unsigned char tx_buflen;             // cantidad de bytes en tx_buffer, a ser transmitidos

static unsigned long currentCredits; // se actualiza con el LP 1A

static unsigned long montoVendedor;      // totaliza la última carga de un vendedor (en centavos
                                         // o créditos, según la VGM).
                                         // Se usa para cargar o descargar

static unsigned long montoMaxVendedorPesos; // ídem monto máximo en pesos, que el vendedor
                                            // puede cargar manualmente (sólo se usa
                                            // para mostrar en el display)

static unsigned long montoVendedorPesos; // ídem montoVendedor pero en pesos (sólo se usa
                                         // para mostrar en el display)

static unsigned long cargaUnitVendedor;  // Monto unitario correspondiente a cada carga efectuada por un vendedor

static HelloMsgType helloMsg;           // Buffer para el mensaje de Hello a enviar

unsigned char HW_type;                  // Olimex (1) o BB2 (2)

CashMsgType cashMsgBuffer;              // Buffer para el mensaje de cashless a enviar

static unsigned char cashProtocol;      // 'A' : AFT - 'E' : EFT

static unsigned char AFT_transactionID;      // Nro de transacción AFT ('1' a '9')
static unsigned char EFT_transactionNumber;  // Nro de transacción AFT (01 a FF)

static unsigned int AFT_asset_number;     // Asset number (se debe configurar igual en todas las VGM)

volatile unsigned int waitCashReplyTimer; // timer para esperar respuesta a un mensaje de cashless (200 ms)
unsigned int cashoutMsgReply;             // indica si un mensaje de cashout es el original (0) o un reintento (>=1)

static unsigned char cash_flag;         // En 1 indica que se debe realizar una carga o descarga de crédito normal
                                        // En 2 indica que la operación AFT se debe hacer sin AFT Lock
                                        // En 3 indica que se debe hacer un reset de pago manual

static unsigned char ticket_flag;       // En 1 indica procesar ticket ingresado
                                        // En 2 indica procesar ticket emitido

static unsigned char cash_op;           // 0: CASHOUT_REQ    1: CASHIN_REQ
static unsigned long cash_monto;
static unsigned char cashin_ok;         // flag que indica que el último cashin fué exitoso
static unsigned char cash_handpay;      // flag que indica resetear pago manual

// Variables para el manejo de la tarjeta chip
volatile unsigned int cardReaderTimer;    // timer para controlar las operaciones del card reader
static unsigned int cardReaderLockTime;   // tiempo de bloqueo del lector de tarjetas luego de retirarla
static unsigned char cardID[32];          // ID grabado en la tarjeta
volatile unsigned int waitClientCardTimer;// timer para la espera de que se inserte una tarjeta de cliente

// Buffer para enviar mensajes a la PC-touch screen
TouchMsgType touchMsgBuffer;

// Unión unnamed para almacenar los distintos flags usados por
// el programa (Obs.: algunos se usan solamente en otros módulos).
volatile union {
  unsigned int flags;
  struct {
    unsigned int waitingEventAck    :1;  // 0: indica que está esperando el ACK a un mensaje de eventos
    unsigned int waitingCashinReply :1;  // 1: indica que espera respuesta a un mensaje de cashin
    unsigned int waitingCashoutReply:1;  // 2: indica que espera respuesta a un mensaje de cashout
    unsigned int waitingCardReaderTimer:1; // 3: indica que se está esperando el vencimiento del timer
    unsigned int waitingClientCard  :1;  // 4: indica que se está esperando que se ingrese una tarjeta de cliente
    unsigned int cardReaderLocked   :1;  // 5: indica bloqueo del lector de tarjetas
    unsigned int cardStatus         :1;  // 6: Último estado de la tarjeta -> 0: ausente - 1: presente
    unsigned int forceCashout       :1;  // 7: indica que se debe forzar un cashout
    unsigned int dacomHandpayPending:1;  // 8: En "1" indica pago manual pendiente (evita leer el lector de tarjetas)
    unsigned int testMode           :1;  // 9: indica que se está en modo test
    unsigned int rx_flag            :1;  // 10: señaliza que se recibió un byte por la UART0
    unsigned int waitingTouchReply  :1;  // 11: indica que se está esperando una respuesta de la PC-touch screen
    unsigned int debug              :1;  // 12: En "1" indica que está habilitado el envío de logs para debug al puerto de test
    unsigned int doorOpen           :1;  // 13: En "1" indica puerta abierta
  };
};

// Prototipos de funciones static
static void get_HW_type(void);              // Verifica sobre que HW está corriendo (Olimex o BB2)
static void InitOsc(void);                  // Inicializa el oscilador y temporizador
static void InitPorts(void);                // Inicializa los puertos
static void initVars(void);                 // Inicializa las variables globales
static inline void blinkLed(unsigned int, unsigned int);   // Hace parpadear el led de test
static void mainLoop(void);                 // Lazo principal del programa
static void sasProcessing(void);            // Efectúa la comunicación con la VGM usando protocolo SAS
static void checkCardReader(void);          // Verifica si se insertó o retiró una tarjeta del lector
static void checkEventMsgRetry(void);       // Verifica si debe retransmitir un mensaje de eventos
static unsigned char getEvento(unsigned char *);   // Le consulta por un evento a la máquina
static void clearEventBuffer(void);         // Resetea el buffer de eventos
static unsigned char polling(void);         // Efectúa un long poll y almacena el resultado en el buffer de transmisión
static void clearLongPollBuffer(void);      // Resetea el buffer de resultados de long polls
static unsigned char enableDisableLP(unsigned char cmd); // Envía un comando de enable o disable de alguna función de la VGM
static unsigned char disableRealTimeEventReport(void);   // Deshabilita la generación de eventos en tiempo real
static void InitUART(void);
static __interrupt void UART0_rx_interrupt(void);
static unsigned char ult_dia_del_mes(void);            // Retorna la cantidad de días del mes

static unsigned int AFT_lock_status_request(unsigned char cmd);  // bloqueo y consulta de estado AFT
static void AFT_interrogate(void);               // envía un AFT interrogate con 0xFF a la VGM
static unsigned char AFT_getTransactionID(unsigned int interrogate); // interroga y obtiene el transactionID
static unsigned int valid_AFT_response(unsigned char cmd);  // verifica la integridad y consistencia de una respuesta AFT
static unsigned char AFT_cashin(unsigned long monto);       // cargar 'monto' en la VGM
static unsigned char AFT_cashout(void);         // descargar todo el crédito de la VGM y transferirlo al servidor

                                                // prepara el mensaje AFT 0x72
static void AFT_prepare_init(unsigned char transferCode, unsigned char transferType, unsigned char flags, unsigned long monto);
static unsigned char EFT_cashin(unsigned long monto);  // cargar 'monto' en la VGM
static unsigned char EFT_cashout(void);                // descargar todo el crédito de la VGM y transferirlo al servidor
static unsigned char getCurrentCredits(unsigned long *); // Obtiene los créditos de la VGM (LP 0x1A)

static void cashin(void);                  // Inicia una carga cashless
static void cashout(void);                 // Realiza una descarga cashless y transfiere el monto al servidor
static unsigned char reset_handpay(void);  // Resetea un pago manual pendiente y transfiere el monto al servidor
static void ticket_in(void);               // TITO - Procesamiento de un ticket ingresado
static void ticket_out(void);              // TITO - Procesamiento de un ticket emitido
#define ticketID cardID                    // Alias para usar el arreglo cardID, para almacenar el código de validación de un ticket
static unsigned char TITO_cashin(unsigned long monto);  // cargar 'monto' de un ticket a la VGM
static unsigned char TITO_cashin_reject(unsigned char errorCode); // rechazo a una carga con ticket
static void set_validation_id(void);       // setea validation ID en la VGM

// Macros
#define EnableBillAcceptor()  enableDisableLP(0x06)
#define DisableBillAcceptor() enableDisableLP(0x07)

//----------------------------------------------------------
// Textos de mensajes que se utilizan más de una vez,
// para no tener varias copias ocupando memoria, se
// los almacena en variables const
// Las constantes numéricas representan los códigos
// usados para enviar cada mensaje a la PC-touch screen
//----------------------------------------------------------

const TOUMSG MSG_HW_OLIMEX               = {0, "HW: OLIMEX", ""};
const TOUMSG MSG_HW_OLIMEX_TOUCH         = {0, "HW: OLIMEX", "TOUCH SCREEN"};
const TOUMSG MSG_HW_BB2                  = {0, "HW: BB2", ""};
const TOUMSG MSG_HW_BB2_TOUCH            = {0, "HW: BB2", "TOUCH SCREEN"};
const TOUMSG MSG_HW_DESCONOCIDO          = {0, "HW: DESCONOCIDO", "MODULO DETENIDO"};
const TOUMSG MSG_VGM_EIBE                = {0, "VGM EIBE"};
const TOUMSG MSG_INICIANDO_SAS           = {0, "Iniciando SAS",""};
const TOUMSG MSG_INICIANDO_SAS_1         = {0, "Iniciando SAS","(1)"};
const TOUMSG MSG_INICIANDO_SAS_2         = {0, "Iniciando SAS","(2)"};
const TOUMSG MSG_INICIANDO_SAS_3         = {0, "Iniciando SAS","(3)"};
const TOUMSG MSG_SAS_OK                  = {0, "SAS OK", ""};
const TOUMSG MSG_INICIANDO_DACOM         = {0, "Iniciando DACOM", ""};
const TOUMSG MSG_PROTOCOLO_NO_SOPORTADO  = {0, "PROTOCOLO", "NO SOPORTADO"};
const TOUMSG MSG_VERSION                 = {0, "VERSION",""};
const TOUMSG MSG_ENTRANDO_MODO_TEST      = {0, "  ENTRANDO AL","   MODO TEST"};
const TOUMSG MSG_SALIENDO_MODO_TEST      = {0, "  SALIENDO DEL","   MODO TEST"};

const TOUMSG MSG_DESCARGANDO_J           = {11, "DESCARGANDO..(J)","ESPERE POR FAVOR"};
const TOUMSG MSG_DESCARGANDO_V           = {12, "DESCARGANDO..(V)","ESPERE POR FAVOR"};
const TOUMSG MSG_DESCARGANDO_M           = {13, "DESCARGANDO..(M)","ESPERE POR FAVOR"};
const TOUMSG MSG_MAQUINA_LIBRE           = {14, " MAQUINA LIBRE", "INGRESE TARJETA"};
const TOUMSG MSG_INGRESE_TARJETA_JUGADOR = {15, "INGRESE TARJETA", "DE JUGADOR....."};
const TOUMSG MSG_LEYENDO_TARJETA         = {16, "ESPERE", "LEYENDO TARJETA"};
const TOUMSG MSG_RETIRE_TARJETA          = {17, "RETIRE TARJETA", ""};
const TOUMSG MSG_NO_TIENE_CREDITO        = {18, "NO TIENE CREDITO", "RECARGUE TARJETA"};
const TOUMSG MSG_MAQUINA_EN_JUEGO        = {19, "MAQUINA EN JUEGO", "  BUENA SUERTE"};
const TOUMSG MSG_ACREDITANDO_PAGO_MANUAL = {20, "ACREDITANDO", "PAGO MANUAL"};
const TOUMSG MSG_CARGA_INVALIDA          = {21, "CARGA INVALIDA", "INTENTE OTRA VEZ"};
const TOUMSG MSG_TARJETA_INVALIDA        = {22, "TARJETA INVALIDA", "INTENTE OTRA VEZ"};
const TOUMSG MSG_ESPERE_PROCESANDO       = {23, "ESPERE", "PROCESANDO......"};
const TOUMSG MSG_ERROR_RETIRE_TARJETA    = {24, "ERROR","RETIRE TARJETA"};
const TOUMSG MSG_ERROR_INTENTE_OTRA_VEZ  = {25, "ERROR", "INTENTE OTRA VEZ"};
const TOUMSG MSG_ERROR_INDETERMINADO     = {26, "ERROR", "INDETERMINADO"};
const TOUMSG MSG_ERROR_TARJETA_INVALIDA  = {27, "ERROR", "TARJETA INVALIDA"};
const TOUMSG MSG_ERROR_EFT_AFT           = {28, "ERROR EFT / AFT", "INTENTE OTRA VEZ"};
const TOUMSG MSG_ERROR_EN_CARGA          = {29, "ERROR EN CARGA","DE CREDITOS"};
const TOUMSG MSG_ERROR_DE_COMUNIC        = {30, "ERROR DE COMUNIC", "INTENTE OTRA VEZ"};
const TOUMSG MSG_PUERTA_ABIERTA          = {31, "NO FUNCIONA", "PUERTA ABIERTA"};
const TOUMSG MSG_FUERA_DE_SERVICIO       = {32, "FUERA DE", "SERVICIO"};
const TOUMSG MSG_MONTO_VENDEDOR          = {33, "", "RETIRE TARJETA"};
const TOUMSG MSG_CASHOUT_REP_REJ         = {34, "CASHOUT_REP_REJ", ""};
const TOUMSG MSG_NO_PERMITIDO            = {35, "NO PERMITIDO","SIGA JUGANDO"};
const TOUMSG MSG_INGRESAR_PIN            = {36, "INGRESE EL PIN",""};
const TOUMSG MSG_PIN_INVALIDO            = {37, "PIN INVALIDO", "RETIRE TARJETA"};
const TOUMSG MSG_REINGRESAR_PIN          = {38, "INGRESE OTRA VEZ", "EL PIN"};
const TOUMSG MSG_TARJETA_BLOQUEADA       = {39, "TARJ. BLOQUEADA", "RETIRE TARJETA"};
const TOUMSG MSG_VEND_SIN_FONDOS         = {40, "NO TIENE FONDOS", "RETIRE TARJETA"};
const TOUMSG MSG_INGRESAR_MONTO          = {41, "INGRESE MONTO","A CARGAR"};
const TOUMSG MSG_ASOCIADA_A_OTRO_SLOT    = {42, "ERROR: TARJETA", "ASOC A OTRO SLOT"};
const TOUMSG MSG_NO_HAY_TARJETA_ASOCIADA = {43, "ERROR: NO HAY", "TARJETA ASOCIADA"};
const TOUMSG MSG_MONTO_INVALIDO          = {44, "ERROR: MONTO", "INVALIDO"};
const TOUMSG MSG_MONTO_EXCESIVO          = {45, "ERROR: MONTO", "EXCESIVO"};
const TOUMSG MSG_ERROR_TICKET_RECHAZADO  = {46, "ERROR: TICKET", "RECHAZADO"};

TOUMSG * const toumsg = (TOUMSG *)ARP_ICMP_TX_BUF; // puntero al buffer usado para generar
                                                   // mensajes variables para el display LCD.
                                  // Obs.: usa el mismo espacio de memoria que Tx2FrameMem
                                  // y tx_buffer, por lo que debe ser usado concurrentemente
                                  // con esas otras funciones.

//----------------------------------------------------------
// Inicio del programa
//----------------------------------------------------------
void main(void)
{
  int i;
  
  InitOsc();

  get_HW_type();
  
  InitPorts();

  // Inicializa el controlador Ethernet, de acuerdo al tipo de placa
  InitEthernet();
  
  // Determina el protocolo a usar para la comunicación con la VGM
  // Lo hace acá porque InitUART() configura la comunicación serie
  // acorde a cada protocolo
  vgmProto = (P6IN & SELE_PROTO) >> 1;
 
  InitFlashCtl();
  InitIP();
  InitUART();

  __enable_interrupt();

  initVars();
  clearEventBuffer();       // Inicializa el buffer del mensaje de eventos
  clearLongPollBuffer();    // Inicializa el buffer del mensaje de long polls
  
  LCD_init();
  
  // Muestra sobre que tipo de placa está corriendo
  switch( HW_type )
  {
  case HW_OLIMEX:
    if( Features.teclado == TECLADO_IP )
      LCD_print(&MSG_HW_OLIMEX_TOUCH);
    else
      LCD_print(&MSG_HW_OLIMEX);
    break;
  case HW_BB2:
    if( Features.teclado == TECLADO_IP )
      LCD_print(&MSG_HW_BB2_TOUCH);
    else
      LCD_print(&MSG_HW_BB2);
    break;
  default:
    LCD_print(&MSG_HW_DESCONOCIDO);
    while(1);
  }

  delay(_2seg);
  
  if( TEST_PRESSED() )
  {
    // Si reinicia con el pulsador de test apretado (P6.0 en "0"),
    // enciende el led de test y entra al loop de test
    testMode = 1;
    TESTLED_ON();

    // Cuando se inicia en modo test, usa la IP predeterminada
    MyIP[0] = MYIP_1 + (unsigned int)(MYIP_2 << 8);
    MyIP[1] = MYIP_3 + (unsigned int)(MYIP_4 << 8);

    // Inicia el watchdog timer
    WDTimer = 0;
    startWatchdog();

    testLoop();

    stopWatchdog();

    // Si tiene una IP configurada en la flash de datos, la vuelve a asignar
    if( MyCfgIP[0] != 0xFFFF || MyCfgIP[1] != 0xFFFF )
    {
      // Toma la IP propia de la flash de datos
      MyIP[0] = MyCfgIP[0];
      MyIP[1] = MyCfgIP[1];
    }
  }

  // Continua la inicialización de manera diferenciada según el protocolo
  switch( vgmProto )
  {
  case EIBE:
    LCD_print(&MSG_VGM_EIBE);
  case SAS:
    LCD_print(&MSG_INICIANDO_SAS);
    delay(_1seg);

    // Bucle para verificar que la máquina funciona y hay comunicación con ella
    while(1)
    {
      if( vgmProto == SAS )
      {
        // Espera recibir 5 bytes con la dirección de la VGM,
        // para asegurarse de que esté activa
        LCD_print(&MSG_INICIANDO_SAS_1);
        for(i=0; i<5; )
          if( UART0_rx(1) && rx_buffer[0]==vgmAddr )
            ++i;
      }
      
      // Para sincronizar con la VGM, hace un GP ficticio a otra dirección
      LCD_print(&MSG_INICIANDO_SAS_2);
      UART0_tx(0x80,1);
      delay(pollingPeriod);
      UART0_tx(0x80 + vgmAddr + 1, 1);
      UART0_rx(32);
      delay(pollingPeriod);
      
      if( vgmProto == SAS )
      {
        // Verifica que -durante un segundo- la VGM no transmita nada
        LCD_print(&MSG_INICIANDO_SAS_3);
        delayTimer = 0;
        while( delayTimer < _1seg && !UART0_rx(1) );
        if( delayTimer >= _1seg )
        {
          LCD_print(&MSG_SAS_OK);
          break;   // OK --> sale del loop
        }
      }else
        break; // En el caso de EIBE, sale a la primera iteración
      
      DoNetworkStuff();
    }

    // Pide un evento para verificar si está en modo
    // real time event report, y en tal caso deshabilitarlo
    if( getEvento(&bufEventos.evento[0]) > 1 )
    {
      if( bufEventos.evento[0] )
        // Si había un evento, lo almacena para su posterior transmisión
        bufEventos.nEventos = 1;
        
      UART0_tx(0x80,1);
      delay(pollingPeriod);

      // Deshabilita la generación de eventos en tiempo real
      for(i=0; i<3; ++i)
      {
        if( disableRealTimeEventReport() )
          break;
        delay(pollingPeriod);
      }
      UART0_tx(0x80,1);
      delay(pollingPeriod);
    }

    // Al inicio verifica si debe habilitar o deshabilitar el billetero
    if( Features.tito == 0x01 )
      EnableBillAcceptor();
    else
      DisableBillAcceptor();
    UART0_tx(0x80,1);
    delay(pollingPeriod);
    
    // Si opera con SAS y se inicia con una tarjeta insertada,
    // averigua que protocolo cashless utiliza, para poder
    // hacer el cashout
    if( vgmProto == EIBE )
      cashProtocol = 'A';   // Las EIBE son siempre AFT
    else if( AFT_getTransactionID(1) )
      cashProtocol = 'A';   // Respondió a una interrogación AFT
    else
      cashProtocol = 'E';   // No respondió a una interrogación AFT --> asume que usa EFT
    delay(pollingPeriod);
    UART0_tx(0x80,1);
    delay(pollingPeriod);
    break;
    
  case DACOM:
    LCD_print(&MSG_INICIANDO_DACOM);
    initDacom();
    delay(_2seg);
    LCD_clear();
    break;
    
  default:    
    LCD_print(&MSG_PROTOCOLO_NO_SOPORTADO);
    // Inicia el watchdog timer
    WDTimer = 0;
    startWatchdog();

    while(1)
    {
      // Reinicia el timer auxiliar del watchdog
      WDTimer = 0;
      // hace parpadear el led de test al doble de
      // frecuencia que lo normal
      blinkLed(_120ms, _250ms );

      DoNetworkStuff();
    }
  }

  strcpy(toumsg->txt[0], "VERSION");
  strcpy(toumsg->txt[1], version);
  toumsg->cod = 0;
  LCD_print(toumsg);
  delay(_2seg);
  
  // Lee en la EEPROM para ver si hay algún mensaje de cashout pendiente
  framRead( 0, (unsigned char *)&cashMsgBuffer, sizeof(cashMsgBuffer) );
  if( cashMsgBuffer.msgCode == CASHOUT_REQ || cashMsgBuffer.msgCode == CASHOUT_HANDPAY_REQ )
  {
    // El micro se apagó o reinició cuando tenía una descarga pendiente,
    // entonces inicia reintentando concretar dicha descarga guardada en la EEPROM
    waitingCashoutReply = 1;
    waitCashReplyTimer = _5seg;
    cashoutMsgReply = 4;

    LCD_print(&MSG_DESCARGANDO_M);
  }
  
  // Obtiene los créditos iniciales, porque si se inicia
  // con crédito y sin tarjeta, descarga la VGM
  while( !getCurrentCredits(&currentCredits) )
  {
    DoNetworkStuff();
  }
  cash_monto = currentCredits;
  
  if( cash_monto )
    // Al iniciarse el módulo la VGM tiene crédito --> fuerza una descarga
    forceCashout = 1;
  
  if( cardStatus )
  {
    // El módulo se inició con una tarjeta en el lector
    LCD_print(&MSG_RETIRE_TARJETA);
    // Detiene la ejecución hasta que se retire la tarjeta
    while( cardStatus = getCardStatus() );
  }

  LCD_print(&MSG_MAQUINA_LIBRE);

  // Va al lazo principal, desde el que se controla toda la operación
  mainLoop();
}

//-----------------------------------------------------
// Determina sobre que tipo de hardware está corriendo.
// Obs.1: BB2 tiene un loop entre P3.6 y P3.7
// Obs.2: Invocar antes de InitPorts
//
// Asigna un valor a HW_type: 1 --> Olimex
//                            2 --> BB2 (ENC28J60)
//-----------------------------------------------------
static void get_HW_type()
{
  unsigned char i, n, p3dir;

  p3dir = P3DIR;
  
  P3SEL = 0;
  P3DIR = 0x80;   // P3.7 : output
    
  for(i=0, n=0; i<10; ++i)
  {
    P3OUT = 0x80;      // Escribe "1" en P3.7
    if(P3IN & 0x40)    // ¿Hay un "1" en P3.6?
      n++;
    P3OUT &= 0x7F;     // Escribe "0" en P3.7
    if(!(P3IN & 0x40)) // ¿Hay un "0" en P3.6?
      n++;
  }

  P3DIR = p3dir;

  if(n==20)
    HW_type = HW_BB2;
  else
    HW_type = HW_OLIMEX;
}

//-------------------------------------
// inicializa todas las variables
//-------------------------------------
static void initVars()
{
  nPoll=0;
  
  // Flags
  flags = 0; // pone a cero todos los flags contenidos en la union unnamed
  
  nSecEventMsg = 0;
  eventMsgRetry = 0;
  cashoutMsgReply = 0;
  serverAlive = 0;
  cash_flag = 0;
  ticket_flag = 0;
  cashin_ok = 0;
  cash_handpay = 0;

  waitingCodeSeg = 0xFFFF;

  // resetea fecha y hora
  tiempo.anio=0;
  tiempo.mes=0;
  tiempo.dia=0;
  tiempo.hora=0;
  tiempo.min=0;
  tiempo.seg=0;
  tiempo.ticks=0;
  
  // Timers
  delayTimer = 0;
  waitEventAckTimer = 0;
  sendMetersTimer = 0;
  ARPTimer = 0;
  sendHelloTimer = 0;
  waitCashReplyTimer = 0;
  pollingTimer = 0;
  cardReaderTimer = 0;
  waitCodeSegTimer = 0;
  waitClientCardTimer = 0;
  
  // tiempos
  sendMetersPeriod = _30seg;
  sendHelloPeriod = _10seg;
  pollingPeriod = _200ms;
  vgmResponseTime = _100ms; //(vgmProto==EIBE)?_50ms:_30ms;  ---OJO---
  aftResponseTime = (vgmProto==EIBE)?_150ms:_100ms;
  cardReaderLockTime = (vgmProto==EIBE)?_10seg:_5seg;
    
  // variables varias
  rx_buflen = 0;
  tx_buflen = 0;
  
  cash_op = 0xFF;
  cashProtocol = 0;
  cash_monto = currentCredits = 0;
  montoVendedor = 0;
  montoVendedorPesos = 0;
  cargaUnitVendedor = 1;
  
  AFT_transactionID = '1';
  EFT_transactionNumber = 1;

  AFT_asset_number = ((vgmProto==EIBE)?10:1234);
  
  nextPoll = POLL_SINC1;

  // Como el mensaje de Hello no se modifica, lo inicializa aquí
  strcpy(helloMsg.hello, "HELLO");
  strcpy(helloMsg.version, version);
  helloMsg.hash = RANDOM();
  helloMsg.crc = CRC((unsigned char *)&helloMsg,sizeof(helloMsg)-2,0);
  cifrar((unsigned char *)&helloMsg,sizeof(helloMsg));
  
  // Determina el estado inicial del lector de tarjetas
  cardStatus = getCardStatus();
}

//-------------------------------------------------------------------------
// Genera un número seudo aleatorio en base al valor de dos temporizadores
//-------------------------------------------------------------------------
unsigned int RANDOM()
{
  unsigned int n = ARPTimer;
  n = SWAPB(n);
  n += delayTimer;
  return n;
}

//------------------------------------------------------------------------------
// enables the 8MHz crystal on XT1 and use it as MCLK
//------------------------------------------------------------------------------
static void InitOsc(void)
{
  WDTCTL = WDTPW + WDTHOLD;                      // stop watchdog timer

  BCSCTL1 |= XTS;                                // XT1 as high-frequency
  __bic_SR_register(OSCOFF);                     // turn on XT1 oscillator

  do
  {
    IFG1 &= ~OFIFG;                              // Clear OFIFG
    DelayCycles(100);                            // Wait ~130us
  } while (IFG1 & OFIFG);                        // Test oscillator fault flag
  
  BCSCTL2 = SELM_3;                              // set XT1 as MCLK

  //----------------------------------------------------------------------
  // Inicializa timers, resetea flags, inicia timer-ISR
  // Obs.1: ACLK es usado también para fijar el baud rate de la uart 0
  //----------------------------------------------------------------------

  BCSCTL1 &= ~DIVA0;                             // ACLK = XT1 / 4 = 2 MHz
  BCSCTL1 |= DIVA1;

  TACCR0 = 2500;                                 // valor de recarga para el timer 0 --> 10 ms (a 250 kHz)
  
  TACTL = ID_3 + TASSEL_1 + MC_1 + TAIE;         // stop timer, use ACLK / 8 = 250 kHz, gen. int.
                                                 // start timer in up-mode
}  

//----------------------------------------------------------------------
// ISR para el WDT
//----------------------------------------------------------------------

#pragma vector = WDT_VECTOR
static __interrupt void WDT_interrupt(void)
{
}

//------------------------------------------------------------------------------
// Inicializa los puertos
// PxSEL = 0 --> I/O    1 --> función específica
// PxDIR = 0 --> input  1 --> output
//------------------------------------------------------------------------------
static void InitPorts(void)
{                           // Configura como salida a todos los puertos no usados

                            // Los puertos 3 y 5 se configuran en "cs8900.c"
                            // También, en "InitUART()" se configuran P3.4 y P3.5 para Tx y Rx
  
  P1SEL = 0;                // Port 1 I/O
  P1IE  = 0;                // Interrupt disable
  P1OUT = 0x40;             // P1.6 (buzzer) en 1 para desactivarlo
  P1DIR = 0xc0;             // P1.0 a P1.5 configurados como entradas
  //--------------------
  P2SEL = 0;                // Port 2 I/O
  P2OUT = 0;
  P2DIR = 0xff;
  //--------------------
  P3SEL = 0;                // Port 3 I/O (En InitUART se reconfiguran los pines Tx y Rx)
  //--------------------
  P4SEL = 0;                // Port 4 I/O
  P4OUT = 0xc0;             // P4.6 y P4.7 serial data y clock para la memoria externa (por defecto en alto)
                            // P4.0 por defecto en cero. En los módulos básicos es la señal de
                            // write para el display LCD. En los módulos con teclado serie, P4.0
                            // se usa para encender la iluminación del teclado, poniéndolo a uno.
  P4DIR = 0xff;             // P4.0 a P4.7 son salidas
  //--------------------
  P5SEL = 0;                // Port 5 I/O (En la versión con CS8900A se usa para D0-D7)
                            // En la versión con ENC28J60 se reconfigura para usar SPI
  //--------------------
  P6SEL = 0;                // usa P6.1 y P6.2 para seleccionar SAS / DACOM / GAMMA
  P6DIR = 0xf8;             // P6.0 entrada pulsador test - P6.1 y P6.2 jumpers p/selectar protocolo
                            // el resto son salidas (P6.3 led azul de test)
  P6OUT = 0x80;             // P6.7 como salida a "1". Previsto para la impresora serie.
}

//--------------------------------------------------------
// Lazo de retardo. El tiempo se indica en decenas de ms
//--------------------------------------------------------
void delay( const unsigned int n_x_10ms )
{
  delayTimer = 0;
  while( delayTimer < n_x_10ms );
}

//--------------------------------------------------------
// Hace parpadear el led de test
//--------------------------------------------------------
static inline void blinkLed(unsigned int t1, unsigned int t2)
{
  if( !testMode )
  {
    // En modo normal, hace parpadear el led de test
    if( blinkingLedTimer >= t1 && blinkingLedTimer < t2 )
    {
      TESTLED_ON();
    }else if( blinkingLedTimer >= t2 ) {
      TESTLED_OFF();
      blinkingLedTimer = 0;
    }
  }
}

//--------------------------------------------------------
// Lazo principal del programa
//--------------------------------------------------------
static void mainLoop(void)
{
  unsigned char beepFlag = 0;
  unsigned int i;
  
  // Inicia el watchdog timer
  WDTimer = 0;
  startWatchdog();

  while(1)
  {
    WDTimer = 0;      // reinicia timer auxiliar del watchdog

    if( testMode )
      testLoop();
      
    // Prueba del pulsador y led de test
    if( TEST_PRESSED() )
    {
      TESTLED_ON();
      delay( _500ms );
      if( TEST_PRESSED() )
      {
        testMode = 1;
        testLoop();
      }else
        TESTLED_OFF();
    }
    
    blinkLed(_250ms, _500ms);
    
    // Si detecta que se ha ingresado una tarjeta,
    // hace un beep y pone un mensaje, para que el usuario
    // note que está siendo atendido
    if( !cardStatus && getCardStatus() )
    {
      if( !beepFlag )
      {
        beepFlag = 1;
        // Si montoVendedor != 0 no modifica el display,
        // para dejar el mensaje con el monto acumulado
        if( !montoVendedor )
          LCD_print(&MSG_LEYENDO_TARJETA);

        BEEP(_100ms);
      }
    }else if( beepFlag ) {
      beepFlag = 0;
      if( !cardStatus && !getCardStatus() )
      {
        // Retiró la tarjeta antes de que procesara la misma
        // para hacer un cashin, entonces pone el cartel de
        // máquina libre (ojo -> habría que analizar si la
        // puerta está abierta o cerrada)
        if( waitingClientCard )
        {
          LCD_print(&MSG_INGRESE_TARJETA_JUGADOR);
        }else{
          LCD_print(&MSG_MAQUINA_LIBRE);
        }
      }
    }
    // Realiza la comunicación con la VGM, usando el protocolo correspondiente
    switch(vgmProto)
    {
    case SAS:
    case EIBE:
      sasProcessing();
      break;
    case DACOM:
      if( !waitingCashinReply && !waitingCashoutReply && !dacomHandpayPending )
        // Verifica si se insertó o se retiró una tarjeta
        checkCardReader();

      if( cash_flag )
      {
        if( cash_op == CASHIN_REQ )
          cashin();
        else if( cash_op == CASHOUT_REQ )
          cashout();
        else
          cash_flag = 0;
      }else{
        dacomProcessing();
      }
      break;
    }
    
    // Verifica el timeout de 5s para el ACK al mensaje de eventos
    if( waitingEventAck && waitEventAckTimer > _5seg )
    {
      // El ACK no llegó en 5s --> intenta retransmitir los eventos
      checkEventMsgRetry();
    }
    
    // Verifica el timeout de 5s para una respuesta de cashin
    if( waitingCashinReply && waitCashReplyTimer > _5seg )
    {
      // Una respuesta a un pedido de crédito no llegó en 5s -> pone un mensaje y sale
      LCD_print(&MSG_ERROR_DE_COMUNIC);
      
      waitingCashinReply = 0;

      // Envía un mensaje DE NAK al servidor (si el servidor había respondido, pero el
      // mensaje se perdió, se le indica al servidor que vuelva para atrás la operación
      // y devuelva los créditos descontados al cliente o vendedor)
      cashMsgBuffer.msgCode = CASHIN_NAK;
      cashMsgBuffer.errorCode = AFT_TRANSFER_CANCELLED_BY_HOST;
      
      // Por las dudas pone en cero el ID de tarjeta y el monto,
      // ya que no se usan en el mensaje de NAK
      sprintf((char *)cashMsgBuffer.ID.cardID, "%031u", 0);
      sprintf((char *)cashMsgBuffer.monto, "%010u", 0);
      cashMsgBuffer.hash = RANDOM();
      cashMsgBuffer.crc = CRC((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer)-2,0);
      cifrar((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer));
      sendUDP( (unsigned char *) &cashMsgBuffer, sizeof(cashMsgBuffer), SERV_CASH, UDP_LOCAL_PORT_CLI, UDP_PORT_CASH );
    }
    
    // Verifica el timeout de 5s para una respuesta de cashout
    if( waitingCashoutReply && waitCashReplyTimer > _5seg )
    {
      // El ACK a una descarga de créditos cashless no llegó en 5s -> retransmite
      sendUDP( (unsigned char *)&cashMsgBuffer, sizeof(cashMsgBuffer), SERV_CASH, UDP_LOCAL_PORT_CLI, UDP_PORT_CASH );
      waitingCashoutReply = 1;
      waitCashReplyTimer = 0;

      // Incrementa el contador de reintentos. Si supera los 3 guarda el mensaje el EEPROM
      // por si acaso se apaga o reinicia el módulo
      if( cashoutMsgReply < 3 )
        cashoutMsgReply++;
      else if( cashoutMsgReply == 3 )
      {
        framWrite( 0, (unsigned char *)&cashMsgBuffer, sizeof(cashMsgBuffer) );
        cashoutMsgReply = 4;
      }
    }
    
    // Verifica el timeout de 5s para recibir un segmento de código que se ha solicitado
    if( waitingCodeSeg!=0x0FFFF && waitCodeSegTimer > _5seg )
    {
      // No llegó un segmento de código --> Aborta la descarga
      // de una nueva imagen y envía mensaje al servidor
      MgmtMsgType *pt = (MgmtMsgType *) UDP_TX_BUF;

      waitingCodeSeg = 0x0FFFF;
      
      pt->msg1.op = MGMT_ABORT_DWNLD;
      strcpy(pt->msg1.version, "(Tout)");
      strcat(pt->msg1.version, newVersion);
      pt->msg1.version[19] = 0;
      pt->msg1.offset = waitingCodeSeg;
      pt->msg1.crc = CRC( (unsigned char *)pt, sizeof(pt->msg1)-2, MGMT_CRC_SEED );
      
      sendUDP((unsigned char *)pt, sizeof(pt->msg1), SERV_MGMT, UDP_LOCAL_PORT_MGMT, UDP_PORT_MGMT);
    }
    
    // Si es el momento, envía el mensaje UDP con los resultados de los Long Polls
    if( sendMetersTimer >= sendMetersPeriod )
    {
      // Pone el ID del mensaje
      longPollBuffer.msgID[0]='L';
      longPollBuffer.msgID[1]='P';
      // Agrega el código de hash
      longPollBuffer.hash = RANDOM();
      // Calcula el CRC
      longPollBuffer.crc = CRC((unsigned char *)&longPollBuffer,sizeof(longPollBuffer)-2,0);
      cifrar((unsigned char *)&longPollBuffer,sizeof(longPollBuffer));
      
      // Envía el mensaje al servidor
      if( sendUDP( (unsigned char *) &longPollBuffer, sizeof(longPollBuffer), SERV_METERS, UDP_LOCAL_PORT_CLI, UDP_PORT_METERS ) )
      {
        // Si concretó el envío, reinicia el temporizado
        sendMetersTimer = 0;
      }
    }
    
    // Si es el momento, envía el mensaje UDP de Hello
    if( sendHelloTimer >= sendHelloPeriod )
    {
      sendUDP( (unsigned char *) &helloMsg, sizeof(helloMsg), SERV_CASH, UDP_LOCAL_PORT_CLI, UDP_PORT_HELLO );
      sendHelloTimer = 0;
      
      if( serverAlive )
        --serverAlive;
      else if( !cardStatus ) {
        // El servidor no responde, pone un mensaje y
        // reduce el período entre mensajes Hello
        LCD_print(&MSG_FUERA_DE_SERVICIO);
      }
      // Por las dudas, cada vez que envía un Hello reduce el período a 3 seg
      // Cada vez que recibe una respuesta lo restaura a 10 seg. De esa
      // manera, si se pierde la conexión con el servidor se detecta más rápido
      // y si la conexión se mantiene, no se altera nada.
      sendHelloPeriod = _3seg;
    }
    
    // Verifica el timeout de 10s para que se ingrese una tarjeta de cliente
    // después de retirada la de un vendedor
    // Fuerza la descarga de los créditos cargados por el vendedor
    if( !waitingCashinReply && (forceCashout || (waitingClientCard && waitClientCardTimer > ((vgmProto==EIBE)?_15seg:_10seg))) )
    {
      LCD_print(&MSG_DESCARGANDO_V);

      // Si el protocolo es SAS, espera el período de polling
      while( (vgmProto==SAS || vgmProto==EIBE) && pollingTimer < pollingPeriod );
          
      cashin_ok = CASHIN_REP_CLI_OK;
      cash_op = CASHOUT_REQ;
      cash_flag = DO_CASHIN;
      cashout(); 
      waitingClientCard = 0;
      forceCashout = 0;

      if( !getCardStatus() )
      {
        LCD_print(&MSG_MAQUINA_LIBRE);
      }else{
        LCD_print(&MSG_LEYENDO_TARJETA);
      }
    }
    
    for(i=0; i<5; ++i)
    {
      // Procesa la recepción y transmisión por Ethernet / IP
      // Lo hace 5 veces seguidas, para acelerar la descarga
      // de tramas recibidas, y evitar que venzan algunos temporizadores
      DoNetworkStuff();
    }
  }
}

//-----------------------------------------------------------------------------------
// Se comunica con la VGM para pedir eventos, hacer Long Poll o cashless, usando SAS
//-----------------------------------------------------------------------------------
static void sasProcessing()
{
  unsigned char evt, aux;
  
  // Verifica si es el momento de hacer algún tipo de polling a la VGM
  if( pollingTimer > pollingPeriod )
  {
    pollingTimer = 0;
    
    switch( nextPoll )
    {
    case POLL_SINC1:
      // Envia un 0x80 para mantener sincronizada a la VGM
      UART0_tx(0x80,1);

      if( waitingEventAck )
        // está esperando el ACK al mensaje de eventos.
        // Saltea el pedido de eventos y va hacer un long poll
        nextPoll = POLL_LP;
      else
        // La próxima vez hay que hacer un GP
        nextPoll = POLL_GP;
      break;
      
    case POLL_GP:
      // Por defecto, el próximo paso es un sincronismo previo a un LP
      nextPoll = POLL_SINC2;
      
      evt = 0;  // Por defecto no hay eventos
      
      if( bufEventos.nEventos < MAX_EVENTOS )
      {
        // Pide un evento a la máquina
        if( getEvento(&evt) )
        {
          // Si obtuvo un evento que no debe ser filtrado, lo almacena
          switch(evt)
          {
          case 0:
            // No hubo evento
            break;
          case EVT_WAITING_USER_INPUT:
            // Como este evento (cuando la VGM lo genera) se
            // envía permanentemente, para no saturar al módulo
            // lo anula.
            evt = 0;
          case EVT_AFT_REQ_4_HOST_CASHOUT:
          case EVT_GAME_LOCKED:
          case AFT_EVENT_TRANSFER_COMPLETE:
          case EVT_GAME_SELECTED:
          case EVT_GAME_ENDED:
          case EVT_HOPPER_LEVEL_LOW:
            break;
          case EVT_GAME_STARTED:
            // Si se presionó el botón de pago (evt 66) y no hay crédito suficiente
            // la VGM no envía el evt 51 y el módulo queda bloqueado hasta que se
            // agote el crédito. Si luego del evt 66 se inicia una jugada, resetea
            // la condición de espera del evt 51.
            cash_handpay = 0;
            break;
          default:
            // Eventos que se reportan al servidor
            bufEventos.evento[bufEventos.nEventos++] = evt;
          }
          
          // Averigua si es algún evento vinculado a un pago manual,
          // que en algunos casos se usa para alterar algunas condiciones
          switch(evt)
          {
          case EVT_CASHOUT_BUTTON_PRESSED:
            // Fue presionado el boton de pago.
            // En las EIBE no hace nada y en el resto de las máquinas
            // se queda a la espera de un pago manual
            if( vgmProto!=EIBE && currentCredits && !cash_handpay && cash_flag!=DO_HANDPAY_RESET )
            {
              // Impide que se efectue un cashout normal, porque se viene
              // un pago manual que debe ser reseteado
              cash_handpay = 1;
            }
            break;
          case EVT_AFT_REQ_4_HOST_CASHOUT:
            // Fue presionado el boton de pago, y como consecuencia genera este
            // evento. En las EIBE ordena hacer un cashout normal. En el
            // resto de las máquinas no hace nada, porque espera un pago manual
            if(vgmProto==EIBE && !cash_flag && currentCredits && !waitingCashinReply && !waitingCashoutReply)
            {
              cash_op = CASHOUT_REQ;
              cash_flag = DO_CASHOUT_BUTTON_EIBE;
            }else if(vgmProto!=EIBE && !cash_flag && cash_handpay!=2 && currentCredits && !waitingCashinReply && !waitingCashoutReply)
            {
              cash_op = CASHOUT_REQ;
              cash_flag = DO_HANDPAY_RESET;  // prepara para hacer un reset de pago manual...
              cash_handpay = 0;              // ...pero espera el evento EVT_HANDPAY_PENDING
            }
            break;
          case EVT_HANDPAY_PENDING:
            // En el próximo LP manda a resetear el pago manual
            cash_handpay = 2;
            break;
          case EVT_HANDPAY_RESET:
            if( getCardStatus() )
              LCD_print(&MSG_RETIRE_TARJETA);
            else
              LCD_print(&MSG_MAQUINA_LIBRE);
            break;
          case EVT_CASHOUT_TICKET_PRINTED:
            cash_handpay = 0;
            ticket_flag = DO_TICKET_OUT;
            break;
          case EVT_VOUCHER_INSERTED:
            ticket_flag = DO_TICKET_IN;
            break;
          case EVT_VALIDATION_ID_NOT_CONFIGURED:
            ticket_flag = DO_SET_VALIDATION_ID;
            break;
          }
          
          // Algunos eventos (abrir y cerrar puerta) implican
          // poner un mensaje en el display
          switch(evt)
          {
          case EVT_MAIN_DOOR_OPEN:
            doorOpen = 1;
            LCD_print(&MSG_PUERTA_ABIERTA);
            break;
          case EVT_MAIN_DOOR_CLOSED:
            doorOpen = 0;
            if( cardStatus )
              LCD_print(&MSG_MAQUINA_EN_JUEGO);
            else
              LCD_print(&MSG_MAQUINA_LIBRE);
            break;
          case EVT_1_BILL_ACCEPTED:
          case EVT_5_BILL_ACCEPTED:
          case EVT_10_BILL_ACCEPTED:
          case EVT_20_BILL_ACCEPTED:
          case EVT_50_BILL_ACCEPTED:
          case EVT_100_BILL_ACCEPTED:
          case EVT_2_BILL_ACCEPTED:
          case EVT_500_BILL_ACCEPTED:
          case EVT_BILL_ACCEPTED:
          case EVT_200_BILL_ACCEPTED:
            if( cardStatus && !doorOpen )
              LCD_print(&MSG_MAQUINA_EN_JUEGO);
            break;
          }
        }else{
          // No obtuvo respuesta al pedido de eventos (repetido 3 veces)
          // --> manda a reiniciar el ciclo con un 0x80
          nextPoll = POLL_SINC1;
        }
      }
      // Si la VGM no tiene más eventos o bien se llenó el buffer, envia los que tenga acumulados
      if( (!evt && bufEventos.nEventos) || bufEventos.nEventos >= MAX_EVENTOS )
      {
        // Primero estampa fecha y hora
        sprintf(bufEventos.fechaHora,"%04d%02d%02d%02d%02d%02d",tiempo.anio,tiempo.mes,tiempo.dia,tiempo.hora,tiempo.min,tiempo.seg);
        // Agrega código de hash
        bufEventos.hash = RANDOM();
        // Luego calcula el CRC
        bufEventos.crc = CRC((unsigned char *)&bufEventos,sizeof(EventMsgType)-2,0);
        // cifra el mensaje
        cifrar((unsigned char *)&bufEventos,sizeof(EventMsgType));
        // Envía el mensaje al servidor
        if( sendUDP( (unsigned char *) &bufEventos, sizeof(EventMsgType), SERV_EVENTS, UDP_LOCAL_PORT_CLI, UDP_PORT_EVENTS ) )
        {
          // activa la espera del ACK
          waitingEventAck=1;
          waitEventAckTimer=0;
        }
      }else if(evt && bufEventos.nEventos < MAX_EVENTOS) {
        // Si obtuvo un evento, vuelve a repetir el pedido
        nextPoll = POLL_SINC1;
      }
      break;
      
    case POLL_SINC2:
      // Envia un 0x80 para mantener sincronizada a la VGM
      UART0_tx(0x80,1);

      if( !waitingCashinReply && !waitingCashoutReply && !cash_handpay && !cash_flag )
        // Verifica si se insertó o se retiró una tarjeta
        checkCardReader();
    
      if( cash_flag && !cash_handpay )
        // Si detecta que se quitó la tarjeta y no hay un pago manual en camino,
        // entonces el próximo paso es consultar coin in, coin out, etc. y luego cashout()
        // Si se insertó una tarjeta, directamente va a hacer cashless()
        if( cash_op == CASHIN_REQ )
          nextPoll = POLL_CASH;  // va a hacer cashin
        else
          nextPoll = POLL_LP1C;  // antes del cashout consulta coin in, coin out, etc.
      else if(cash_handpay == 2)
        // El próximo paso es reset_handpay() --> antes consulta coin in,... etc
        nextPoll = POLL_LP1C;
      else if(ticket_flag)
        nextPoll = POLL_TICKET;
      else
        // La próxima vez hay que hacer un LP
        nextPoll = POLL_LP;
      break;

    case POLL_LP:
      // Hace un LP
      polling();
      nextPoll = POLL_SINC1;
      break;

    case POLL_LP1C:
      // Antes de un cashout o handpay consulta coin in, coin out,
      // jackpot, games won y games played
      aux = nPoll;
      nPoll = POLL_COIN_IN;
      polling();
      nPoll = aux;
      nextPoll = POLL_SINC3;

    case POLL_SINC3:
      // Envia un 0x80 para mantener sincronizada a la VGM
      UART0_tx(0x80,1);
      // La próxima vez efectúa la operación de cashless requerida
      nextPoll = POLL_CASH;
      break;

    case POLL_CASH:
      if( cash_flag == DO_HANDPAY_RESET )
      {
        // Pago ordenado con el botón de pago (evento 6A de AFT)
        LCD_print(&MSG_ACREDITANDO_PAGO_MANUAL);
        cashout();
        if( getCardStatus() )
          LCD_print(&MSG_RETIRE_TARJETA);
        if(cash_flag)
          // Falló la descarga. Reinicia el ciclo para intentar más tarde
          nextPoll = POLL_SINC1;
        else
          // Descarga exitosa. En el próximo paso va a actualizar currentCredits
          nextPoll = POLL_SINC4;
      }else if( cash_flag && !cash_handpay )
      {
        if( vgmProto==EIBE && cash_flag==DO_CASHOUT_BUTTON_EIBE )
        {
          // Descarga por botón de pago presionado (en EIBE)
          LCD_print(&MSG_ACREDITANDO_PAGO_MANUAL);
          cashout();
          if( getCardStatus() )
            LCD_print(&MSG_RETIRE_TARJETA);
          if(cash_flag)
            // Falló la descarga. Reinicia el ciclo para intentar más tarde
            nextPoll = POLL_SINC1;
        }else{
          // Carga normal por ingreso de tarjeta
          // o descarga normal por retiro de tarjeta
          if( cash_op == CASHIN_REQ )
            cashin();
          else if( cash_op == CASHOUT_REQ )
            cashout();
        }
        // Como una transacción cashless puede demorar más que el
        // período de polling, reinicia el temporizador, para
        // espaciar el próximo paso de la secuencia de polling
        pollingTimer = 0;
        // En el próximo paso va a actualizar currentCredits
        nextPoll = POLL_SINC4;
      }else if( cash_handpay == 2 )
      {
        // Requerimiento de reset de pago manual (evento 51)
        LCD_print(&MSG_ACREDITANDO_PAGO_MANUAL);
        
        if( reset_handpay() )
        {
          if( getCardStatus() )
            LCD_print(&MSG_RETIRE_TARJETA);

          // Como reset_handpay() puede demorar más que el
          // período de polling, reinicia el temporizador, para
          // espaciar el próximo paso de la secuencia de polling
          pollingTimer = 0;
          // En el próximo paso va a actualizar currentCredits
          nextPoll = POLL_SINC4;
        }else{
          // Falló el reset de pago manual. Reinicia el ciclo
          // para intentar de nuevo en la próxima vuelta
          nextPoll = POLL_SINC1;
        }
      }else
        // Reinicia el ciclo (va a hacer un GP)
        nextPoll = POLL_SINC1;
      break;
      
    case POLL_TICKET:
      if( ticket_flag == DO_TICKET_IN )
        ticket_in();
      else if( ticket_flag == DO_TICKET_OUT )
        ticket_out();
      else if( ticket_flag == DO_SET_VALIDATION_ID )
        set_validation_id();
      else
        ticket_flag = 0;        
      nextPoll = POLL_SINC1;
      break;
      
    case POLL_SINC4:
      // Envia un 0x80 para mantener sincronizada a la VGM
      UART0_tx(0x80,1);

      // La próxima vez hace una consulta de créditos
      nextPoll = POLL_LP1A;
      break;

    case POLL_LP1A:
      // Después de un cashin, cashout o handpay
      // consulta los créditos actuales para actualizar currentCredits
      aux = nPoll;
      nPoll = POLL_CREDITS;
      polling();
      nPoll = aux;

      if( waitingCashinReply || waitingCashoutReply )
      {
        // Como puede haber más o menos demora para recibir la respuesta
        // del servidor con los créditos a cargar, no reinicia el ciclo de polling
        // hasta que el cashin o cashout no se haya completado
        nextPoll = POLL_SINC4;
      }else{
        nextPoll = POLL_SINC1;
      }
      break;
    }
  }
}

//----------------------------------------------------------------------------------------------
// Verifica el estado del lector de tarjetas chip.
// Si una tarjeta fue insertada obtiene sus datos, registra sus datos para el pedido de cash.
// Si fue retirada, registra el evento para descargar créditos de la VGM.
// Obs.: esta función lee 32 bytes de la tarjeta, a partir de la dirección 32. Si se quisiera
//       escribir en ese rango de direcciones, valen los siguientes ejemplos:
//              cardWrite("DANIEL GOMEZ - 1234567890 - ABCD", 32, 32);
//              cardWrite("GUILLERMO FRIEDRICH - 1234567890", 32, 32);
//              cardWrite("ALBERTO -EL FLACO- TUBIA - 12345", 32, 32);
//----------------------------------------------------------------------------------------------
static void checkCardReader()
{
  unsigned int i,k;
  
  if( cardReaderLocked && (cardReaderTimer > cardReaderLockTime) && !waitingCashoutReply )
  {
    cardReaderLocked = 0;
    if( !getCardStatus() )
    {
      if( waitingClientCard )
        LCD_print(&MSG_INGRESE_TARJETA_JUGADOR);
      else
        LCD_print(&MSG_MAQUINA_LIBRE);
    }else{
      // Si montoVendedor != 0 no modifica el display,
      // para dejar el mensaje con el monto acumulado
      if( !montoVendedor )
        LCD_print(&MSG_LEYENDO_TARJETA);
    }
  }
  // Verifica el estado del lector de tarjetas chip
  if( !cardReaderLocked && !cash_flag )
  {
    if( !waitingCardReaderTimer )
    {
      // Verifica si cambió el estado del lector de tarjetas
      if(cardStatus != getCardStatus())
      {
        if(cardStatus)
        {
          // Retiró la tarjeta; lo primero es deshabilitar el billetero
          if( Features.tito != 0x01 )
            DisableBillAcceptor();
          
          // Debe manda a descargar, pero antes
          // verifica si no está esperando para completar un cashin
          if( waitingCashinReply )
            return;
          
          cardStatus = 0;
          cash_op = CASHOUT_REQ;
          cash_flag = DO_CASHOUT_STD;
          // Después de retirar la tarjeta, bloquea la lectura de tarjetas por un tiempo,
          // para evitar perturbaciones si el jugador pone y saca la tarjeta repetidamente
          cardReaderLocked = 1;
          cardReaderTimer = 0;
          
          // Por las dudas resetea el flag de espera de una respuesta de la PC-touch screen
          waitingTouchReply = 0;

          if( cashin_ok == CASHIN_REP_VEND_OK )
          {
            LCD_print(&MSG_INGRESE_TARJETA_JUGADOR);
            // Activa la espera de la tarjeta de cliente
            waitClientCardTimer = 0;
            waitingClientCard = 1;
          }else{
            LCD_print(&MSG_DESCARGANDO_J);
          }
        }else if( serverAlive ) {
          // Se insertó una tarjeta. Antes de procesarla verifica
          // que haya comunicación con el servidor.
          cardReaderTimer = 0;
          waitingCardReaderTimer = 1;
        }
      }
    }else{
      if( cardReaderTimer > _500ms )
      {
        waitingCardReaderTimer = 0;
        // Si se mantiene el nuevo estado del lector de tarjetas, lo toma como válido
        if(cardStatus != getCardStatus())
        {
          if( !cardStatus )
          {
            // El nuevo estado es: tarjeta presente --> hacer cashin
            BEEP(_200ms);
            
            // Primero resetea la tarjeta y lee la respuesta
            cardAnswerToReset(cardID);
            for(i=0; i<4; ++i)
              if(cardID[i] != 0xFF)
                break;
            if(i >= 4)
            {
              LCD_print(&MSG_TARJETA_INVALIDA);
              return;
            }
            
            // Borra los datos de la tarjeta anterior
            for(i=0; i<32; ++i)
              cardID[i] = 0xFF;
            
            // Luego lee los datos grabados en la tarjeta
            for(k=0; k<3; ++k)
            {
              if( !cardRead(CMD_READ_MAIN, cardID, 32, 32) )
              {
                // La tarjeta fue quitada --> aborta el proceso
                return;
              }
              memcpy(cashMsgBuffer.ID.cardID, cardID, 32);
              for(i=0; i<32; ++i)
              {
                if( cardID[i] != 0xFF )
                {
                  k=4;
                  break;
                }
              }
              if(k<3)
                delay(_100ms);  // Espera antes de reintentar la lectura
            }
            if(k==3)
            {
              LCD_print(&MSG_TARJETA_INVALIDA);
              return;
            }

            // Acepta la tarjeta ingresada y se prepara para pedir créditos
            cardStatus = !cardStatus;
            
            // Si montoVendedor != 0 muestra en el primer renglón el monto acumulado
            if( montoVendedor )
            {
              // Muestra en el display el total que lleva cargado.
              *toumsg = MSG_ESPERE_PROCESANDO;
              sprintf(toumsg->txt[0], "Total: %-lu", montoVendedorPesos);
              LCD_print(toumsg);
            }else
              LCD_print(&MSG_ESPERE_PROCESANDO);

            cash_op = CASHIN_REQ;
            // Si es un módulo con teclado, pide que se ingrese el PIN
            if( Features.teclado == TECLADO_IP )
            {
              // módulo con teclado por IP
              touchMsgBuffer.msgCode  = TOUCH_GET_PIN;
              touchMsgBuffer.hash = RANDOM();
              touchMsgBuffer.crc = CRC((unsigned char *)&touchMsgBuffer,sizeof(touchMsgBuffer)-2,0);
              sendUDP( (unsigned char *) &touchMsgBuffer, sizeof(touchMsgBuffer), SERV_TOUCH, UDP_LOCAL_PORT_CLI, UDP_PORT_TOUCH );
            
              waitingTouchReply = 1;
              
              // Resetea el timer de espera de tarjeta de cliente, por las dudas de que
              // estuviera ingresando una tarjeta de cliente después de una carga de vendedor,
              // para que no venza el timeout mientras ingresa el PIN
              waitClientCardTimer = 0;
            }else if(Features.teclado == TECLADO_SERIE) {
              // módulo con teclado serial
              LCD_print(&MSG_INGRESAR_PIN);
              Gets((char*)cashMsgBuffer.ID.pin, 8, 3, 1);
              cash_flag = DO_CASHIN;
              // Resetea el timer de espera de tarjeta de cliente, por las dudas de que
              // estuviera ingresando una tarjeta de cliente después de una carga de vendedor,
              // para que no venza el timeout mientras ingresa el PIN
              waitClientCardTimer = 0;
            }else{
              // módulo sin teclado
              cashMsgBuffer.ID.pin[0]=0;
              cash_flag = DO_CASHIN;
            }
          }
        }
      }
    }
  }
}

//--------------------------------------------------------
// Verifica si debe retransmitir un mensaje de eventos
// Obs.: debe ser invocada al vencer el timeout de
// espera del ACK a un mensaje de eventos
//--------------------------------------------------------
static void checkEventMsgRetry()
{
  if( ++eventMsgRetry < 3 )
  {
    // Ordena retransmitir el último mensaje de eventos, porque no se recibió el ACK
    if( sendUDP( (unsigned char *) &bufEventos, sizeof(EventMsgType), SERV_EVENTS, UDP_LOCAL_PORT_CLI, UDP_PORT_EVENTS ) )
    {
      // activa la espera del ACK
      waitingEventAck = 1;
      waitEventAckTimer = 0;
    }
  }else{
    // Transmitió 3 veces sin recibir ACK --> aborta el proceso y resetea el buffer de eventos
    waitingEventAck = 0;  // detiene la espera del ACK
    clearEventBuffer();   // borra el buffer de eventos e incrementa el nro de secuencia
    eventMsgRetry = 0;    // resetea el contador de retransmisiones de eventos
  }
}

//-----------------------------------------------------
// Le pide un evento a la VGM
// Retorna 1 si recibió respuesta, 0 si no
// En *evt deposita el código de evento recibido
//-----------------------------------------------------
static unsigned char getEvento(unsigned char *evt)
{
  int i;
  
  // Lazo for para repetir hasta 3 veces el pedido de eventos.
  // Si a la 3ra vez no se obtuvo respuesta, retorna con 0 
  for(i=0; i<3; ++i)
  {
    UART0_tx(0x80 | vgmAddr, 1);   // pide un evento a la VGM

    // Espera la máxima longitud de un mensaje de evento (15), cuando
    // está habilitado el modo real time event report
    
    switch( UART0_rx(15) )  
    {
    case 0:
      // No hubo respuesta
    case 1:
      // Respondió en forma normal con el código de evento
      *evt = rx_buffer[0];
      return 1;
    default:
      // respondió con un mensaje de tipo respuesta a long poll,
      // porque está en modo real time event report
      if( rx_buflen >= 5 && !CRC(rx_buffer,rx_buflen,0) )
      {
        *evt = rx_buffer[2];
        return rx_buflen;
      }
      break;
    }
  }
  return 0;   // No obtuvo respuesta
}

//----------------------------------------------------------------
// Borra el buffer donde almacena los eventos
//----------------------------------------------------------------
static void clearEventBuffer(void)
{
  int i;
  bufEventos.msgID[0]='E';
  bufEventos.msgID[1]='V';
  bufEventos.nSec = ++nSecEventMsg;
  bufEventos.nEventos=0;
  for(i=0; i<MAX_EVENTOS; ++i)
      bufEventos.evento[i] = 0;
}

//------------------------------------------
// Verifica el ACK de eventos recibido
//------------------------------------------
void checkEventAck(void)
{
  EventAckType *p = (EventAckType *) UDP_RX_BUF;
  
  if(waitingEventAck && UDPRxDataCount==sizeof(EventAckType))
  {
    decifrar((unsigned char *)p,sizeof(EventAckType));
    
    if(!CRC((unsigned char *)p,sizeof(EventAckType),0) && p->msgID[0]=='E' && p->msgID[1]=='V')
    {
      if( p->nSec == nSecEventMsg )
      {
        if ( p->codError==0 || (p->codError==1 && eventMsgRetry))
        {
          // Recibió 0x00 (OK) o bien 0x01 (Nro de Sec repetido) luego de una retransmisión
          waitingEventAck = 0;  // detiene la espera del ACK
          clearEventBuffer();   // borra el buffer de eventos e incrementa el nro de secuencia
          eventMsgRetry = 0;    // resetea el flag de retransmisión de eventos
        }else if( p->codError==1 ) {
          // Nro de Secuencia repetido (y no es una retransmisión)
          // Incrementa el N° de secuencia en el buffer de transmisión
          // y resetea el flag de espera de ACK, para que prepare otra transmisión
          ++nSecEventMsg;
          waitingEventAck = 0;  // detiene la espera del ACK
          eventMsgRetry = 0;    // resetea el flag de retransmisión de eventos
        }
      }
    }
  }
}

//----------------------------------------------------------------
// Borra el buffer donde almacena las respuestas a los Long Polls
//----------------------------------------------------------------

#define p ((unsigned char *)&longPollBuffer)

static void clearLongPollBuffer(void)
{
  int i;
  
  longPollBuffer.msgID[0]='L';
  longPollBuffer.msgID[1]='P';

  for(i=2; i<sizeof(LongPollMsgType); ++i)
      p[i] = 0xff;
  for(i=0; i<20; ++i)
  {
    longPollBuffer.lp1f[i] = 0;
    longPollBuffer.lp54[i] = 0;
  }
}

#undef p

//-------------------------------------------------
// Envía un comando de enable o disable de alguna
// función de la VGM
//-------------------------------------------------
static unsigned char enableDisableLP(unsigned char cmd)
{
  unsigned char buf[4], i;
  unsigned int crc;
  
  if(vgmProto==SAS)
  {
    buf[0]=vgmAddr;
    buf[1]=cmd;
    crc=CRC(buf,2,0);
    buf[2]=crc & 0x00ff;
    buf[3]=(crc >> 8) & 0x00ff;
    
    // transmite 
    UART0_tx(buf[0], 1);  // 1er byte con wakeup
    // el resto del mensaje normal
    for(i=1; i<4; ++i)
      UART0_tx(buf[i], 0);
    
    // espera respuesta
    if( !UART0_rx(32) || rx_buffer[0] != vgmAddr )
      return 0;         // No tuvo respuesta o resultado incorrecto
  }
  return 1;
}

//--------------------------------------------
// Deshabilita la generación de eventos
// en tiempo real
// Retorna 1 si tuvo éxito, 0 si no
//--------------------------------------------
static unsigned char disableRealTimeEventReport(void)
{
  unsigned char buf[5], i;
  unsigned int crc;
  
  buf[0]=vgmAddr;
  buf[1]=0x0E;
  buf[2]=0x00;
  crc=CRC(buf,3,0);
  buf[3]=crc & 0x00ff;
  buf[4]=(crc >> 8) & 0x00ff;
  
  // transmite 
  UART0_tx(buf[0], 1);  // 1er byte con wakeup
                        // el resto del mensaje normal
  for(i=1; i<5; ++i)
    UART0_tx(buf[i], 0);

  // espera respuesta
  if( !UART0_rx(32) || rx_buffer[0] != vgmAddr )
    return 0;         // No tuvo respuesta o CRC incorrecto

  return 1;
}

//---------------------------------------------------------------------------
// Efectúa un long poll y almacena el resultado en el buffer correspondiente.
// Cada vez efectúa un long poll con un código diferente.
// Retorna rx_buflen --> 0 indica que no hubo respuesta
//---------------------------------------------------------------------------
static unsigned char polling()
{
  int i=0, n;
  unsigned char *p;

  UART0_tx(vgmAddr,1);                      // dirección de la máquina
  UART0_tx(longPoll[nPoll].lpCommand, 0);   // comando requerido

  // Espera la respuesta

  if( !UART0_rx( longPoll[nPoll].size + 4 ))
  { // La VGM no respondió
    // Pasa al siguiente comando para la próxima vuelta
    nPoll = (nPoll + 1) % TOTAL_POLLS;
    return 0;
  }  
  // obtiene la longitud esperada de acuerdo al comando enviado
  // Obs.: incluye dirección, comando y crc

  n = longPoll[nPoll].size;

  // Verifica que la respuesta sea correcta, y la almacena

  if(CRC(rx_buffer,rx_buflen,0)==0 && rx_buffer[0]==vgmAddr && rx_buffer[1]==longPoll[nPoll].lpCommand)
  {
    p = ((unsigned char *) &longPollBuffer) + longPoll[nPoll].offset;
    
    for(i=0;i<n;++i)
      p[i] = rx_buffer[i+2];

    if(longPoll[nPoll].lpCommand == 0x1A && !waitingClientCard)
      currentCredits = bcd2long(p,8);
  }
  // Pasa al siguiente comando para la próxima vuelta
  nPoll = (nPoll + 1) % TOTAL_POLLS;

  return rx_buflen;
}

//---------------------------------------------------------------------------
// Mensaje de prueba de cashless
// Cada vez que recibe un mensaje por este puerto, simula que se puso o se
// sacó la tarjeta del lector
//---------------------------------------------------------------------------
void processTestMsg()
{
  // Si recibió un byte en cero --> cash out; != 0 --> cash in
  switch( UDP_RX_BUF[0] )
  {
  case 1:
    cash_op = CASHIN_REQ;
    cash_flag = DO_CASHIN;
    break;
  case 0:
    cash_op = CASHOUT_REQ;
    cash_flag = DO_CASHOUT_STD;
    break;
  case '>':
    remoteTest();
    break;
  default:
    // Graba tarjeta
    cardWrite(UDP_RX_BUF, 32, 32);    
    break;
  }
  // OJO: partes de esta función hay que eliminarlas en la versión final, es sólo
  // para probar cashless y grabación de tarjetas desde la PC
}

//----------------------------------------------------
// Procesa un mensaje recibido de la PC-touch screen
//----------------------------------------------------

#define rx ((TouchMsgType *)UDP_RX_BUF)

void processTouchReply(void)
{
  if( Features.teclado==TECLADO_IP && waitingTouchReply )
  {
    waitingTouchReply = 0;
    
    if( !CRC(UDP_RX_BUF, sizeof(TouchMsgType), 0) )
    {
      switch(rx->msgCode)
      {
      case TOUCH_SEND_MONTO:
        // Prepara el mensaje para pedir crédito al servidor.
        cashMsgBuffer.msgCode = CASHIN_REQ;
        memcpy(cashMsgBuffer.ID.cardID, cardID, 32);
        cashMsgBuffer.cashinOption = CASHIN_VEND;  // El monto fue ingresado manualmente por el vendedor
        cashMsgBuffer.protocol = 0;
        memcpy(cashMsgBuffer.monto, rx->monto, 11);
        cashMsgBuffer.hash = RANDOM();
        cashMsgBuffer.crc = CRC((unsigned char *)&cashMsgBuffer, sizeof(cashMsgBuffer)-2, 0);
        cifrar((unsigned char *)&cashMsgBuffer, sizeof(cashMsgBuffer));
        
        // Envía requerimiento al servidor
        if( sendUDP( (unsigned char *) &cashMsgBuffer, sizeof(cashMsgBuffer), SERV_CASH, UDP_LOCAL_PORT_CLI, UDP_PORT_CASH ) )
        {
          waitingCashinReply = 1;
          waitCashReplyTimer = 0;
        }
        
        // Toma como carga unitaria del vendedor el monto de la carga manual
        sscanf((const char *)rx->monto, "%lu", &cargaUnitVendedor);
        break;

      case TOUCH_SEND_PIN:        
        if( getCardStatus() )
        {
          strcpy((char*)cashMsgBuffer.ID.pin, (char *)rx->pin);
          cash_op = CASHIN_REQ;
          cash_flag = DO_CASHIN;
        }
        break;
      }
    }
  }
}

#undef rx

//------------------------------------------------------------------------------
// Procesa una respuesta a un mensaje de cashless
//------------------------------------------------------------------------------

#define rx ((CashMsgType *)UDP_RX_BUF)

void processCashlessReply()
{
  unsigned char status, msgCode;
  unsigned int i;
  unsigned long monto;

  // Verifica que esté esperando una respuesta a un cashin o cashout
  if( waitingCashinReply )
  {
    decifrar(UDP_RX_BUF, sizeof(CashMsgType));
    
    // Verifica que haya recibido una respuesta al requerimiento de cashin
    if( !CRC(UDP_RX_BUF, sizeof(CashMsgType), 0) )
    {
      waitingCashinReply = 0;   // Detiene la espera
    
      memcpy( (unsigned char *)&cashMsgBuffer, UDP_RX_BUF, sizeof(CashMsgType));

      // Verifica si el pedido de cashin fue rechazado
      if( cashMsgBuffer.msgCode == CASHIN_REP_REJ )
      {
        if( cashMsgBuffer.protocol != 'T' )
        {
          // Rechazo a una carga de cashless
          switch(cashMsgBuffer.errorCode)
          {
          case CASH_TARJETA_INVALIDA:
            LCD_print(&MSG_ERROR_TARJETA_INVALIDA);
            break;
          case CASH_PIN_INVALIDO:
            if( Features.teclado==TECLADO_SERIE || Features.teclado==TECLADO_IP )
              LCD_print(&MSG_PIN_INVALIDO);
            else
              LCD_print(&MSG_ERROR_INDETERMINADO);
            break;
          case CASH_REINGRESAR_PIN:
            if( Features.teclado==TECLADO_IP && !waitingTouchReply )
            {
              touchMsgBuffer.msgCode  = TOUCH_GET_PIN;
              touchMsgBuffer.hash = RANDOM();
              touchMsgBuffer.crc = CRC((unsigned char *)&touchMsgBuffer,sizeof(touchMsgBuffer)-2,0);
              sendUDP( (unsigned char *) &touchMsgBuffer, sizeof(touchMsgBuffer), SERV_TOUCH, UDP_LOCAL_PORT_CLI, UDP_PORT_TOUCH );
              waitingTouchReply = 1;
              // Resetea el timer de espera de tarjeta de cliente, por las dudas de que
              // estuviera ingresando una tarjeta de cliente después de una carga de vendedor,
              // para que no venza el timeout mientras ingresa el PIN
              waitClientCardTimer = 0;
            }else if( Features.teclado==TECLADO_SERIE ) {
              LCD_print(&MSG_REINGRESAR_PIN);
              Gets((char*)cashMsgBuffer.ID.pin, 8, 3, 1);
              // Ordena enviar un nuevo CASHIN_REQ al servidor
              cash_op = CASHIN_REQ;
              cash_flag = DO_CASHIN;
            }else
              LCD_print(&MSG_ERROR_INDETERMINADO);
            break;
          case CASH_TARJETA_BLOQUEADA:
            LCD_print(&MSG_TARJETA_BLOQUEADA);
            break;
          case CASH_VENDEDOR_SIN_FONDOS:
            LCD_print(&MSG_VEND_SIN_FONDOS);
            break;
          case CASH_ASOCIADA_A_OTRO_SLOT:
            LCD_print(&MSG_ASOCIADA_A_OTRO_SLOT);
            break;
          case CASH_MONTO_INVALIDO:
            LCD_print(&MSG_MONTO_INVALIDO);
            break;
          case CASH_MONTO_EXCESIVO:
            LCD_print(&MSG_MONTO_EXCESIVO);
            break;
          default:
            LCD_print(&MSG_ERROR_INDETERMINADO);
            break;
          }
          return;
        }else{
          // Rechazo a una carga con ticket
          LCD_print(&MSG_ERROR_TICKET_RECHAZADO);
          TITO_cashin_reject(cashMsgBuffer.errorCode);
          return;
        }
      }
        
      switch( cashMsgBuffer.msgCode )
      {
      case CASHIN_REP_CLI_OK:
      case CASHIN_REP_VEND_OK:
        break;
      case CASHIN_REP_VEND_MAX:
        if( Features.teclado==TECLADO_SERIE || Features.teclado==TECLADO_IP )
          break;
      default:
        return;  // El mensaje recibido no es coherente con el estado del módulo --> lo descarta
      }
      
      if( cashMsgBuffer.protocol != 'T' )
      {
        // Cashless --> Si el ID de la tarjeta no coincide con el del mensaje, aborta
        for(i=0; i<32; ++i)
        {
          if(cardID[i] != cashMsgBuffer.ID.cardID[i])
          {
            LCD_print(&MSG_ERROR_INTENTE_OTRA_VEZ);
            
            // Envía el mensaje DE NAK al servidor
            cashMsgBuffer.msgCode = CASHIN_NAK;
            cashMsgBuffer.errorCode = AFT_TRANSFER_CANCELLED_BY_HOST;
            sprintf((char *)cashMsgBuffer.ID.cardID, "%031u", 0);
            sprintf((char *)cashMsgBuffer.monto, "%010u", 0);
            cashMsgBuffer.hash = RANDOM();
            cashMsgBuffer.crc = CRC((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer)-2,0);
            cifrar((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer));
            sendUDP( (unsigned char *) &cashMsgBuffer, sizeof(cashMsgBuffer), SERV_CASH, UDP_LOCAL_PORT_CLI, UDP_PORT_CASH );
            return;
          }
        }
      }else{
        // TITO --> Si el validation ID no coincide con el del ticket, aborta
        for(i=0; i<9; ++i)
        {
          if(ticketID[i] != cashMsgBuffer.ticket.validation_number[i])
          {
            // OJO --> acá hay que expulsar el ticket
            TITO_cashin_reject(TITO_UNABLE_TO_VALIDATE);
            
            LCD_print(&MSG_ERROR_TICKET_RECHAZADO);
            
            // Envía el mensaje DE NAK al servidor
            cashMsgBuffer.msgCode = CASHIN_NAK;
            cashMsgBuffer.errorCode = TITO_UNABLE_TO_VALIDATE;
            sprintf((char *)cashMsgBuffer.ID.cardID, "%031u", 0);
            sprintf((char *)cashMsgBuffer.monto, "%010u", 0);
            cashMsgBuffer.hash = RANDOM();
            cashMsgBuffer.crc = CRC((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer)-2,0);
            cifrar((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer));
            sendUDP( (unsigned char *) &cashMsgBuffer, sizeof(cashMsgBuffer), SERV_CASH, UDP_LOCAL_PORT_CLI, UDP_PORT_CASH );
            return;
          }
        }
      }
      // Si es un módulo con teclado y el mensaje recibido contiene el monto
      // máximo que puede cargar un vendedor, el vendedor debe ingresar manualmente
      // el monto a cargar a la VGM
      if( Features.teclado==TECLADO_IP && cashMsgBuffer.msgCode==CASHIN_REP_VEND_MAX )
      {
        // Módulo con PC y touch screen --> envía un mensaje a la PC-touch screen
        // para que el vendedor ingrese el monto a cargar
        touchMsgBuffer.msgCode  = TOUCH_GET_MONTO;
        memcpy(touchMsgBuffer.maxMonto, cashMsgBuffer.monto, 11);
        touchMsgBuffer.hash = RANDOM();
        touchMsgBuffer.crc = CRC((unsigned char *)&touchMsgBuffer,sizeof(touchMsgBuffer)-2,0);

        sendUDP( (unsigned char *) &touchMsgBuffer, sizeof(touchMsgBuffer), SERV_TOUCH, UDP_LOCAL_PORT_CLI, UDP_PORT_TOUCH );

        waitingTouchReply = 1;
        return;
      }else if( Features.teclado==TECLADO_SERIE && cashMsgBuffer.msgCode==CASHIN_REP_VEND_MAX ) {
        // Módulo con teclado con conexión serie. El vendedor debe ingresar
        // el monto a cargar y luego se envía el mensaje de pedido al servidor.
        
        // Antes, guarda el monto máximo de que dispone el vendedor, en pesos,
        // enviado por el servidor
        sscanf((const char *)cashMsgBuffer.monto, "%lu", &montoMaxVendedorPesos);

        // Ingresar por teclado el monto a cargar
        LCD_print(&MSG_INGRESAR_MONTO);
        Gets((char*)cashMsgBuffer.monto, 10, 3, 0);

        // Guarda como carga unitaria del vendedor, el monto ingresado manualmente
        sscanf((const char *)cashMsgBuffer.monto, "%lu", &cargaUnitVendedor);

        // Prepara el mensaje para pedir crédito al servidor.
        cashMsgBuffer.msgCode = CASHIN_REQ;
        memcpy(cashMsgBuffer.ID.cardID, cardID, 32);
        cashMsgBuffer.cashinOption = CASHIN_VEND; // El monto fue ingresado manualmente por el vendedor
        cashMsgBuffer.protocol = 0;
        cashMsgBuffer.hash = RANDOM();
        cashMsgBuffer.crc = CRC((unsigned char *)&cashMsgBuffer, sizeof(cashMsgBuffer)-2, 0);
        
        cifrar((unsigned char *)&cashMsgBuffer, sizeof(cashMsgBuffer));

        // Envía requerimiento al servidor
        if( sendUDP( (unsigned char *) &cashMsgBuffer, sizeof(cashMsgBuffer), SERV_CASH, UDP_LOCAL_PORT_CLI, UDP_PORT_CASH ) )
        {
          waitingCashinReply = 1;
          waitCashReplyTimer = 0;
        }
        return;
      }
      
      // Si es SAS y el protocolo no es AFT ni EFT ni TITO, aborta
      if( vgmProto==SAS && !(cashMsgBuffer.protocol=='E' || cashMsgBuffer.protocol=='A' || cashMsgBuffer.protocol=='T') )
      {
        LCD_print(&MSG_ERROR_EFT_AFT);
        
        // Envía el mensaje DE NAK al servidor
        cashMsgBuffer.msgCode = CASHIN_NAK;
        cashMsgBuffer.errorCode = EFT_TRANSFER_CANCELLED_BY_HOST;
        sprintf((char *)cashMsgBuffer.ID.cardID, "%031u", 0);
        sprintf((char *)cashMsgBuffer.monto, "%010u", 0);
        cashMsgBuffer.hash = RANDOM();
        cashMsgBuffer.crc = CRC((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer)-2,0);
        cifrar((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer));
        sendUDP( (unsigned char *) &cashMsgBuffer, sizeof(cashMsgBuffer), SERV_CASH, UDP_LOCAL_PORT_CLI, UDP_PORT_CASH );
        return;
      }
      
      // Si es una VGM EIBE, fuerza a usar AFT
      if( vgmProto == EIBE )
        cashMsgBuffer.protocol = 'A';        
      
      // Guarda el protocolo cashless a usar
      cashProtocol = cashMsgBuffer.protocol;

      // Obtiene el monto a acreditar (de acuerdo a la VGM, este monto
      // puede estar dado en centavos o en créditos)
      sscanf((char *)cashMsgBuffer.monto, " %lu", &monto);

      if( rx->msgCode == CASHIN_REP_CLI_OK && !(vgmProto==EIBE || rx->protocol=='T'))
      {
        // Obtiene los créditos previos a la carga, porque no admite la
        // carga de un cliente si la máquina previamente tenía créditos.
        // Lo que sí está permitido es que un vendedor agregue créditos o que
        // un cliente (luego de un vendedor) haga una carga en cero.
        
        // Obs.: se modificó para que las EIBE permitan que un cliente cargue aunque
        // la VGM tenga crédito (por las características del juego de bingo) (17/3/11)
        
        // Obs.2: modificado posteriormente para que permita cargar con Ticket
        // a pesar de que la VGM tenga crédito 
        
        if( !getCurrentCredits(&currentCredits) || (currentCredits && monto) )
        {
          LCD_print(&MSG_CARGA_INVALIDA);
          
          // Envía el mensaje DE NAK al servidor
          cashMsgBuffer.msgCode = CASHIN_NAK;
          cashMsgBuffer.errorCode = AFT_TRANSFER_CANCELLED_BY_HOST;
          sprintf((char *)cashMsgBuffer.ID.cardID, "%031u", 0);
          sprintf((char *)cashMsgBuffer.monto, "%010u", 0);
          cashMsgBuffer.hash = RANDOM();
          cashMsgBuffer.crc = CRC((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer)-2,0);
          cifrar((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer));
          sendUDP( (unsigned char *) &cashMsgBuffer, sizeof(cashMsgBuffer), SERV_CASH, UDP_LOCAL_PORT_CLI, UDP_PORT_CASH );
          return;
        }
      }
      
      if( rx->msgCode == CASHIN_REP_VEND_OK )
      {
        // Si es una carga de vendedor no carga la VGM, sino que acumula en
        // montoVendedor y envía un mensaje de ACK (simulando una carga exitosa)
        cashMsgBuffer.msgCode = CASHIN_ACK;
        cashMsgBuffer.errorCode = status = 0;
        sprintf((char *)cashMsgBuffer.ID.cardID, "%031u", 0);
        sprintf((char *)cashMsgBuffer.monto, "%010u", 0);
        cashMsgBuffer.hash = RANDOM();
        cashMsgBuffer.crc = CRC((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer)-2,0);
        cifrar((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer));
      }else{
        // Es una carga de cliente --> carga la VGM usando el protocolo
        // correspondiente y prepara el mensaje de ACK o NAK
        
        switch(vgmProto)
        {
        case SAS:
        case EIBE:
          if( cashMsgBuffer.protocol == 'E' )
          {
            if(!getCardStatus())
            {
              // Se retiró la tarjeta mientras se esperaba el cashin reply -->
              // --> no hace la carga y la reporta como errónea
              status = EFT_TRANSFER_CANCELLED_BY_HOST;
              break;
            }
            // Intenta cargar la VGM
            for(i=0; i<2; ++i)
            {
              if(monto+montoVendedor)
              {
                if(debug)
                  sendLogMsg("EFT CASHIN (INI)",0,monto+montoVendedor);

                status = EFT_cashin(monto+montoVendedor);

                if(debug)
                  sendLogMsg("EFT CASHIN (FIN)",status,monto+montoVendedor);
              }else{
                status = EFT_SUCCESS;
              }
              if( status == EFT_SUCCESS )
                break;
              delay(pollingPeriod);
              UART0_tx(0x80,1);
              delay(pollingPeriod);
            }
          }else if( cashMsgBuffer.protocol == 'A' ) {
            if(!getCardStatus())
            {
              // Se retiró la tarjeta mientras se esperaba el cashin reply -->
              // --> no hace la carga y la reporta como errónea
              status = AFT_TRANSFER_CANCELLED_BY_HOST;
              break;
            }
            // Intenta cargar la VGM
            if(monto+montoVendedor)
            {
              if(debug)
                sendLogMsg("AFT CASHIN (INI)", 0, monto+montoVendedor);

              status = AFT_cashin(monto+montoVendedor);

              if(debug)
                sendLogMsg("AFT CASHIN (FIN)", status, monto+montoVendedor);
            }else{
              status = AFT_FULL_TRANSFER_OK;
            }
          }else if( cashMsgBuffer.protocol == 'T' ) {
            // Intenta cargar la VGM
            if(monto)
            {
              if(debug)
                sendLogMsg("TITO CASHIN (INI)", 0, monto);

              status = TITO_cashin(monto);

              if(debug)
                sendLogMsg("TITO CASHIN (FIN)", status, monto);
            }else{
              status = AFT_FULL_TRANSFER_OK;
            }
          }
          break;
        case DACOM:
          if(!getCardStatus())
          {
            // Se retiró la tarjeta mientras se esperaba el cashin reply -->
            // --> no hace la carga y la reporta como errónea
            status = DACOM_TRANSFER_CANCELLED_BY_HOST;
            break;
          }
          // Como el primer mensaje para subir o bajar crédito a la VGM debe estar
          // a menos de 20 ms de haber recibido una trama tipo 1, se hacen hasta dos
          // intentos. Si el primero falla por haberse iniciado después del tiempo
          // máximo, se espera una segunda trama tipo 1 e inmediatamente se efectúa
          // la transferencia.
          for(i=0; i<2; ++i)
          {
            status = ((monto+montoVendedor)? dacom_cashin(monto+montoVendedor) : DACOM_SUCCESS);
            // Intenta cargar la VGM
            if(monto+montoVendedor)
            {
              if(debug)
                sendLogMsg("DACOM CASHIN (INI)",0,monto+montoVendedor);

              status = dacom_cashin(monto+montoVendedor);

              if(debug)
                sendLogMsg("DACOM CASHIN (FIN)",status,monto+montoVendedor);
            }else{
              status = DACOM_SUCCESS;
            }
            if( status == DACOM_SUCCESS )
              break;
          }
          break;
        }
        cashMsgBuffer.msgCode = ((status)? CASHIN_NAK : CASHIN_ACK);
        cashMsgBuffer.errorCode = status;
        sprintf((char *)cashMsgBuffer.ID.cardID, "%031u", 0);
        sprintf((char *)cashMsgBuffer.monto, "%010u", 0);
        cashMsgBuffer.hash = RANDOM();
        cashMsgBuffer.crc = CRC((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer)-2,0);
        cifrar((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer));
      }
      
      // Actualiza el tiempo de espera para la lectura de la próxima tarjeta
      if( rx->msgCode == CASHIN_REP_VEND_OK )
      {
        // Si la tarjeta actual es de vendedor, para la próxima espera un tiempo más breve
        // Obs.: en las DACOM espera un poco más, porque son muy rápidas.        
        cardReaderLockTime = ((vgmProto==DACOM)?_1500ms:_10ms);
      }else{
        // Si la tarjeta actual es de cliente, para la próxima espera el tiempo más largo
        cardReaderLockTime = (vgmProto==EIBE)?_10seg:_5seg;
      }
      
      // Guarda el resultado del último cashin exitoso
      if( !status )
      {
        // La carga en la VGM fué exitosa. Guarda en cashin_ok el código del mensaje de
        // respuesta del servidor (CASHIN_REP_CLI_OK o CASHIN_REP_VEND_OK), para saber
        // si en el cashout debe descargar la VGM o no.
        // Obs.: cuando se retira la tarjeta de un vendedor no se descargan los créditos
        cashin_ok = rx->msgCode;
        
        if( cashin_ok == CASHIN_REP_VEND_OK )
        {
          // Si la carga la hizo un vendedor, acumula el monto de la misma
          montoVendedor += monto;
          montoVendedorPesos += cargaUnitVendedor;
        }else{
          // Si la carga la hizo un cliente, resetea el monto acumulado del vendedor
          montoVendedor = 0;
          montoVendedorPesos = 0;
        }
      }

      // Envía el mensaje de ACK o NAK al servidor
      sendUDP( (unsigned char *) &cashMsgBuffer, sizeof(cashMsgBuffer), SERV_CASH, UDP_LOCAL_PORT_CLI, UDP_PORT_CASH );
      
      if( !status )
      {
        // La carga fue exitosa
        if( cashin_ok == CASHIN_REP_CLI_OK )
        {
          // Cargó con una tarjeta de cliente
          if( monto || waitingClientCard )
            LCD_print(&MSG_MAQUINA_EN_JUEGO);
          else
            LCD_print(&MSG_NO_TIENE_CREDITO);
          
          // Por si acaso la tarjeta de cliente hubiera sido ingresada luego de una
          // de vendedor, resetea el flag de "esperando tarjeta de cliente"
          waitingClientCard = 0;
          
          // Habilita el billetero
          EnableBillAcceptor();
          UART0_tx(0x80,1);
          delay(pollingPeriod);
        }else if( cashin_ok == CASHIN_REP_VEND_OK ) {
          // Cargó con una tarjeta de vendedor
          if( monto )
          {
            // Muestra en el display el total que lleva cargado.
            *toumsg = MSG_MONTO_VENDEDOR;
            if( Features.teclado == TECLADO_SERIE || Features.teclado == TECLADO_IP )
              // Si el vendedor cargó el monto manualmente, muestra el monto cargado
              // y el saldo del Vendedor
              sprintf(toumsg->txt[0], "C: %-lu (V: %-lu)", montoVendedorPesos, montoMaxVendedorPesos - montoVendedorPesos);
            else
              // Si el módulo no tiene teclado, muestra el acumulado a cargar a la VGM
              sprintf(toumsg->txt[0], "Carga: %-lu", montoVendedorPesos);
            LCD_print(toumsg);

            if( Features.teclado==TECLADO_SERIE || Features.teclado==TECLADO_IP )
            {
              // En los módulos que tienen teclado, la carga del vendedor se realiza
              // manualmente en una sola iteración. Por eso, espera hasta que
              // el vendedor retire la tarjeta
              stopWatchdog();
              while( getCardStatus() );
              startWatchdog();
            }
            // Simula haber quitado la tarjeta y hecho una descarga, para que
            // vuelva a detectar la tarjeta e inicie otro ciclo de carga.
            
            // Si el módulo tiene PC-touch screen no reinicia el ciclo, porque
            // el importe a cargar se ingresó manualmente en una sola vez.
            cardStatus = 0;
            cardReaderLocked = 1;
            cardReaderTimer = 0;
            // Previendo que debe quitar la tarjeta de vendedor,
            // también activa la espera de la tarjeta de cliente
            waitClientCardTimer = 0;
            waitingClientCard = 1;
          }else{
            LCD_print(&MSG_NO_TIENE_CREDITO);
          }
        }
      }else{
        // La carga falló
        LCD_print(&MSG_ERROR_EN_CARGA);
      }
    }
  }else if( waitingCashoutReply ){
    
    decifrar(UDP_RX_BUF, sizeof(CashMsgType));
    
    // Verifica que la respuesta al cashout haya sido correcta
    if( !CRC(UDP_RX_BUF, sizeof(CashMsgType), 0) )
    {
      if( rx->msgCode == CASHOUT_REP_OK || rx->msgCode == CASHOUT_REP_REJ )
      {
        // Ya sea que la descarga fue aceptada o rechazada, finaliza la transacción
        // La descarga pudo haber sido rechazada por el servidor, quizá porque en el cashin
        // el servidor había rechazado la tarjeta. Igualmente da por finalizada la transacción
        waitingCashoutReply = 0;

        // Reinicia el temporizador que demora la lectura de la próxima tarjeta
        cardReaderTimer = 0;

        // Si había un mensaje de CASHOUT_REQ guardado en la EEPROM, lo borra
        framRead( 0, &msgCode, 1 );
        if( msgCode != 0xff )
        {
          msgCode = 0xff;
          framWrite( 0, &msgCode, 1 );
        }

        if( rx->msgCode == CASHOUT_REP_REJ )
        {
          // El servidor rechazó la descarga.
          // Para que no se pierda el monto descargado, lo presenta
          // en el display y deja al módulo en un loop infinito.
          *toumsg = MSG_CASHOUT_REP_REJ;
          strcpy(toumsg->txt[1], (char *)cashMsgBuffer.monto );
          LCD_print(toumsg);
          delay(_2seg);

          sscanf( (char *)cashMsgBuffer.monto, " %lu", &monto );
          if( monto )
            while(1);  // deja bloqueado al módulo
        }
      }
    }
  }
}

#undef rx

//------------------------------------------------------------------------------
// Funciones para el manejo de la comunicación serie RS232
//------------------------------------------------------------------------------

//----------------------------------------------------------
// Inicializa la UART 0 dependiendo del protocolo de la VGM
// SAS : 19200 - 8+1 bits - 1 stop
// DACOM : 9600 - 8 bits - paridad par - 1 stop
// GAMMA : 9600 - 8 bits - paridad par - 1 stop
// EIBE : idem SAS
// Por default usa los valores de Gamma
// Asume que ACLK=2 MHz
//----------------------------------------------------------
static void InitUART(void)
{
  P3SEL |= 0x30;                // bits 4-5 del puerto 3 son para la UART0

  ME1 |= URXE0 + UTXE0;         // Habilita Tx y Rx en UART0
  
  switch( vgmProto )
  {
  case SAS:
  case EIBE:
    U0MCTL = 0x04;
    U0BR0 = 104;
    U0BR1 = 0;
    U0CTL = CHAR + MM;          // 8-bit + space
    break;

  case DACOM:
  case GAMMA:
  default:
    U0MCTL = 0x92;
    U0BR0 = 208;
    U0BR1 = 0;
    U0CTL = CHAR + PENA + PEV;  // 8-bit + paridad par
    break;
  }
  U0TCTL = SSEL0;               // clock source ACLK (2 MHZ)
  U0RCTL = 0;
  
  IE1 |= URXIE0;                // habilita interrupción por RX
}

//----------------------------------------------------------------------
// ISR para la recepción de datos por la UART 0
// Obs.: procesa el byte recibido en función de lo que se está esperando
//----------------------------------------------------------------------

#pragma vector = UART0RX_VECTOR
static __interrupt void UART0_rx_interrupt(void)
{
  rx_byte = U0RXBUF;    // lee y almacena el byte recibido

  rxByteTimer = 0;      // reinicia temporizador entre bytes

  switch( vgmProto )
  {
  case SAS:
  case EIBE:
    rx_flag=0x01;       // señaliza que se recibió un byte
    break;

  case DACOM:
    // almacena el byte recibido en el buffer de recepción
    if( rx_byte == 0x0FF )
      rx_buflen = 0;
    if( rx_buflen < 128 )
      rx_buffer[rx_buflen++] = rx_byte;
    break;
  }
}

//-----------------------------------------------------------
// Función para enviar un byte por la UART 0
// Si wakeup != 0 --> transmite el byte con el 9no bit en '1'
//-----------------------------------------------------------
void UART0_tx(unsigned char b, unsigned char wakeup)
{
  // espera que haya terminado de enviar el byte anterior
  while (!(IFG1 & UTXIFG0));

  if( vgmProto == SAS || vgmProto == EIBE )
  {
    // si el protocolo de la VGM es SAS, usa el bit de wakeup
    if(wakeup)
      U0TCTL |= TXWAKE;   // 9no bit en '1'
    else
      U0TCTL &= ~TXWAKE;  // 9no bit en '0'
  }
  
  // transmite
  U0TXBUF = b;
}

//-----------------------------------------
// función para calcular el CRC 16 (Kermit)
//-----------------------------------------
unsigned int CRC(unsigned char const message[], unsigned int nBytes, unsigned int crcval)
{
    unsigned int remainder = crcval;
    static const unsigned int POLYNOMIAL = 0x8408;

    // Realiza la dividión modulo-2, de a un byte por vez
    for(unsigned int byte = 0; byte < nBytes; ++byte)
    {
        // Coloca el siguiente byte en el resto
        remainder ^= message[byte];

        // Realiza la división modulo-2, de a un bit por vez
        for (unsigned char bit = 8; bit > 0; --bit)
        {
            // Intenta dividir el bit actual
            if (remainder & 1)
                remainder = (remainder >> 1) ^ POLYNOMIAL;
            else
                remainder = (remainder >> 1);
        }
    }
    // El resto de la división es el CRC.

    // Lo coloca en orden Little Endian
    //remainder = remainder >> 8 | remainder << 8;

    return (remainder);
}

//-------------------------------------------
// función para cifrar / decifrar un mensaje
//-------------------------------------------
void cifrar(unsigned char message[], int nBytes)
{
  int i, j;
  for(i=0, j=0; i<(nBytes-2); ++i, j=(j+1)%2)
  {
    message[i] ^= message[nBytes-2+j];
  }
}

//-----------------------------------------------------------
// Espera un mensaje por la UART y lo deposita en rx_buffer.
// Espera un cierto tiempo por el primer byte, y hasta 20 ms
// entre un byte y el otro.
// Retorna rx_buflen --> 0 indica que no recibió nada
// y salió por timeout
//-----------------------------------------------------------
unsigned char UART0_rx( unsigned char maxlen )
{
  rx_flag=0;
  waitLongPollTimer=0;
  rx_buflen=0;

  for(rx_buflen=0; rx_buflen < maxlen; )
  {
    if( !rx_buflen )
    {
      // Tiempo de espera del primer byte
      if( waitLongPollTimer > vgmResponseTime )
        break;  // da por finalizada la recepción
    }else 
      // Tiempo de espera de los siguientes bytes
      if( waitLongPollTimer > _20ms )
        break;  // da por finalizada la recepción
    
    if(rx_flag)
    {
      waitLongPollTimer=0;  // resetea timer
      rx_flag=0;
      rx_buffer[rx_buflen++] = rx_byte;
    }
  }
  return rx_buflen;
}

//----------------------------------------------------------------------------
// Idem UART0_rx(), pero usando un tiempo de respuesta
// distinto, para los comandos AFT
//----------------------------------------------------------------------------
unsigned char UART0_AFT_rx( unsigned char maxlen )
{
  unsigned int oldResponseTime = vgmResponseTime;
  unsigned char resp;
  vgmResponseTime = aftResponseTime;
  resp = UART0_rx(maxlen);
  vgmResponseTime = oldResponseTime;
  return resp;
}

//------------------------------------------------
// Retorna la cantidad de días del mes
//------------------------------------------------
static unsigned char ult_dia_del_mes()
{
  static unsigned char nDias[12]={31,28,31,30,31,30,31,31,30,31,30,31};
  if( tiempo.mes != 2 )
    return nDias[tiempo.mes];
  else if( (tiempo.anio % 4 == 0) && ( (tiempo.anio % 100) || (tiempo.anio % 400 == 0) ) )
    return 29;
  else return 28;
}

//------------------------------------------------
// Computa un timer tick y actualiza fecha y hora
//------------------------------------------------
void actualizaFechaHora(void)
{
  if( ++tiempo.ticks >= _1seg )
  {
    tiempo.ticks = 0;
    if( ++tiempo.seg > 59 )
    {
      tiempo.seg=0;
      if( ++tiempo.min > 59 )
      {
        tiempo.min = 0;
        if( ++tiempo.hora > 23 )
        {
          tiempo.hora = 0;
          if( tiempo.dia > ult_dia_del_mes() )
          {
            tiempo.dia = 1;
            if( ++tiempo.mes > 12 )
            {
              tiempo.mes = 1;
              ++tiempo.anio;
            }
          }
        }
      }
    }
  }
}

//---------------------------------------------------------
// Envía un mensaje para el log de debug
//---------------------------------------------------------

#define msg ((char *)UDP_TX_BUF + 2)

void sendLogMsg(char * txt, unsigned char status, unsigned long monto)
{
  sprintf(msg,"[%02d:%02d:%02d,%02d0] %s - Error: %-2x - Monto: %-lu",
          tiempo.hora,tiempo.min,tiempo.seg,tiempo.ticks, txt, status, monto);
  
  sendUDP( (unsigned char *)msg, strlen(msg)+1, SERV_MGMT, UDP_LOCAL_PORT_TEST, UDP_PORT_TEST );
}

#undef msg

//---------------------------------------------------------
// Verifica el mensaje de Eco Reply recibido del servidor,
// y actualiza fecha y hora
//---------------------------------------------------------

#define p ((HelloReplyMsgType *)UDP_RX_BUF)

void checkHelloReply(void)
{
  int i;
  
  if(UDPRxDataCount==sizeof(HelloReplyMsgType))
  {
    decifrar((unsigned char *)p,sizeof(HelloReplyMsgType));
    
    if( !CRC((unsigned char *)p,sizeof(HelloReplyMsgType),0) && p->msgID[0]=='F' && p->msgID[1]=='H')
    {
      if( !serverAlive && !cardStatus && !getCardStatus() )
      {
        // Se restableció la conexión con el servidor
        LCD_print(&MSG_MAQUINA_LIBRE);
      }    
      // Por las dudas, cada vez que envía un Hello reduce el período a 3 seg
      // Cada vez que recibe una respuesta lo restaura a 10 seg. De esa
      // manera, si se pierde la conexión con el servidor se detecta más rápido
      // y si la conexión se mantiene, no se altera nada.
      sendHelloPeriod = _10seg;
      
      serverAlive = 3;            // (el 3 es para permitir dos respuestas perdidas)
      
      // Obtiene la fecha y hora enviada por el servidor
      tiempo.anio=tiempo.mes=tiempo.dia=tiempo.hora=tiempo.min=tiempo.seg=0;
      
      for(i=0; i<4; ++i)
        tiempo.anio = tiempo.anio * 10 + (p->anio[i] - '0');
      
      for(i=0; i<2; ++i)
      {
        tiempo.mes = tiempo.mes * 10 + (p->mes[i] - '0');
        tiempo.dia = tiempo.dia * 10 + (p->dia[i] - '0');
        tiempo.hora = tiempo.hora * 10 + (p->hora[i] - '0');
        tiempo.min = tiempo.min * 10 + (p->min[i] - '0');
        tiempo.seg = tiempo.seg * 10 + (p->seg[i] - '0');
      }
      
      if( Features.teclado==FEATURES_DEFAULT || Features.teclado==TECLADO_NO )
      {
        // Obtiene el monto unitario de cada carga de vendedor, informada por el servidor
        sscanf(p->cargaUnitVendedor, "%lu", &cargaUnitVendedor);
      }
    }
  }
}

#undef p

//-------------------------------------------------------------------
// Convierte un valor en unsigned long a una cadena de n dígitos BCD
//-------------------------------------------------------------------
void long2bcd(unsigned char * d, unsigned long m, int n)
{
  int i, j;
  char buf[16];
  sprintf(buf,"%0*lu",n,m);
  for(i=0, j=0; i<n/2; ++i, j+=2)
  {
    d[i] = ((buf[j]-'0') << 4) | ((buf[j+1]-'0') & 0x0f);
  }
}

//-------------------------------------------------------------------
// Convierte a un valor en unsigned long una cadena de n dígitos BCD
//-------------------------------------------------------------------
unsigned long bcd2long(unsigned char * d, int n)
{
  int i;
  unsigned long m=0; //,m1,m2;

  for(i=0; i<n/2; ++i)
  {
//    m = m * 100;
//    m1 = d[i] >> 4;
//    m2 = d[i] & 0x0f;
//    m = m + m1*10 + m2;

    m = m * 100 + ((unsigned long)(d[i] >> 4))*10 + (unsigned long)(d[i] & 0x0f);
  }
  return m;
}

//--------------------------------------------
// Envía un AFT status request, para obtener
// los montos cashable y non-restricted
// cmd : 0x00 lock request
//       0x80 cancel lock
//       0xff status request
// Retorna 0x00 : Lock OK
//         0x40 : Lock pendiente
//         0xff : no bloqueado
//--------------------------------------------

#define pr ((AFT74Type *)rx_buffer)

static unsigned int AFT_lock_status_request(unsigned char cmd)
{
  unsigned int i;
  
  tx_buffer[0]=vgmAddr;
  tx_buffer[1]=0x74;
  tx_buffer[2]=cmd;   // comando
  tx_buffer[3]=0x03;  // transfer condition (VGM->host y host->VGM)
  tx_buffer[4]=0x05;  // timeout 200 centésimas de segundo (en BCD) (MSB)
  tx_buffer[5]=0x00;  // (LSB)
  i=CRC(tx_buffer,6,0);
  tx_buffer[6]=i & 0x00ff;
  tx_buffer[7]=(i >> 8) & 0x00ff;
  
  // transmite 
  UART0_tx(tx_buffer[0], 1);  // 1er byte con wakeup
                              // el resto del mensaje normal
  for(i=1; i<8; ++i)
    UART0_tx(tx_buffer[i], 0);

  // Espera respuesta (hasta 5 veces el tiempo de respuesta)
  if( !UART0_AFT_rx(sizeof(AFT74Type)) )
    return AFT_NO_RESPONSE;
  else if( !valid_AFT_response(0x74) )
    return AFT_BAD_RESPONSE;
    
  // Obtiene el asset number que luego va a usar para la transacción AFT
  AFT_asset_number = pr->resp.assetNumber[1] << 8 | pr->resp.assetNumber[0];
  
  // Obtiene el monto cashable que tiene la VGM. Si el lock se
  // hace antes de una descarga, éste es el monto a descargar
  cash_monto = bcd2long(pr->resp.cashableAmount, 10);

  return pr->resp.gameLockStatus;
}

#undef pr

//-----------------------------------------------------------
// Verifica que una respuesta AFT sea válida en general.
// Posteriormente se deben verificar cuestiones específicas,
// de acuerdo a cada caso.
// Retorna 1 (OK) o 0 (Error)
//-----------------------------------------------------------
static unsigned int valid_AFT_response(unsigned char cmd)
{
  if( rx_buflen )                           // que el mensaje no sea nulo
    if( CRC(rx_buffer,rx_buflen,0) == 0 )   // que el CRC sea correcto
      if( rx_buffer[0] == vgmAddr )         // que la dirección de la VGM sea la correcta
        if( rx_buffer[1] == cmd )           // que el comando AFT sea 0x72
          if( rx_buflen == (rx_buffer[2] + 5) )   // que la longitud indicada coincida con la real
            return 1;
  return 0;
}

//--------------------------------------------
// Envía un AFT interrogate
//--------------------------------------------
static void AFT_interrogate(void)
{
  unsigned int i;
  
  tx_buffer[0]=vgmAddr;
  tx_buffer[1]=0x72;
  tx_buffer[2]=2;
  tx_buffer[3]=0xFF;  // complete transfer
  tx_buffer[4]=0x00;  // transaction index
  i=CRC(tx_buffer,5,0);
  tx_buffer[5]=i & 0x00ff;
  tx_buffer[6]=(i >> 8) & 0x00ff;
  
  // transmite 
  UART0_tx(tx_buffer[0], 1);  // 1er byte con wakeup
                        // el resto del mensaje normal
  for(i=1; i<7; ++i)
    UART0_tx(tx_buffer[i], 0);
}

//-------------------------------------------------
// Prepara el mensaje AFT Init, para subir o bajar
// crédito, de acuerdo al tipo y monto solicitado
//-------------------------------------------------

#define pt ((AFT72Type *)tx_buffer)

static void AFT_prepare_init(unsigned char transferCode, unsigned char transferType, unsigned char flags, unsigned long monto)
{
  unsigned int i;
  
  tx_buflen = 57;
  
  pt->init.vgmAddr = vgmAddr;
  pt->init.commandCode = 0x72;
  pt->init.length = tx_buflen - 5;
  pt->init.transferCode = transferCode;
  pt->init.transactionIndex = 0;
  pt->init.transferType = transferType;
  long2bcd( pt->init.cashableAmount, monto, 10 );
  long2bcd( pt->init.restrictedAmount, 0, 10 );
  long2bcd( pt->init.nonRestrictedAmount, 0, 10 );
  pt->init.transferFlags = flags;
  pt->init.assetNumber[0] = (unsigned char)(AFT_asset_number & 0xFF);
  pt->init.assetNumber[1] = (unsigned char)(AFT_asset_number >> 8) & 0xFF;
  pt->init.assetNumber[2] = 0;
  pt->init.assetNumber[3] = 0;
  for(i=0; i<20; ++i)
    pt->init.registrationKey[i] = 0;
  pt->init.transactionIDLength = 1;
  pt->init.transactionID = AFT_transactionID;
  for(i=0; i<4; ++i)
    pt->init.expiration[i] = 0;
  pt->init.poolID[0] = 0;
  pt->init.poolID[1] = 0;
  pt->init.receiptDataLength = 0;
  i = CRC( (unsigned char *)pt, tx_buflen - 2, 0);
  pt->init.crc[0] = i & 0x00ff;
  pt->init.crc[1] = (i >> 8) & 0x00ff;
}

#undef pt

//----------------------------------------------------------------------------------
// Hace una interrogación para cerrar una eventual transacción incompleta
// anterior, y obtener el número de transaction ID
// Si interrogate es cero, simplemente incrementa el valor actual, sin
// hacer la interrogación.
// Retorna: 1 si OK
//          0 si no pudo obtenerlo
//----------------------------------------------------------------------------------

#define pr ((AFT72Type *)rx_buffer)

static unsigned char AFT_getTransactionID( unsigned int interrogate )
{
  if( !interrogate )
  {
    // No interroga --> incremente el transactionID anterior
    AFT_transactionID = ((AFT_transactionID < '0' || AFT_transactionID > '8')? '0' : AFT_transactionID+1);
    return 1;
  }
  // Hace una interrogación para ver el ID de la última transacción
  AFT_interrogate();
  
  // Espera respuesta (hasta 5 veces el tiempo de respuesta normal)
  // y verifica que la misma sea válida.
  if( !UART0_AFT_rx(sizeof(AFT72Type)) || !valid_AFT_response(0x72) )
  {
    // No recibió respuesta válida a la interrogación --> pone
    // un número de transacción fuera de la secuencia normal
    AFT_transactionID = 'A' + (tiempo.seg % 20);
    return 0;
  }
  
  if( (pr->resp.transferStatus & AFT_TRANSFER_STATUS_MASK) == AFT_TRANSFER_OK )
  {
    // Obtiene el último ID para incrementarlo
    AFT_transactionID = ((pr->resp.transactionID < '0' || pr->resp.transactionID > '8')? '0' : pr->resp.transactionID+1);
    // Retorna el status de la última transacción AFT
  }else{
    // Pone un número de transacción fuera de la secuencia normal
    AFT_transactionID = 'A' + (tiempo.seg % 20);
  }
  return 1;
}

#undef pr

//----------------------------------------------------------------------------------
// Transacción AFT para cargar crédito en la VGM
// Retorna: 0 (AFT_FULL_TRANSFER_OK)
//          o el Transfer Status que indica por que no se completó la transferencia
//          o 0x0F (VGM no responde)
//          o 0x2F (Error en AFT Lock)
//          o 0x1F (otra causa de error)
//----------------------------------------------------------------------------------

#define pr ((AFT72Type *)rx_buffer)

static unsigned char AFT_cashin(unsigned long monto)
{
  unsigned int i, j=0;

  if( vgmProto != EIBE )
  {
    // Intenta bloquear la VGM
    do
    {
      if( j == AFT_LOCK_PENDING )
        j = AFT_lock_status_request(AFT_STATUS_REQUEST);
      else
        j = AFT_lock_status_request(AFT_LOCK_REQUEST);
      delay(pollingPeriod);
      UART0_tx(0x80,1);
      delay( pollingPeriod );
    }while( j == AFT_LOCK_PENDING );
  }

  delay(pollingPeriod);
  UART0_tx(0x80,1);
  delay( pollingPeriod );

  // Obtiene el transactionID a usar
  AFT_getTransactionID(0);
  
  delay( pollingPeriod );
  UART0_tx(0x80,1);
  delay( pollingPeriod );
  
  // Prepara el AFT init
  AFT_prepare_init(AFT_FULL_TRANSFER_REQ, AFT_CASHIN, 0x00, monto);

  // y lo envía por la UART0
  UART0_tx(tx_buffer[0],1);                 // primer byte --> dirección de la VGM - wakeup bit = 1
  for(i=1; i<tx_buflen; ++i)
    UART0_tx(tx_buffer[i],0);               // resto del mensaje -> wakeup bit = 0
  
  // Espera respuesta
  if( !UART0_AFT_rx(sizeof(AFT72Type)) )
    return AFT_NO_RESPONSE;                 // La VGM no respondió
  
  // Verifica que la respuesta sea válida e indique transferencia pendiente
  if( !valid_AFT_response(0x72) )
    return AFT_BAD_RESPONSE;
  
  if( pr->resp.transferStatus != AFT_TRANSFER_PENDING )
    return pr->resp.transferStatus;
  
  do {
    delay(pollingPeriod);
    UART0_tx(0x80,1);
    delay(pollingPeriod);

    // Envía la interrogación con 0xFF para completar la transferencia
    AFT_interrogate();
    
  } while(!UART0_AFT_rx(sizeof(AFT72Type)) || !valid_AFT_response(0x72) || pr->resp.transferStatus==AFT_TRANSFER_PENDING);

  // Retorna el transfer status de la última interrogación
  return pr->resp.transferStatus;
}

#undef pr

//----------------------------------------------------------------------------------
// Transacción AFT para descargar todo el crédito de la VGM
// Retorna: 0 (AFT_FULL_TRANSFER_OK)
//          o el Transfer Status que indica por que no se completó la transferencia
//          o 0x0F (la VGM no responde)
//          o 0x2F (Error en AFT Lock)
//          o 0x1F (otra causa de error)
//          La variable cash_monto queda cargada con el monto descargado
//----------------------------------------------------------------------------------

#define pr ((AFT72Type *)rx_buffer)

static unsigned char AFT_cashout(void)
{
  unsigned int i=0, j=0;

  cash_monto = 0;

  if( cash_flag == DO_CASHOUT_STD || cash_flag == DO_HANDPAY_RESET )
  {
    // Descarga por retiro de tarjeta o por botón de pago
    // con evento 6A --> intenta bloquear la VGM
    do
    {
      if( j == AFT_LOCK_PENDING )
        j = AFT_lock_status_request(AFT_STATUS_REQUEST);
      else
        j = AFT_lock_status_request(AFT_LOCK_REQUEST);
      delay(pollingPeriod);
      UART0_tx(0x80,1);
      delay( pollingPeriod );
    }while( cash_monto && (++i < 10) && j == AFT_LOCK_PENDING );
    
    if( j == AFT_NO_RESPONSE || j == AFT_BAD_RESPONSE )
      return j;
    
    if( cash_monto && (i>= 10 || j != AFT_LOCKED) )
      return AFT_LOCK_ERROR;  // No pudo obtener el bloqueo de AFT
    
  }else if( cash_flag==DO_CASHOUT_BUTTON_EIBE && vgmProto==EIBE ) {
    // EIBE : descarga por botón de pago presionado
    // No bloquea la VGM, sino que consulta créditos a descargar

    if( !getCurrentCredits(&currentCredits) )
      return AFT_NO_RESPONSE;                 // La VGM no respondió

    cash_monto = currentCredits;
  }else{
    // Situación incompatible (no debería salir nunca por acá)
    return AFT_UNEXPECTED_ERROR;
  }  

  // Si la VGM está en cero, sale como si hubiera descargado
  if( !cash_monto )
    return AFT_FULL_TRANSFER_OK;

  // Obtiene el transactionID a usar
  AFT_getTransactionID(0);
  
  delay( pollingPeriod );
  UART0_tx(0x80,1);
  delay( pollingPeriod );
  
  // Prepara el AFT init
  AFT_prepare_init(AFT_FULL_TRANSFER_REQ, AFT_CASHOUT, AFT_HOST_CASHOUT_MODE_HARD, cash_monto);

  // y lo envía mensaje por la UART0
  UART0_tx(tx_buffer[0],1);                 // primer byte --> dirección de la VGM - wakeup bit = 1
  
  for(i=1; i<tx_buflen; ++i)
    UART0_tx(tx_buffer[i],0);               // resto del mensaje -> wakeup bit = 0

  cash_monto = 0;
  
  // Espera respuesta
  if( !UART0_AFT_rx( sizeof(AFT72Type) ) )
    return AFT_NO_RESPONSE;                 // La VGM no respondió
  
  // Verifica que la respuesta sea válida e indique transferencia pendiente
  if( !valid_AFT_response(0x72) )
    return AFT_BAD_RESPONSE;
  
  if( pr->resp.transferStatus != AFT_TRANSFER_PENDING )
    return pr->resp.transferStatus;

  do {
    delay(pollingPeriod);
    UART0_tx(0x80,1);
    delay(pollingPeriod);

    // Envía la interrogación con 0xFF para completar la transferencia
    AFT_interrogate();
    
  }while(!UART0_AFT_rx(sizeof(AFT72Type)) || !valid_AFT_response(0x72) || pr->resp.transferStatus==AFT_TRANSFER_PENDING);

  if( pr->resp.transferStatus == AFT_FULL_TRANSFER_OK )
    // Obtiene el monto del crédito descargado
    cash_monto = bcd2long(pr->resp.cashableAmount,10);
  else
    cash_monto = 0;
  
  // Retorna el transfer status de la última interrogación
  return pr->resp.transferStatus;
}

#undef pr

//-------------------------------------
// Carga crédito a la VGM mediante EFT
//-------------------------------------

#define pt ((EFTType *)tx_buffer)
#define pr ((EFTType *)rx_buffer)

static unsigned char EFT_cashin(unsigned long monto)
{
  unsigned int i, j;

  tx_buflen = 10;
  
  EFT_transactionNumber++;
  
  // Prepara el envío del primer mensaje, sin ACK
  
  pt->comm69.vgmAddr = vgmAddr;
  pt->comm69.eftCommand = 0x69;
  pt->comm69.transactionNumber = EFT_transactionNumber;
  pt->comm69.ack = 0;
  long2bcd( pt->comm69.transferAmount, monto, 8 );
  i = CRC(tx_buffer, tx_buflen-2, 0);
  pt->comm69.crc[0] = i & 0x00ff;
  pt->comm69.crc[1] = (i >> 8) & 0x00ff;

  UART0_tx(tx_buffer[0],1);
  
  for(i=1;i<tx_buflen;++i)
    UART0_tx(tx_buffer[i],0);

  // Espera la respuesta
  delayTimer = 0;
  while( delayTimer < _2seg && !UART0_AFT_rx(sizeof(EFTType)) );
  
  if( !rx_buflen )
    return EFT_NO_RESPONSE;         // La VGM no respondió al comando EFT

  if( CRC(rx_buffer,rx_buflen,0) )
    return EFT_BAD_RESPONSE;        // La respuesta EFT tiene error de CRC

  if( pr->resp69.status != EFT_SUCCESS )
    return pr->resp69.status;

  // Espera durante el período entre pollings
  delay( pollingPeriod );

  // Prepara el envío del segundo mensaje, ahora con ACK
  pt->comm69.ack = 1;

  i = CRC(tx_buffer, tx_buflen-2, 0);
  pt->comm69.crc[0] = i & 0x00ff;
  pt->comm69.crc[1] = (i >> 8) & 0x00ff;
  
  // Si no recibe respuesta al ACK, intenta el envìo hasta 9 veces
  for(j=0; j<9; ++j)
  {
    UART0_tx(tx_buffer[0],1);
  
    for(i=1;i<tx_buflen;++i)
      UART0_tx(tx_buffer[i],0);
    
    if( UART0_AFT_rx(sizeof(EFTType)) && !CRC(rx_buffer,rx_buflen,0) )
      break;
    else
      delay(pollingPeriod);
  }

  if( j<9 )
  {
    // La VGM respondió al ACK --> retorna el código de status de la operación
    
    return pr->resp69.status;
  }
  
  // La VGM no respondió al ACK o la respuesta es errónea
  if( rx_buflen )
    return EFT_BAD_RESPONSE;  // La respuesta al ACK llegó alterada
  else
    return EFT_NO_RESPONSE;   // No se había obtenido respuesta de la VGM al ACK
}

#undef pr
#undef pt

//-------------------------------------------
// Descarga crédito de la VGM mediante EFT
//-------------------------------------------

#define pt ((EFTType *)tx_buffer)
#define pr ((EFTType *)rx_buffer)

static unsigned char EFT_cashout()
{
  unsigned int i, j, crc;

  cash_monto = 0;

  tx_buflen = 6;
  
  EFT_transactionNumber++;
  
  // Prepara el primer mensaje EFT, sin ACK
  pt->comm64.vgmAddr = vgmAddr;
  pt->comm64.eftCommand = 0x64;
  pt->comm64.transactionNumber = EFT_transactionNumber;
  pt->comm64.ack = 0;
  
  crc = CRC(tx_buffer, tx_buflen-2, 0);
  pt->comm64.crc[0] = crc & 0x00ff;
  pt->comm64.crc[1] = (crc >> 8) & 0x00ff;

  UART0_tx(tx_buffer[0],1);
  
  for(i=1;i<tx_buflen;++i)
    UART0_tx(tx_buffer[i],0);

  delayTimer = 0;
  while( delayTimer < _2seg && !UART0_AFT_rx(sizeof(EFTType)) );

  if( !rx_buflen )
    return EFT_NO_RESPONSE;         // La VGM no respondió al comando EFT

  if( CRC(rx_buffer,rx_buflen,0) )
    return EFT_BAD_RESPONSE;        // La respuesta EFT tiene error de CRC
  
  if( pr->resp64.status != EFT_SUCCESS )
    return pr->resp64.status;

  // Espera durante el período entre pollings
  delay( pollingPeriod );

  // Prepara el segundo mensaje EFT, ahora con ACK
  pt->comm64.ack = 1;

  crc = CRC(tx_buffer, tx_buflen-2, 0);
  pt->comm64.crc[0] = crc & 0x00ff;
  pt->comm64.crc[1] = (crc >> 8) & 0x00ff;
  
  // Si no se recibe respuesta al ACK, se intenta el envío hasta 9 veces
  for(j=0; j<9; ++j)
  {
    UART0_tx(tx_buffer[0],1);
  
    for(i=1; i<tx_buflen; ++i)
      UART0_tx(tx_buffer[i],0);
    
    if( UART0_AFT_rx(sizeof(EFTType)) && !CRC(rx_buffer,rx_buflen,0) )
      break;
    else
      delay(pollingPeriod);
  }

  if( j<9 )
  {
    // La VGM respondió correctamente al ACK --> 
    // obtiene el monto descargado y retorna el código de status de la operación
    cash_monto = bcd2long(pr->resp64.transferAmount, 10);

    return pr->resp64.status;
  }
  
  // La VGM no respondió al ACK o la respuesta es errónea
  if( rx_buflen )
    return EFT_BAD_RESPONSE;  // La respuesta al ACK llegó alterada
  else
    return EFT_NO_RESPONSE;   // No se había obtenido respuesta de la VGM al ACK
}

#undef pt
#undef pr

//-----------------------------------------------
// Inicia una transferencia cashless
//-----------------------------------------------
static void cashin()
{
  // Prepara el mensaje para pedir crédito al servidor.
  cashMsgBuffer.msgCode = CASHIN_REQ;
  memcpy(cashMsgBuffer.ID.cardID, cardID, 32);
  if( Features.teclado==TECLADO_IP || Features.teclado==TECLADO_SERIE )
  {
    // El módulo tiene PC con touch screen --> el vendedor carga el importe manualmente
    cashMsgBuffer.cashinOption = CASHIN_TOUCH;
  }else{
    // Módulo básico --> procedimiento de carga estándar
    cashMsgBuffer.cashinOption = CASHIN_STD;
  }
  cashMsgBuffer.protocol = 0;
  // En el pedido de carga informa el monto acumulado de cargas de un
  // vendedor. Va en cero, excepto que la carga anterior haya sido
  // efectuada por un vendedor.
  sprintf((char *)cashMsgBuffer.monto, "%010lu", montoVendedor);
  cashMsgBuffer.hash = RANDOM();
  cashMsgBuffer.crc = CRC((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer)-2,0);
  cifrar((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer));
  
  // Envía requerimiento al servidor
  if( sendUDP( (unsigned char *) &cashMsgBuffer, sizeof(cashMsgBuffer), SERV_CASH, UDP_LOCAL_PORT_CLI, UDP_PORT_CASH ) )
  {
    waitingCashinReply = 1;
    waitCashReplyTimer = 0;
  }

  // Resetea el flag de requerimiento de cashless
  cash_flag = 0;
}
//-----------------------------------------------------------------
// Realiza una descarga cashless y transfiere el monto al servidor
//-----------------------------------------------------------------
static void cashout()
{
  // Descarga la máquina (y obtiene el código de error a enviar en el mensaje)
  switch(vgmProto)
  {
  case SAS:
  case EIBE:
    if(cashProtocol=='A')
    {
      // Intenta el cashout           
      if(debug)
        sendLogMsg("AFT CASHOUT (INI)",0,0);
      
      cashMsgBuffer.errorCode = AFT_cashout();
      
      if(debug)
        sendLogMsg("AFT CASHOUT (FIN)",cashMsgBuffer.errorCode,cash_monto);
      
      if( cashMsgBuffer.errorCode == AFT_FULL_TRANSFER_OK )
      {
        // Va a enviar el mensaje al servidor
        break;
      }         
      // Puede ser que la descarga no se haya podido concretar por haber
      // un pago manual pendiente que lo impida.
      // Retorna sin hacer nada para permitir verificar los eventos, y en
      // todo caso volverá a intentar la descarga en la próxima vuelta.
      return;
      
    }else if(cashProtocol=='E'){
      // Antes de descargar consulta si la VGM tiene crédito
      if( !getCurrentCredits(&currentCredits) )
      {
        // La VGM no respondió a la consulta de créditos --> aborta
        // la descarga para volver a intentar un tiempo después.
        return;
      } 
      cash_monto = currentCredits;
      if( cash_monto )
      {
        // La VGM tiene crédito --> intenta descargarla
        while(1)
        {
          // Intenta el cashout
          if(debug)
            sendLogMsg("EFT CASHOUT INI",0,0);
          
          cashMsgBuffer.errorCode = EFT_cashout();
          
          if(debug)
            sendLogMsg("EFT CASHOUT FIN",cashMsgBuffer.errorCode,cash_monto);
          
          if(cashMsgBuffer.errorCode != EFT_SUCCESS)
          {
            // falló la descarga, espera un momento antes de reintentar
            delay(pollingPeriod);
            // Envía 0x80 y espera durante el período entre pollings
            UART0_tx(0x80,1);
            delay( pollingPeriod );
          }else
            break;
        }
      }else{
        // La VGM no tiene crédito --> saltea la descarga de la máquina
        // y va directamente a enviar el mensaje al servidor
        cashMsgBuffer.errorCode = EFT_SUCCESS;
      }
    }        
    break;
  case DACOM:
    // Se queda indefinidamente intentando descargar, hasta que lo logre
    do
    {
      if(debug)
        sendLogMsg("DACOM CASHOUT (INI)",0,0);
      
      cashMsgBuffer.errorCode = dacom_cashout(&cash_monto);
      
      if(debug)
        sendLogMsg("DACOM CASHOUT (FIN)",cashMsgBuffer.errorCode,cash_monto);
      
    }while(cashMsgBuffer.errorCode != DACOM_SUCCESS);
    break;
  }
  
  // Prepara el mensaje a enviar al servidor
  cashMsgBuffer.msgCode = CASHOUT_REQ;
  cashMsgBuffer.protocol = cashProtocol;
  
  memcpy(cashMsgBuffer.lp1c, longPollBuffer.lp1c, 32);
  
  // El monto a reportar puede provenir de lo que tuviera la VGM (cash_monto) o
  // de lo que hubiera estado cargando un vendedor (montoVendedor), cuando sale por
  // timeout de espera de tarjeta de cliente.
  if( montoVendedor )
    sprintf((char *)cashMsgBuffer.monto, "%010lu", montoVendedor);
  else
    sprintf((char *)cashMsgBuffer.monto, "%010lu", cash_monto);
  
  cashMsgBuffer.hash = RANDOM();
  cashMsgBuffer.crc = CRC((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer)-2,0);
  cifrar((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer));
  
  // Envía mensaje al servidor, para que acredite el monto descargado
  sendUDP( (unsigned char *)&cashMsgBuffer, sizeof(cashMsgBuffer), SERV_CASH, UDP_LOCAL_PORT_CLI, UDP_PORT_CASH );
  waitingCashoutReply = 1;
  waitCashReplyTimer = 0;
  cashoutMsgReply = 0;
  
  // Por las dudas resetea el monto acumulado
  // de la carga del último vendedor
  montoVendedor = 0;  
  montoVendedorPesos = 0;  
  
  // Resetea el flag de requerimiento de cashless
  cash_flag = 0;
}

//--------------------------------------------------------------------
// Resetea un pago manual pendiente y transfiere el monto al servidor
// Retorna: 1 OK
//          0 Error
//--------------------------------------------------------------------

#define pi ((HandpayInfoType *)rx_buffer)
#define pr ((ResetHandpayResponseType *)rx_buffer)

static unsigned char reset_handpay(void)
{
  unsigned int i, j;
  static unsigned long monto, monto51 = 0;

  // Consulta estado de handpay
  for(j=0; j<3; ++j)
  {
    UART0_tx(vgmAddr, 1);
    UART0_tx(0x1B, 0);

    if( UART0_rx(sizeof(HandpayInfoType)) && !CRC(rx_buffer,rx_buflen,0) )
      break;
  }

  if( j > 2 )
    return 0;     // No hubo respuesta al LP 1B --> aborta

  // Envía 0x80 y espera durante el período entre pollings
  delay( pollingPeriod );
  UART0_tx(0x80,1);
  delay( pollingPeriod );

  // Obtiene el monto a descargar y prepara el mensaje de cashout
  
  // Obs.: como puede suceder que después de haber obtenido el monto a descargar
  // se haya abierto la puerta de la VGM (u otro evento que bloquea el pago manual)
  // y no se haya podido concretar el reset, se debe guardar dicho monto dado que
  // un pedido de info posterior da cero.
  // Por eso, si un pedido de info de pago manual da monto cero, se utiliza el
  // valor que contenga de antemano la variable "monto51".
  
  if( monto = bcd2long(pi->amount, 10) )
    monto51 = monto;
  
  // Envía mensaje para resetear el pago manual
  tx_buflen = 4;
  tx_buffer[0] = vgmAddr;
  tx_buffer[1] = 0x94;
  i = CRC(tx_buffer, tx_buflen-2, 0);
  tx_buffer[2] = i & 0x00ff;
  tx_buffer[3] = (i >> 8) & 0x00ff;
  
  // Si no recibe respuesta al reset handpay, intenta el envìo hasta 3 veces
  for(j=0; j<3; ++j)
  {
    UART0_tx(tx_buffer[0],1);
    
    for(i=1;i<tx_buflen;++i)
      UART0_tx(tx_buffer[i],0);
    
    if( UART0_rx(sizeof(ResetHandpayResponseType)) && !CRC(rx_buffer,rx_buflen,0) )
      break;
  }
  
  // Envía 0x80 y espera durante el período entre pollings
  delay( pollingPeriod );
  UART0_tx(0x80,1);
  delay( pollingPeriod );

  if( j > 2 )
  {
    if( monto )
      return 0;     // No hubo respuesta al reset y todavía hay un monto a descargar -->
                    // aborta para volver a intentar o esperar a que se resetee con la llave
  }else{
    if( pr->resetCode == 1 )
      // No pudo resetear --> aborta para volver a intentar en la próxima vuelta
      return 0;
  }
  
  // El pago manual fue reseteado, ya sea mediante el LP94 o la llave.
  
  // Prepara el mensaje que enviará al servidor
  cashMsgBuffer.msgCode = CASHOUT_HANDPAY_REQ;
  cashMsgBuffer.protocol = cashProtocol;
  memcpy(cashMsgBuffer.lp1c, longPollBuffer.lp1c, 32);
  cashMsgBuffer.errorCode = pi->level;
  sprintf((char *)cashMsgBuffer.monto, "%010lu", monto51);
  cashMsgBuffer.hash = RANDOM();
  cashMsgBuffer.crc = CRC((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer)-2,0);
  cifrar((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer));
      
  // Envía mensaje al servidor, para que acredite el monto descargado
  sendUDP( (unsigned char *)&cashMsgBuffer, sizeof(cashMsgBuffer), SERV_CASH, UDP_LOCAL_PORT_CLI, UDP_PORT_CASH );
  waitingCashoutReply = 1;
  waitCashReplyTimer = 0;
  cashoutMsgReply = 0;
  
  // resetea la variable que contiene el monto descargado
  monto51 = 0;

  // por las dudas resetea el monto acumulado del último vendedor
  montoVendedor = 0;
  montoVendedorPesos = 0;
  
  // resetea el flag de requerimiento de reset de pago manual
  cash_handpay = 0;
  
  return 1;
}

#undef pi
#undef pr

//--------------------------------------------------------
// Obtiene el estado de créditos de la VGM (LP 0x1A)
// Retorna: 0 si no obtuvo respuesta
//          !=0 si OK --> deja en vgm_credits los créditos
//--------------------------------------------------------
static unsigned char getCurrentCredits(unsigned long * vgm_credits)
{
  int np = nPoll;
  unsigned char ret;
  
  switch(vgmProto)
  {
  case SAS:
  case EIBE:
    nPoll = POLL_CREDITS; // fuerza a que el próximo LP sea para consultar créditos a la VGM
    if( ret = polling() )
      *vgm_credits = bcd2long(longPollBuffer.lp1a, 8);
    nPoll = np;           // restaura el indice de LP original

    delay(pollingPeriod);
    UART0_tx(0x80,1);     // envía un 0x80 como ACK implícito a la VGM
    delay(pollingPeriod);

    break;
    
  case DACOM:
    // Espera recibir una trama de tipo 1
    delayTimer = 0;
    do
    {
      ret = dacomProcessing();
    }while( ret != 1 && delayTimer < _5seg );
    
    if( ret==1 )
      *vgm_credits = bcd2long(longPollBuffer.lp1a, 8);
    else
      ret = 0;
    break;
  }
  return ret;
}

//-------------------------------------------------------
// TITO - Carga de créditos a la VGM.
//-------------------------------------------------------

#define pr ((RedeemTicketType *)rx_buffer)
#define pt ((RedeemTicketType *)tx_buffer)

static unsigned char TITO_cashin(unsigned long monto)
{
  int i;
  
  pt->req.vgmAddr = vgmAddr;
  pt->req.command = 0x71;        // redeem ticket
  pt->req.length  = 16;
  pt->req.transfer_code = 0x00;  // valid cashable ticket
  long2bcd(pt->req.transfer_amount, monto, 10);
  pt->req.parsing_code = 0x00;   // 0x00 --> validation number de 18 dígitos
  memcpy(pt->req.validation_data, ticketID, 9);
  pt->req.crc=CRC(tx_buffer,sizeof(pt->req)-2,0);
  
  // transmite 
  UART0_tx(tx_buffer[0], 1);  // 1er byte con wakeup
                              // el resto del mensaje normal
  for(i=1; i<sizeof(pt->req); ++i)
    UART0_tx(tx_buffer[i], 0);
  
  // Espera respuesta
  if( !UART0_rx(sizeof(pr->resp)) )
    return TITO_NO_RESPONSE;                 // La VGM no respondió
  
  // Verifica que la respuesta sea válida e indique transferencia pendiente
  if( CRC(rx_buffer, sizeof(pr->resp), 0) )
    return TITO_BAD_RESPONSE;
  
  if( !(pr->resp.machine_status==TITO_CASHABLE_TICKET_REDEEMED || pr->resp.machine_status==TITO_TICKET_REDEMPTION_PENDING) )
    return pr->resp.machine_status;
  
  do {
    delay(pollingPeriod);
    UART0_tx(0x80,1);
    delay(pollingPeriod);
  
    // Envía la interrogación con 0xFF para completar la transferencia
    pt->cons.vgmAddr = vgmAddr;
    pt->cons.command = 0x71;        // redeem ticket
    pt->cons.length  = 1;
    pt->cons.transfer_code = 0xff;  // valid cashable ticket
    pt->cons.crc=CRC(tx_buffer,sizeof(pt->cons)-2,0);
  
    // transmite 
    UART0_tx(tx_buffer[0], 1);  // 1er byte con wakeup;
                                // el resto del mensaje normal
    for(i=1; i<sizeof(pt->cons); ++i)
      UART0_tx(tx_buffer[i], 0);
    
  }while( !UART0_rx(sizeof(pr->resp)) || CRC(rx_buffer, sizeof(pr->resp), 0) || pr->resp.machine_status==TITO_TICKET_REDEMPTION_PENDING);

  // Retorna el transfer status de la última interrogación
  return pr->resp.machine_status;
}

#undef pr
#undef pt

//-------------------------------------------------------
// TITO - Rechazo a una carga con ticket
//-------------------------------------------------------

#define pr ((RedeemTicketType *)rx_buffer)
#define pt ((RedeemTicketType *)tx_buffer)

static unsigned char TITO_cashin_reject(unsigned char errorCode)
{
  int i;
  
  pt->req.vgmAddr = vgmAddr;
  pt->req.command = 0x71;        // redeem ticket
  pt->req.length  = 16;
  pt->req.transfer_code = errorCode;
  long2bcd(pt->req.transfer_amount, 0L, 10);
  pt->req.parsing_code = 0x00;   // 0x00 --> validation number de 18 dígitos
  memcpy(pt->req.validation_data, ticketID, 9);
  pt->req.crc=CRC(tx_buffer,sizeof(pt->req)-2,0);
  
  // transmite 
  UART0_tx(tx_buffer[0], 1);  // 1er byte con wakeup
                              // el resto del mensaje normal
  for(i=1; i<sizeof(pt->req); ++i)
    UART0_tx(tx_buffer[i], 0);
  
  // Espera respuesta
  if( !UART0_rx(sizeof(pr->resp)) )
    return AFT_NO_RESPONSE;                 // La VGM no respondió
  
  // Verifica que la respuesta sea válida e indique transferencia pendiente
  if( CRC(rx_buffer, sizeof(pr->resp), 0) )
    return AFT_BAD_RESPONSE;
  
  return pr->resp.machine_status;
}

#undef pr
#undef pt

//-------------------------------------------------------
// TITO - Procesamiento de un ticket insertado en la VGM.
// Se obtiene el ID del ticket y se pide al servidor
// que lo valide y envíe el monto a cargar
//-------------------------------------------------------

#define pr ((TicketValidationDataType *)rx_buffer)
#define pt ((RedeemTicketType *)tx_buffer)

static void ticket_in()
{
  int j;
  
  // Si no recibe respuesta al LP 70, intenta el envío hasta 3 veces
  for(j=0; j<3; ++j)
  {
    UART0_tx(0x01,1);
    UART0_tx(0x70,0);
    
    if( UART0_rx(sizeof(TicketValidationDataType)) && !CRC(rx_buffer,rx_buflen,0) )
      break;
  }
  
  if( j > 2 )
    // No tuvo respuesta --> retorna para intentar más tarde
    return;
      
  // Prepara el mensaje para validar el ticket y pedir crédito al servidor.
  cashMsgBuffer.msgCode = CASHIN_TICKET_REQ;
  cashMsgBuffer.cashinOption = CASHIN_STD;
  cashMsgBuffer.protocol = 'T';
  memcpy(cashMsgBuffer.ticket.validation_number, pr->validation_data, 9);
  
  // Guarda el código de validación del ticket, para verificar la respuesta
  // al CASHIN_TICKET_REQ, enviada por el servidor.
  memcpy(ticketID, pr->validation_data, 9);
  
  sprintf((char *)cashMsgBuffer.monto, "%010lu", 0L);
  cashMsgBuffer.hash = RANDOM();
  cashMsgBuffer.crc = CRC((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer)-2,0);
  cifrar((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer));
  
  // Envía requerimiento al servidor
  if( sendUDP( (unsigned char *) &cashMsgBuffer, sizeof(cashMsgBuffer), SERV_CASH, UDP_LOCAL_PORT_CLI, UDP_PORT_CASH ) )
  {
    waitingCashinReply = 1;
    waitCashReplyTimer = 0;
  }

  // Resetea el flag de requerimiento de TITO
  ticket_flag = 0;
}

#undef pr
#undef pt

//-----------------------------------------------------
// TITO - Procesamiento de un ticket emitido por la VGM
// Se envían los datos del ticket al servidor
//-----------------------------------------------------

#define pr ((EnhancedValidationInfoType *)rx_buffer)

static void ticket_out()
{
  unsigned int i, j;
  
  // Envía un LP 4D para obtener los datos y monto del ticket
  tx_buflen = 5;
  tx_buffer[0] = vgmAddr;
  tx_buffer[1] = 0x4D;
  tx_buffer[2] = 0x00;
  i = CRC(tx_buffer, tx_buflen-2, 0);
  tx_buffer[3] = i & 0x00ff;
  tx_buffer[4] = (i >> 8) & 0x00ff;
  
  // Si no recibe respuesta al LP 4D, intenta el envìo hasta 3 veces
  for(j=0; j<3; ++j)
  {
    UART0_tx(tx_buffer[0],1);
    
    for(i=1;i<tx_buflen;++i)
      UART0_tx(tx_buffer[i],0);
    
    if( UART0_rx(sizeof(EnhancedValidationInfoType)) && !CRC(rx_buffer,rx_buflen,0) )
      break;
  }
  
  // Envía 0x80 y espera durante el período entre pollings
  delay( pollingPeriod );
  UART0_tx(0x80,1);
  delay( pollingPeriod );
  
  if( j > 2 )
    // No tuvo respuesta --> retorna para intentar más tarde
    return;
  
  // Prepara el mensaje que enviará al servidor
  cashMsgBuffer.msgCode = CASHOUT_TICKET_REQ;
  cashMsgBuffer.errorCode = 0;
  cashMsgBuffer.protocol = 'T';
  memcpy(cashMsgBuffer.ticket.date, pr->date, 4);
  memcpy(cashMsgBuffer.ticket.time, pr->time, 3);
  cashMsgBuffer.ticket.validation_number[0] = pr->validation_system_id;
  memcpy(cashMsgBuffer.ticket.validation_number + 1, pr->validation_number, 8);
  cashMsgBuffer.ticket.ticket_number = pr->ticket_number;
  memcpy(cashMsgBuffer.ticket.expiration, pr->expiration, 4);
  
  sprintf((char *)cashMsgBuffer.monto, "%010lu", bcd2long(pr->amount,10));
  cashMsgBuffer.hash = RANDOM();
  cashMsgBuffer.crc = CRC((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer)-2,0);
  cifrar((unsigned char *)&cashMsgBuffer,sizeof(cashMsgBuffer));
  
  // Envía mensaje al servidor, para que acredite el monto del ticket emitido
  sendUDP( (unsigned char *)&cashMsgBuffer, sizeof(cashMsgBuffer), SERV_CASH, UDP_LOCAL_PORT_CLI, UDP_PORT_CASH );

  waitingCashoutReply = 1;
  waitCashReplyTimer = 0;
  cashoutMsgReply = 0;
  
  // Resetea el flag de requerimiento de TITO
  ticket_flag = 0;
}

#undef pr

//--------------------------------------------------------------
// TITO - Setea validation ID para el modo de validación segura
//--------------------------------------------------------------

#define pt ((SetSecureEnhancedValidationIDType *)tx_buffer)
#define pr ((SetSecureEnhancedValidationIDType *)rx_buffer)

static void set_validation_id()
{
  unsigned int i, j;
  
  // Envía un LP 4C para setear el validation ID
  // que necesita la validación enhanced
  
  tx_buflen = sizeof(SetSecureEnhancedValidationIDType);
  
  pt->vgmAddr = 0x01;
  pt->command = 0x4C;

  pt->machine_id[0] = RANDOM();
  pt->machine_id[1] = MyIP[0];
  pt->machine_id[2] = MyIP[1];
  
  pt->sequence_number[0] = RANDOM();
  pt->sequence_number[1] = MyIP[0];
  pt->sequence_number[2] = MyIP[1];

  pt->crc = CRC((unsigned char *)pt, sizeof(SetSecureEnhancedValidationIDType)-2, 0);
  
  // Si no recibe respuesta al LP 4C, intenta el envìo hasta 3 veces
  for(j=0; j<3; ++j)
  {
    UART0_tx(tx_buffer[0],1);
    
    for(i=1;i<tx_buflen;++i)
      UART0_tx(tx_buffer[i],0);
    
    if( UART0_rx(sizeof(SetSecureEnhancedValidationIDType)) && !CRC(rx_buffer,rx_buflen,0) )
      break;
  }
  
  // Envía 0x80 y espera durante el período entre pollings
  delay( pollingPeriod );
  UART0_tx(0x80,1);
  delay( pollingPeriod );

  // Resetea el flag de requerimiento de TITO
  ticket_flag = 0;
}

#undef pt
#undef pr
