// cashless.h : archivo de encabezado con constantes, tipos y prototipos de cashless.c

// Constantes que indican el tipo de hardware de la placa
#define HW_OLIMEX 1
#define HW_BB2    2

// Constantes que definen los protocolos que maneja la VGM

#define SAS   0
#define DACOM 1     // Aristocrat australiana
#define GAMMA 2     // Aristocrat americana
#define EIBE  3     // Máquinas de Bingo EIBE (Méxicanas-Brasileras-Españolas)

// máscaras para lectura de pulsadores y jumpers, y manejo del led de test

#define BOTON_TEST 0x01   // P6.0
#define LED_TEST   0x08   // P6.3
#define SELE_PROTO 0x06   // P6.2 y P6.1
#define BUZZER     0x40   // P1.6         (se activa con "0")

// macros para activar y desactivar la emisión de un tono en el buzzer
#define BUZZER_ON()  (P1OUT &= ~BUZZER)    // emite sonido
#define BUZZER_OFF() (P1OUT |= BUZZER)     // silencio
#define BEEP(time)   {BUZZER_ON();delay(time);BUZZER_OFF();}

// macros para encender y apagar el led de test
#define TESTLED_ON()  (P6OUT |= LED_TEST)     // enciende
#define TESTLED_OFF() (P6OUT &= ~LED_TEST)    // apaga

// macro para testear si el pulsador de test está presionado
#define TEST_PRESSED()  (~P6IN & BOTON_TEST)   // retorna true si está presionado

// macro para determinar si el módulo tiene PC y touch screen
// P1.5 a "1" indica que hay PC-touch screen
//#define TOUCH_SCREEN()  (P1IN & 0x20)

// Inicia el watchdog timer: ACLK/32768=61 ms
#define startWatchdog() (WDTCTL=WDTPW+WDTSSEL+WDTCNTCL)

// Detiene el watchdog timer
#define stopWatchdog()  (WDTCTL=WDTPW+WDTHOLD)

// Resetea el watchdog timer
#define resetWatchdog() (WDTCTL=WDTPW+WDTCNTCL)

// verifica si el watchdog timer está habilitado
#define watchdogEnabled() (!(WDTCTL & WDTHOLD))

// Constantes que definen tiempos (en base a los 10 ms del timer)

#define _10ms   1
#define _20ms   2
#define _30ms   3
#define _40ms   4
#define _50ms   5
#define _100ms  10
#define _110ms  11
#define _120ms  12
#define _150ms  15
#define _200ms  20
#define _210ms  21
#define _250ms  25
#define _300ms  30
#define _400ms  40
#define _500ms  50
#define _800ms  80
#define _1seg   100
#define _1500ms 150
#define _2seg   200
#define _3seg   300
#define _4seg   400
#define _5seg   500
#define _10seg  1000
#define _15seg  1500
#define _20seg  2000
#define _30seg  3000
#define _1min   6000
#define _2min   12000

// constantes para identificar el tipo de poll a realizar

#define POLL_SINC1            0   // Solo envia el 0x80 para sincronizar
#define POLL_GP               1
#define POLL_SINC2            2   // Solo envia el 0x80 para sincronizar
#define POLL_LP               3
#define POLL_CASH             4
#define POLL_SINC3            5   // Solo envia el 0x80 para sincronizar
#define POLL_LP1A             6   // Consulta de créditos posterior a una operación cashless
#define POLL_SINC4            7   // Solo envia el 0x80 para sincronizar
#define POLL_LP1C             8   // Consulta coin in, coin out, jackpot, games won y games played, previo a un cashout
#define POLL_TICKET           9   // Procesa TITO

// Constantes para identificar variantes de la operación de cashless a realizar,
// en la variable cash_flag
#define DO_CASHIN                1   // Ordena hacer un cashin
#define DO_CASHOUT_STD           1   // Ordena hacer un cashout estándar
#define DO_CASHOUT_BUTTON_EIBE   2   // Ordena hacer un cashout por botón de pago en una EIBE
#define DO_HANDPAY_RESET         3   // Ordena resetear el pago manual, por boton de pago presionado

// Constantes para identificar la operación de TITO, en la variable ticket_flag
#define DO_TICKET_IN             1   // Procesar un ticket ingresado en la VGM
#define DO_TICKET_OUT            2   // Enviar al servidor la info del ticket emitido
#define DO_SET_VALIDATION_ID     3   // Configurar el validation ID para la máquina

// Estructura para almacenar los comandos de los long polls, la longitud de sus respuestas
// y el offset para ubicar las respuestas dentro del buffer

typedef struct
{
  unsigned char lpCommand;  // código del comando
  unsigned char size;       // cantidad de bytes de la respuesta (sin dirección, comando ni CRC)
  unsigned char offset;     // posición inicial dentro del buffer de transmisión
}LongPollType;

// Estructura del mensaje en que se se reportan los resultados de los LONG POLLS
// Si alguno de los LP no respondió, el arreglo correspondiente se envía lleno con 0xFF,
// excepto lp1f y lp54, que se llenan con 0x00

typedef struct
{
  char msgID[2];           // Id. del mensaje de Long Polls --> "LP"
  unsigned char lp10[4];   // Jurisdictional Cancelled Credit Meter
  unsigned char lp17[4];   // games lost
  unsigned char lp18[4];   // games since last power up - slot door closures
  unsigned char lp1a[4];   // current credits
  unsigned char lp1c[32];  // coin in - coin out - drop - jackpot - games played - games won - door open - pwr reset
  unsigned char lp1f[20];  // game ID - additional ID - denomination - max bet - prog group - game options - paytable ID - base percentage
  unsigned char lp2a[4];   // true coin in
  unsigned char lp2b[4];   // true coun out
  unsigned char lp31[4];   // $1 bills
  unsigned char lp32[4];   // $2 bills
  unsigned char lp33[4];   // $5 bills
  unsigned char lp34[4];   // $10 bills
  unsigned char lp35[4];   // $20 bills
  unsigned char lp36[4];   // $50 bills
  unsigned char lp37[4];   // $100 bills
  unsigned char lp54[20];  // data lenght (1 byte) - SAS versión (3 bytes ASCII) - serial number (ASCII)
  unsigned int hash;
  unsigned int crc;
}LongPollMsgType;

#define TOTAL_POLLS 16     // Cantidad total de comandos de Long Poll a efectuar

#define POLL_CREDITS 3     // Indice al comando del LP 1A
#define POLL_COIN_IN 4     // Indice al comando del LP 1C

// Eventos

#define MAX_EVENTOS 32     // cantidad máxima de eventos por mensaje

#define EVT_MAIN_DOOR_OPEN             0x11
#define EVT_MAIN_DOOR_CLOSED           0x12
#define EVT_CARD_CAGE_OPEN             0x15
#define EVT_CARD_CAGE_CLOSED           0x16
#define EVT_CASH_DOOR_OPEN             0x19
#define EVT_CASH_DOOR_CLOSED           0x1A
#define EVT_WAITING_USER_INPUT         0x1F
#define EVT_COIN_IN_TILT               0x21
#define EVT_BILL_ACCEPTOR_HW_FAILURE   0x29
#define EVT_BILL_REJECTED              0x2B
#define EVT_CMOS_RAM_BAD_DEVICE        0x33
#define EVT_MEMORY_ERROR_RESET         0x3A
#define EVT_POWER_SAVE_ERROR           0x3B
#define EVT_CASHOUT_TICKET_PRINTED     0x3D
#define EVT_VALIDATION_ID_NOT_CONFIGURED 0x3F
#define EVT_1_BILL_ACCEPTED            0x47
#define EVT_5_BILL_ACCEPTED            0x48
#define EVT_10_BILL_ACCEPTED           0x49
#define EVT_20_BILL_ACCEPTED           0x4A
#define EVT_50_BILL_ACCEPTED           0x4B
#define EVT_100_BILL_ACCEPTED          0x4C
#define EVT_2_BILL_ACCEPTED            0x4D
#define EVT_500_BILL_ACCEPTED          0x4E
#define EVT_BILL_ACCEPTED              0x4F
#define EVT_200_BILL_ACCEPTED          0x50
#define EVT_HANDPAY_PENDING            0x51
#define EVT_HANDPAY_RESET              0x52
#define EVT_CASHOUT_BUTTON_PRESSED     0x66
#define EVT_VOUCHER_INSERTED           0x67
#define EVT_VOUCHER_TRANSFER_COMPLETE  0x68
#define EVT_AFT_TRANSFER_COMPLETE      0x69
#define EVT_AFT_REQ_4_HOST_CASHOUT     0x6A
#define EVT_GAME_LOCKED                0x6F
#define EVT_GAME_STARTED               0x7E
#define EVT_GAME_ENDED                 0x7F
#define EVT_HOPPER_LEVEL_LOW           0x81
#define EVT_ATTENDANT_MENU_ENTER       0x82
#define EVT_ATTENDANT_MENU_EXIT        0x83
#define EVT_SELF_TEST_MENU_ENTER       0x84
#define EVT_SELF_TEST_MENU_EXIT        0x85
#define EVT_GAME_SELECTED              0x8C

// Estructura del mensaje de eventos

#pragma pack(1)

typedef struct
{
  char msgID[2];           // Id. del mensaje de Eventos --> "EV"
  unsigned char nSec;      // Nro de secuencia del mensaje
  unsigned char nEventos;  // Cantidad de eventos contenidos en el mensaje
  unsigned char evento[MAX_EVENTOS];   // arreglo para almacenar los eventos obtenidos de la máquina
  char fechaHora[14];      // fecha y hora en ASCII: AAAAMMDDhhmmss
  unsigned int hash;
  unsigned int crc;
}EventMsgType;

// Estructura del ACK de eventos

typedef struct
{
  char msgID[2];           // Id. del mensaje de Long Polls --> "LP"
  unsigned char nSec;      // Nro de secuencia del mensaje
  unsigned char codError;  // Código de error (0=OK, 1=nSec repetido)
  unsigned int hash;
  unsigned int crc;
}EventAckType;

#pragma pack()

// Estructura del mensaje de Hello (Echo Request), que el módulo usa para detecta
// la presencia del servidor e informarle la versión de firmware

typedef struct
{
  char hello[6];           // Id. del mensaje --> "HELLO"
  char version[20];        // Hasta 19 ASCII + null
  unsigned int hash;
  unsigned int crc;
}HelloMsgType;

// Estructura del mensaje de Echo Reply, que transporta fecha y hora, y el monto
// de carga unitaria configurado para los vendedores
// 

typedef struct
{
  char msgID[2];              // Id. del mensaje de Fecha y Hora --> "FH"
  char anio[4];               // Año: 4 dígitos ASCII
  char mes[2];                // Mes: 2 dígitos ASCII ("01" al "12")
  char dia[2];                // Día: 2 dígitos ASCII ("01" al "31")
  char hora[2];               // Hora: 2 dígitos ASCII ("00" al "23")
  char min[2];                // Minutos: 2 dígitos ASCII ("00" al "59")
  char seg[2];                // Segundos: 2 dígitos ASCII ("00" al "59")
  char cargaUnitVendedor[10]; // Hasta 9 dígitos en ASCII + null
  struct
  {
    unsigned char msgType;   // tipo de mensaje
    unsigned char msgOption; // opciones del mensaje
    char txt[4][17];         // 4 renglones de texto ASCII + null
  }msg;
  unsigned int hash;
  unsigned int crc;
}HelloReplyMsgType;

// Estructura para la fecha y hora

typedef struct
{
  unsigned short anio;
  unsigned char mes;
  unsigned char dia;
  unsigned char hora;
  unsigned char min;
  unsigned char seg;
  unsigned short ticks;    // contador de "ticks". Se incrementa cada 10 ms por la ISR
}FechaHoraType;

// constantes usadas en distintos campos de los mensajes AFT

// transfer code
#define AFT_FULL_TRANSFER_REQ    0x00
#define AFT_PARTIAL_TRANSFER_REQ 0x01
#define AFT_CANCEL_TRANSFER_REQ  0x80
#define AFT_COMPLETE_TRANSFER    0xFF
// transfer type
#define AFT_CASHIN  0x00
#define AFT_CASHOUT 0x80
// transfer flags
#define AFT_HOST_CASHOUT_ENABLE_CTL   0x01
#define AFT_HOST_CASHOUT_ENABLE       0x02
#define AFT_HOST_CASHOUT_MODE_HARD    0x04
#define AFT_CASHOUT_FROM_VGM_REQUEST  0x08
// Transfer status --------------------------------
// Los 3 MSB indican grupos de códigos
#define AFT_TRANSFER_STATUS_MASK          0xE0  // Máscara para ver sólo los 3 MSB
// 000x xxxx OK
#define AFT_TRANSFER_OK                   0x00  // Representa a todo el grupo 000x xxxx
#define AFT_FULL_TRANSFER_OK              0x00
#define AFT_PARTIAL_TRANSFER_OK           0x01
// 010x xxxx pendiente
#define AFT_TRANSFER_PENDING              0x40  // Único código del grupo 010x xxxx
// 100x xxxx transferencia fallida
#define AFT_TRANSFER_FAILED               0x80  // Representa a todo el grupo 100x xxxx
#define AFT_TRANSFER_CANCELLED_BY_HOST    0x80
#define AFT_TRANSACTION_ID_NOT_UNIQUE     0x81
#define AFT_INVALID_TRANSFER_FUNCTION     0x82
#define AFT_INVALID_TRANSFER_AMOUNT       0x83
#define AFT_TRANSFER_AMOUNT_EXCEED_LIMIT  0x84
#define AFT_TRANSFER_AMOUNT_NOT_EVEN_MULTIPLE 0x85
#define AFT_UNABLE_TO_PERFORM_TRANSFER    0x87
#define AFT_ASSET_NUMBER_MISMATCH         0x93
#define AFT_INVALID_TRANSACTION_ID        0x95
#define AFT_UNEXPECTED_ERROR              0x9F
// 110x xxxx transacción incompatible o no soportada
#define AFT_NOT_COMPATIBLE_WITH_IN_PROGRESS 0xC0
#define AFT_UNSSUPORTED_TRANSFER_CODE       0xC1
// 111x xxxx no hay información disponible
#define AFT_NO_TRANSFER_INFO_AVAILABLE    0xFF  
  // El siguiente no es un código de estado AFT, pero se lo define para
  // reportar que no se recibe respuesta a un comando AFT
#define AFT_NO_RESPONSE                   0xE1  // Clasificado como "no hay info disponible"
  // Idem para reportar una respuesta errónea a un comando AFT
#define AFT_BAD_RESPONSE                  0xE2  // Clasificado como "no hay info disponible"
  // Idem para reportar error en AFT Lock
#define AFT_LOCK_ERROR                    0x8F  // Clasificado como "transferencia fallida"

// Eventos AFT
#define AFT_EVENT_TRANSFER_COMPLETE       0x69

// Union que describe los distintos mensajes AFT 72

typedef union
{
  // Estructura del mensaje de AFT (LP 72) usado para subir o bajar centavos
  struct
  {
    unsigned char vgmAddr;                // 0x01
    unsigned char commandCode;            // 0x72
    unsigned char length;                 // 0x35 (53)
    unsigned char transferCode;           // 0x00 Full transfer request  (0x80 cancel transfer request)
    unsigned char transactionIndex;       // 0x00
    unsigned char transferType;           // 0x00 para subir centavos a la máquina, 0x80 para bajar
    unsigned char cashableAmount[5];      // 5 bytes (10 dígitos BCD), cantidad de centavos a transferir
    unsigned char restrictedAmount[5];    // 5 bytes (10 dígitos BCD), todo en cero
    unsigned char nonRestrictedAmount[5]; // 5 bytes (10 dígitos BCD), todo en cero
    unsigned char transferFlags;          // 0x06
    unsigned char assetNumber[4];         // 0x00001234 o 1234 (ver)
    unsigned char registrationKey[20];    // todos ceros
    unsigned char transactionIDLength;    // 0x01
    unsigned char transactionID;          // valores variables entre '1' y '9'
    unsigned char expiration[4];          // todos ceros
    unsigned char poolID[2];              // ceros
    unsigned char receiptDataLength;      // 0x00
    unsigned char crc[2];
  }init;
  
  // Estructura del mensaje AFT para interrogar y/o finalizar una transacción
  struct
  {
    unsigned char vgmAddr;                // 0x01
    unsigned char commandCode;            // 0x72
    unsigned char length;                 // 2
    unsigned char transferCode;           // 0xFF 
    unsigned char transactionIndex;       // 0x00 (la última transacción)
    unsigned char crc[2];
  }interrogate;

  // Estructura de la respuesta AFT
  struct
  {
    unsigned char vgmAddr;                // 0x01
    unsigned char commandCode;            // 0x72
    unsigned char length;                 // 
    unsigned char transactionBufferPosition;  // 0x00 actual o más reciente
    unsigned char transferStatus;         // 
    unsigned char receiptStatus;          // 
    unsigned char transferType;           // 0x00 para subir centavos a la máquina, 0x80 para bajar
    unsigned char cashableAmount[5];      // 5 bytes (10 dígitos BCD), cantidad de centavos a transferir
    unsigned char restrictedAmount[5];    // 5 bytes (10 dígitos BCD), todo en cero
    unsigned char nonRestrictedAmount[5]; // 5 bytes (10 dígitos BCD), todo en cero
    unsigned char transferFlags;          // 0x06
    unsigned char assetNumber[4];         // 0x00001234 o 1234 (ver)
    unsigned char transactionIDLength;    // 0x01
    unsigned char transactionID;          // valores variables entre 0x01 y 0xFF
    unsigned char transactionDate[4];     // 8 dígitos BCD: MMDDAAAA
    unsigned char transactionTime[3];     // 6 dígitos BCD: hhmmss en formato 24 hs
    unsigned char expiration[4];          // todos ceros
    unsigned char poolID[2];              // ceros
    unsigned char stuff[32];  // relleno --> ojo: ver
  }resp;
}AFT72Type;

// Opciones de comando para LP 74 (AFT status y lock request)
#define AFT_LOCK_REQUEST   0x00
#define AFT_LOCK_CANCEL    0x80
#define AFT_STATUS_REQUEST 0xff
// Opciones de estado de bloqueo para la respuesta al LP 74
#define AFT_LOCKED         0x00
#define AFT_LOCK_PENDING   0x40
#define AFT_NOT_LOCKED     0xff

typedef union
{
  // Estructura del mensaje de AFT (LP 74) usado para bloquear y consultar estado de AFT
  struct
  {
    unsigned char vgmAddr;                // 0x01
    unsigned char commandCode;            // 0x74
    unsigned char lockCode;               // 0xff (interrogate current status)
    unsigned char transferCondition;      // 0x00 (no importa)
    unsigned char lockTimeout[2];         // todos ceros (no importa)
    unsigned char crc[2];
  }req;

  // Estructura de la respuesta de estado de AFT
  struct
  {
    unsigned char vgmAddr;                // 0x01
    unsigned char commandCode;            // 0x74
    unsigned char length;                 // 23
    unsigned char assetNumber[4];         // 0x00001234 o 1234 (ver)
    unsigned char gameLockStatus;         // 0xff
    unsigned char availableTransfers;     // 
    unsigned char hostCashoutStatus;      // 
    unsigned char aftStatus;              // 
    unsigned char maxBufferIndex;         // 0x46 - 0x7f
    unsigned char cashableAmount[5];      // 5 bytes (10 dígitos BCD), en centavos
    unsigned char restrictedAmount[5];    // 5 bytes (10 dígitos BCD), en centavos
    unsigned char nonRestrictedAmount[5]; // 5 bytes (10 dígitos BCD), en centavos
    unsigned char transferLimit[5];       // 5 bytes (10 dígitos BCD), en centavos
    unsigned char restrictedExpiration[4];
    unsigned char restrictedPoolID[2];
    unsigned char crc[2];
  }resp;
}AFT74Type;

//----------------------------------------------------------
// Códigos de transfer status de EFT, reportados por la VGM
//----------------------------------------------------------

#define EFT_SUCCESS                     0x00 // Operation successfully completed
#define EFT_DOOR_OPEN                   0x02 // Gaming machine door open
#define EFT_EXCEEDED_AMOUNT             0x04 // Transfer amount exceeded gaming machine credit limit
#define EFT_INVALID_TRANSACTION_NUMBER  0x05 // Invalid transaction number
#define EFT_NO_CREDIT_SWITCH            0x06 // Gaming machine does not have a credit switch
#define EFT_VGM_TILT                    0x07 // Gaming machine is in a tilt condition
#define EFT_INVALID_ACK                 0x08 // Invalid ACK message received
#define EFT_VGM_PLAYING                 0x09 // Gaming machine is in game play mode
#define EFT_NON_BCD_DATA                0x0A // Data field contains non-BCD data
#define EFT_TRANSFER_ALREADY_COMPLETED  0x0B // Host transfer request has already been completed
#define EFT_VGM_DISABLED                0x0C // Gaming machine disabled
#define EFT_VGM_OUT_OF_SERVICE          0x0D // Gaming machine out of service
#define EFT_VGM_BUSY                    0x0E // Gaming machine busy

  // El siguiente no es un código de estado EFT, pero se lo define para
  // reportar que no se recibe respuesta a un comando EFT
#define EFT_NO_RESPONSE                 AFT_NO_RESPONSE
  // Idem para reportar una respuesta errónea a un comando EFT
#define EFT_BAD_RESPONSE                AFT_BAD_RESPONSE
  // Idem para reportar una transacción abortada por el host (por
  // ejemplo, por inconsistencia en el mensaje de ACK del servidor)
#define EFT_TRANSFER_CANCELLED_BY_HOST  AFT_TRANSFER_CANCELLED_BY_HOST

// Union que describe los distintos mensajes EFT

typedef union
{
  struct
  {
    unsigned char vgmAddr;
    unsigned char eftCommand;         // 0x69
    unsigned char transactionNumber;
    unsigned char ack;
    unsigned char transferAmount[4];
    unsigned char crc[2];    
  }comm69;

  struct
  {
    unsigned char vgmAddr;
    unsigned char eftCommand;         // 0x69
    unsigned char transactionNumber;
    unsigned char ack;
    unsigned char status;
    unsigned char transferAmount[4];
    unsigned char crc[2];    
  }resp69;

  struct
  {
    unsigned char vgmAddr;
    unsigned char eftCommand;         // 0x64
    unsigned char transactionNumber;
    unsigned char ack;
    unsigned char crc[2];    
  }comm64;

  struct
  {
    unsigned char vgmAddr;
    unsigned char eftCommand;         // 0x64
    unsigned char transactionNumber;
    unsigned char ack;
    unsigned char status;
    unsigned char transferAmount[5];
    unsigned char crc[2];    
  }resp64;
}EFTType;

// códigos para los mensajes UDP de transacciones cashless

#define CASHIN_REQ            0 // MOD->SERV - inicia un pedido de cash - envía el ID de la tarjeta
#define CASHIN_REP_CLI_OK     1 // SERV->MOD - respuesta afirmativa a un pedido de cash de un cliente
#define CASHIN_REP_VEND_OK    2 // SERV->MOD - respuesta afirmativa a un pedido de cash de un vendedor
#define CASHIN_REP_VEND_MAX  22 // SERV->MOD - el servidor informa el máximo que puede cargar el vendedor
#define CASHIN_REP_REJ        3 // SERV->MOD - respuesta negativa a un pedido de cash
#define CASHIN_ACK            4 // MOD->SERV - confirmación de la transacción
#define CASHIN_NAK            5 // MOD->SERV - rechazo de la transacción

#define CASHOUT_REQ       10    // MOD->SERV - inicia una descarga de cash - envía el monto
#define CASHOUT_REP_OK    11    // SERV->MOD - respuesta afirmativa a una descarga de cash
#define CASHOUT_REP_REJ   12    // SERV->MOD - respuesta negativa a una descarga de cash
#define CASHOUT_ACK       13    // MOD->SERV - confirmación de la transacción
#define CASHOUT_NAK       14    // MOD->SERV - rechazo de la transacción

#define CASHOUT_HANDPAY_REQ 20  // MOD->SERV - inicia una descarga de cash
                                //             originada por un pago manual
                                //             envía el monto en créditos

#define CASHIN_TICKET_REQ   30  // MOD->SERV - envía al servidor los datos de un ticket ingresado,
                                //             para que el servidor lo valide y envíe el monto
                                //             a cargar a la VGM.
#define CASHOUT_TICKET_REQ  40  // MOD->SERV - envía al servidor el monto y los datos de un pago con ticket

// Códigos de error reportados por el servidor luego de un pedido de CASHIN o CASHOUT
#define CASH_REQ_OK                  0
#define CASH_TARJETA_INVALIDA        1
#define CASH_PIN_INVALIDO            2
#define CASH_REINGRESAR_PIN          3
#define CASH_TARJETA_BLOQUEADA       4
#define CASH_VENDEDOR_SIN_FONDOS     5
#define CASH_ASOCIADA_A_OTRO_SLOT    6
#define CASH_NO_HAY_TARJETA_ASOCIADA 7  // Cuando en una descarga la VGM no tiene una tarjeta asociada
#define CASH_MONTO_INVALIDO          8
#define CASH_MONTO_EXCESIVO          9  // Monto ingresado manualmente por el vendedor

// Códigos para cashinOption (cuando se envía un mensaje CASHIN_REQ)

#define CASHIN_STD    0   // Módulo básico --> procedimiento de carga estándar

#define CASHIN_TOUCH  1   // La VGM tiene PC con touch para que el vendedor
                          // ingrese manualmente el monto a cargar. Si la tarjeta es de un
                          // vendedor, el servidor responde con CASHIN_REP_VEND_MAX y el monto
                          // máximo que puede cargar.
                          // Si la tarjeta es de un cliente, el servidor responde con
                          // CASHIN_REP_CLI_OK.

#define CASHIN_VEND   2   // El vendedor ingresó manualmente el monto a cargar, y va indicado en
                          // el campo "monto".

// Estructura para los mensajes de cashless entre el módulo y el servidor

#pragma pack(1)

typedef struct
{
  unsigned char msgCode;

  // El 2do byte tiene distinto significado según el momento en que se lo usa
  union
  {
    // En CASHOUT_REQ, el módulo reporta en error code el transfer status de AFT, EFT o DACOM.
    // El servidor usa error code para reportar la causa de rechazo a un pedido
    // En 0 significa que no hay error
    unsigned char errorCode;
    
    // En CASHIN_REQ el módulo indica la opción requerida para la carga
    // 0 : pedido de carga estándar
    // 1 : si la tarjeta es de cliente --> carga estándar
    //     si la tarjeta es de vendedor --> consulta el crédito disponible del vendedor, para
    //                                      efectuar el ingreso manual desde el touch screen
    // 2 : pedido de carga de un vendedor --> en el monto se indica el monto a cargar,
    //                                        ingresado manualmente por el vendedor
    unsigned char cashinOption;
  };
  
  unsigned char protocol;     // Usado en CASHIN_REP_CLI_OK o CASHIN_REP_VEND_OK, para que el servidor
                              // indique el protocolo cashless a usar: 'E': EFT, 'A': AFT, 'D': DACOM
                              // 'T': TITO

  // El 4to campo tiene distinto significado según el momento en que se lo usa
  union
  {
    struct
    {
      unsigned char cardID[32]; // 31 ASCII ID de la tarjeta + NULL
                                // En los mensajes de CASHIN_... va el ID de
                                // la tarjeta
      char pin[10];             // PIN ingresado por el usuario, ASCII + NULL
                                // Se envía en el mensaje de CASHIN_REQ
    }ID;
    
    struct
    {
      unsigned char date[4];              // BCD - MMDDYYYY
      unsigned char time[3];              // BCD - HHMMSS
      unsigned char validation_number[9]; // BCD - 
      unsigned int  ticket_number;        // binario 0 a 9999
      unsigned char expiration[4];        // BCD - MMDDYYYY o número de días (1 a 9999)
    }ticket;

    unsigned char lp1c[32];   // coin in - coin out - drop - jackpot - games played - games won - door open - pwr reset
                              // En los mensajes de CASHOUT_... van los valores del LP1C,
                              // que el servidor usa para estadísticas de juego

    char msg[4][17];          // 4 renglones de 16 ASCII + null
                              // El servidor lo envía en la respuesta
                              // de cashout, para que el módulo lo presente
                              // en el display
  };
  unsigned char monto[11];    // 10 dígitos en ASCII + NULL
  unsigned int  hash;
  unsigned int  crc;
}CashMsgType;

#pragma pack()

// Estructura del mensaje usado para obtener
// información de un pago manual pendiente

typedef struct
{
  unsigned char vgmAddr;
  unsigned char command;         // 0x1B
  unsigned char progGroup;
  unsigned char level;
  unsigned char amount[5];
  unsigned char partialPay[2];
  unsigned char resetID;
  unsigned char reserved[10];
  unsigned char crc[2];    
}HandpayInfoType;

// Estructura del mensaje de respuesta a un reset de pago manual

typedef struct
{
  unsigned char vgmAddr;
  unsigned char command;         // 0x1B
  unsigned char resetCode;
  unsigned char crc[2];    
}ResetHandpayResponseType;

//--- Formatos de LP usados en TITO

// TITO - Send Enhanced Validation Info (Respuesta a un LP 4D)

#pragma pack(1)

typedef struct
{
  unsigned char vgmAddr;
  unsigned char command;         // 0x4D
  unsigned char validation_type;
  unsigned char index_number;
  unsigned char date[4];
  unsigned char time[3];
  unsigned char validation_number[8];
  unsigned char amount[5];
  unsigned int  ticket_number;
  unsigned char validation_system_id;
  unsigned char expiration[4];
  unsigned int  poll_id;
  unsigned int  crc;
}EnhancedValidationInfoType;

// TITO - Send Ticket Validation Data (Respuesta a un LP 70)
typedef struct
{
  unsigned char vgmAddr;
  unsigned char command;         // 0x70
  unsigned char length;
  unsigned char ticket_status;
  unsigned char ticket_amount[5];
  unsigned char parsing_code;
  unsigned char validation_data[9];
  unsigned int  crc;    
}TicketValidationDataType;  

// TITO - Redeem Ticket Message (LP 71)

typedef union
{
  struct
  {
    unsigned char vgmAddr;
    unsigned char command;         // 0x71
    unsigned char length;          // 16
    unsigned char transfer_code;   // 0x00: valid cashable ticket - 0x80: unable to validate
    unsigned char transfer_amount[5];   // monto en centavos, en BCD
    unsigned char parsing_code;         // 0x00 --> validation number de 18 dígitos BCD
    unsigned char validation_data[9];   // validation number de 18 dígitos BCD
    unsigned int  crc;
  }req;

  struct
  {
    unsigned char vgmAddr;
    unsigned char command;         // 0x71
    unsigned char length;          // 1
    unsigned char transfer_code;   // 0xff: request for current ticket status
    unsigned int  crc;    
  }cons;

  struct
  {
    unsigned char vgmAddr;
    unsigned char command;         // 0x71
    unsigned char length;
    unsigned char machine_status;
    unsigned char amount[5];
    unsigned char parsing_code;    // 0x00
    unsigned char validation_data[9];
    unsigned int  crc;
  }resp;
}RedeemTicketType;

typedef struct
{
  unsigned char vgmAddr;
  unsigned char command;         // 0x4C
  unsigned char machine_id[3];
  unsigned char sequence_number[3];
  unsigned int  crc;
}SetSecureEnhancedValidationIDType;

#pragma pack()

// Códigos de machine status para las respuestas al LP 71
// Los códigos del 0x80 al 0x81 son códigos de error, que se reportan al servidor
// Obs.: el campo <protocol> en 'T' permite discriminar que <errorCode> proviene de TITO

#define TITO_CASHABLE_TICKET_REDEEMED   0x00   // Idem AFT
#define TITO_TICKET_REDEMPTION_PENDING  0x40   // Idem AFT

// Los siguientes están asimilados a los códigos de error de AFT
#define TITO_NO_RESPONSE   AFT_NO_RESPONSE     // 0xE1
#define TITO_BAD_RESPONSE  AFT_BAD_RESPONSE    // 0xE2

// Códigos de Ticket Transfer (se usan en el LP 71, para ordenar la carga de la VGM
// o el rechazo -y expulsión- del ticket).
// Obs.: los códigos entre 0x81 y 0x88 explicitan ciertos errores, pero no los usamos.

#define TITO_VALID_CASHABLE_TICKET     0x00   // OK: cargar monto
#define TITO_UNABLE_TO_VALIDATE        0x80   // Rechazar la carga
#define TITO_REQUEST_TICKET_STATUS     0xff   // Consultar estado del último ticket

// Códigos de mensajes intercambiados entre el módulo y la PC con touch screen (msgCode)

#define TOUCH_GET_MONTO  1   // El módulo pide el ingreso del monto a cargar por el vendedor
                             // --> el campo maxMonto[] contiene el máximo permitido al vendedor

#define TOUCH_SEND_MONTO 2   // La PC envía el monto ingresado por el vendedor --> en monto[]
                             // va el valor ingresado (ASCII terminado en null)

#define TOUCH_DISPLAY    3   // El módulo envía un mensaje con el texto a presentar
                             // en el display

#define TOUCH_GET_PIN    4   // El módulo pide el ingreso del PIN del jugador

#define TOUCH_SEND_PIN   5   // La PC envía el PIN ingresado por el jugador (ASCII terminado en null)

// Estructura para almacenar código y texto del mensaje a presentar en el display LCD
// y la PC-touch screen
// Obs.: cod = 0 indica que el texto indicado no tiene un código asociado

#pragma pack(1)

typedef struct
{
  unsigned char cod;
  char txt[2][17];
}TOUMSG;

// Estructura del mensaje usado para la comunicación con la PC-touch screen

typedef struct
{
  unsigned char msgCode;
  union
  {
    TOUMSG display;               // módulo->PC : código y texto del mensaje a mostrar
    unsigned char monto[11];      // PC->módulo : monto ingresado manualmente
    unsigned char maxMonto[11];   // módulo->PC : monto máximo de que dispone el vendedor
    unsigned char pin[11];        // PC->módulo : PIN ingresado manualmente
  };
  unsigned int hash;
  unsigned int crc;
}TouchMsgType;

#pragma pack()

// Prototipos de funciones externas, accesibles desde otros módulos

unsigned int CRC(unsigned char const message[], unsigned int nBytes, unsigned int crcval); // función para calcular el CRC
void cifrar(unsigned char message[], int nBytes); // función para cifrar / decifrar un mensaje
#define decifrar(m,n) cifrar(m,n)                 // macro para invocar a cifrar para decifrar
void actualizaFechaHora(void);              // computa un tick y actualiza fecha y hora
void checkHelloReply(void);                 // verifica el mensaje de Hello Reply recibido, y actualiza fecha y hora
void checkEventAck(void);                   // Verifica el ACK de eventos recibido
void processCashlessReply(void);            // Procesa una respuesta a un mensaje de cashless
void processTestMsg(void);                  // Procesa un mensaje de prueba, para transferir un comando a la VGM
void processTouchReply(void);               // Procesa un mensaje recibido de la PC-touch screen
void delay( const unsigned int n_x_10ms);   // Lazo de retardo. El tiempo se indica en decenas de ms

void UART0_tx(unsigned char b, unsigned char wakeup); // Envía un byte por la UART0
unsigned char UART0_rx(unsigned char maxlen);     // Espera la recepción de un mensaje por la UART0
unsigned char UART0_AFT_rx(unsigned char maxlen); // Idem, pero con tiempo de respuesta diferente para AFT

void long2bcd(unsigned char * d, unsigned long m, int n);  // convierte unsigned long a BCD
unsigned long bcd2long(unsigned char * d, int n);          // convierte BCD a unsigned long

                                            // Envía un mensaje para el log de debug
void sendLogMsg(char * txt, unsigned char status, unsigned long monto);

unsigned int RANDOM(void);  // genera un número seudo aleatorio en base al valor de dos temporizadores

// variables externas

extern unsigned char vgmProto;              // SAS - DACOM - GAMMA - EIBE

extern const char hexa[];

extern unsigned char rx_buffer[];           // buffer para almacenar las respuestas recibidas por la UART0
extern volatile unsigned char rx_buflen;    // cantidad de bytes almacenados en rx_buffer

extern unsigned char * const tx_buffer;     // puntero al buffer para almacenar el mensaje a enviar
                                            // a la VGM por la UART0, o al display LCD, etc. 
                                            // Obs.: debe inicializarse para apuntar a TxFrame2Mem

extern unsigned char tx_buflen;             // cantidad de bytes almacenados en tx_buffer

extern volatile unsigned int pollingTimer;  // Timer para controlar el pedido de eventos y long polls
extern unsigned int pollingPeriod;          // Período entre pedido de eventos y long polls
extern volatile unsigned int waitLongPollTimer;   // timer para esperar la respuesta a un long poll (100 ms)
extern volatile unsigned int waitEventTimer;      // timer para esperar la respuesta a un pedido de eventos (20 ms)
extern unsigned int eventMsgRetry;          // flag que indica que el mensaje de eventos es una retransmisión
extern volatile unsigned int waitEventAckTimer;   // timer para esperar el ACK a un mensaje de eventos (200 ms)
extern volatile unsigned int sendMetersTimer;     // timer para controlar el período entre envío de contadores
extern unsigned int sendHelloPeriod;        // período entre envio de Hellos
extern volatile unsigned int sendHelloTimer;      // timer para controlar el envío de mensajes de Hello
extern unsigned int serverAlive;            // flag que indica si el servidor está vivo. Se activa al recibir
                                            // la respuesta al hello -conteniedo fecha y hora-

extern EventMsgType bufEventos;             // buffer para almacenar los eventos obtenidos de la máquina
extern LongPollMsgType longPollBuffer;      // Buffer para almacenar las respuestas a los long polls

extern FechaHoraType tiempo;                // aaaa/mm/dd hh:mm:ss
extern volatile unsigned int delayTimer;    // Timer para retardos

extern volatile unsigned int blinkingLedTimer;  // Timer para el parpadeo de led de test

extern volatile unsigned int WDTimer;// Timer auxiliar para el manejo del watchdog. Se lo
                                     // resetea en el loop principal del programa. El WDT se
                                     // resetea en la ISR del timer. Si WDTimer supera
                                     // los 10 seg, no se resetea más el WDT para que el micro
                                     // se reinicie

extern CashMsgType cashMsgBuffer;           // Buffer para el mensaje de cashless a enviar

extern volatile unsigned int waitCashReplyTimer; // timer para esperar respuesta a un mensaje de cashless (100 ms)
extern unsigned int cashoutMsgReply;             // indica si un mensaje de cashout es el original (0) o un reintento (>=1)
extern volatile unsigned int cardReaderTimer;    // timer para controlar la operación del lector de tarjetas (500 ms)
extern volatile unsigned int waitClientCardTimer;// timer para la espera de que se inserte una tarjeta de cliente

extern volatile unsigned int rxByteTimer;   // tiempo desde el último byte recibido

extern unsigned char HW_type;               // Olimex (1) o BB2 (2)

extern TouchMsgType touchMsgBuffer;         // Buffer para enviar datos a la PC-touch screen

// Unión unnamed para almacenar los distintos flags usados por
// el programa (Obs.: algunos se usan solamente en otros módulos).
extern volatile union {
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

//----------------------------------------------------------
// Textos de mensajes que se utilizan más de una vez,
// para no tener varias copias ocupando memoria, se
// los almacena en variables const
//----------------------------------------------------------

extern const TOUMSG MSG_MAQUINA_LIBRE;
extern const TOUMSG MSG_INGRESE_TARJETA;
extern const TOUMSG MSG_NO_FUNCIONA;
extern const TOUMSG MSG_PUERTA_ABIERTA;
extern const TOUMSG MSG_MAQUINA_EN_JUEGO;
extern const TOUMSG MSG_BUENA_SUERTE;
extern const TOUMSG MSG_ERROR;
extern const TOUMSG MSG_ERROR_RETIRE_TARJETA;
extern const TOUMSG MSG_RETIRE_TARJETA;
extern const TOUMSG MSG_NO_PERMITIDO;
extern const TOUMSG MSG_ENTRANDO_MODO_TEST;
extern const TOUMSG MSG_SALIENDO_MODO_TEST;

extern TOUMSG * const toumsg;      // puntero al buffer usado para generar mensajes
                                   // variables para el display LCD.
                                   // Obs.: usa el mismo espacio de memoria que Tx2FrameMem
                                   // y tx_buffer, por lo que debe ser usado concurrentemente
                                   // con esas otras funciones.

