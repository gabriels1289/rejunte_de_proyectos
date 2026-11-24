// ks0108.h : archivo de encabezado con constantes, tipos y prototipos de ks0108.c

// Prototipos de funciones
void ks0108_init(void);                                          // Inicializa el display gráfico
void ks0108WriteCommand(unsigned char cmd, unsigned char chip);  // Escribe un comando
void ks0108WriteData(unsigned char data);                        // Escribe un dato
void ks0108FillRect(unsigned char x, unsigned char y, unsigned char width, unsigned char height, unsigned char color);
void ks0108GotoXY(unsigned char x, unsigned char y);
void ks0108ClearScreen(void);
void ks0108Putchar(int x, int y, char c);
int  ks0108Puts(int line, char * s);
void ks0108Bitmap(unsigned char const * const bmp);
void ks0108Icono(int x, int y, unsigned char const * const bmp);


