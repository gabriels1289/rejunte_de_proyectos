using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Impresoras
{
    class Program
    {
        static void Main(string[] args)
        {
            String validation = "001234123412341234";
            String establishment = "Bingo Bahia";
            String location = "Chiclana 250";
            String city = "Bahia Blanca";
            String date = "21/09/2013";
            String time = "09:54:27";
            String ticket = "0027";
            String monto_letras = "cincuenta y dos pesos con treinta y siete centavos";
            String monto_pesos = "52,37";
            String dias_validos = "1";
            String machine = "12";

            Impresora printer = new Impresora();
            printer.imprimirTicketStaticVoiding();
            System.Threading.Thread.Sleep(5000);
            printer.imprimirTicketCashout(validation, establishment, location, city, date, time, ticket, monto_letras, monto_pesos, dias_validos, machine);
            System.Threading.Thread.Sleep(5000);
            printer.imprimirTicketJackpot(validation, establishment, location, city, date, time, ticket, monto_letras, monto_pesos, dias_validos, machine);
            System.Threading.Thread.Sleep(5000);
            printer.imprimirTicketHandpayCashoutReceipt(validation, establishment, location, city, date, time, ticket, monto_letras, monto_pesos, dias_validos, machine);
            System.Threading.Thread.Sleep(5000);
            printer.imprimirTicketCashoutReceipt(validation, establishment, location, city, date, time, ticket, monto_letras, monto_pesos, dias_validos, machine);
            System.Threading.Thread.Sleep(5000);
            printer.imprimirTicketDemo(validation, establishment, location, city, date, time, ticket, monto_letras, monto_pesos, dias_validos, machine);
            System.Threading.Thread.Sleep(5000);
            printer.imprimirTicketJackpotReceipt(validation, establishment, location, city, date, time, ticket, monto_letras, monto_pesos, dias_validos, machine);
            System.Threading.Thread.Sleep(5000);
            printer.imprimirTicketVoid(validation, establishment, location, city, date, time, ticket, monto_letras, monto_pesos, dias_validos, machine);
            System.Threading.Thread.Sleep(5000);
            printer.imprimirTicketVoidedDemo(validation, establishment, location, city, date, time, ticket, monto_letras, monto_pesos, dias_validos, machine);
            System.Threading.Thread.Sleep(5000);
            printer.test();
            Console.WriteLine("TERMINO");
            Console.Read();
        }
    }
}
