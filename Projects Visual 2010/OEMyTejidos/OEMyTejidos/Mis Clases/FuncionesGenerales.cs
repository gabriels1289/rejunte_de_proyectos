using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace OEMyTejidos
{
    public class FuncionesGenerales
    {
        //VARIABLES GLOBALES
        //Variables para Modelos de Tejidos (tipo)
        //delta x de cada tejido expresado en Metros
        public Double deltaxmedio2 = 0.0, deltaxmedio3 = 0.0, deltaxmedio4 = 0.0;

        //Sigmas de cada medio a X frecuencia
        public Double sigma2 = 0.0, sigma3 = 0.0, sigma4 = 0.0;

        //Epsilon r de cada medio a X frecuencia
        public Double epsilonr2 = 0.0, epsilonr3 = 0.0, epsilonr4 = 0.0;

        //defino las variables para almacenar los Ai
        Double a2 = 0.0, a3 = 0.0, a4 = 0.0;

        //defino las variables para almacenar los Gamai
        Double gama2 = 0.0, gama3 = 0.0, gama4 = 0.0;

        //defino las variables para almacenar las longitudes que van a usar B2 , B3 y el Total
        int cantidadMedio2 = 0, cantidadMedio3 = 0, cantidadMedio4 = 0;
        public int cantidadTotal = 0;

        //defino la variable frecuencia en Hz
        public Double frecuencia = 0.0;

        //defino epsilon0 [F/m]
        const Double epsilon0 = 8.85e-12;

        //Strings con el modelo de tejido
        public String modelo = "";
        public String modeloChart = "";
        public String medio2 = "";
        public String medio3 = "";
        public String medio4 = "";

        //String con la frecuencia
        public String frec = "";

        //propiedades de los medios no dependientes de la frecuencia
        public Double conductividadTermicaMedio2 = 0.0;
        public Double densidadTejidoMedio2 = 0.0;
        public Double calorEspecificoMedio2 = 0.0;
        public Double conductividadTermicaMedio3 = 0.0;
        public Double densidadTejidoMedio3 = 0.0;
        public Double calorEspecificoMedio3 = 0.0;
        public Double conductividadTermicaMedio4 = 0.0;
        public Double densidadTejidoMedio4 = 0.0;
        public Double calorEspecificoMedio4 = 0.0;

        Double[][] E = new Double[5][];
        public Double[] Q;
        public Double[][] T = new Double[3][];
        //METODOS	
        public void ObtencionCampoE()
        {
            Calculos.CalculoAiyGamai(epsilonr2, epsilon0, frecuencia, sigma2, ref a2, ref gama2);
            Calculos.CalculoAiyGamai(epsilonr3, epsilonr4, frecuencia, sigma3, ref a3, ref gama3);
            Calculos.CalculoAiyGamai(epsilonr4, epsilonr3, frecuencia, sigma4, ref a4, ref gama4);

            Calculos.CalculosDeLongitudes(deltaxmedio2, deltaxmedio3, deltaxmedio4, ref cantidadMedio2, ref cantidadMedio3, ref cantidadMedio4, ref cantidadTotal);

            Double B2 = 0.0;
            Double B3 = 0.0;
            Double[] Ei = { 0, 0, 0, 0, 0, 0 };

            B2 = Calculos.CalculoBi(gama2, 0, deltaxmedio2);
            B3 = Calculos.CalculoBi(gama3, deltaxmedio2, deltaxmedio2 + deltaxmedio3);

            Calculos.NuevoCalculoMatricial(ref Ei, a2, a3, a4, B2, B3);

            E[0] = new Double[cantidadTotal];
            E[1] = new Double[cantidadTotal];
            E[2] = new Double[cantidadTotal];
            E[3] = new Double[cantidadTotal];
            E[4] = new Double[cantidadTotal];

            Calculos.CalculoCampo(E, cantidadMedio2, cantidadMedio3, cantidadMedio4, Ei, gama2, gama3, gama4, deltaxmedio2, deltaxmedio3, deltaxmedio4);

        }

        public void ObtencionQ()
        {
            Q = new Double[cantidadTotal];
            Calculos.CalculoQ(Q, sigma2, sigma3, sigma4, E, cantidadMedio2, cantidadMedio3, cantidadMedio4);
        }

        public void ObtencionT()
        {
            T[0] = new Double[200];
            T[1] = new Double[200];
            T[2] = new Double[200];
            int indice = 0;
            Calculos.CalculoTemperatura(Q, cantidadMedio2, conductividadTermicaMedio2, calorEspecificoMedio2, densidadTejidoMedio2, ref T[0], ref indice);
            indice += cantidadMedio2;
            Calculos.CalculoTemperatura(Q, cantidadMedio3, conductividadTermicaMedio3, calorEspecificoMedio3, densidadTejidoMedio3, ref T[1], ref indice);
            indice += cantidadMedio3;
            Calculos.CalculoTemperatura(Q, cantidadMedio4, conductividadTermicaMedio4, calorEspecificoMedio4, densidadTejidoMedio4, ref T[2], ref indice);
        }
    }
}
