#include <winsock2.h>
#include <stdio.h>

#include "mgmt.h"

MgmtMsgType msg;

unsigned short CRC (unsigned char const message[], int nBytes, unsigned short crcval);

int main( int argc, char * argv[] )
{
    SOCKET sock;
    int nlen, i;
    struct sockaddr_in name;
    struct hostent *hp;
    char buf [1024];
    WSADATA wsaData;
    int iResult;

    // Inicializa Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(iResult)
    {
        printf("Falla en WSAStartup: %d\n", iResult);
        return 1;
    }

    // Crea un socket para UDP sobre IPV4
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if ( sock < 0 )
    {
        printf("Falla en socket()\n");
        exit(1);
    }

    // Al socket le asigna un "nombre", para poder esperar mensajes por él */
    name.sin_family = AF_INET;          // formato de dirección: IP
    name.sin_addr.s_addr = INADDR_ANY;  // acepta paquetes de cualquier IP de origen
    name.sin_port = htons(1205);        // número de puerto (debe estar libre)

    if( bind ( sock, (SOCKADDR *)&name, sizeof(name) ) == -1)
    {
        printf("Falla en bind()\n");
        exit(1);
    }

    // Se prepara para enviar un mensaje: obtiene la dirección IP y
    // el Nº de Port del servidor indicados en la línea de comando.
    // Si no hay parámetros usa la IP de loopback

    if( argc < 3 || argc > 4 )
    {
        printf("Usar: firma <CRC16 | MD5> [semilla] <IP>\n");
        printf("Ejemplo: firma CRC16 1a3c 172.16.0.30\n");
        printf("         firma MD5 10203040abcdefgh 172.16.0.34\n");
        printf("         firma MD5 172.16.0.45\n");
        exit(1);
    }else if( argc == 4 ) {
        hp = gethostbyname( argv[3] );
    }else {
        hp = gethostbyname( argv[2] );
    }
    if( hp == 0 ) exit(1);

    // Ahora la estructura name la usará para indicar la IP y puerto de destino
    // para enviar el mensaje.

    // Copia la dirección IP a la estructura name

    name.sin_family = AF_INET;
    memcpy( &name.sin_addr.s_addr, hp->h_addr, hp->h_length );
    name.sin_port = htons( 1235 );

    // Prepara y envía el mensaje

    if( stricmp(argv[1], "CRC16") == 0 ) {
        msg.msg4.op = MGMT_GET_HASH_CRC16;
        msg.msg4.seedLen = 2;
        if( argc == 4 )
        {
            sscanf(argv[2],"%2x %2x",&msg.msg4.seed[0], &msg.msg4.seed[1]);
        }else{
            msg.msg4.seed[0] = msg.msg4.seed[1] = 0;
        }
    }else if( stricmp(argv[1], "MD5") == 0 ) {
        msg.msg4.op = MGMT_GET_HASH_MD5;
        if( argc == 4 )
        {
            i = sscanf(argv[2],
                "%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x",
                &msg.msg4.seed[0], &msg.msg4.seed[1], &msg.msg4.seed[2], &msg.msg4.seed[3],
                &msg.msg4.seed[4], &msg.msg4.seed[5], &msg.msg4.seed[6], &msg.msg4.seed[7],
                &msg.msg4.seed[8], &msg.msg4.seed[9], &msg.msg4.seed[10], &msg.msg4.seed[11],
                &msg.msg4.seed[12], &msg.msg4.seed[13], &msg.msg4.seed[14], &msg.msg4.seed[15]);

            msg.msg4.seedLen = ((i>0)?i:0);
        }else{
            msg.msg4.seedLen = 0;
        }
    }else
        exit(2);

    msg.msg4.crc = CRC((unsigned char *)&msg, sizeof(msg.msg4)-2, MGMT_CRC_SEED);

    if ( sendto( sock, (char *)&msg, sizeof(msg.msg4), 0, (SOCKADDR *)&name, sizeof(name) ) == -1 )
    {
        printf("Falla en sendto()\n");
    }else{
        nlen = sizeof(name);

        // Luego de enviado un mensaje, se queda esperando
        // una respuesta en el mismo puerto
        if ( recvfrom(sock, (char *)&msg, sizeof(msg), 0, (SOCKADDR *)&name, &nlen ) == -1 )
        {
            printf("Falla en recvfrom()\n");
        }else{
            printf ( "Recibido de %-lu.%-lu.%-lu.%-lu:%-d: ",
                     name.sin_addr.s_addr & 0xff,
                     (name.sin_addr.s_addr >> 8) & 0xff,
                     (name.sin_addr.s_addr >> 16) & 0xff,
                     (name.sin_addr.s_addr >> 24) & 0xff,
                     ntohs(name.sin_port));
            if(msg.msg4.op==MGMT_SEND_HASH_MD5)
                printf("MD5 = ");
            else if(msg.msg4.op==MGMT_SEND_HASH_CRC16)
                printf("CRC16 = ");
            else printf("Respuesta errónea");

            if(msg.msg4.op==MGMT_SEND_HASH_MD5)
                for(i=0; i < 16; ++i)
                    printf("%02x",msg.msg4.digest[i]);
            else if(msg.msg4.op==MGMT_SEND_HASH_CRC16)
                for(i=0; i < 2; ++i)
                    printf("%02x",msg.msg4.digest[i]);
            printf("\n");
        }
    }
    closesocket( sock );
    return 0;
}

//-----------------------------------------
// función para calcular el CRC 16 (Kermit)
//-----------------------------------------
unsigned short CRC (unsigned char const message[], int nBytes, unsigned short crcval)
{
  unsigned short remainder = crcval;
  static const unsigned short POLYNOMIAL = 0x8408;
  int by;
  unsigned char bit;

  // Realiza la dividión modulo-2, de a un byte por vez
  for (by = 0; by < nBytes; ++by)
  {
    // Coloca el siguiente byte en el resto
    remainder ^= message[by];
    //remainder &= 0x0FFFF;

    // Realiza la división modulo-2, de a un bit por vez
    for (bit = 8; bit > 0; --bit)
    {
      // Intenta dividir el bit actual
      if (remainder & 1)
        remainder = (remainder >> 1) ^ POLYNOMIAL;
      else
        remainder = (remainder >> 1);
      //remainder &= 0x0FFFF;
    }
  }
  // El resto de la división es el CRC.

  // Lo coloca en orden Little Endian
  //remainder = remainder >> 8 | remainder << 8;

  return (remainder);
}

