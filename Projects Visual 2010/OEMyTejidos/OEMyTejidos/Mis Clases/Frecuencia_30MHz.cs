using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace OEMyTejidos
{
    public static class Frecuencia_30MHz
    {
        //Sigma:Conductividad Electrica
        //Epsilon_r:Permitividad Relativa
        public const Double frecuencia = 30e6;
        public const Double sigmaPielSeca = 0.341, epsilon_rPielSeca = 152;
        public const Double sigmaPielHumeda = 0.434, epsilon_rPielHumeda = 108;
        public const Double sigmaGrasa = 0.033, epsilon_rGrasa = 8;
        public const Double sigmaMusculo = 0.658, epsilon_rMusculo = 92;
        public const Double sigmaHueso = 0.052, epsilon_rHueso = 20;
    }
}
