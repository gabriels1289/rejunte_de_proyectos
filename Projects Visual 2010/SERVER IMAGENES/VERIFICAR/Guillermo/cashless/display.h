
//----------------------------------------
// Funciones para manejar el display LCD
//----------------------------------------

//----------------------------------------
// Enviar un comando al display LCD
//----------------------------------------
void LCD_command(unsigned char cmd);

//----------------------------------------
// Enviar un caracter al display LCD
//----------------------------------------
void LCD_write(unsigned char data);

//----------------------------------------
// Borrar el display LCD
//----------------------------------------
void LCD_clear(void);

//---------------------------------------------------
// Posicionar el cursor del display LCD en el inicio
//---------------------------------------------------
void LCD_home(void);

//------------------------------------------------------
// Posicionar el cursor del display LCD en la 2da línea
//------------------------------------------------------
void LCD_line2(void);

//------------------------------------------------------
// Imprime un mensaje en el display LCD
// Si corresponde lo envía a la PC-touch screen
//------------------------------------------------------
void LCD_print(const TOUMSG * msg);

//-----------------------------------------------
// Envía una cadena de caracteres al display LCD
//-----------------------------------------------
void LCD_puts( int line, const char * s );

//----------------------------------------
// Inicializa el display LCD
//----------------------------------------
void LCD_init(void);

// bits del puerto P4 para controlar el LCD
#define LCD_RW   0x01   // bit 0: R/W - Obs.: no se usa, está fijado a cero (Write) por defecto
#define LCD_RS   0x02   // bit 1: Register select
#define LCD_ENA  0x04   // bit 2: Enable

// bit 7 del puerto P1 para controlar la luz de fondo del LCD
#define LCD_BACKLIGHT 0x80

// Macros
/*
#define LCD_print1(msg) {LCD_home();LCD_print(msg);TOU_print(1,msg);}
#define LCD_print2(msg) {LCD_line2();LCD_print(msg);TOU_print(2,msg);}
*/
#define LCD_backlight_off()  P1OUT &= ~LCD_BACKLIGHT
#define LCD_backlight_on()   P1OUT |= LCD_BACKLIGHT

