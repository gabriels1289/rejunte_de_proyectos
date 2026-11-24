using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace prueba_Texto_a_hexa
{
    class Program
    {
        static void Main(string[] args)
        {
            string path = Directory.GetCurrentDirectory();
            path += @"\datos.dat";
            FileStream fsSource = new FileStream(path,FileMode.Open, FileAccess.Read);
            BinaryReader br = new BinaryReader(fsSource);

            // Read the source file into a byte array.
            byte[] bytes = new byte[fsSource.Length];
            int numBytesToRead = (int)fsSource.Length;
            //int numBytesRead = 0;
            //while (numBytesToRead > 0)
            //{
            //    // Read may return anything from 0 to numBytesToRead.
            //    int n = fsSource.Read(bytes, numBytesRead, numBytesToRead);

            //    // Break when the end of the file is reached.
            //    if (n == 0)
            //        break;

            //    numBytesRead += n;
            //    numBytesToRead -= n;
            //}
            // numBytesToRead = bytes.Length;
            byte letter = 0;
            byte aux = 0;
            byte aux2 = 0;
            int n = 0;
            Console.WriteLine("CANTIDAD DEL ARCHIVO: {0}", fsSource.Length);
            while (n < fsSource.Length)
            {
                letter = br.ReadByte();
                aux = (byte)(letter << 4);
                letter = br.ReadByte();
                aux2 = (byte)(letter << 4);
                //Console.WriteLine("BYTE MSB: {0:X2}", aux);
                //Console.WriteLine("BYTE LSB: {0:X2}", aux2);
                aux = (byte)((byte)aux + (byte)((aux2 & 0xf0 ) >> 4));
                Console.WriteLine("DATO: {0:X2}", aux);             
                n+=2;
            }

            //foreach (byte a in bytes)
            //{
            //    Console.WriteLine("BYTE: {0:X2}", a);
            //}
            br.Close();
            fsSource.Close();
            Console.WriteLine("FIN DEL ENVIO");
            Console.ReadLine();
        }
    }
}
