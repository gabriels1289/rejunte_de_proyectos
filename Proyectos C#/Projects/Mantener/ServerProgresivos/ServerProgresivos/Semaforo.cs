using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ServerProgresivos
{
    public class Semaforo
    {
        /*
         * Clase para coordinar la actualización de datos para estructuras de multiples hilos
         */
        Boolean estado = true;

        public Semaforo()
        {
            estado = true;
        }

        public Boolean canExecute()
        {
            return estado;
        }

        public void bloquearEjecucion()
        {
            estado = false;
        }

        public void liberarEjecucion()
        {
            estado = true;
        }
    }
}
