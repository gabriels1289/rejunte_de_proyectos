using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace OEMyTejidos
{
    class Calculos
    {
        //Defino el valor del campo a partir de la densidad de potencia
        const Double potencia = 307.00162866; //Math.Sqrt(250*377)

        //defino la constante de paso en metros
        private const Double paso = 0.0001;

        //defino epsilon0 [F/m]
        private const Double epsilon0 = 8.85e-12;

        //defino mu0 [N/A^2]
        private const Double mu = 4e-7 * Math.PI;

        //defino las variables para almacenar los Ai
        //Double a2 = 0.0, a3 = 0.0, a4 = 0.0;

        //defino las variables para almacenar los Gamai
        //Double gama2 = 0.0, gama3 = 0.0;

        //defino las variables para almacenar las longitudes que van a usar B2 , B3 y el Total
        //int longitudB2 = 0, longitudB3 = 0, longitudTotal=0;

        //defino la variable frecuencia en radianes
        //Double omega = 2 * Math.PI * 3e6;

        //metodo para calcular el Ai y el gamai
        static public void CalculoAiyGamai(Double epsilonrI, Double epsilonrI_1, Double frecuencia, Double SigmaI, ref Double Ai, ref Double GamaI)
        {
            Double omega = 2 * Math.PI * frecuencia;
            Double epsilonI = epsilonrI * epsilon0;
            Double epsilonI_1 = 0;
            if (epsilonrI_1 != epsilon0)
                epsilonI_1 = epsilonrI * epsilon0;
            else
                epsilonI_1 = epsilon0;
            Ai = (Math.Sqrt(mu / epsilonI)) / (Math.Sqrt(mu / epsilonI_1));
            GamaI = -1 / Math.Sqrt(2 / (omega * mu * SigmaI));
        }

        static public void CalculosDeLongitudes(Double deltaxmedio2, Double deltaxmedio3, Double deltaxmedio4, ref int cantidadMedio2, ref int cantidadMedio3, ref int cantidadMedio4, ref int cantidadTotal)
        {
            cantidadMedio2 = (int)Math.Round(deltaxmedio2 / paso);
            cantidadMedio3 = (int)Math.Round(deltaxmedio3 / paso);
            cantidadMedio4 = (int)Math.Round(deltaxmedio4 / paso);
            cantidadTotal = cantidadMedio2 + cantidadMedio3 + cantidadMedio4;
        }

        static public Double CalculoBi(Double GamaI, Double distancia_al_medio_anterior, Double coordenada_de_referencia)
        {
            return Math.Exp(GamaI * (coordenada_de_referencia - distancia_al_medio_anterior));
        }


        static public void NuevoCalculoMatricial(ref Double[] E, Double A2, Double A3, Double A4, Double B2, Double B3)
        {
            E[0] = 0;
            E[1] = 0;
            E[2] = 0;
            E[3] = 0;
            E[4] = 0;
            E[5] = 0;
            Double[,] matriz = new Double[6, 7] {    {1  ,-1  ,1  ,0   ,0  ,0  ,-1}, 
                                                     {-1 ,-A2 ,A2 ,0   ,0  ,0  ,-1},
                                                     {0  ,0   ,0  ,-1  ,-1 ,0  ,0},
                                                     {0  ,0   ,0  ,-A3 ,A3 ,0  ,0},
                                                     {0  ,0   ,0  ,0   ,0  ,-1 ,0},
                                                     {0  ,0   ,0  ,0   ,0  ,A4 ,0} };
            matriz[2, 1] = B2;
            matriz[2, 2] = 1 / B2;
            matriz[3, 1] = B2;
            matriz[3, 2] = -1 / B2;
            matriz[4, 3] = B3;
            matriz[4, 4] = 1 / B3;
            matriz[5, 3] = B3;
            matriz[5, 4] = -1 / B3;

            LinearEquationsSolver.GaussianElimination(matriz, E);
        }

        static public void CalculoQ(Double[] Q, Double Sigma2, Double Sigma3, Double Sigma4, Double[][] E, int cantidadMedio2, int cantidadMedio3, int cantidadMedio4)
        {
            int n = 0;
            for (int i = 0; i < cantidadMedio2; i++)
            {
                Q[n] = Math.Pow(Math.Abs(E[0][i] + E[1][i]), 2) * Sigma2 / 2;
                n++;
            }

            for (int i = 0; i < cantidadMedio3; i++)
            {
                Q[n] = Math.Pow(Math.Abs(E[2][i] + E[3][i]), 2) * Sigma3 / 2;
                n++;
            }

            for (int i = 0; i < cantidadMedio4; i++)
            {
                Q[n] = Math.Pow(Math.Abs(E[4][i]), 2) * Sigma4 / 2;
                n++;
            }
        }

        static public void CalculoCampo(Double[][] E, int cantidadMedio2, int cantidadMedio3, int cantidadMedio4, Double[] Ei, Double Gama2, Double Gama3, Double Gama4,
            Double deltaxmedio2, Double deltaxmedio3, Double deltaxmedio4)
        {
            Double aux = 0;
            //E2,0
            for (int i = 0; i < cantidadMedio2; i++)
            {
                E[0][i] = potencia * Ei[1] * Math.Exp(-Gama2 * (deltaxmedio2 - aux));
                aux += paso;
            }
            //E2,1
            aux = 0;
            for (int i = 0; i < cantidadMedio2; i++)
            {
                E[1][i] = potencia * Ei[2] * Math.Exp(-Gama2 * (deltaxmedio2 - aux));
                aux += paso;
            }

            //E3,0
            aux = 0;
            for (int i = 0; i < cantidadMedio3; i++)
            {
                E[2][i] = potencia * Ei[3] * Math.Exp(-Gama3 * (deltaxmedio3 - aux));
                aux += paso;
            }
            //E3,1
            aux = 0;
            for (int i = 0; i < cantidadMedio3; i++)
            {
                E[3][i] = potencia * Ei[4] * Math.Exp(-Gama3 * (deltaxmedio3 - aux));
                aux += paso;
            }

            //E4,0
            aux = 0;
            for (int i = 0; i < cantidadMedio4; i++)
            {
                E[4][i] = potencia * Ei[5] * Math.Exp(-Gama4 * (deltaxmedio4 - aux));
                aux += paso;
            }

        }

        static public void CalculoTemperatura(Double[] Q, int cantidadMedio, Double conductividadTermica, Double calorEspecifico, Double densidadTejido, ref Double[] TFinal, ref int index)
        {
            //para definir la cantidad de filas y columnas
            const int filas = 200;

            Double perfusionSanguinea = 0.0036;
            Double calorEspecificoSangre = 0.906;
            Double temperaturaArterial = 311;

            Double deltaxCuadrado = Math.Pow(paso, 2);
            const Double deltaTiempo = 0.1;

            //calculo las constantes
            Double cte1 = 1 / (densidadTejido * calorEspecifico);
            Double cte2 = (perfusionSanguinea * calorEspecificoSangre) / (densidadTejido * calorEspecifico);
            Double cte3 = conductividadTermica / (densidadTejido * calorEspecifico);

            //caso Medio2
            int columnas = cantidadMedio;
            Double[,] T = new Double[filas, columnas];

            //T[n + 1, j] = ((cte1 * Q[index+j] - cte2 * (T[n, j] - temperaturaArterial) + cte3 * (T[n, j + 1] + T[n, j - 1] - 2 * T[n, j]) / deltaxCuadrado) * deltaTiempo) + T[n, j];

            for (int n = 0; n < filas - 1; n++)
            {
                for (int j = 0; j < columnas; j++)
                {
                    if (n == 0)
                    {
                        T[0, j] = 310;
                        T[1, j] = 310;
                    }
                    else
                    {
                        if (j == 0)
                        {
                            //   T[n, j - 1] = T[n-1, j+1];
                            T[n + 1, j] = ((cte1 * Q[index + j] - cte2 * (T[n, j] - temperaturaArterial) + cte3 * (T[n, j + 1] + T[n - 1, j + 1] - 2 * T[n, j]) / deltaxCuadrado) * deltaTiempo) + T[n, j];
                        }
                        else if (j == (columnas-1))
                        {
                            //T[n, j + 1] = T[n,j];
                            T[n + 1, j] = ((cte1 * Q[index + j] - cte2 * (T[n, j] - temperaturaArterial) + cte3 * (T[n, j] + T[n, j - 1] - 2 * T[n, j]) / deltaxCuadrado) * deltaTiempo) + T[n, j];
                        }
                        else if (j < (columnas-1))
                            T[n + 1, j] = ((cte1 * Q[index + j] - cte2 * (T[n, j] - temperaturaArterial) + cte3 * (T[n, j + 1] + T[n, j - 1] - 2 * T[n, j]) / deltaxCuadrado) * deltaTiempo) + T[n, j];
                    }
                }
            }

            for (int t = 0; t < filas; t++)
            {
                for (int j = 0; j < columnas; j++)
                {
                    TFinal[t] += T[t, j];
                }
                TFinal[t] /= columnas;
            }

        }
    }
}
