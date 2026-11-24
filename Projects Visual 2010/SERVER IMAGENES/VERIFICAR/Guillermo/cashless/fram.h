// fram.h

// prototipos de funciones para lectura - escritura en la FRAM

unsigned char framRead( unsigned int addr, unsigned char *buf, unsigned int nBytes );

unsigned char framWrite( unsigned int addr, unsigned char *buf, unsigned char nBytes );

