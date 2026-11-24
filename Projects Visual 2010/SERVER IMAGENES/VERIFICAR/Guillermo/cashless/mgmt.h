// Mgmt.h
// Header del módulo de administración 

// constantes usadas para el código de operación de los mensajes de administración

#define MGMT_GET_VERSION      1  // SRV -> MOD: Consultar la versión actual en el módulo
#define MGMT_SEND_VERSION     2  // MOD -> SRV: Módulo informa versión actual
#define MGMT_SET_VERSION      3  // SRV -> MOD: Ordenar la instalación de una cierta versión
#define MGMT_SET_IP_MAC       4  // SRV -> MOD: Ordena setear IP y MAC
#define MGMT_GET_CODESEG      5  // MOD -> SRV: módulo pide un cierto segmento de una nueva versión
#define MGMT_SEND_CODESEG     6  // SRV -> MOD: Servidor envía un segmento de código de una nueva versión
#define MGMT_ABORT_DWNLD      7  // MOD -> SRV: módulo informa que abortó descarga de una nueva versión
#define MGMT_SEND_BITMAP      8  // SRV -> MOD: Servidor envía una porción de un bitmap
#define MGMT_GET_FEATURES     9  // SRV -> MOD: Consultar las features configuradas en el módulo
#define MGMT_SEND_FEATURES   10  // MOD -> SRV: Módulo informa las features configuradas
#define MGMT_SET_FEATURES    11  // SRV -> MOD: Servidor ordena configurar features del módulo
#define MGMT_DELE_FEATURES   12  // SRV -> MOD: Servidor ordena borrar features (vuelve a valores por defecto)
#define MGMT_GET_HASH_MD5    13  // SRV -> MOD: Servidor pide el hash MD5 del código (0x1100 a 0xFFFF)
#define MGMT_SEND_HASH_MD5   14  // MOD -> SRV: Módulo envía el hash MD5 del código
#define MGMT_GET_HASH_CRC16  15  // SRV -> MOD: Servidor pide el hash CRC16 del código (0x1100 a 0xFFFF)
#define MGMT_SEND_HASH_CRC16 16  // MOD -> SRV: Módulo envía el hash CRC16 del código


#define MGMT_CRC_SEED 0x5AC3   // valor inicial para el cálculo del CRC (usado como password)

//--------------------------------------------------------
// Features del módulo
//--------------------------------------------------------
#define FEATURES_DEFAULT 0xffff  // valores de reset

#define DISPLAY_2x16    0  // display de 2 x 16 caracteres
#define DISPLAY_KS0108  1  // display gráfico monocromo 128x64 (basado en KS0108)
#define DISPLAY_IP      2  // display en PC conectada por IP

#define TECLADO_NO      0  // sin teclado
#define TECLADO_SERIE   1  // teclado con conexión por serie asincrónica, a 9600-8-N-1
#define TECLADO_IP      2  // teclado en PC conectado por IP

//-------------------------------------------------------------------------------
// Estructura para almacenar las opciones del módulo, configurables por software
//-------------------------------------------------------------------------------
typedef struct
{
  unsigned int display;    // 00 o FF : display de texto 2x16
                           // 01 : display gráfico monocromo de 128 x 64 pixels (ks0108)
                           // 02 : display en módulo conectado por IP
  unsigned int teclado;    // 00 o FF : sin teclado
                           // 01 : teclado o touch screen en módulo conectado por UART
                           // 02 : teclado o touch screen en módulo conectado por IP
  unsigned int impresora;  // 00 o FF : sin impresora

  unsigned int font;
  
  unsigned int IP_server_evt[2];    // IP del servidor de eventos
  unsigned int IP_server_met[2];    // IP del servidor de contadores
  unsigned int IP_server_cash[2];   // IP del servidor de cashless
  unsigned int IP_server_mgmt[2];   // IP del servidor de administración
  unsigned int IP_server_touch[2];  // IP de la PC-touch screen
  unsigned int IP_server_graf[2];   // IP del servidor que envía gráficos para el display

  unsigned int tito;        // 00 o FF: TITO deshabilitado
                            // 01: TITO habilitado
}FeaturesType;

//-------------------------------------------------------------------------------
// Estructura para los mensajes de gestión
//-------------------------------------------------------------------------------
typedef union
{
  struct
  {
    unsigned int op;          // código de la operación
    char version[20];         // Nro de versión en ASCII
    unsigned char IP[4];      // dirección IP (ej.: 172.16.0.20, en ese orden)
    unsigned int offset;      // offset inicial de un bloque de 512 bytes (LSB-MSB)
    unsigned int crc;         // CRC generado a partir de un valor inicial
  }msg1;
  
  struct
  {
    unsigned int op;          // código de la operación
    char version[20];         // Nro de versión en ASCII
    unsigned int offset;      // offset inicial del bloque de código o datos de 512 bytes
    union
    {
      unsigned int code[256]; // segmento de código de 512 bytes
      unsigned char data[512];// bloque de datos de 512 bytes (por ej: la mitad de un bitmap de 128x64)
    };
    unsigned int crc;         // CRC generado a partir de un valor inicial
  }msg2;
  
  struct
  {
    unsigned int op;          // código de la operación
    FeaturesType features;    // características opcionales del módulo, configurables por software
    unsigned int crc;         // CRC generado a partir de un valor inicial
  }msg3;

  struct
  {
    unsigned int op;          // código de la operación
    union {
      struct
      {
        int seedLen;
        unsigned char seed[16];  // semilla inicial para dispersar el cálculo del hash
      };
      unsigned char digest[16];  // resumen MD5
    };
    unsigned int crc;         // CRC generado a partir de un valor inicial
  }msg4;

}MgmtMsgType;

//------------------------------------
// Variables externas
//------------------------------------
extern volatile unsigned int waitCodeSegTimer;  // Timer para esperar un segmento de código pedido para actualizar el programa
extern unsigned int waitingCodeSeg;    // Offset inicial del segmento de código que está esperando
extern const char version[];           // Nombre de la versión actual
extern char newVersion[];              // Nombre de nueva versión
extern FeaturesType Features;          // Características opcionales configurables por software

//------------------------------------
// Prototipos de funciones
//------------------------------------
void processMgmtMsg(void);    // Procesamiento de mensaje de gestión recibido

//--------------------------------------------------------------
// Obtiene una nueva imagen del programa, lo graba en la flash
// de código y reinicia el módulo
//--------------------------------------------------------------
static void updatePgm(void);

//-----------------------------------------------------------
// Lazo de ejecución para cuando se inicia el módulo
// en modo test (reset con el pulsador test apretado)
//-----------------------------------------------------------
void testLoop(void);

//-----------------------------------------------------------
// Ejecuta algún test solicitado en forma remota por UDP
//-----------------------------------------------------------
void remoteTest(void);

//------------------------------------------------
// convierte hexa codificado en ASCII a hexa
//------------------------------------------------
unsigned char ascii2hexa(unsigned char a);

// Funciones de testeo

//------------------------------------------------
// Imprime un texto en ASCII en el display
//------------------------------------------------
static void test_PNTLCDA(void);

//-------------------------------------------------------------
// Lee el estado de los jumpers J1 y J2, y el pulsador de test
//-------------------------------------------------------------
static void test_RDJMPTS(void);

//-------------------------------------------------------------
// Testea la comunicación serie
//-------------------------------------------------------------
static void test_WRRDCOM(void);

//-------------------------------------------------------------
// Prueba de escritura en la memoria EEPROM externa
//-------------------------------------------------------------
static void test_WREEMEM(void);

//-------------------------------------------------------------
// Prueba de lectura en la memoria EEPROM externa
//-------------------------------------------------------------
static void test_RDEEMEM(void);

//-------------------------------------------------------------
// Borrado de una página de la memoria EEPROM externa
//-------------------------------------------------------------
static void test_CLEEMEM(void);

//-------------------------------------------------------------
// Resetea una smart card y obtiene el answer-to-reset
//-------------------------------------------------------------
static void test_RSTCARD(void);

//-------------------------------------------------------------
// Escribe una secuencia de bytes en la smart card
//-------------------------------------------------------------
static void test_WRICARD(void);

//-------------------------------------------------------------
// Lee n datos de una smart card
//-------------------------------------------------------------
static void test_REACARD(void);

//----------------------------------------------
// Prueba el buzzer
//----------------------------------------------
static void test_WBUZZER(void);

//----------------------------------------------
// Prueba el led de test
//----------------------------------------------
static void test_TESTLED(void);

//------------------------------------------------------
// Consulta o modifica el período de polling para SAS
//------------------------------------------------------
static void test_POLLING_PERIOD(void);

//------------------------------------------------------
// Chequea el estado de los puertos auxiliares
// (deben estar puenteados)
//------------------------------------------------------
static void test_AUX_PORTS(void);
