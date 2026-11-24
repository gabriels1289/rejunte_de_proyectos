using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;
using System.IO;

namespace Impresoras
{
    class Impresora
    {
        //--------------------------------- Field ------------------------------------
        private String buffer;
        private String validation_guion;
        private PuertoSerie puerto;
        private FileStream fs;
        private StreamWriter writer;


        /// <summary>
        /// Costructor de la clase
        /// </summary>
        public Impresora()
        {
            puerto = new PuertoSerie();
        }

        //-------------------------------- Methods -----------------------------------
        /// <summary>
        /// Verifica y archiva el estado de la impresora
        /// <remarks>
        /// Nombre del archivo: EstadoImpresora. Ubicacion: Debug
        /// </remarks>
        /// </summary>
        public void test()
        {

            /*Reply Format:
             *S|<unit_addr>|<soft_ver>|<status_flags1>|<status_flags2>|<status_flags3>|<status_flags4>|<status_flags5>|<temp_numb>|*
             *where:
             *<unit_addr>:  Unit address.Always '0' for point systems
             *<soft_ver>:   Software version installed.Range 9 bytes in the format of xxxxxxxxx where the actual characters wil depend on the
             *              customer's version.This field will not contain the characters *,|,^,or~
             *<status_flags1...5>:  Status flags according table (or function estado)
             *<temp_numb>:  If no system errors reported, this will contain the Template I.D. of the last ^P batch job processed.
             *              If a system error exists, the field will contain the Template number of the last ^P batch job processed
             *              that had an error. Range: 2 bytes 'Px'.A space (20h) character will be used after the P letter if no jobs
             *              have been processed since power-up, a reset, or a clear error status ^C command was received
             */

            //Abro el archivo
            string path = Directory.GetCurrentDirectory();
            fs = new FileStream(path+@"\EstadoImpresora.txt", FileMode.OpenOrCreate, FileAccess.Write, FileShare.None);
            writer = new StreamWriter(fs);

            //Envio el ENQ
            Byte[] CMD = new Byte[] { 0x05 };
            puerto.write(CMD);

            System.Threading.Thread.Sleep(200);

            //Obtengo Respuesta
            Console.WriteLine("VOY A LEER EL PUERTO");
            buffer = puerto.getString();
            Console.WriteLine("LEI DEL PUERTO: {0}", buffer);
            //proceso la respuesta
            writer.WriteLine("unit address: {0}", buffer[3].ToString());
            writer.Write("\nsoftware version: ");
            for (int i = 5; i < 14; i++)
            {
                writer.Write("{0}", buffer[i].ToString());
            }

            writer.Write(System.Environment.NewLine + System.Environment.NewLine + "Status Flags 1: {0}\t Hexa: {1:x}\t", buffer[15].ToString(), buffer[15]);
            convert(buffer[15]);
            imprimir_status(buffer[15], 1);
            writer.Write(System.Environment.NewLine + "Status Flags 2: {0}\t Hexa: {1:x}\t", buffer[17].ToString(), buffer[17]);
            convert(buffer[17]);
            imprimir_status(buffer[17], 2);
            writer.Write(System.Environment.NewLine + "Status Flags 3: {0}\t Hexa: {1:x}\t", buffer[19].ToString(), buffer[19]);
            convert(buffer[19]);
            imprimir_status(buffer[19], 3);
            writer.Write(System.Environment.NewLine + "Status Flags 4: {0}\t Hexa: {1:x}\t", buffer[21].ToString(), buffer[21]);
            convert(buffer[21]);
            imprimir_status(buffer[21], 4);
            writer.Write(System.Environment.NewLine + "Status Flags 5: {0}\t Hexa: {1:x}\t", buffer[23].ToString(), buffer[23]);
            convert(buffer[23]);
            imprimir_status(buffer[23], 5);
            writer.Write(System.Environment.NewLine + "Temp_num: {0}", buffer[26].ToString());
            writer.Close();
        }

        /// <summary>
        /// Imprime un Ticket Cashout
        /// </summary>
        /// <param name="validation"></param>
        /// <param name="establishment"></param>
        /// <param name="location"></param>
        /// <param name="city"></param>
        /// <param name="date"></param>
        /// <param name="time"></param>
        /// <param name="ticket"></param>
        /// <param name="monto_letras"></param>
        /// <param name="monto_pesos"></param>
        /// <param name="dias_validos"></param>
        /// <param name="machine"></param>
        /// <returns></returns>
        public bool imprimirTicketCashout(String validation, String establishment, String location, String city, String date, String time, String ticket, String monto_letras, String monto_pesos, String dias_validos, String machine)
        {

            validation_guion = validation;

            validation_guion = validation_guion.Insert(2, "-");
            validation_guion = validation_guion.Insert(7, "-");
            validation_guion = validation_guion.Insert(12, "-");
            validation_guion = validation_guion.Insert(17, "-");

            String msg1 = "^P|0|1|";

            //armado del mensaje:
            String msg = "";

            msg += msg1;
            msg += validation_guion;
            msg += "|";
            msg += establishment;
            msg += "|";
            msg += location;
            msg += "|";
            msg += city;
            msg += "|||";
            msg += validation_guion;
            msg += "|";
            msg += date;
            msg += "|";
            msg += time;
            msg += "|";
            msg += "TICKET # ";
            msg += ticket;
            msg += "|";
            msg += monto_letras;
            msg += "||";
            msg += "$";
            msg += monto_pesos;
            msg += "||";
            msg += dias_validos;
            msg += " dias";
            msg += "|";
            msg += "MACHINE# ";
            msg += machine;
            msg += "|";
            msg += validation;
            msg += "|^";

            //Envio el comando al puerto serie
            puerto.write(msg);

            //cout << "Cashout Ticket" << endl;
            //cout << "String sent------> " << msg << " (" << strlen(msg) << " bytes)" << endl;
            return true;
        }

        /// <summary>
        /// Imprime un Ticket Jackpot
        /// </summary>
        /// <param name="validation"></param>
        /// <param name="establishment"></param>
        /// <param name="location"></param>
        /// <param name="city"></param>
        /// <param name="date"></param>
        /// <param name="time"></param>
        /// <param name="ticket"></param>
        /// <param name="monto_letras"></param>
        /// <param name="monto_pesos"></param>
        /// <param name="dias_validos"></param>
        /// <param name="machine"></param>
        /// <returns></returns>
        public bool imprimirTicketJackpot(String validation, String establishment, String location, String city, String date, String time, String ticket, String monto_letras, String monto_pesos, String dias_validos, String machine)
        {

            validation_guion = validation;

            validation_guion = validation_guion.Insert(2, "-");
            validation_guion = validation_guion.Insert(7, "-");
            validation_guion = validation_guion.Insert(12, "-");
            validation_guion = validation_guion.Insert(17, "-");

            String msg1 = "^P|1|1|";

            //armado del mensaje:
            String msg = "";
            msg += msg1;
            msg += validation_guion;
            msg += "|";
            msg += establishment;
            msg += "|";
            msg += location;
            msg += "|";
            msg += city;
            msg += "|||";
            msg += validation_guion;
            msg += "|";
            msg += date;
            msg += "|";
            msg += time;
            msg += "|";
            msg += "TICKET # ";
            msg += ticket;
            msg += "|";
            msg += monto_letras;
            msg += "||";
            msg += "$";
            msg += monto_pesos;
            msg += "||";
            msg += dias_validos;
            msg += " dias";
            msg += "|";
            msg += "MACHINE# ";
            msg += machine;
            msg += "|";
            msg += validation;
            msg += "|^";

            //Envio el comando
            puerto.write(msg);

            return true;
        }

        /// <summary>
        /// Imprime un Ticket Demo
        /// </summary>
        /// <param name="validation"></param>
        /// <param name="establishment"></param>
        /// <param name="location"></param>
        /// <param name="city"></param>
        /// <param name="date"></param>
        /// <param name="time"></param>
        /// <param name="ticket"></param>
        /// <param name="monto_letras"></param>
        /// <param name="monto_pesos"></param>
        /// <param name="dias_validos"></param>
        /// <param name="machine"></param>
        /// <returns></returns>
        public bool imprimirTicketDemo(String validation, String establishment, String location, String city, String date, String time, String ticket, String monto_letras, String monto_pesos, String dias_validos, String machine)
        {

            validation_guion = validation;

            validation_guion = validation_guion.Insert(2, "-");
            validation_guion = validation_guion.Insert(7, "-");
            validation_guion = validation_guion.Insert(12, "-");
            validation_guion = validation_guion.Insert(17, "-");

            String msg1 = "^P|2|1|";

            //armado del mensaje:
            String msg = "";
            msg += msg1;
            msg += validation_guion;
            msg += "|";
            msg += establishment;
            msg += "|";
            msg += location;
            msg += "|";
            msg += city;
            msg += "|||";
            msg += validation_guion;
            msg += "|";
            msg += date;
            msg += "|";
            msg += time;
            msg += "|";
            msg += "TICKET # ";
            msg += ticket;
            msg += "|";
            msg += monto_letras;
            msg += "||";
            msg += "$";
            msg += monto_pesos;
            msg += "||";
            msg += dias_validos;
            msg += " dias";
            msg += "|";
            msg += "MACHINE# ";
            msg += machine;
            msg += "|";
            msg += validation;
            msg += "|^";


            //Envio el comando
            puerto.write(msg);

            return true;
        }

        /// <summary>
        /// Imprime un Ticket Void
        /// </summary>
        /// <param name="validation"></param>
        /// <param name="establishment"></param>
        /// <param name="location"></param>
        /// <param name="city"></param>
        /// <param name="date"></param>
        /// <param name="time"></param>
        /// <param name="ticket"></param>
        /// <param name="monto_letras"></param>
        /// <param name="monto_pesos"></param>
        /// <param name="dias_validos"></param>
        /// <param name="machine"></param>
        /// <returns>
        /// </returns>
        public bool imprimirTicketVoid(String validation, String establishment, String location, String city, String date, String time, String ticket, String monto_letras, String monto_pesos, String dias_validos, String machine)
        {

            validation_guion = validation;

            validation_guion = validation_guion.Insert(2, "-");
            validation_guion = validation_guion.Insert(7, "-");
            validation_guion = validation_guion.Insert(12, "-");
            validation_guion = validation_guion.Insert(17, "-");

            String msg1 = "^P|3|1|";

            //armado del mensaje:
            String msg = "";
            msg += msg1;
            msg += establishment;
            msg += "|";
            msg += location;
            msg += "|";
            msg += city;
            msg += "||";
            msg += "$";
            msg += monto_pesos;
            msg += "|";
            msg += monto_letras;
            msg += "||";
            msg += date;
            msg += "|";
            msg += time;
            msg += "|";
            msg += "TICKET # ";
            msg += ticket;
            msg += "||";
            msg += validation_guion;
            msg += "||";
            msg += dias_validos;
            msg += " dias";
            msg += "|";
            msg += "MACHINE# ";
            msg += machine;
            msg += "|";
            msg += validation;
            msg += "|^";

            //Envio el comando
            puerto.write(msg);

            return true;
        }

        /// <summary>
        /// Imprime un Ticket Jackpot Receipt
        /// </summary>
        /// <param name="validation"></param>
        /// <param name="establishment"></param>
        /// <param name="location"></param>
        /// <param name="city"></param>
        /// <param name="date"></param>
        /// <param name="time"></param>
        /// <param name="ticket"></param>
        /// <param name="monto_letras"></param>
        /// <param name="monto_pesos"></param>
        /// <param name="dias_validos"></param>
        /// <param name="machine"></param>
        /// <returns></returns>
        public bool imprimirTicketJackpotReceipt(String validation, String establishment, String location, String city, String date, String time, String ticket, String monto_letras, String monto_pesos, String dias_validos, String machine)
        {

            validation_guion = validation;

            validation_guion = validation_guion.Insert(2, "-");
            validation_guion = validation_guion.Insert(7, "-");
            validation_guion = validation_guion.Insert(12, "-");
            validation_guion = validation_guion.Insert(17, "-");

            String msg1 = "^P|4|1|";

            //armado del mensaje:
            String msg = "";
            msg += msg1;
            msg += validation_guion;
            msg += "|";
            msg += establishment;
            msg += "|";
            msg += location;
            msg += "|";
            msg += city;
            msg += "||";
            msg += "$";
            msg += monto_pesos;
            msg += "|";
            msg += monto_letras;
            msg += "||";
            msg += date;
            msg += "|";
            msg += time;
            msg += "|";
            msg += "TICKET # ";
            msg += ticket;
            msg += "||";
            msg += validation_guion;
            msg += "||";
            msg += dias_validos;
            msg += " dias";
            msg += "|";
            msg += "MACHINE# ";
            msg += machine;
            msg += "|";
            msg += validation;
            msg += "|^";

            //Envio el comando
            puerto.write(msg);

            return true;
        }

        /// <summary>
        /// Imprime un Ticket Voided Demo
        /// </summary>
        /// <param name="validation">
        /// Validation number (ej: 001234123412341234)
        /// </param>
        /// <param name="establishment">
        /// Establecimiento (ej: Bingo Bahia)
        /// </param>
        /// <param name="location">
        /// Locacion (ej: Chiclana 250)
        /// </param>
        /// <param name="city">
        /// Ciudad (ej: Bahia Blanca)
        /// </param>
        /// <param name="date">
        /// Fecha (ej: 21/09/2013)
        /// </param>
        /// <param name="time">
        /// Hora (ej: 09:54:27)
        /// </param>
        /// <param name="ticket">
        /// Numero de ticket (ej: 0027)
        /// </param>
        /// <param name="monto_letras">
        /// Monto en letras (ej: cincuenta y dos pesos con treinta y siete centavos)
        /// </param>
        /// <param name="monto_pesos">
        /// Monto en pesos (ej: 52,37)
        /// </param>
        /// <param name="dias_validos">
        /// Cantidad de dias que es valido el ticket (ej: 1)
        /// </param>
        /// <param name="machine">
        /// Numero de maquina (ej: 12)
        /// </param>
        /// <returns>
        /// Retorna true si tuvo exito
        /// </returns>
        public bool imprimirTicketVoidedDemo(String validation, String establishment, String location, String city, String date, String time, String ticket, String monto_letras, String monto_pesos, String dias_validos, String machine)
        {

            validation_guion = validation;

            validation_guion = validation_guion.Insert(2, "-");
            validation_guion = validation_guion.Insert(7, "-");
            validation_guion = validation_guion.Insert(12, "-");
            validation_guion = validation_guion.Insert(17, "-");

            String msg1 = "^P|5|1|";

            //armado del mensaje:
            String msg = "";
            msg += msg1;
            msg += validation_guion;
            msg += "|";
            msg += establishment;
            msg += "|";
            msg += location;
            msg += "|";
            msg += city;
            msg += "||";
            msg += "$";
            msg += monto_pesos;
            msg += "|";
            msg += monto_letras;
            msg += "||";
            msg += date;
            msg += "|";
            msg += time;
            msg += "|";
            msg += "TICKET # ";
            msg += ticket;
            msg += "||";
            msg += validation_guion;
            msg += "||";
            msg += dias_validos;
            msg += " dias";
            msg += "|";
            msg += "MACHINE# ";
            msg += machine;
            msg += "|";
            msg += validation;
            msg += "|^";

            //Envio el comando
            puerto.write(msg);

            return true;
        }

        /// <summary>
        /// Imprime un Ticket Static Voiding
        /// </summary>
        /// <returns>
        /// Retorna true si tuvo exito
        /// </returns>
        public bool imprimirTicketStaticVoiding()
        {
            String msg1 = "^P|6|1||||||||^";

            //armado del mensaje:
            String msg = "";
            msg += msg1;

            //Envio el comando
            puerto.write(msg);

            return true;
        }

        /// <summary>
        /// Imprime un Ticket Cashout Receipt
        /// </summary>
        /// <param name="validation"></param>
        /// <param name="establishment"></param>
        /// <param name="location"></param>
        /// <param name="city"></param>
        /// <param name="date"></param>
        /// <param name="time"></param>
        /// <param name="ticket"></param>
        /// <param name="monto_letras"></param>
        /// <param name="monto_pesos"></param>
        /// <param name="dias_validos"></param>
        /// <param name="machine"></param>
        /// <returns></returns>
        public bool imprimirTicketCashoutReceipt(String validation, String establishment, String location, String city, String date, String time, String ticket, String monto_letras, String monto_pesos, String dias_validos, String machine)
        {

            validation_guion = validation;

            validation_guion = validation_guion.Insert(2, "-");
            validation_guion = validation_guion.Insert(7, "-");
            validation_guion = validation_guion.Insert(12, "-");
            validation_guion = validation_guion.Insert(17, "-");

            String msg1 = "^P|7|1|";

            //armado del mensaje:
            String msg = "";
            msg += msg1;
            msg += validation_guion;
            msg += "|";
            msg += establishment;
            msg += "|";
            msg += location;
            msg += "|";
            msg += city;
            msg += "|||";
            msg += validation_guion;
            msg += "|";
            msg += date;
            msg += "|";
            msg += time;
            msg += "|";
            msg += "TICKET # ";
            msg += ticket;
            msg += "|";
            msg += monto_letras;
            msg += "||";
            msg += "$";
            msg += monto_pesos;
            msg += "||";
            msg += dias_validos;
            msg += " dias";
            msg += "|";
            msg += "MACHINE# ";
            msg += machine;
            msg += "|";
            msg += validation;
            msg += "|^";

            //Envio el comando
            puerto.write(msg);

            return true;
        }

        /// <summary>
        /// Imprime un Ticket de Handpay Cashout Receipt
        /// </summary>
        /// <param name="validation"></param>
        /// <param name="establishment"></param>
        /// <param name="location"></param>
        /// <param name="city"></param>
        /// <param name="date"></param>
        /// <param name="time"></param>
        /// <param name="ticket"></param>
        /// <param name="monto_letras"></param>
        /// <param name="monto_pesos"></param>
        /// <param name="dias_validos"></param>
        /// <param name="machine"></param>
        /// <returns></returns>
        public bool imprimirTicketHandpayCashoutReceipt(String validation, String establishment, String location, String city, String date, String time, String ticket, String monto_letras, String monto_pesos, String dias_validos, String machine)
        {

            validation_guion = validation;

            validation_guion = validation_guion.Insert(2, "-");
            validation_guion = validation_guion.Insert(7, "-");
            validation_guion = validation_guion.Insert(12, "-");
            validation_guion = validation_guion.Insert(17, "-");

            String msg1 = "^P|8|1|";

            //armado del mensaje:
            String msg = "";
            msg += msg1;
            msg += validation_guion;
            msg += "|";
            msg += establishment;
            msg += "|";
            msg += location;
            msg += "|";
            msg += city;
            msg += "||$";
            msg += monto_pesos;
            msg += "|";
            msg += monto_letras;
            msg += "||";
            msg += date;
            msg += "|";
            msg += time;
            msg += "|TICKET # ";
            msg += ticket;
            msg += "||";
            msg += validation_guion;
            msg += "||";
            msg += dias_validos;
            msg += " dias";
            msg += "|";
            msg += "MACHINE# ";
            msg += machine;
            msg += "|";
            msg += validation;
            msg += "|^";

            //Envio el comando
            puerto.write(msg);

            return true;
        }

        /// <summary>
        /// Convierte a binario el flag de estado
        /// </summary>
        /// <param name="flag_estado"></param>
        private void convert(char flag_estado)
        {
            int i;
            UInt16 status_flag = flag_estado;
            writer.Write("En Binario: ");
            for (i = 7; i >= 0; i--)
            {
                //writer.Write("{%c0}", (status_flag & (1 << i)) ? '1' : '0');
                writer.Write("{0}", Extension.GetBit(status_flag, i) ? '1' : '0');
            }
        }

        /// <summary>
        /// Obtiene el estado de cada bit de los flags
        /// </summary>
        /// <param name="flag_estado"></param>
        /// <param name="numero_flag"></param>
        private void imprimir_status(char flag_estado, int numero_flag)
        {
            UInt16 status_flag = flag_estado;

            //Extension.GetBit(status_flag, 14) ? estado(numero_flag, 5, true) : estado(numero_flag, 5, false);
            if (Extension.GetBit(status_flag, 14))
                estado(numero_flag, 5, true);
            else
                estado(numero_flag, 5, false);
            if (Extension.GetBit(status_flag, 13))
                estado(numero_flag, 4, true);
            else
                estado(numero_flag, 4, false);
            if (Extension.GetBit(status_flag, 12))
                estado(numero_flag, 3, true);
            else
                estado(numero_flag, 3, false);
            if (Extension.GetBit(status_flag, 11))
                estado(numero_flag, 2, true);
            else
                estado(numero_flag, 2, false);
            if (Extension.GetBit(status_flag, 10))
                estado(numero_flag, 1, true);
            else
                estado(numero_flag, 1, false);
            if (Extension.GetBit(status_flag, 1))
                estado(numero_flag, 0, true);
            else
                estado(numero_flag, 0, false);
        }

        /// <summary>
        /// Imprime en el archivo el estado correspondiente de cada bit
        /// </summary>
        /// <param name="numero_flag"></param>
        /// <param name="numero_bit"></param>
        /// <param name="x"></param>
        private void estado(int numero_flag, int numero_bit, bool x)
        {
            if (numero_flag == 1)
            {
                if (numero_bit == 5)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tIdle");
                    else
                        writer.Write(System.Environment.NewLine + "\tBusy processing print or command");
                }
                if (numero_bit == 4)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tSystem ok");
                    else
                        writer.Write(System.Environment.NewLine + "\tSystem error");
                }
                if (numero_bit == 3)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tPlaten engaged");
                    else
                        writer.Write(System.Environment.NewLine + "\tPlaten up");
                }
                if (numero_bit == 2)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tPaper loaded");
                    else
                        writer.Write(System.Environment.NewLine + "\tPaper out");
                }
                if (numero_bit == 1)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tHead ok");
                    else
                        writer.Write(System.Environment.NewLine + "\tHead error");
                }
                if (numero_bit == 0)
                {
                    if (x == false)
                        writer.WriteLine(System.Environment.NewLine + "\tVolatge ok");
                    else
                        writer.WriteLine(System.Environment.NewLine + "\tVoltage error");
                }
            }

            if (numero_flag == 2)
            {
                if (numero_bit == 5)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tTemperature ok");
                    else
                        writer.Write(System.Environment.NewLine + "\tTemperature error");
                }
                if (numero_bit == 4)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tLibrary refs ok");
                    else
                        writer.Write(System.Environment.NewLine + "\tLibrary ref error");
                }
                if (numero_bit == 3)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tPR data ok");
                    else
                        writer.Write(System.Environment.NewLine + "\tPR data error");
                }
                if (numero_bit == 2)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tLibrary load ok");
                    else
                        writer.Write(System.Environment.NewLine + "\tLibrary load error");
                }
                if (numero_bit == 1)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tBuffer space ok");
                    else
                        writer.Write(System.Environment.NewLine + "\tBuffer overflow");
                }
                if (numero_bit == 0)
                {
                    if (x == false)
                        writer.WriteLine(System.Environment.NewLine + "\tJob memory ok");
                    else
                        writer.WriteLine(System.Environment.NewLine + "\tJob memory overflow");
                }
            }

            if (numero_flag == 3)
            {
                if (numero_bit == 5)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tCommand ok");
                    else
                        writer.Write(System.Environment.NewLine + "\tCommand error");
                }
                if (numero_bit == 4)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tPrint libraries ok");
                    else
                        writer.Write(System.Environment.NewLine + "\tPrint libraries corrupted");
                }
                if (numero_bit == 3)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tChute clear");
                    else
                        writer.Write(System.Environment.NewLine + "\tPaper in chute");
                }
                if (numero_bit == 2)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tFlash ok");
                    else
                        writer.Write(System.Environment.NewLine + "\tFlash prog error");
                }
                if (numero_bit == 1)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tPrinter online");
                    else
                        writer.Write(System.Environment.NewLine + "\tPrinter offline");
                }
                if (numero_bit == 0)
                {
                    if (x == false)
                        writer.WriteLine(System.Environment.NewLine + "\tSupply index found");
                    else
                        writer.WriteLine(System.Environment.NewLine + "\tMissing supply index");
                }
            }

            if (numero_flag == 4)
            {
                if (numero_bit == 5)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tX");
                    else
                        writer.Write(System.Environment.NewLine + "\tX");
                }
                if (numero_bit == 4)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tX");
                    else
                        writer.Write(System.Environment.NewLine + "\tX");
                }
                if (numero_bit == 3)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tTCL print mode");
                    else
                        writer.Write(System.Environment.NewLine + "\tJournal printing");
                }
                if (numero_bit == 2)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tCutter ok");
                    else
                        writer.Write(System.Environment.NewLine + "\tCutter error");
                }
                if (numero_bit == 1)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tPaper path ok");
                    else
                        writer.Write(System.Environment.NewLine + "\tPaper jam may exist");
                }
                if (numero_bit == 0)
                {
                    if (x == false)
                        writer.WriteLine(System.Environment.NewLine + "\tPaper full");
                    else
                        writer.WriteLine(System.Environment.NewLine + "\tPaper low");
                }
            }

            if (numero_flag == 5)
            {
                if (numero_bit == 5)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tValidation # clear");
                    else
                        writer.Write(System.Environment.NewLine + "\tValidation # done");
                }
                if (numero_bit == 4)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tNot at top of form");
                    else
                        writer.Write(System.Environment.NewLine + "\tAt top of form");
                }
                if (numero_bit == 3)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tReady to RX");
                    else
                        writer.Write(System.Environment.NewLine + "\tXOFF");
                }
                if (numero_bit == 2)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tPrinter pocketed");
                    else
                        writer.Write(System.Environment.NewLine + "\tPrinter open");
                }
                if (numero_bit == 1)
                {
                    if (x == false)
                        writer.Write(System.Environment.NewLine + "\tClear");
                    else
                        writer.Write(System.Environment.NewLine + "\tBarcode Data Is Accessed");
                }
                if (numero_bit == 0)
                {
                    if (x == false)
                        writer.WriteLine(System.Environment.NewLine + "\tClear");
                    else
                        writer.WriteLine(System.Environment.NewLine + "\tReset/Power up");
                }
            }
        }

    }
}