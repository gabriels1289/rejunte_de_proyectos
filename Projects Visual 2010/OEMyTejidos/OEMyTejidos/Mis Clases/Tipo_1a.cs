using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace OEMyTejidos
{
    public static class Tipo_1a
    {
        public const Double espesorPielHidratada = 0.0021;
        public const Double espesorMusculo = 0.005;
        public const Double espesorHueso = 0.01;
        
        //parametros de la piel no dependientes de la frecuencia
        public const Double conductividadTermicaPiel = 0.35;
        public const Double calorEspecificoPiel=3300;
        public const Double densidadPiel=1200;
        //parametros del musculo no dependientes de la frecuencia
        public const Double conductividadTermicaMusculo = 0.45;
        public const Double calorEspecificoMusculo = 3700;
        public const Double densidadMusculo = 1025;
        //parametros del hueso no dependientes de la frecuencia
        public const Double conductividadTermicaHueso = 0.38;
        public const Double calorEspecificoHueso = 1260;
        public const Double densidadHueso = 2100;

        public const String modelo = "Tipo 1a: Modelo Fibroso - Piel Hidratada";
        public const String modeloChart = "Tipo 1a (F-PH)";
        public const String capa1 = "Piel Hidratada";
        public const String capa2 = "Músculo";
        public const String capa3 = "Hueso";
    }
}
