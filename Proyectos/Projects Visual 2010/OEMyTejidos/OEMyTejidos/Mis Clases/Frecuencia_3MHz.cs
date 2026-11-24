using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace OEMyTejidos
{
    public static class Frecuencia_3MHz
    {
        //Sigma:Conductividad Electrica
        //Epsilon_r:Permitividad Relativa
        public const Double frecuencia = 3e6;
        public const Double sigmaPielSeca = 0.063, epsilon_rPielSeca = 745;
        public const Double sigmaPielHumeda = 0.293, epsilon_rPielHumeda = 642;
        public const Double sigmaGrasa = 0.025, epsilon_rGrasa = 21;
        public const Double sigmaMusculo = 0.568, epsilon_rMusculo = 522;
        public const Double sigmaHueso = 0.031, epsilon_rHueso = 83;
    }                       
}
