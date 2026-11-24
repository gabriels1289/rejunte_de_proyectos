using System;
using System.Collections.Generic;
using System.Text;

namespace IRCASHDATAMODEL.Utiles.Varios
{
    public class CRC
    {
        /// <summary>
        /// Algoritmo para calcular el CRC a un arreglo de Bytes. KERMIT
        /// </summary>
        /// <param name="mensaje"></param>
        /// <param name="nBytes"></param>
        /// <param name="crcval"></param>
        /// <returns></returns>
        public static UInt16 CRC_KERMIT(Byte[] mensaje, int nBytes, UInt16 crcval)
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
    }
}
