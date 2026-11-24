using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ServerPrueba
{
    public class BMP
    {
        #region variables
        Byte[] tipoFichero = new Byte[2]; //o Byte[] tipoFichero = {0,0}
        Byte[] tamañoArchivo = new Byte[4];
        Byte[] reservado1 = new Byte[2];
        Byte[] reservado2 = new Byte[2];
        Byte[] inicioDatosImagen = new Byte[4];
        Byte[] tamañoCabezera = new Byte[4];
        Byte[] anchoEnPixels = new Byte[4];
        Byte[] altoEnPixels = new Byte[4];
        Byte[] numeroPlanos = new Byte[2];
        Byte[] tamañoPunto = new Byte[2];
        Byte[] compresion = new Byte[4];
        Byte[] tamañoImagen = new Byte[4];
        Byte[] resolucionHorizontal = new Byte[4];
        Byte[] resolucionVertical = new Byte[4];
        Byte[] tamañoTablaColor = new Byte[4];
        Byte[] contadorColoresImportantes = new Byte[4];
        #endregion variables

        public BMP(Byte[] _header)
        {
            tipoFichero[0] = _header[0];
            tipoFichero[1] = _header[1];
            tamañoArchivo[0] = _header[2];
            tamañoArchivo[1] = _header[3];
            tamañoArchivo[2] = _header[4];
            tamañoArchivo[3] = _header[5];
            reservado1[0] = _header[6];
            reservado1[1] = _header[7];
            reservado2[0] = _header[8];
            reservado2[1] = _header[9];
            inicioDatosImagen[0] = _header[10];
            inicioDatosImagen[1] = _header[11];
            inicioDatosImagen[2] = _header[12];
            inicioDatosImagen[3] = _header[13];
            tamañoCabezera[0] = _header[14];
            tamañoCabezera[1] = _header[15];
            tamañoCabezera[2] = _header[16];
            tamañoCabezera[3] = _header[17];
            anchoEnPixels[0] = _header[18];
            anchoEnPixels[1] = _header[19];
            anchoEnPixels[2] = _header[20];
            anchoEnPixels[3] = _header[21];
            altoEnPixels[0] = _header[22];
            altoEnPixels[1] = _header[23];
            altoEnPixels[2] = _header[24];
            altoEnPixels[3] = _header[25];
            numeroPlanos[0] = _header[26];
            numeroPlanos[1] = _header[27];
            tamañoPunto[0] = _header[28];
            tamañoPunto[1] = _header[29];
            compresion[0] = _header[30];
            compresion[1] = _header[31];
            compresion[2] = _header[32];
            compresion[3] = _header[33];
            tamañoImagen[0] = _header[34];
            tamañoImagen[1] = _header[35];
            tamañoImagen[2] = _header[36];
            tamañoImagen[3] = _header[37];
            resolucionHorizontal[0] = _header[38];
            resolucionHorizontal[1] = _header[39];
            resolucionHorizontal[2] = _header[40];
            resolucionHorizontal[3] = _header[41];
            resolucionVertical[0] = _header[42];
            resolucionVertical[1] = _header[43];
            resolucionVertical[2] = _header[44];
            resolucionVertical[3] = _header[45];
            tamañoTablaColor[0] = _header[46];
            tamañoTablaColor[1] = _header[47];
            tamañoTablaColor[2] = _header[48];
            tamañoTablaColor[3] = _header[49];
            contadorColoresImportantes[0] = _header[50];
            contadorColoresImportantes[1] = _header[51];
            contadorColoresImportantes[2] = _header[51];
            contadorColoresImportantes[3] = _header[53];
        }

        #region Get
        public String TipoFichero()
        {
            String aux = System.Text.Encoding.UTF8.GetString(tipoFichero);
            return aux;
        }

        public Int32 TamañoArchivo()
        {
            return BitConverter.ToInt32(tamañoArchivo, 0);
        }

        public Int32 InicioDatosImagen()
        {
            return BitConverter.ToInt32(inicioDatosImagen, 0);
        }

        public Int32 TamañoCabezera()
        {
            return BitConverter.ToInt32(tamañoCabezera, 0);
        }

        public Int32 AnchoEnPixels()
        {
            return BitConverter.ToInt32(anchoEnPixels, 0);
        }

        public Int32 AltoEnPixels()
        {
            return BitConverter.ToInt32(altoEnPixels, 0);
        }

        public Int16 NumeroDePlanos()
        {
            return BitConverter.ToInt16(numeroPlanos, 0);
        }

        public Int16 TamañoPunto()
        {
            return BitConverter.ToInt16(tamañoPunto, 0);
        }

        public Int32 Compresion()
        {
            return BitConverter.ToInt32(compresion, 0);
        }

        public Int32 TamañoImagen()
        {
            return BitConverter.ToInt32(tamañoImagen, 0);
        }

        public Int32 ResolucionHorizontal()
        {
            return BitConverter.ToInt32(resolucionHorizontal, 0);
        }

        public Int32 ResolucionVertical()
        {
            return BitConverter.ToInt32(resolucionVertical, 0);
        }

        public Int32 TamañoTablaColor()
        {
            return BitConverter.ToInt32(tamañoTablaColor, 0);
        }

        public Int32 ContadorColoresImportantes()
        {
            return BitConverter.ToInt32(contadorColoresImportantes, 0);
        }
        #endregion Get
    }
}
