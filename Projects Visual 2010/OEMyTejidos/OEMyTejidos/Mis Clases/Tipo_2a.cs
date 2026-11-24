using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace OEMyTejidos
{
    public static class Tipo_2a
    {
        public const Double espesorPielHidratda = 0.0021;
        public const Double espesorGrasa = 0.01;
        public const Double espesorMusculo = 0.005;

        //parametros de la piel no dependientes de la frecuencia
        public const Double conductividadTermicaPiel = 0.35;
        public const Double calorEspecificoPiel = 3300;
        public const Double densidadPiel = 1200;
        //parametros del hueso no dependientes de la frecuencia
        public const Double conductividadTermicaGrasa = 0.20;
        public const Double calorEspecificoGrasa = 2300;
        public const Double densidadGrasa = 900;
        //parametros del musculo no dependientes de la frecuencia
        public const Double conductividadTermicaMusculo = 0.45;
        public const Double calorEspecificoMusculo = 3700;
        public const Double densidadMusculo = 1025;

        public const String modelo = "Tipo 2a: Modelo Adiposo - Piel Hidratada";
        public const String modeloChart = "Tipo 2a (A-PH)";
        public const String capa1 = "Piel Hidratada";
        public const String capa2 = "Grasa";
        public const String capa3 = "Músculo";
    }
}
