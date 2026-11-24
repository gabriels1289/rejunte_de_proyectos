#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace std;

int main()
{
    FILE *f;    //puntero al archivo original
    FILE *p;    //puntero al archivo para el coocox
    unsigned char buffer[391681];       //480x272x3 Bytes
    unsigned long int n=0;
    unsigned char encabezado[54];
    uint32_t alto = 0;
    uint32_t ancho = 0;
    uint32_t tamanio = 0;
    int pixel = 0;
    char archivo[30];       // nombre del archivo, maximo 20 caracteres
    char archivo_c[30];
    char archivo_bmp[30];

    cout << "\t\tPASANDO ARCHIVO: .BMP a .C" << endl << endl;
    cout << "Nombre del archivo:  ";
    cin  >> archivo;
    strcpy(archivo_c,archivo);
    strcpy(archivo_bmp,archivo);
    strcat(archivo_c,".c");
    strcat(archivo_bmp, ".bmp");
    cout << "Archivo: " <<  archivo_bmp << endl;

    if( !(p = fopen( archivo_c, "w" ) ))
    {
        cout << "Error al crear imagen.c" << endl;
        getchar();
        getchar();
        return 0;
    }

    if( (f = fopen( archivo_bmp, "r" )) )
    {
        fread(encabezado, sizeof(encabezado),1,f);
        ancho = encabezado[21];
        ancho = (ancho << 8) | encabezado[20];
        ancho = (ancho << 8) | encabezado[19];
        ancho = (ancho << 8) | encabezado[18];
        cout << "\nANCHO: \t\t" << ancho;
        alto = encabezado[25];
        alto = (alto << 8) | encabezado[24];
        alto = (alto << 8) | encabezado[23];
        alto = (alto << 8) | encabezado[22];
        cout << "\nALTO: \t\t" << alto;
        pixel = encabezado[29];
        pixel = (pixel << 8) | encabezado[28];
        cout << "\nPIXEL: \t\t" << pixel << " BITS";
        tamanio = ancho * alto * (pixel/8);
        cout << "\nIMAGEN: \t" << tamanio << " BYTES";

        fprintf(p, "//--------------------------------------------------------------\n");
        fprintf(p, "// File     : %s                                        \n",archivo_c);
        fprintf(p, "//--------------------------------------------------------------\n\n");
        fprintf(p, "//--------------------------------------------------------------\n");
        fprintf(p, "// Includes\n");
        fprintf(p, "//--------------------------------------------------------------\n\n");
        fprintf(p, "#include %c..%cSSD1963.h%c\n",0x22, 0x5C, 0x22);
        fprintf(p, "\nconst uint8_t arreglo_%s[]= {",archivo);

        fseek(f, 54, SEEK_SET);

        fread( buffer, sizeof(unsigned char), tamanio, f);
        for(n = 0; n <= tamanio; n++)
        {
            if (n%40 == 0)
                fprintf(p, "\n\t");
            if(n!=tamanio)
                fprintf(p, "%d, ", buffer[n]);
            else
                fprintf(p, "%d ", buffer[n]);
        }

        cout << "\n\nALMACENO: \t" << n-1 << " BYTES" << endl;
        fclose(f);

        fprintf(p, "};\n\nSSD_imag %s = { \n",archivo);
        fprintf(p, "\tarreglo_%s,\t\t// Data_picture \n",archivo);
        fprintf(p, "\t%d,\t\t\t\t// Width_picture \n", ancho);
        fprintf(p, "\t%d,\t\t\t\t// Heigh_picture \n", alto);
        fprintf(p, "\t%d,\t\t\t\t// TotalByte_picture \n\t};", tamanio);
        fclose(p);
    }
    cout << "\nPasaje Archivo  .BMP a .C Terminado" << endl;

    return 0;
}
