using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Impresoras
{
    public static class Extension
    {
        /// <summary>
        /// Obtiene el estado de un bit
        /// </summary>
        /// <param name="b"></param>
        /// <param name="bitNumber"></param>
        /// <returns></returns>
        public static bool GetBit(this int b, int bitNumber)
        {
            return (b & (1 << bitNumber - 1)) != 0;

        }
    }
}
