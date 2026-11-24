using System;
using System.Collections.Generic;
using System.Text;

namespace IRCASHDATAMODEL.Utiles.Varios
{
    public class Conversion
    {

        private static String Hexa = "0123456789ABCDEF";
       
        /// <summary>
        /// Convierte una cadena de bytes a su version imprimible.
        /// </summary>
        /// <param name="cadenaBytes"></param>
        /// <returns></returns>
        public static String convertirACadenaHexadecimal(String cadenaBytes)
        {
            String resultado = String.Empty;
            Byte lsb;
            Byte msb;

            foreach (char c in cadenaBytes)
            {
                int code = (int)c;
                lsb = Convert.ToByte(code & 0x0F);
                msb = Convert.ToByte(code >> 4);
                resultado = resultado + Hexa[msb] + Hexa[lsb];
            }
            return resultado;
        }

        public static String convertirByteAStringHexadecimal(Byte code)
        {
            Byte lsb;
            Byte msb;
            String resultado;
            lsb = Convert.ToByte(code & 0x0F);
            msb = Convert.ToByte(code >> 4);
            resultado = Convert.ToString(Hexa[msb]) + Convert.ToString(Hexa[lsb]);
            return resultado;
        }
    }
}
