using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Microsoft.Windows.Controls;
using System.Windows.Controls.DataVisualization.Charting;

namespace OEMyTejidos
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        FuncionesGenerales[] varia = new FuncionesGenerales[8];
        bool generalfuncionset = false;

        public MainWindow()
        {
            InitializeComponent();
            this.WindowStartupLocation = System.Windows.WindowStartupLocation.CenterScreen;
            MessageBox.Show("UTN - FRBB\nDepartamento De Ingenieria Electrónica\n\nProfesora: Patricia Baldini\nAlumno: Gabriel Schvert",
                "Final Medios De Enlace", System.Windows.MessageBoxButton.OK, System.Windows.MessageBoxImage.Information);
        }

        private void button1_Click(object sender, RoutedEventArgs e)
        {
            chartCurva.Title = "";
            chartCurva.Visibility = System.Windows.Visibility.Hidden;
            for (int i = 0; i < 8; i++)
            {
                varia[i] = null;
            }
            chartCurva.Series.Clear();

            if (cBGrafico.SelectedItem != null)
            {
                //if (cBTipoModelo.SelectedItem != null || cBFrecuencia.SelectedItem != null && ( cBGrafico.SelectedIndex == 0 || cBGrafico.SelectedIndex == 1))

                if (cBGrafico.SelectedIndex == 0 || cBGrafico.SelectedIndex == 1)
                {
                    //if (cBTipoModelo.SelectedItem != null && cBFrecuencia.SelectedItem != null) //Absorción Energética Vs Posición
                    if (((bool)chkBTipo1a.IsChecked == true || (bool)chkBTipo1b.IsChecked == true || (bool)chkBTipo2a.IsChecked == true || (bool)chkBTipo2b.IsChecked) && 
                        ((bool)chkB3MHz.IsChecked == true || (bool)chkB30MHz.IsChecked == true))
                    {
                        bool[] TiposSeteados = new bool[4];
                        generalfuncionset = true;
                        TiposSeteados[0] = (bool)chkBTipo1a.IsChecked;
                        TiposSeteados[1] = (bool)chkBTipo1b.IsChecked;
                        TiposSeteados[2] = (bool)chkBTipo2a.IsChecked;
                        TiposSeteados[3] = (bool)chkBTipo2b.IsChecked;

                        bool[] FrecuenciasSeteadas = new bool[2];

                        FrecuenciasSeteadas[0] = (bool)chkB3MHz.IsChecked;
                        FrecuenciasSeteadas[1] = (bool)chkB30MHz.IsChecked;
                        int tipo = 0, frec = 0, posicion = 0;
                        for (int i = 0; i < TiposSeteados.Length; i++)
                        {
                            for (int j = 0; j < FrecuenciasSeteadas.Length; j++)
                            {
                                if (TiposSeteados[i] == true && FrecuenciasSeteadas[j] == true)
                                {
                                    tipo = i;
                                    frec = j;
                                    Inicializa.InicializarVariables(tipo, frec, ref varia[posicion]);
                                }

                                posicion++;
                            }
                        }
                        if (cBGrafico.SelectedIndex == 0)//Absorción Energética Vs Posición
                        {
                        curvas(varia, cBGrafico.SelectedIndex);
                        chartCurva.Visibility = System.Windows.Visibility.Visible;
                        }
                        else if (cBGrafico.SelectedIndex == 1)//Aumento De Temperatura Vs Tiempo
                        {
                            //MessageBox.Show("UPS.. Lo sentimos! Esto es embarazoso.\n Por el momento esta función no esta Diponible", "Aumento De Temperatura Vs Tiempo");
                            curvas(varia, cBGrafico.SelectedIndex);
                            chartCurva.Visibility = System.Windows.Visibility.Visible;
                        }
                    }
                    else
                        MessageBox.Show("Seleccione al menos un item por cada Selector");
                }
                                
                else if (cBGrafico.SelectedIndex == 2)//Permitividad Relativa Vs Frecuencia
                {
                    for (int indice = 0; indice < 5; indice++)
                    {
                        ShowChartPermitividadVsFrecuencia(indice);
                    }
                    chartCurva.Visibility = System.Windows.Visibility.Visible;
                }
                else if (cBGrafico.SelectedIndex == 3)//Conductividad Vs Frecuencia
                {
                    for (int indice = 0; indice < 5; indice++)
                    {
                        ShowChartConductividadVsFrecuencia(indice);
                    }
                    chartCurva.Visibility = System.Windows.Visibility.Visible;
                }
                else if (cBGrafico.SelectedIndex == 4)//Tangente De Perdida Vs Frecuencia
                {
                    for (int indice = 0; indice < 5; indice++)
                    {
                        ShowChartTangenteVsFrecuencia(indice);
                    }
                    chartCurva.Visibility = System.Windows.Visibility.Visible;
                }
                else if (cBGrafico.SelectedIndex == 5)//Longitud De Onda Vs Frecuencia
                {
                    for (int indice = 0; indice < 5; indice++)
                    {
                        ShowChartLongitudVsFrecuencia(indice);
                    }
                    chartCurva.Visibility = System.Windows.Visibility.Visible;
                }
                else if (cBGrafico.SelectedIndex == 6)//Profundidad De Penetración Vs Frecuencia
                {
                    for (int indice = 0; indice < 5; indice++)
                    {
                        ShowChartProfundidadVsFrecuencia(indice);
                    }
                    chartCurva.Visibility = System.Windows.Visibility.Visible;
                }
                else MessageBox.Show("HOUSTON WE HAVE A PROBLEM!!");

                //chartCurva.Visibility = System.Windows.Visibility.Visible;
            }
            else
                MessageBox.Show("Seleccione al menos un item para Graficar");
        }

        private void bTBorrar_Click(object sender, RoutedEventArgs e)
        {
            chartCurva.Title = "";
            chartCurva.Visibility = System.Windows.Visibility.Hidden;
            for (int i = 0; i < 8; i++)
            {
                varia[i] = null;
            }
            chartCurva.Series.Clear();
        }

        private void bTExportar_Click(object sender, RoutedEventArgs e)
        {
            if (cBGrafico.SelectedItem != null)
            {
                if (cBGrafico.SelectedIndex != 0 || cBGrafico.SelectedIndex != 1)
                {
                    Exportar exp = new Exportar();
                    exp.EscribirArchivo(cBGrafico.SelectedIndex, varia, generalfuncionset);
                }
                else if (cBTipoModelo.SelectedItem != null || cBFrecuencia.SelectedItem != null || cBGrafico.SelectedItem != null)
                {
                    Exportar exp = new Exportar();
                    exp.EscribirArchivo(cBGrafico.SelectedIndex, varia, generalfuncionset);
                }

                else
                    MessageBox.Show("Seleccione al menos un valor por cada Selector");
            }
        }

        private void curvas(FuncionesGenerales[] variable, int opcion)
        {
            //List<KeyValuePair<Double, Double>> valueList = new List<KeyValuePair<double,double>>();

            //Double paso = 0.0;
            int indice = 0;

            if (opcion == 0)
            {
                chartCurva.Title = "Absorción Energética Vs Posición";
                for (int i = 0; i < variable.Length; i++)
                {
                    if (variable[i] != null)
                    {
                        ShowChart(variable[i], indice);
                        indice++;
                    }
                }
            }
            else if (opcion == 1)
            { 
                //MessageBox.Show("Por el Momento esta opción no esta implementada");
                chartCurva.Title = "Aumento De Temperatura Vs Tiempo";
                for (int i = 0; i < variable.Length; i++)
                {
                    if (variable[i] != null)
                    {
                        ShowChart2(variable[i], indice);
                        indice++;
                    }
                }
            }
            
        }

        /// <summary>
        /// Gets the new data point style.
        /// </summary>
        /// <returns></returns>
        private static Style GetNewDataPointStyle()
        {
            Random random = new Random();
            int rnd = random.Next(0, 256);
            int rnd2 = random.Next(0, 256);
            int rnd3 = random.Next(0, 256);

            Color background = Color.FromRgb((Byte)rnd, (Byte)rnd2, (Byte)rnd3);
            Style style = new Style(typeof(DataPoint));
            Setter st1 = new Setter(DataPoint.BackgroundProperty, new SolidColorBrush(background));
            Setter st2 = new Setter(DataPoint.BorderBrushProperty, new SolidColorBrush(Colors.White));
            Setter st3 = new Setter(DataPoint.BorderThicknessProperty, new Thickness(0.1));

            Setter st4 = new Setter(DataPoint.TemplateProperty, null);
            style.Setters.Add(st1);
            style.Setters.Add(st2);
            style.Setters.Add(st3);
            style.Setters.Add(st4);
            return style;
        }

        private static Style GetNewDataPointStyleBlue()
        {
            Color background = Color.FromRgb(0x00, 0x00, 0xff);
            Style style = new Style(typeof(DataPoint));
            Setter st1 = new Setter(DataPoint.BackgroundProperty, new SolidColorBrush(background));
            Setter st2 = new Setter(DataPoint.BorderBrushProperty, new SolidColorBrush(Colors.White));
            Setter st3 = new Setter(DataPoint.BorderThicknessProperty, new Thickness(0.1));

            Setter st4 = new Setter(DataPoint.TemplateProperty, null);
            style.Setters.Add(st1);
            style.Setters.Add(st2);
            style.Setters.Add(st3);
            style.Setters.Add(st4);
            return style;
        }

        private static Style GetNewDataPointStyleRed()
        {
            Color background = Color.FromRgb(0xff, 0x00, 0x00);
            Style style = new Style(typeof(DataPoint));
            Setter st1 = new Setter(DataPoint.BackgroundProperty, new SolidColorBrush(background));
            Setter st2 = new Setter(DataPoint.BorderBrushProperty, new SolidColorBrush(Colors.White));
            Setter st3 = new Setter(DataPoint.BorderThicknessProperty, new Thickness(0.1));

            Setter st4 = new Setter(DataPoint.TemplateProperty, null);
            style.Setters.Add(st1);
            style.Setters.Add(st2);
            style.Setters.Add(st3);
            style.Setters.Add(st4);
            return style;
        }

        private static Style GetNewDataPointStyleGreen()
        {
            Color background = Color.FromRgb(0x00, 0xff, 0x00);
            Style style = new Style(typeof(DataPoint));
            Setter st1 = new Setter(DataPoint.BackgroundProperty, new SolidColorBrush(background));
            Setter st2 = new Setter(DataPoint.BorderBrushProperty, new SolidColorBrush(Colors.White));
            Setter st3 = new Setter(DataPoint.BorderThicknessProperty, new Thickness(0.1));

            Setter st4 = new Setter(DataPoint.TemplateProperty, null);
            style.Setters.Add(st1);
            style.Setters.Add(st2);
            style.Setters.Add(st3);
            style.Setters.Add(st4);
            return style;
        }

        private static Style GetNewDataPointStyleBlack()
        {
            Color background = Color.FromRgb(0x00, 0x00, 0x00);
            Style style = new Style(typeof(DataPoint));
            Setter st1 = new Setter(DataPoint.BackgroundProperty, new SolidColorBrush(background));
            Setter st2 = new Setter(DataPoint.BorderBrushProperty, new SolidColorBrush(Colors.White));
            Setter st3 = new Setter(DataPoint.BorderThicknessProperty, new Thickness(0.1));

            Setter st4 = new Setter(DataPoint.TemplateProperty, null);
            style.Setters.Add(st1);
            style.Setters.Add(st2);
            style.Setters.Add(st3);
            style.Setters.Add(st4);
            return style;
        }

        private static Style GetNewDataPointStyleYellow()
        {
            Color background = Color.FromRgb(0xff, 0xff, 0x00);
            Style style = new Style(typeof(DataPoint));
            Setter st1 = new Setter(DataPoint.BackgroundProperty, new SolidColorBrush(background));
            Setter st2 = new Setter(DataPoint.BorderBrushProperty, new SolidColorBrush(Colors.White));
            Setter st3 = new Setter(DataPoint.BorderThicknessProperty, new Thickness(0.1));

            Setter st4 = new Setter(DataPoint.TemplateProperty, null);
            style.Setters.Add(st1);
            style.Setters.Add(st2);
            style.Setters.Add(st3);
            style.Setters.Add(st4);
            return style;
        }

        private static Style GetNewDataPointStyleAqua()
        {
            Color background = Color.FromRgb(0x00, 0xff, 0xff);
            Style style = new Style(typeof(DataPoint));
            Setter st1 = new Setter(DataPoint.BackgroundProperty, new SolidColorBrush(background));
            Setter st2 = new Setter(DataPoint.BorderBrushProperty, new SolidColorBrush(Colors.White));
            Setter st3 = new Setter(DataPoint.BorderThicknessProperty, new Thickness(0.1));

            Setter st4 = new Setter(DataPoint.TemplateProperty, null);
            style.Setters.Add(st1);
            style.Setters.Add(st2);
            style.Setters.Add(st3);
            style.Setters.Add(st4);
            return style;
        }

        private static Style GetNewDataPointStyleOrange()
        {
            Color background = Color.FromRgb(0xff, 0xcc, 0x00);
            Style style = new Style(typeof(DataPoint));
            Setter st1 = new Setter(DataPoint.BackgroundProperty, new SolidColorBrush(background));
            Setter st2 = new Setter(DataPoint.BorderBrushProperty, new SolidColorBrush(Colors.White));
            Setter st3 = new Setter(DataPoint.BorderThicknessProperty, new Thickness(0.1));

            Setter st4 = new Setter(DataPoint.TemplateProperty, null);
            style.Setters.Add(st1);
            style.Setters.Add(st2);
            style.Setters.Add(st3);
            style.Setters.Add(st4);
            return style;
        }

        private static Style GetNewDataPointStylePink()
        {
            Color background = Color.FromRgb(0xff, 0x33, 0xcc);
            Style style = new Style(typeof(DataPoint));
            Setter st1 = new Setter(DataPoint.BackgroundProperty, new SolidColorBrush(background));
            Setter st2 = new Setter(DataPoint.BorderBrushProperty, new SolidColorBrush(Colors.White));
            Setter st3 = new Setter(DataPoint.BorderThicknessProperty, new Thickness(0.1));

            Setter st4 = new Setter(DataPoint.TemplateProperty, null);
            style.Setters.Add(st1);
            style.Setters.Add(st2);
            style.Setters.Add(st3);
            style.Setters.Add(st4);
            return style;
        }

        //para el grafico de Aumento de temperatura Vs Posicion
        private void ShowChart(FuncionesGenerales variable, int index)
        {
            List<KeyValuePair<Double, Double>> valueList0 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList1 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList2 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList3 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList4 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList5 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList6 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList7 = new List<KeyValuePair<double, double>>();
            Double paso = 0.0;

            LineSeries lineSeries = new LineSeries();
            lineSeries.Title = variable.modeloChart + " - " + variable.frec;
            lineSeries.DependentValuePath = "Value";
            lineSeries.IndependentValuePath = "Key";

            if (index == 0)
            {
                foreach (Double j in variable.Q)
                {
                    valueList0.Add(new KeyValuePair<Double, Double>(paso, j));
                    paso += 0.1;
                }
                lineSeries.ItemsSource = valueList0;
                Style dataPointStyle = GetNewDataPointStyleRed();
                lineSeries.DataPointStyle = dataPointStyle;
            }
            else if (index == 1)
            {
                foreach (Double j in variable.Q)
                {
                    valueList1.Add(new KeyValuePair<Double, Double>(paso, j));
                    paso += 0.1;
                }
                lineSeries.ItemsSource = valueList1;
                Style dataPointStyle = GetNewDataPointStyleGreen();
                lineSeries.DataPointStyle = dataPointStyle;
            }
            else if (index == 2)
            {
                foreach (Double j in variable.Q)
                {
                    valueList2.Add(new KeyValuePair<Double, Double>(paso, j));
                    paso += 0.1;
                }
                lineSeries.ItemsSource = valueList2;
                Style dataPointStyle = GetNewDataPointStyleBlue();
                lineSeries.DataPointStyle = dataPointStyle;
            }
            else if (index == 3)
            {
                foreach (Double j in variable.Q)
                {
                    valueList3.Add(new KeyValuePair<Double, Double>(paso, j));
                    paso += 0.1;
                }
                lineSeries.ItemsSource = valueList3;
                Style dataPointStyle = GetNewDataPointStyleAqua();
                lineSeries.DataPointStyle = dataPointStyle;
            }
            else if (index == 4)
            {
                foreach (Double j in variable.Q)
                {
                    valueList4.Add(new KeyValuePair<Double, Double>(paso, j));
                    paso += 0.1;
                }
                lineSeries.ItemsSource = valueList4;
                Style dataPointStyle = GetNewDataPointStyleBlack();
                lineSeries.DataPointStyle = dataPointStyle;
            }
            else if (index == 5)
            {
                foreach (Double j in variable.Q)
                {
                    valueList5.Add(new KeyValuePair<Double, Double>(paso, j));
                    paso += 0.1;
                }
                lineSeries.ItemsSource = valueList5;
                Style dataPointStyle = GetNewDataPointStyleOrange();
                lineSeries.DataPointStyle = dataPointStyle;
            }
            else if (index == 6)
            {
                foreach (Double j in variable.Q)
                {
                    valueList6.Add(new KeyValuePair<Double, Double>(paso, j));
                    paso += 0.1;
                }
                lineSeries.ItemsSource = valueList6;
                Style dataPointStyle = GetNewDataPointStylePink();
                lineSeries.DataPointStyle = dataPointStyle;
            }
            else if (index == 7)
            {
                foreach (Double j in variable.Q)
                {
                    valueList7.Add(new KeyValuePair<Double, Double>(paso, j));
                    paso += 0.1;
                }
                lineSeries.ItemsSource = valueList7;
                Style dataPointStyle = GetNewDataPointStyleYellow();
                lineSeries.DataPointStyle = dataPointStyle;
            }
            chartCurva.Series.Add(lineSeries);
        }

        //para el grafico de Absorción Energética
        private void ShowChart2(FuncionesGenerales variable, int index)
        {
            List<KeyValuePair<Double, Double>> valueList0 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList1 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList2 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList3 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList4 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList5 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList6 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList7 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList8 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList9 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList10 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList11 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList12 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList13 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList14 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList15 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList16 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList17 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList18 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList19 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList20 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList21 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList22 = new List<KeyValuePair<Double, double>>();
            List<KeyValuePair<Double, Double>> valueList23 = new List<KeyValuePair<Double, double>>();
            Double paso=0.0;

            LineSeries lineSeries = new LineSeries();
            lineSeries.Title = variable.modeloChart + " - " + variable.frec;
            lineSeries.DependentValuePath = "Value";
            lineSeries.IndependentValuePath = "Key";

            if (index == 0)
            {
                foreach (Double j in variable.T[0])
                {
                    valueList0.Add(new KeyValuePair<Double, Double>(paso, j));
                    paso += 0.2;
                }
                lineSeries.ItemsSource = valueList0;
                Style dataPointStyle = GetNewDataPointStyleRed();
                lineSeries.DataPointStyle = dataPointStyle;

                //foreach (Double j in variable.T[1])
                //{
                //    valueList1.Add(new KeyValuePair<Double, Double>(paso, j));
                //    paso += 0.2;
                //}
                //lineSeries.ItemsSource = valueList1;
                //dataPointStyle = GetNewDataPointStyleRed();
                //lineSeries.DataPointStyle = dataPointStyle;

                //foreach (Double j in variable.T[2])
                //{
                //    valueList2.Add(new KeyValuePair<Double, Double>(paso, j));
                //    paso += 0.2;
                //}
                //lineSeries.ItemsSource = valueList2;
                //dataPointStyle = GetNewDataPointStyleRed();
                //lineSeries.DataPointStyle = dataPointStyle;

            }
            chartCurva.Series.Add(lineSeries);
        }

        //para el grafico de Permitividad Relativa Vs Frecuencia
        private void ShowChartPermitividadVsFrecuencia(int index)
        {
            Frecuencias frecuencias = new Frecuencias();
            Grasa grasa = new Grasa();
            Hueso hueso = new Hueso();
            Musculo musculo = new Musculo();
            PielHidratada pielHidratada = new PielHidratada();
            PielSeca pielSeca = new PielSeca();

            chartCurva.Title = "Permitividad Relativa Vs Frecuencia";
            List<KeyValuePair<Double, Double>> valueList0 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList1 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList2 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList3 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList4 = new List<KeyValuePair<double, double>>();

            LineSeries lineSeries = new LineSeries();

            if (index == 0)
            {
                lineSeries.Title = Hueso.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList0.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], hueso.relative_permittivity[i]));
                }
                lineSeries.ItemsSource = valueList0;
                Style dataPointStyle = GetNewDataPointStyleAqua();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }

            else if (index == 1)
            {
                lineSeries.Title = Grasa.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList1.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], grasa.relative_permittivity[i]));

                }
                lineSeries.ItemsSource = valueList1;
                Style dataPointStyle = GetNewDataPointStyleRed();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }

            else if (index == 2)
            {
                lineSeries.Title = Musculo.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList2.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], musculo.relative_permittivity[i]));

                }
                lineSeries.ItemsSource = valueList2;
                Style dataPointStyle = GetNewDataPointStyleGreen();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }

            else if (index == 3)
            {
                lineSeries.Title = PielHidratada.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList3.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], pielHidratada.relative_permittivity[i]));

                }
                lineSeries.ItemsSource = valueList3;
                Style dataPointStyle = GetNewDataPointStyleBlue();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }

            else if (index == 4)
            {
                lineSeries.Title = PielSeca.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList4.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], pielSeca.relative_permittivity[i]));

                }
                lineSeries.ItemsSource = valueList4;
                Style dataPointStyle = GetNewDataPointStylePink();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }
        }

        //para el grafico de Conductividad Relativa Vs Frecuencia
        private void ShowChartConductividadVsFrecuencia(int index)
        {
            Frecuencias frecuencias = new Frecuencias();
            Grasa grasa = new Grasa();
            Hueso hueso = new Hueso();
            Musculo musculo = new Musculo();
            PielHidratada pielHidratada = new PielHidratada();
            PielSeca pielSeca = new PielSeca();

            chartCurva.Title = "Conductividad Vs Frecuencia";
            List<KeyValuePair<Double, Double>> valueList0 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList1 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList2 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList3 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList4 = new List<KeyValuePair<double, double>>();

            LineSeries lineSeries = new LineSeries();

            if (index == 0)
            {
                lineSeries.Title = Hueso.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList0.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], hueso.conductivity[i]));
                }
                lineSeries.ItemsSource = valueList0;
                Style dataPointStyle = GetNewDataPointStyleAqua();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }

            else if (index == 1)
            {
                lineSeries.Title = Grasa.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList1.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], grasa.conductivity[i]));

                }
                lineSeries.ItemsSource = valueList1;
                Style dataPointStyle = GetNewDataPointStyleRed();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }

            else if (index == 2)
            {
                lineSeries.Title = Musculo.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList2.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], musculo.conductivity[i]));

                }
                lineSeries.ItemsSource = valueList2;
                Style dataPointStyle = GetNewDataPointStyleGreen();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }

            else if (index == 3)
            {
                lineSeries.Title = PielHidratada.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList3.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], pielHidratada.conductivity[i]));

                }
                lineSeries.ItemsSource = valueList3;
                Style dataPointStyle = GetNewDataPointStyleBlue();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }

            else if (index == 4)
            {
                lineSeries.Title = PielSeca.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList4.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], pielSeca.conductivity[i]));

                }
                lineSeries.ItemsSource = valueList4;
                Style dataPointStyle = GetNewDataPointStylePink();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }
        }

        //para el grafico de Tangente De Perdida Vs Frecuencia
        private void ShowChartTangenteVsFrecuencia(int index)
        {
            Frecuencias frecuencias = new Frecuencias();
            Grasa grasa = new Grasa();
            Hueso hueso = new Hueso();
            Musculo musculo = new Musculo();
            PielHidratada pielHidratada = new PielHidratada();
            PielSeca pielSeca = new PielSeca();

            chartCurva.Title = "Tangente De Perdida Vs Frecuencia";
            List<KeyValuePair<Double, Double>> valueList0 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList1 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList2 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList3 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList4 = new List<KeyValuePair<double, double>>();

            LineSeries lineSeries = new LineSeries();

            if (index == 0)
            {
                lineSeries.Title = Hueso.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList0.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], hueso.loss_tangent[i]));
                }
                lineSeries.ItemsSource = valueList0;
                Style dataPointStyle = GetNewDataPointStyleAqua();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }

            else if (index == 1)
            {
                lineSeries.Title = Grasa.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList1.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], grasa.loss_tangent[i]));

                }
                lineSeries.ItemsSource = valueList1;
                Style dataPointStyle = GetNewDataPointStyleRed();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }

            else if (index == 2)
            {
                lineSeries.Title = Musculo.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList2.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], musculo.loss_tangent[i]));

                }
                lineSeries.ItemsSource = valueList2;
                Style dataPointStyle = GetNewDataPointStyleGreen();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }

            else if (index == 3)
            {
                lineSeries.Title = PielHidratada.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList3.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], pielHidratada.loss_tangent[i]));

                }
                lineSeries.ItemsSource = valueList3;
                Style dataPointStyle = GetNewDataPointStyleBlue();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }

            else if (index == 4)
            {
                lineSeries.Title = PielSeca.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList4.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], pielSeca.loss_tangent[i]));

                }
                lineSeries.ItemsSource = valueList4;
                Style dataPointStyle = GetNewDataPointStylePink();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }
        }

        //para el grafico de Longitud De Onda Vs Frecuencia
        private void ShowChartLongitudVsFrecuencia(int index)
        {
            Frecuencias frecuencias = new Frecuencias();
            Grasa grasa = new Grasa();
            Hueso hueso = new Hueso();
            Musculo musculo = new Musculo();
            PielHidratada pielHidratada = new PielHidratada();
            PielSeca pielSeca = new PielSeca();

            chartCurva.Title = "Longitud De Onda Vs Frecuencia";
            List<KeyValuePair<Double, Double>> valueList0 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList1 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList2 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList3 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList4 = new List<KeyValuePair<double, double>>();

            LineSeries lineSeries = new LineSeries();

            if (index == 0)
            {
                lineSeries.Title = Hueso.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList0.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], hueso.wavelength[i]));
                }
                lineSeries.ItemsSource = valueList0;
                Style dataPointStyle = GetNewDataPointStyleAqua();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }

            else if (index == 1)
            {
                lineSeries.Title = Grasa.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList1.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], grasa.wavelength[i]));

                }
                lineSeries.ItemsSource = valueList1;
                Style dataPointStyle = GetNewDataPointStyleRed();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }

            else if (index == 2)
            {
                lineSeries.Title = Musculo.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList2.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], musculo.wavelength[i]));

                }
                lineSeries.ItemsSource = valueList2;
                Style dataPointStyle = GetNewDataPointStyleGreen();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }

            else if (index == 3)
            {
                lineSeries.Title = PielHidratada.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList3.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], pielHidratada.wavelength[i]));

                }
                lineSeries.ItemsSource = valueList3;
                Style dataPointStyle = GetNewDataPointStyleBlue();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }

            else if (index == 4)
            {
                lineSeries.Title = PielSeca.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList4.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], pielSeca.wavelength[i]));

                }
                lineSeries.ItemsSource = valueList4;
                Style dataPointStyle = GetNewDataPointStylePink();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }
        }

        //para el grafico de Profundidad De Penetración Vs Frecuencia
        private void ShowChartProfundidadVsFrecuencia(int index)
        {
            Frecuencias frecuencias = new Frecuencias();
            Grasa grasa = new Grasa();
            Hueso hueso = new Hueso();
            Musculo musculo = new Musculo();
            PielHidratada pielHidratada = new PielHidratada();
            PielSeca pielSeca = new PielSeca();

            chartCurva.Title = "Profundidad De Penetración Vs Frecuencia";
            List<KeyValuePair<Double, Double>> valueList0 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList1 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList2 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList3 = new List<KeyValuePair<double, double>>();
            List<KeyValuePair<Double, Double>> valueList4 = new List<KeyValuePair<double, double>>();

            LineSeries lineSeries = new LineSeries();

            if (index == 0)
            {
                lineSeries.Title = Hueso.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList0.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], hueso.penetration_depth[i]));
                }
                lineSeries.ItemsSource = valueList0;
                Style dataPointStyle = GetNewDataPointStyleAqua();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }

            else if (index == 1)
            {
                lineSeries.Title = Grasa.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList1.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], grasa.penetration_depth[i]));

                }
                lineSeries.ItemsSource = valueList1;
                Style dataPointStyle = GetNewDataPointStyleRed();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }

            else if (index == 2)
            {
                lineSeries.Title = Musculo.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList2.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], musculo.penetration_depth[i]));

                }
                lineSeries.ItemsSource = valueList2;
                Style dataPointStyle = GetNewDataPointStyleGreen();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }

            else if (index == 3)
            {
                lineSeries.Title = PielHidratada.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList3.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], pielHidratada.penetration_depth[i]));

                }
                lineSeries.ItemsSource = valueList3;
                Style dataPointStyle = GetNewDataPointStyleBlue();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }

            else if (index == 4)
            {
                lineSeries.Title = PielSeca.tejido;
                lineSeries.DependentValuePath = "Value";
                lineSeries.IndependentValuePath = "Key";
                for (int i = 0; i < frecuencias.frecuency.Length; i++)
                {
                    valueList4.Add(new KeyValuePair<Double, Double>(frecuencias.frecuency[i], pielSeca.penetration_depth[i]));

                }
                lineSeries.ItemsSource = valueList4;
                Style dataPointStyle = GetNewDataPointStylePink();
                lineSeries.DataPointStyle = dataPointStyle;
                chartCurva.Series.Add(lineSeries);
            }
        }
    }
}
