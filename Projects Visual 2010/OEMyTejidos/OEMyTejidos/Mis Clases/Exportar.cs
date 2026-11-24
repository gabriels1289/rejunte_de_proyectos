using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows;

namespace OEMyTejidos
{
    public class Exportar
    {
        FileStream fs;
        StreamWriter sw;

        public void EscribirArchivo(int opcion, FuncionesGenerales[] variable, bool generalfuncionset)
        {
            Frecuencias frecuencias = new Frecuencias();
            Grasa grasa = new Grasa();
            Hueso hueso = new Hueso();
            Musculo musculo = new Musculo();
            PielHidratada pielHidratada = new PielHidratada();
            PielSeca pielSeca = new PielSeca();
            try
            {
                //Open the File
                string path = Directory.GetCurrentDirectory();
                path += @"\Datos Exportados.csv";
                //string path = System.IO.Path.Combine(Environment.CurrentDirectory, @"\Datos Exportados.csv");
                fs = new FileStream(path, FileMode.OpenOrCreate, FileAccess.Write, FileShare.None);
                sw = new StreamWriter(fs);

                //if(cBGrafico.SelectedIndex == 1) GRAFICO Aumento De Temperatura Vs Tiempo

                switch (opcion)
                {

                    case 0:
                        if (generalfuncionset == false)
                            MessageBox.Show("Primero realize la selección y grafica de datos");
                        else
                            foreach (FuncionesGenerales a in variable)
                            {
                                sw.WriteLine("Absorción Energética Vs Posición");
                                if (a != null)
                                {
                                    Double posicion = 0;
                                    sw.WriteLine("Modelo De Tejido: " + a.modelo);
                                    sw.WriteLine("Capa 1: " + a.medio2);
                                    sw.WriteLine("Capa 2: " + a.medio3);
                                    sw.WriteLine("Capa 3: " + a.medio4);
                                    sw.WriteLine("Frecuencia: " + a.frec);

                                    //agregarle las propiedades de los medios
                                    for (int i = 0; i < a.Q.Length; i++)
                                    {
                                        sw.WriteLine(posicion.ToString() + ";" + a.Q[i].ToString());
                                        posicion += 0.0001;
                                    }
                                    sw.WriteLine(System.Environment.NewLine + System.Environment.NewLine + System.Environment.NewLine);
                                }
                            }
                        break;
                    case 2:
                        //Permitividad Relativa Vs Frecuencia
                        sw.WriteLine("Piel Seca");
                        sw.WriteLine("Frecuencia; Permitividad Relativa");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i] + ";" + pielSeca.relative_permittivity[i]);
                        }
                        sw.WriteLine(System.Environment.NewLine + System.Environment.NewLine + System.Environment.NewLine);
                        sw.WriteLine("Piel Hidratada");
                        sw.WriteLine("Frecuencia; Permitividad Relativa");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + pielHidratada.relative_permittivity[i].ToString());
                        }
                        sw.WriteLine(System.Environment.NewLine + System.Environment.NewLine + System.Environment.NewLine);
                        sw.WriteLine("Grasa");
                        sw.WriteLine("Frecuencia;Permitividad Relativa");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + grasa.relative_permittivity[i].ToString());
                        }
                        sw.WriteLine(System.Environment.NewLine + System.Environment.NewLine + System.Environment.NewLine);
                        sw.WriteLine("Músculo");
                        sw.WriteLine("Frecuencia; Permitividad Relativa");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + musculo.relative_permittivity[i].ToString());
                        }
                        sw.WriteLine(System.Environment.NewLine + System.Environment.NewLine + System.Environment.NewLine);
                        sw.WriteLine("Hueso");
                        sw.WriteLine("Frecuencia; Permitividad Relativa");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + hueso.relative_permittivity[i].ToString());
                        }
                        break;
                    case 3:
                        //Conductividad Vs Frecuencia
                        sw.WriteLine("Piel Seca");
                        sw.WriteLine("Frecuencia; Conductividad");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + pielSeca.conductivity[i].ToString());
                        }
                        sw.WriteLine(System.Environment.NewLine + System.Environment.NewLine + System.Environment.NewLine);
                        sw.WriteLine("Piel Hidratada");
                        sw.WriteLine("Frecuencia; Conductividad");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + pielHidratada.conductivity[i].ToString());
                        }
                        sw.WriteLine(System.Environment.NewLine + System.Environment.NewLine + System.Environment.NewLine);
                        sw.WriteLine("Grasa");
                        sw.WriteLine("Frecuencia; Conductividad");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + grasa.conductivity[i].ToString());
                        }
                        sw.WriteLine(System.Environment.NewLine + System.Environment.NewLine + System.Environment.NewLine);
                        sw.WriteLine("Músculo");
                        sw.WriteLine("Frecuencia; Conductividad");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + musculo.conductivity[i].ToString());
                        }
                        sw.WriteLine(System.Environment.NewLine + System.Environment.NewLine + System.Environment.NewLine);
                        sw.WriteLine("Hueso");
                        sw.WriteLine("Frecuencia; Conductividad");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + hueso.conductivity[i].ToString());
                        }
                        break;

                    case 4:
                        //Tangente De Perdida Vs Frecuencia
                        sw.WriteLine("Piel Seca");
                        sw.WriteLine("Frecuencia; Tangente De Perdida");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + pielSeca.loss_tangent[i].ToString());
                        }
                        sw.WriteLine(System.Environment.NewLine + System.Environment.NewLine + System.Environment.NewLine);
                        sw.WriteLine("Piel Hidratada");
                        sw.WriteLine("Frecuencia; Tangente De Perdida");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + pielHidratada.loss_tangent[i].ToString());
                        }
                        sw.WriteLine(System.Environment.NewLine + System.Environment.NewLine + System.Environment.NewLine);
                        sw.WriteLine("Grasa");
                        sw.WriteLine("Frecuencia; Tangente De Perdida");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + grasa.loss_tangent[i].ToString());
                        }
                        sw.WriteLine(System.Environment.NewLine + System.Environment.NewLine + System.Environment.NewLine);
                        sw.WriteLine("Músculo");
                        sw.WriteLine("Frecuencia; Tangente De Perdida");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + musculo.loss_tangent[i].ToString());
                        }
                        sw.WriteLine(System.Environment.NewLine + System.Environment.NewLine + System.Environment.NewLine);
                        sw.WriteLine("Hueso");
                        sw.WriteLine("Frecuencia; Tangente De Perdida");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + hueso.loss_tangent[i].ToString());
                        }
                        break;

                    case 5:
                        //Longitud De Onda Vs Frecuencia
                        sw.WriteLine("Piel Seca");
                        sw.WriteLine("Frecuencia; Longitud de Onda");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + pielSeca.wavelength[i].ToString());
                        }
                        sw.WriteLine(System.Environment.NewLine + System.Environment.NewLine + System.Environment.NewLine);
                        sw.WriteLine("Piel Hidratada");
                        sw.WriteLine("Frecuencia; Longitud de Onda");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + pielHidratada.wavelength[i].ToString());
                        }
                        sw.WriteLine(System.Environment.NewLine + System.Environment.NewLine + System.Environment.NewLine);
                        sw.WriteLine("Grasa");
                        sw.WriteLine("Frecuencia; Longitud de Onda");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + grasa.wavelength[i].ToString());
                        }
                        sw.WriteLine(System.Environment.NewLine + System.Environment.NewLine + System.Environment.NewLine);
                        sw.WriteLine("Músculo");
                        sw.WriteLine("Frecuencia; Longitud de Onda");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + musculo.wavelength[i].ToString());
                        }
                        sw.WriteLine(System.Environment.NewLine + System.Environment.NewLine + System.Environment.NewLine);
                        sw.WriteLine("Hueso");
                        sw.WriteLine("Frecuencia; Longitud de Onda");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + hueso.wavelength[i].ToString());
                        }
                        break;

                    case 6:
                        //Profundidad De Penetración Vs Frecuencia
                        sw.WriteLine("Piel Seca");
                        sw.WriteLine("Frecuencia; Profundidad De Penetración");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + pielSeca.penetration_depth[i].ToString());
                        }
                        sw.WriteLine(System.Environment.NewLine + System.Environment.NewLine + System.Environment.NewLine);
                        sw.WriteLine("Piel Hidratada");
                        sw.WriteLine("Frecuencia; Profundidad De Penetración");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + pielHidratada.penetration_depth[i].ToString());
                        }
                        sw.WriteLine(System.Environment.NewLine + System.Environment.NewLine + System.Environment.NewLine);
                        sw.WriteLine("Grasa");
                        sw.WriteLine("Frecuencia; Profundidad De Penetración");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + grasa.penetration_depth[i].ToString());
                        }
                        sw.WriteLine(System.Environment.NewLine + System.Environment.NewLine + System.Environment.NewLine);
                        sw.WriteLine("Músculo");
                        sw.WriteLine("Frecuencia; Profundidad De Penetración");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + musculo.penetration_depth[i].ToString());
                        }
                        sw.WriteLine(System.Environment.NewLine + System.Environment.NewLine + System.Environment.NewLine);
                        sw.WriteLine("Hueso");
                        sw.WriteLine("Frecuencia; Profundidad De Penetración");
                        for (int i = 0; i < frecuencias.frecuency.Length; i++)
                        {
                            sw.WriteLine(frecuencias.frecuency[i].ToString() + ";" + hueso.penetration_depth[i].ToString());
                        }
                        break;

                }


                //close the file
                sw.Close();
            }
            catch (Exception e)
            {
                MessageBox.Show("Excepcion: " + e.Message);
            }
        }
    }
}
