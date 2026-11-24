using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace IREcoService
{
    public class Utiles
    {
        /// <summary>
        /// Calculo del CRC segun Algoritmo Anexo Protocolo UDP
        /// </summary>
        /// <param name="mensaje"></param>
        /// <param name="nBytes"></param>
        /// <param name="crcval"></param>
        /// <returns></returns>
        /// 
        private static String Hexa = "0123456789ABCDEF";


        //Función para calcular el CRC de una cadema de Bytes.
        //Utilizado para verificar integridad de paquetes.
        public static UInt16 CRC(Byte[] mensaje, int nBytes, UInt16 crcval)
        {
            UInt16 resto = 0;
            UInt16 POLINOMIO = 0x8408;


            for (int _byte = 0; _byte < nBytes; ++_byte)
            {
                resto ^= mensaje[_byte];
                for (UInt16 _bit = 8; _bit > 0; --_bit)
                {
                    if ((resto & 1) == 1)
                        resto = (UInt16)((resto >> 1) ^ POLINOMIO);
                    else
                        resto = (UInt16)(resto >> 1);
                }
            }
            //    resto = (UInt16)(resto >> 8 | resto << 8);

            return resto;
        }

        /// <summary>
        /// Convierte un numero en BCD de 8 digitos (4 bytes) a entero de 32bits
        /// </summary>
        /// <param name="msb"></param>
        /// <param name="msb2"></param>
        /// <param name="lsb2"></param>
        /// <param name="lsb"></param>
        /// <returns></returns>
        public static Int32 convertirBCD8Digitos(Byte msb, Byte msb2, Byte lsb2, Byte lsb)
        {
            Int32 resultado;

            resultado = (int)lsb & 0x0F;   // Digito menos significativo
            int tmp = (int)(lsb >> 4);
            resultado = resultado + tmp * 10;

            resultado = resultado + (int)(lsb2 & 0x0F) * 100;
            tmp = (int)(lsb2 >> 4);
            resultado = resultado + tmp * 1000;

            resultado = resultado + (int)(msb2 & 0x0F) * 10000;
            tmp = (int)(msb2 >> 4);
            resultado = resultado + tmp * 100000;

            resultado = resultado + (int)(msb & 0x0F) * 1000000;
            tmp = (int)(msb >> 4);
            resultado = resultado + tmp * 10000000; // Digito mas significativo

            return resultado;
        }

        /// <summary>
        /// Convierte un entero de 4 bytes a su equivalente en BCD.
        /// </summary>
        /// <param name="numero"></param>
        /// <returns></returns>
        public Byte[] convertirABCD(Int32 numero)
        {
            Byte[] numeroBCD = new Byte[4];
            Byte lsb, lsb2, msb2, msb;

            Int32 tmp, numeroTMP;
            Int32 d1, d2, resultado;
            numeroTMP = numero;

            tmp = numeroTMP % 100;

            d1 = (tmp % 10);
            d2 = (tmp / 10) * 16;
            resultado = d1 + d2;
            lsb = (Byte)(resultado);
            numeroTMP = numeroTMP / 100;

            tmp = numeroTMP % 100;
            d1 = (tmp % 10);
            d2 = (tmp / 10) * 16;
            resultado = d1 + d2;
            lsb2 = (Byte)(resultado);
            numeroTMP = numeroTMP / 100;

            tmp = numeroTMP % 100;
            d1 = (tmp % 10);
            d2 = (tmp / 10) * 16;
            resultado = d1 + d2;
            msb2 = (Byte)(resultado);
            numeroTMP = numeroTMP / 100;

            tmp = numeroTMP % 100;
            d1 = (tmp % 10);
            d2 = (tmp / 10) * 16;
            resultado = d1 + d2;
            msb = (Byte)(resultado);

            numeroBCD[0] = lsb;
            numeroBCD[1] = lsb2;
            numeroBCD[2] = msb2;
            numeroBCD[3] = msb;

            return numeroBCD;

        }

        public static Byte[] convertirABCD2(Int32 numero)
        {
            Byte[] numeroBCD = new Byte[5];
            Byte lsb, lsb2, medio, msb2, msb;

            Int32 tmp, numeroTMP;
            Int32 d1, d2, resultado;
            numeroTMP = numero;

            tmp = numeroTMP % 100;

            d1 = (tmp % 10);
            d2 = (tmp / 10) * 16;
            resultado = d1 + d2;
            lsb = (Byte)(resultado);
            numeroTMP = numeroTMP / 100;

            tmp = numeroTMP % 100;
            d1 = (tmp % 10);
            d2 = (tmp / 10) * 16;
            resultado = d1 + d2;
            lsb2 = (Byte)(resultado);
            numeroTMP = numeroTMP / 100;

            tmp = numeroTMP % 100;
            d1 = (tmp % 10);
            d2 = (tmp / 10) * 16;
            resultado = d1 + d2;
            medio = (Byte)(resultado);
            numeroTMP = numeroTMP / 100;

            tmp = numeroTMP % 100;
            d1 = (tmp % 10);
            d2 = (tmp / 10) * 16;
            resultado = d1 + d2;
            msb2 = (Byte)(resultado);
            numeroTMP = numeroTMP / 100;

            tmp = numeroTMP % 100;
            d1 = (tmp % 10);
            d2 = (tmp / 10) * 16;
            resultado = d1 + d2;
            msb = (Byte)(resultado);

            numeroBCD[0] = lsb;
            numeroBCD[1] = lsb2;
            numeroBCD[2] = medio;
            numeroBCD[3] = msb2;
            numeroBCD[4] = msb;

            return numeroBCD;

        }

        //Least  Significative Byte
        public static byte LSByte(UInt16 numero)
        {
            byte resultado = Convert.ToByte(numero & 0xFF);

            return resultado;
        }

        //Most Significative Byte
        public static byte MSByte(UInt16 numero)
        {
            byte resultado = Convert.ToByte(numero >> 8);

            return resultado;
        }

        //Transforma una cadena de bytes en un string. 
        //Util para imprimir la cadena de bytes. 
        public static String convertirACadenaHexadecimal(Byte[] cadenaBytes)
        {
            String resultado = String.Empty;
            Byte lsb;
            Byte msb;

            for (int i = 0; i < cadenaBytes.Length; i++)
            {
                lsb = Convert.ToByte(cadenaBytes[i] & 0x0F);
                msb = Convert.ToByte(cadenaBytes[i] >> 4);
                resultado = resultado + Hexa[msb] + Hexa[lsb];

            }
            return resultado;
        }

        public static void log(String msg, Int32 puerto)
        {

            string path = @"C:\logcash" + puerto + ".txt";
            TextWriter tw = new StreamWriter(path, true);
            tw.WriteLine(msg);
            tw.Close();
        }

        /// <summary>
        /// Funcion para encriptar un paquete
        /// </summary>
        /// <param name="paquete"></param>
        /// <returns></returns>
        public static Byte[] encrypt(Byte[] paquete)
        {
            int longitud = paquete.Length;//84+2
            Byte[] tmp = new Byte[longitud];

            Byte crcLSB, crcMSB;

            crcLSB = paquete[longitud - 2];
            crcMSB = paquete[longitud - 1];

            int i = 0;
            while (i < (longitud - 2))
            {
                tmp[i] = (Byte)(paquete[i] ^ crcLSB); //XOR
                i++;
                tmp[i] = (Byte)(paquete[i] ^ crcMSB);
                i++;

            }

            tmp[longitud - 2] = crcLSB;
            tmp[longitud - 1] = crcMSB;
            return tmp;
        }

        internal static int convertir_BCD_a_Decimal(byte bcd)
        {

            byte lsb = Convert.ToByte(bcd & 0x0F);
            byte msb = Convert.ToByte(bcd >> 4);

            return (lsb + msb * 10);
        }

    }
}