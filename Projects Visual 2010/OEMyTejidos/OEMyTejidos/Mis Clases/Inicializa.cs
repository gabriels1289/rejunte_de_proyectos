using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace OEMyTejidos
{
    class Inicializa
    {

        public static void InicializarVariables(int seleTipo, int seleFrecuencia, ref FuncionesGenerales var)
        {
            ////MODELOS DE TEJIDOS
            var = new FuncionesGenerales();
            switch (seleTipo)
            {
                case 0:
                    var.deltaxmedio2 = Tipo_1a.espesorPielHidratada;
                    var.deltaxmedio3 = Tipo_1a.espesorMusculo;
                    var.deltaxmedio4 = Tipo_1a.espesorHueso;
                    var.modelo = Tipo_1a.modelo;
                    var.modeloChart = Tipo_1a.modeloChart;
                    var.medio2 = Tipo_1a.capa1;
                    var.medio3 = Tipo_1a.capa2;
                    var.medio4 = Tipo_1a.capa3;
                    var.conductividadTermicaMedio2 = Tipo_1a.conductividadTermicaPiel;
                    var.calorEspecificoMedio2 = Tipo_1a.calorEspecificoPiel;
                    var.densidadTejidoMedio2 = Tipo_1a.densidadPiel;
                    var.conductividadTermicaMedio3 = Tipo_1a.conductividadTermicaMusculo;
                    var.calorEspecificoMedio3 = Tipo_1a.calorEspecificoMusculo;
                    var.densidadTejidoMedio3 = Tipo_1a.densidadMusculo;
                    var.conductividadTermicaMedio4 = Tipo_1a.conductividadTermicaHueso;
                    var.calorEspecificoMedio4 = Tipo_1a.calorEspecificoHueso;
                    var.densidadTejidoMedio4 = Tipo_1a.densidadHueso;
                    break;

                case 1:
                    var.deltaxmedio2 = Tipo_1b.espesorPielSeca;
                    var.deltaxmedio3 = Tipo_1b.espesorMusculo;
                    var.deltaxmedio4 = Tipo_1b.espesorHueso;
                    var.modelo = Tipo_1b.modelo;
                    var.modeloChart = Tipo_1b.modeloChart;
                    var.medio2 = Tipo_1b.capa1;
                    var.medio3 = Tipo_1b.capa2;
                    var.medio4 = Tipo_1b.capa3;
                    var.conductividadTermicaMedio2 = Tipo_1b.conductividadTermicaPiel;
                    var.calorEspecificoMedio2 = Tipo_1b.calorEspecificoPiel;
                    var.densidadTejidoMedio2 = Tipo_1b.densidadPiel;
                    var.conductividadTermicaMedio3 = Tipo_1b.conductividadTermicaMusculo;
                    var.calorEspecificoMedio3 = Tipo_1b.calorEspecificoMusculo;
                    var.densidadTejidoMedio3 = Tipo_1b.densidadMusculo;
                    var.conductividadTermicaMedio4 = Tipo_1b.conductividadTermicaHueso;
                    var.calorEspecificoMedio4 = Tipo_1b.calorEspecificoHueso;
                    var.densidadTejidoMedio4 = Tipo_1b.densidadHueso;
                    break;

                case 2:
                    var.deltaxmedio2 = Tipo_2a.espesorPielHidratda;
                    var.deltaxmedio3 = Tipo_2a.espesorGrasa;
                    var.deltaxmedio4 = Tipo_2a.espesorMusculo;
                    var.modelo = Tipo_2a.modelo;
                    var.modeloChart = Tipo_2a.modeloChart;
                    var.medio2 = Tipo_2a.capa1;
                    var.medio3 = Tipo_2a.capa2;
                    var.medio4 = Tipo_2a.capa3;
                    var.conductividadTermicaMedio2 = Tipo_2a.conductividadTermicaPiel;
                    var.calorEspecificoMedio2 = Tipo_2a.calorEspecificoPiel;
                    var.densidadTejidoMedio2 = Tipo_2a.densidadPiel;
                    var.conductividadTermicaMedio4 = Tipo_2a.conductividadTermicaGrasa;
                    var.calorEspecificoMedio4 = Tipo_2a.calorEspecificoGrasa;
                    var.densidadTejidoMedio4 = Tipo_2a.densidadGrasa;
                    var.conductividadTermicaMedio3 = Tipo_2a.conductividadTermicaMusculo;
                    var.calorEspecificoMedio3 = Tipo_2a.calorEspecificoMusculo;
                    var.densidadTejidoMedio3 = Tipo_2a.densidadMusculo;
                    break;

                case 3:
                    var.deltaxmedio2 = Tipo_2b.espesorPielSeca;
                    var.deltaxmedio3 = Tipo_2b.espesorGrasa;
                    var.deltaxmedio4 = Tipo_2b.espesorMusculo;
                    var.modelo = Tipo_2b.modelo;
                    var.modeloChart = Tipo_2b.modeloChart;
                    var.medio2 = Tipo_2b.capa1;
                    var.medio3 = Tipo_2b.capa2;
                    var.medio4 = Tipo_2b.capa3;
                    var.conductividadTermicaMedio2 = Tipo_2b.conductividadTermicaPiel;
                    var.calorEspecificoMedio2 = Tipo_2b.calorEspecificoPiel;
                    var.densidadTejidoMedio2 = Tipo_2b.densidadPiel;
                    var.conductividadTermicaMedio4 = Tipo_2b.conductividadTermicaGrasa;
                    var.calorEspecificoMedio4 = Tipo_2b.calorEspecificoGrasa;
                    var.densidadTejidoMedio4 = Tipo_2b.densidadGrasa;
                    var.conductividadTermicaMedio3 = Tipo_2b.conductividadTermicaMusculo;
                    var.calorEspecificoMedio3 = Tipo_2b.calorEspecificoMusculo;
                    var.densidadTejidoMedio3 = Tipo_2b.densidadMusculo;
                    break;
            }

            //PARAMETROS DEPENDIENTES DE LA FRECUENCIA
            if (seleFrecuencia == 0)
            {
                var.frecuencia = Frecuencia_3MHz.frecuencia;
                var.frec = "3 MHz";
                switch (seleTipo)
                {
                    case 0:
                        var.epsilonr2 = Frecuencia_3MHz.epsilon_rPielHumeda;
                        var.epsilonr3 = Frecuencia_3MHz.epsilon_rMusculo;
                        var.epsilonr4 = Frecuencia_3MHz.epsilon_rHueso;
                        var.sigma2 = Frecuencia_3MHz.sigmaPielHumeda;
                        var.sigma3 = Frecuencia_3MHz.sigmaMusculo;
                        var.sigma4 = Frecuencia_3MHz.sigmaHueso;
                        break;

                    case 1:
                        var.epsilonr2 = Frecuencia_3MHz.epsilon_rPielSeca;
                        var.epsilonr3 = Frecuencia_3MHz.epsilon_rMusculo;
                        var.epsilonr4 = Frecuencia_3MHz.epsilon_rHueso;
                        var.sigma2 = Frecuencia_3MHz.sigmaPielSeca;
                        var.sigma3 = Frecuencia_3MHz.sigmaMusculo;
                        var.sigma4 = Frecuencia_3MHz.sigmaHueso;
                        break;

                    case 2:
                        var.epsilonr2 = Frecuencia_3MHz.epsilon_rPielHumeda;
                        var.epsilonr3 = Frecuencia_3MHz.epsilon_rGrasa;
                        var.epsilonr4 = Frecuencia_3MHz.epsilon_rMusculo;
                        var.sigma2 = Frecuencia_3MHz.sigmaPielHumeda;
                        var.sigma3 = Frecuencia_3MHz.sigmaGrasa;
                        var.sigma4 = Frecuencia_3MHz.sigmaMusculo;
                        break;

                    case 3:
                        var.epsilonr2 = Frecuencia_3MHz.epsilon_rPielSeca;
                        var.epsilonr3 = Frecuencia_3MHz.epsilon_rGrasa;
                        var.epsilonr4 = Frecuencia_3MHz.epsilon_rMusculo;
                        var.sigma2 = Frecuencia_3MHz.sigmaPielSeca;
                        var.sigma3 = Frecuencia_3MHz.sigmaGrasa;
                        var.sigma4 = Frecuencia_3MHz.sigmaMusculo;
                        break;
                }
            }
            else
            {
                var.frecuencia = Frecuencia_30MHz.frecuencia;
                var.frec = "30 MHz";
                switch (seleTipo)
                {
                    case 0:
                        var.epsilonr2 = Frecuencia_30MHz.epsilon_rPielHumeda;
                        var.epsilonr3 = Frecuencia_30MHz.epsilon_rMusculo;
                        var.epsilonr4 = Frecuencia_30MHz.epsilon_rHueso;
                        var.sigma2 = Frecuencia_30MHz.sigmaPielHumeda;
                        var.sigma3 = Frecuencia_30MHz.sigmaMusculo;
                        var.sigma4 = Frecuencia_30MHz.sigmaHueso;
                        break;

                    case 1:
                        var.epsilonr2 = Frecuencia_30MHz.epsilon_rPielSeca;
                        var.epsilonr3 = Frecuencia_30MHz.epsilon_rMusculo;
                        var.epsilonr4 = Frecuencia_30MHz.epsilon_rHueso;
                        var.sigma2 = Frecuencia_30MHz.sigmaPielSeca;
                        var.sigma3 = Frecuencia_30MHz.sigmaMusculo;
                        var.sigma4 = Frecuencia_30MHz.sigmaHueso;
                        break;

                    case 2:
                        var.epsilonr2 = Frecuencia_30MHz.epsilon_rPielHumeda;
                        var.epsilonr3 = Frecuencia_30MHz.epsilon_rGrasa;
                        var.epsilonr4 = Frecuencia_30MHz.epsilon_rMusculo;
                        var.sigma2 = Frecuencia_30MHz.sigmaPielHumeda;
                        var.sigma3 = Frecuencia_30MHz.sigmaGrasa;
                        var.sigma4 = Frecuencia_30MHz.sigmaMusculo;
                        break;

                    case 3:
                        var.epsilonr2 = Frecuencia_30MHz.epsilon_rPielSeca;
                        var.epsilonr3 = Frecuencia_30MHz.epsilon_rGrasa;
                        var.epsilonr4 = Frecuencia_30MHz.epsilon_rMusculo;
                        var.sigma2 = Frecuencia_30MHz.sigmaPielSeca;
                        var.sigma3 = Frecuencia_30MHz.sigmaGrasa;
                        var.sigma4 = Frecuencia_30MHz.sigmaMusculo;
                        break;
                }
            }

            //PARAMETROS NO DEPENDIENTES DE LA FRECUENCIA
            //.....

            var.ObtencionCampoE();
            var.ObtencionQ();
            var.ObtencionT();
        }
    }
}
