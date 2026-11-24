using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace AnalizadorDeTramas
{
    public partial class Form1 : Form
    {
        // variables globales
        String filePath = "";
        Stream myStream;
        String fileContent = String.Empty;
        List<string[]> parsedData;
        string[] lines;
        string[] lineasDestino;
        string strBusquedaOpen = "* * * * * * * * * * COM Port Opened * * * * * * * * * * ";
        string strBusquedaClose = "* * * * * * * * * * COM Port Closed * * * * * * * * * * ";
        int indiceColores = 0;
        struct GuiaColores
        {
            //public int indice = 0;
            public Color color;
            //public bool usado = false;
           /* public GuiaColores(int _indice, Color _color, bool _usado){
                indice = 0;
                color=_color;
                usado=_usado;
            }*/
            public GuiaColores(Color _color){
                color=_color;                
            }

        }

        //GuiaColores[] colores = new[]{new GuiaColores(Color.Yellow), new GuiaColores(Color.Red), new GuiaColores(Color.Purple), new GuiaColores(Color.RoyalBlue)};
        Color[] colores = new[] { Color.Yellow, Color.YellowGreen, Color.WhiteSmoke, Color.Turquoise, Color.Teal};

        public Form1()
        {
            InitializeComponent();
            // agrego el menu contextual a los richTextBox que tengo
            richTextBoxNotas.AddContextMenu();
            richTextBoxTramas.AddContextMenu();
        }

    private void abrirToolStripMenuItem_Click(object sender, EventArgs e){        
            // seteo el directorio inicial y los filtros de archivos para openFileDialog
            //openFileDialog.InitialDirectory = System.Environment.GetFolderPath(Environment.SpecialFolder.Personal);
            openFileDialog.Filter = "txt files (*.txt)|*.txt|All files (*.*)|*.*";
            openFileDialog.RestoreDirectory = true;
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                if ((myStream = openFileDialog.OpenFile()) != null)
                {
                    // obtiene la ruta al archivo
                    parsedData = new List<string[]>();
                    filePath = openFileDialog.FileName;
                    lines = File.ReadAllLines(filePath);
                    //creo archivo destino
                    //lineasDestino = new String[lines.Length];
                    int i = 0, nroLinea = 0;
                    string cadena = "";
                    richTextBoxTramas.Text = "";
                    Cursor.Current = Cursors.AppStarting;
                    foreach (string line in lines)
                    {
                        if (i >= 8)
                        {
                            if (line != strBusquedaOpen && line != strBusquedaClose)
                            {
                                //lo inserto en el destino
                                //parsedData.Add(lines[i]);
                                //lineasDestino[nroLinea] = line.Substring(19);
                                cadena += line.Substring(19) + "\n";                                
                                nroLinea++;
                            }
                        }
                        i++;
                    }
                    richTextBoxTramas.Text = cadena;

                    //this.richTextBoxTramas.Select(this.richTextBoxTramas.GetFirstCharIndexFromLine(2), this.richTextBoxTramas.Lines[2].Length);
                    //this.richTextBoxTramas.SelectionBackColor = Color.Red;
                }
            }
    }

        private void abrirToolStripMenuItem_Click0(object sender, EventArgs e)
        {
            // seteo el directorio inicial y los filtros de archivos para openFileDialog
            //openFileDialog.InitialDirectory = System.Environment.GetFolderPath(Environment.SpecialFolder.Personal);
            openFileDialog.Filter = "txt files (*.txt)|*.txt|All files (*.*)|*.*";
            openFileDialog.RestoreDirectory = true;
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                if ((myStream = openFileDialog.OpenFile()) != null)
                {
                    // obtiene la ruta al archivo
                    filePath = openFileDialog.FileName;
                    fileContent = File.ReadAllText(filePath);
                    int indiceBusqueda;

                    // remuevo toda la informacion no útil del principio y final del archivo
                    string strBusqueda = "* * * * * * * * * * COM Port Opened * * * * * * * * * * ";
                    if (fileContent.IndexOf(strBusqueda) != -1)
                    {
                        indiceBusqueda = fileContent.IndexOf(strBusqueda);
                        // la instrucción anterior me devuelve como índice el comienzo de la cadena, le tengo que sumar la longitud de la misma y los 2 saltos de línea
                        // pero como la funcion remove de la clase string esta basada en posicion 0, n.length-1
                        indiceBusqueda += strBusqueda.Length + 1;
                        // remover los n primeros caracteres = indiceBusqueda
                        fileContent = fileContent.Remove(0, indiceBusqueda);
                    }

                    strBusqueda = "* * * * * * * * * * COM Port Closed * * * * * * * * * * ";
                    if (fileContent.IndexOf(strBusqueda) != -1)
                    {
                        indiceBusqueda = fileContent.IndexOf(strBusqueda);
                        // remover los n primeros caracteres = indiceBusqueda
                        fileContent = fileContent.Remove(indiceBusqueda, strBusqueda.Length);
                    }

                    // remuevo las marcas de tiempo del archivo
                    strBusqueda = " [RX]:"; // este indice siempre es 12 caracteres desplazados de la estampa de tiempo
                    //string auxiliar = "xx:xx:xx.xxx [RX]:"; esta cadena tiene 18 caracteres
                    while (fileContent.IndexOf(strBusqueda) != -1)
                    {
                        indiceBusqueda = fileContent.IndexOf(strBusqueda);
                        fileContent = fileContent.Remove(indiceBusqueda - 12, 18);
                    }

                    // quito los espacios en blanco del principio y final
                    fileContent = fileContent.Trim(Environment.NewLine.ToCharArray());

                    richTextBoxTramas.Text = fileContent;
                    //MessageBox.Show("Index: " + primerIndice.ToString());
                }
            }
        }

        private void salirToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void guardarComoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            saveFileDialog.InitialDirectory = System.Environment.GetFolderPath(Environment.SpecialFolder.Personal);
            saveFileDialog.Filter = "txt files (*.txt)|*.txt|All files (*.*)|*.*";
            saveFileDialog.RestoreDirectory = true;

            if (saveFileDialog.ShowDialog() == DialogResult.OK)
            {
                File.WriteAllText(saveFileDialog.FileName, richTextBoxTramas.Text);
            }
        }

        private void atrasToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //se utiliza el metodo undo en el rich textbox para volver atras
            richTextBoxTramas.Undo();
        }

        private void adelanteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //se utiliza el metodo redo en le rich textbox para regresar adelante
            richTextBoxTramas.Redo();
        }

        private void copiarToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //se utilza el metodo copy para copiar lo que este seleccionado en el rich textbox
            richTextBoxTramas.Copy();
        }

        private void pegarToolStripMenuItem_Click(object sender, EventArgs e)
        {

            //se utilza el metodo paste para pegar lo que tenemos copiado en el rich textbox
            richTextBoxTramas.Paste();
        }

        private void cortarToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //se utilza el metodo cut para cortar lo que este seleccionado en el rich textbox
            richTextBoxTramas.Cut();
        }

        private void seleccionarTodoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //se utilza el metodo SellecAll para seleccionar todo lo que este en el rich textbox
            richTextBoxTramas.SelectAll();
        }

        private void borrarTodoToolStripMenuItem_Click(object sender, EventArgs e)
        {

            //se utiliza el metodo clear para limpiar el texto del rich textbox
            richTextBoxTramas.Clear();
        }

        private void fuenteToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            //se crea un objeto de tipo fontdialog que servira para cabiar el tipo de fuente y tamaño
            FontDialog font = new FontDialog();
            //se aplica el tipo de fuente del rich textbox al objeto que hemos creado
            font.Font = richTextBoxTramas.Font;
            //se hace la condicion para cambiar el tipo de fuente al presionar el boton ok
            if (font.ShowDialog() == DialogResult.OK)
            {
                richTextBoxTramas.Font = font.Font;
            }
        }

        private void colorDeFuenteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //se crea un objeto de tipo colordialog que servira para cabiar color de la fuente 
            ColorDialog color = new ColorDialog();
            //se hace la condicion para cambiar el color de la fuente al presionar el boton ok 
            if (color.ShowDialog() == DialogResult.OK)
            {
                richTextBoxTramas.ForeColor = color.Color;
            }
        }

        private void colorDeFondoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //se crea un objeto de tipo colordialog que servira para cabiar color del rich textbox
            ColorDialog fondo = new ColorDialog();
            //se hace la condicion para cambiar el tipo de color del rich textbox al presionar el boton ok
            if (fondo.ShowDialog() == DialogResult.OK)
            {
                richTextBoxTramas.BackColor = fondo.Color;
            }
        }

        private void buttonCadena_Click(object sender, EventArgs e)
        {
            String palabra = textBoxBusqueda.Text;
            Cursor.Current = Cursors.AppStarting;

            //richTextBoxTramas.Text = richTextBoxTramas.Text.Replace(palabra, Environment.NewLine + palabra + Environment.NewLine);

            int startindex = 0, wordstartIndex = 0;
            //MessageBox.Show("PASE!!!!!");
            while (startindex < richTextBoxTramas.TextLength)
            {

                wordstartIndex = richTextBoxTramas.Find(palabra, startindex, RichTextBoxFinds.None);
                if (wordstartIndex != -1)
                {
                    richTextBoxTramas.SelectionStart = wordstartIndex;
                    richTextBoxTramas.SelectionLength = palabra.Length;
                    richTextBoxTramas.SelectionBackColor = colores[indiceColores];
                }
                else
                    break;
                startindex = wordstartIndex + palabra.Length;
                 
            }
            indiceColores = (indiceColores + 1) % 5;
            /*int nroLinea = 0;
            foreach (string line in richTextBoxTramas.Lines)
            {
                if(line.Contains(palabra)){
                this.richTextBoxTramas.Select(this.richTextBoxTramas.GetFirstCharIndexFromLine(nroLinea), this.richTextBoxTramas.Lines[nroLinea].Length);
                this.richTextBoxTramas.SelectionBackColor = Color.Red;
                }
                nroLinea++;
            }
            */
            Cursor.Current = Cursors.Default;
        }

        private void buttonCalcular_Click(object sender, EventArgs e)
        {
            // obtengo el crc presumido de la cadena a calcular (los ultimos 2 bytes)
            // antes quito los espacios en blanco de la cadena y transformo el string en bytes
            Byte[] cadena = Utiles.StringToByteArrayFastest(textBoxCadenaCRC.Text.Replace(" ", ""));
            int longitud = cadena.Length;
            Byte crcLSB, crcMSB;
            crcLSB = cadena[longitud - 2];
            crcMSB = cadena[longitud - 1];
            labelCRCpresumido.Text = crcLSB.ToString("X2") + " " + crcMSB.ToString("X2");

            // calculo el CRC de la cadena pasada
            Byte crcLSB2, crcMSB2;
            UInt16 crc = Utiles.CRC(cadena, longitud-2, 0);
            crcLSB2 = Convert.ToByte(crc & 0xFF);
            crcMSB2 = Convert.ToByte(crc >> 8);
            labelCRCcalculado.Text = crcLSB2.ToString("X2") + " " + crcMSB2.ToString("X2");
        }

    }
}
