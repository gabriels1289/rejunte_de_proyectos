using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;

namespace Impresoras
{
    class PuertoSerie
    {
        //--------------------------------- Field ------------------------------------
        private SerialPort port;
        private int[] data = new int[8];
        private byte[] send = new byte[12];
        private byte[] test = new byte[12];
        private byte[] leer = new byte[12];
        private byte[] borrar = new byte[12];

        /// <summary>
        /// Inicia la comunicacion por el puerto serie
        /// </summary>
        public PuertoSerie()
        {
            //Instantiate the communications Port with some basic settings
            //port = new SerialPort(SerialPort.GetPortNames()[0], 9600, Parity.None, 8, StopBits.One);    
            port = new SerialPort("COM7", 38400, Parity.None, 8, StopBits.One);
            port.Open();    // Open the port for communications
        }

        //-------------------------------- Methods -----------------------------------

        /// <summary>
        /// Envia los datos por el puerto serie
        /// </summary>
        /// <param name="info"></param>
        public void write(byte[] info)
        {
            port.Write(info, 0, info.Length);   // Write a set of bytes
        }

        /// <summary>
        /// Envia los datos por el puerto serie
        /// </summary>
        /// <param name="info"></param>
        public void write(String info)
        {
            port.Write(info);   // Write a set of bytes
        }

        /// <summary>
        /// Envia los datos por el puerto serie
        /// </summary>
        /// <param name="info"></param>
        public void write(Char[] info)
        {
            port.Write(info, 0, info.Length);   // Write a set of bytes
        }

        /// <summary>
        /// Obtiene la cantidad de bytes disponibles en el puerto serie
        /// </summary>
        /// <returns></returns>
        public int bytesAvailable()
        {
            return port.BytesToRead;
        }

        /// <summary>
        /// Lee un caracter del puerto serie
        /// </summary>
        /// <param name="a"></param>
        public void getChar(ref Char a)
        {
            a = (char)port.ReadChar();
        }

        /// <summary>
        /// Lee el puerto serie
        /// </summary>
        /// <returns></returns>
        public String getString()
        {
            //return port.ReadLine();
            return port.ReadExisting();
        }

        /// <summary>
        /// Cierra el puerto serie
        /// </summary>
        public void CerrarPuerto()
        {
            port.Close();   // Close the port
        }

        /// <summary>
        /// Convierte un string a un arreglo de bytes
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        byte[] strToByteArray(string str)
        {
            System.Text.ASCIIEncoding enc = new System.Text.ASCIIEncoding();
            return enc.GetBytes(str);
        }
    }
}

