using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Threading;

namespace ServidorDeImagen
{
    public partial class Form1 : Form
    {
        String path;    //string con el path al directorio ImagenesRedimensionadas
        String[] imagenesModificadas;   //arreglo con los nombres de las imagenes en el directorio ImagenesRedimensionadas
        List<String> direccionesIP; //lista de direcciones IP
        String imagenActual;    //para manejar la imagen actual
        System.Windows.Forms.Timer clock;    //timer que maneja el temporizado
        int segundos; // Seconds.
        int minutos; // Minutes.
        int horas;   // Hours.
        Int32 contador = 0; //contador que hace las veces de indice para seleccionar la imagen actual de entre la coleccion
        System.Windows.Forms.Timer clockTablaIP;
        MyServerUDP server = new MyServerUDP();
        List<IPDate> lista = new List<IPDate>();
        Thread serverThread;

        public Form1()
        {
            InitializeComponent();
            direccionesIP = new List<String>();
            path = Directory.GetCurrentDirectory();
            path += @"\ImagenesRedimensionadas\";

            if (!Directory.Exists(path))
                Directory.CreateDirectory(path);

            // ver los cambios en el directorio de la aplicación y sobre todos los archivos
            FileSystemWatcher watcher = new FileSystemWatcher(path);

            // ver el nombre del archivo y tamaño cambiado
            watcher.NotifyFilter = NotifyFilters.FileName | NotifyFilters.Size;
            watcher.Changed += new FileSystemEventHandler(OnChange);
            watcher.Created += new FileSystemEventHandler(OnChange);
            watcher.Deleted += new FileSystemEventHandler(OnChange);
            watcher.Renamed += new RenamedEventHandler(OnRenamed);
            watcher.EnableRaisingEvents = true;
                       
            //actualizo el listbox con las imagenes redimensionadas
            SetText();

            //me suscribo al evento de LinkClicked
            linklblDirectorioModificadas.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(linklblDirectorioModificadas_LinkClicked);

            //obtengo los valores de tiempo entre imagen
            horas = Convert.ToInt16(txbHora.Text.ToString());
            minutos = Convert.ToInt16(txbMinutos.Text.ToString());
            segundos = Convert.ToInt16(txbSegundos.Text.ToString());        

            //seteo el timer
            clock = new System.Windows.Forms.Timer();
            clock.Interval = 1000;
            clock.Start();
            clock.Tick += new EventHandler(Timer_Tick);

            clockTablaIP = new System.Windows.Forms.Timer();
            // Set to 30 seconds.
            clockTablaIP.Interval = 30000;
            clockTablaIP.Tick += new EventHandler(clockTablaIP_Tick);

            // Enable timer.
            clockTablaIP.Enabled = true;

            serverThread = new Thread(server.DoWork);
            // Loop until worker thread activates.
            //while (!serverThread.IsAlive) ;

            // Put the main thread to sleep for 1 millisecond to
            // allow the worker thread to do some work:
            Thread.Sleep(1);
            serverThread.IsBackground = true;
            // Start the worker thread.
            serverThread.Start();

            //Actualizo las imagenes
            ActualizarImagenes();
        }
        //crear el metodo para manejar la imagen actual
                
        private void RefrescarTiempoRestante()
        {
            try
            {
                if (horas < 10)
                    lblTimeLeft.Text = "0" + horas.ToString() + ":";
                else
                    lblTimeLeft.Text = horas.ToString() + ":";
                if (minutos < 10)
                    lblTimeLeft.Text += "0" + minutos.ToString() + ":";
                else
                    lblTimeLeft.Text += minutos.ToString() + ":";
                if (segundos < 10)
                    lblTimeLeft.Text += "0" + segundos.ToString();
                else
                    lblTimeLeft.Text += segundos.ToString();
            }
            catch (Exception ex) { MessageBox.Show(ex.Message, "ERROR"); }
        }

        private void RefrescarPropiedadesImagen()
        {
            try
            {
                String aux = "";
                Byte[] header = new Byte[54];
                FileStream stream = new FileStream(imagenActual, FileMode.Open, FileAccess.Read);
                BinaryReader reader = new BinaryReader(stream);
                int cantidadLeidos = reader.Read(header, 0, 54);
                BMP bmp = new BMP(header);
                this.lstboxPropiedadesImagen.Items.Clear();
                aux = "Tipo de fichero: " + bmp.TipoFichero();
                this.lstboxPropiedadesImagen.Items.Add(aux);
                aux = "Tamaño del Archivo: " + bmp.TamañoArchivo();
                this.lstboxPropiedadesImagen.Items.Add(aux);
                aux = "Inicio de los datos de la imagen: " + bmp.InicioDatosImagen();
                this.lstboxPropiedadesImagen.Items.Add(aux);
                aux = "Tamaño de la cabecera: " + bmp.TamañoCabezera();
                this.lstboxPropiedadesImagen.Items.Add(aux);
                aux = "Ancho (en pixels): " + bmp.AnchoEnPixels();
                this.lstboxPropiedadesImagen.Items.Add(aux);
                aux = "Alto (en pixels): " + bmp.AltoEnPixels();
                this.lstboxPropiedadesImagen.Items.Add(aux);
                aux = "Número de planos: " + bmp.NumeroDePlanos();
                this.lstboxPropiedadesImagen.Items.Add(aux);
                aux = "Tamaño de cada punto: " + bmp.TamañoPunto();
                this.lstboxPropiedadesImagen.Items.Add(aux);
                aux = "Compresión: " + bmp.Compresion();
                this.lstboxPropiedadesImagen.Items.Add(aux);
                aux = "Tamaño de la imagen: " + bmp.TamañoImagen();
                this.lstboxPropiedadesImagen.Items.Add(aux);
                aux = "Resolución horizontal: " + bmp.ResolucionHorizontal();
                this.lstboxPropiedadesImagen.Items.Add(aux);
                aux = "Resolución vertical: " + bmp.ResolucionVertical();
                this.lstboxPropiedadesImagen.Items.Add(aux);
                aux = "Tamaño de la tabla de color: " + bmp.TamañoTablaColor();
                this.lstboxPropiedadesImagen.Items.Add(aux);
                aux = "Contador de colores importantes: " + bmp.ContadorColoresImportantes();
                this.lstboxPropiedadesImagen.Items.Add(aux);

                reader.Close();
                stream.Close();
            }
            catch (Exception exp)
            {
                MessageBox.Show(exp.Message);
            }
        }

        private void ActualizarImagenes()
        {
            imagenesModificadas = System.IO.Directory.GetFiles(path, "*.bmp");
            //muestro la imagen actual para la imagen siguiente verifico que halla mas de una si no muestro la misma imagen
            if (imagenesModificadas.Length > 0)
            {
                contador = (contador + 1) % imagenesModificadas.Length;
                imagenActual = imagenesModificadas[contador];
                pictBoxActual.Image = Image.FromFile(imagenesModificadas[contador]);
                pictBoxActual.SizeMode = PictureBoxSizeMode.StretchImage;
                RefrescarPropiedadesImagen();
                pictBoxSiguiente.Image = Image.FromFile(imagenesModificadas[(contador + 1) % imagenesModificadas.Length]);
                pictBoxSiguiente.SizeMode = PictureBoxSizeMode.StretchImage;
                server.SendImagen(imagenActual);
            }
        }

        private void btnCargarCarpeta_Click(object sender, EventArgs e)
        {
            String Contenedor = "";
            String fileName;
            if (((Convert.ToInt32(txbAncho.Text) <= 480 && Convert.ToInt32(txbAlto.Text) <= 272) &&
                               (Convert.ToInt32(txbAncho.Text) > 0 && Convert.ToInt32(txbAlto.Text) > 0)))
            //|| ((Convert.ToInt32(txbAncho.Text) <= 272 && Convert.ToInt32(txbAlto.Text) <= 480) &&
            //(Convert.ToInt32(txbAncho.Text) > 0 && Convert.ToInt32(txbAlto.Text) > 0)))
            {
                openFD.InitialDirectory = System.Environment.GetFolderPath(Environment.SpecialFolder.Personal);
                openFD.Filter = "JPEG Images|*.jpeg;*.jpg|GIF Images|*.gif|BITMAPS|*.bmp|PNG Images|*.png";
                if (openFD.ShowDialog() == DialogResult.Cancel)
                {
                    MessageBox.Show("Operación Cancelada");
                }
                else
                {
                    String[] chosenFiles = openFD.FileNames;

                    #region RedimensionadoSalvado
                    try
                    {
                        string[] Array;
                        Image image = null;

                        foreach (string chosenFile in chosenFiles)
                        {
                            if (chosenFile.Substring(chosenFile.Length - 4, 4).ToUpper() == ".JPG" || chosenFile.Substring(chosenFile.Length - 4, 4).ToUpper() == ".JPEG"
                                || chosenFile.Substring(chosenFile.Length - 4, 4).ToUpper() == ".BMP" || chosenFile.Substring(chosenFile.Length - 4, 4).ToUpper() == ".PNG")
                            {
                                //Obtenemos la imagen del filesystem
                                image = Image.FromFile(@chosenFile);
                                //Redimensionamos la imagen

                                image = RedimensionarImagen.ResizeImage(image, Convert.ToInt32(txbAncho.Text), Convert.ToInt32(txbAlto.Text));

                                Array = chosenFile.Split('\\');
                                Contenedor = Directory.GetCurrentDirectory(); //file.Substring(0, file.LastIndexOf("\\") + 1);
                                Contenedor += @"\ImagenesRedimensionadas\";
                                if (!Directory.Exists(Contenedor))
                                    Directory.CreateDirectory(Contenedor);

                                fileName = Array[Array.Length - 1];
                                fileName = fileName.Remove(fileName.Length - 4, 4);
                                fileName += ".bmp";
                                //MessageBox.Show("Filename: " + fileName, "Test", MessageBoxButtons.OK); //comentar esto, era solo una prueba
                                //Guardamos la imagen modificada 
                                image.Save(string.Format(@"{0}{1}", Contenedor, fileName));
                                image.Dispose();

                            }
                        }

                        //System.Diagnostics.Process.Start(@Contenedor); //con esta linea abro la carpeta ImagenesRedimensionadas
                    }
                    catch (Exception exp)
                    {
                        MessageBox.Show(exp.Message);
                    }
                    #endregion RedimensionadoSalvado
                }
            }
            else
                MessageBox.Show("Dimensiones Inapropiadas", "Error de dimensiones", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        }

        private void linklblDirectorioModificadas_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            // Specify that the link was visited. 
            linklblDirectorioModificadas.LinkVisited = true;

            // Navigate to a URL.
            System.Diagnostics.Process.Start(path);
        }

        #region eventos
        private void clockTablaIP_Tick(object Sender, EventArgs e)
        {
            // Set the caption to the current time.
            server.RefrescarTablaIPs();
            lista = server.GetTablaIP();
            lstboxIPs.Items.Clear();
            foreach (IPDate a in lista)
            {
                lstboxIPs.Items.Add(a.IP.ToString() + "  -  " + a.Date.ToLongTimeString());
            }
        }

        private void Timer_Tick(object sender, EventArgs e)
        {
            // Verify if the time didn't pass.
            if ((minutos == 0) && (horas == 0) && (segundos == 0))
            {
                // If the time is over, clear all settings and fields.
                /*aca deberia meter todo lo que se tiene que hacer cuando comienza el timer */
                ActualizarImagenes();

                horas = Convert.ToInt16(txbHora.Text.ToString());
                minutos = Convert.ToInt16(txbMinutos.Text.ToString());
                segundos = Convert.ToInt16(txbSegundos.Text.ToString());
                //refresco el tiempo restante
                RefrescarTiempoRestante();
            }
            else
            {
                // Else continue counting.
                if (segundos < 1)
                {
                    segundos = 59;
                    if (minutos == 0)
                    {
                        minutos = 59;
                        if (horas != 0)
                            horas -= 1;
                    }
                    else
                        minutos -= 1;
                }
                else
                    segundos -= 1;

                // Display the current values of hours, minutes and seconds in
                // the corresponding fields.
                //refresco el tiempo restante
                RefrescarTiempoRestante();
            }
        }

        delegate void SetTextCallback();

        private void SetText()
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.lstboxImagenes.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(SetText);
                this.Invoke(d, new object[] { });
            }
            else
            {
                imagenesModificadas = System.IO.Directory.GetFiles(path, "*.bmp");
                // Shutdown the painting of the ListBox as items are added.
                this.lstboxImagenes.Items.Clear();
                //this.lstboxImagenes.BeginUpdate();
                // Loop through and add 50 items to the ListBox.
                foreach (String imagen in imagenesModificadas)
                {
                    string[] Array = imagen.Split('\\');
                    string fileName = Array[Array.Length - 1];
                    fileName = fileName.Remove(fileName.Length - 4, 4);
                    fileName += ".bmp";
                    this.lstboxImagenes.Items.Add(fileName);
                }
                // Allow the ListBox to repaint and display the new items.
                //this.lstboxImagenes.EndUpdate();
            }
        }

        private void OnChange(object sender, FileSystemEventArgs e)
        {
            //MessageBox.Show("File: " + e.FullPath + " - change type: " + e.ChangeType, "OnChange");
            //hacer algo asi: donde files tiene q ser global a la funciones que manejan las imagenes *-*
            //setearle a la clase que maneje las imagenes el string con el fileImages
            //string[] imagenesModificadas = Directory.GetFiles(path);
            try
            {
                //actualizo el listbox con las imagenes redimensionadas
                //ActualizarListBoxImagenes();
                SetText();
            }
            catch (Exception ex)
            { MessageBox.Show(ex.Message); }
        }

        private void OnRenamed(object sender, RenamedEventArgs e)
        {
            //MessageBox.Show("file: " + e.OldName + "renamed to " + e.Name, "OnRename");
            try
            {
                //actualizo el listbox con las imagenes redimensionadas
                //ActualizarListBoxImagenes();
                SetText();
            }
            catch (Exception ex) { MessageBox.Show(ex.Message); }
        }

        private void txbHora_TextChanged(object sender, EventArgs e)
        {
            try
            {
                horas = Convert.ToInt16(txbHora.Text.ToString());
            }
            catch (Exception ex) { MessageBox.Show("Verifique la hora ingresada", "ERROR"); }
        }

        private void txbMinutos_TextChanged(object sender, EventArgs e)
        {
            try{
            minutos = Convert.ToInt16(txbMinutos.Text.ToString());
            }
            catch (Exception ex) { MessageBox.Show("Verifique los minutos ingresados", "ERROR"); }
        }

        private void txbSegundos_TextChanged(object sender, EventArgs e)
        {
            try{
            segundos = Convert.ToInt16(txbSegundos.Text.ToString());
            }
            catch (Exception ex) { MessageBox.Show("Verifique los segundos ingresados", "ERROR"); }
        }
        #endregion eventos

        private void btnIDcliente_Click(object sender, EventArgs e)
        {
            // Navigate to a URL.
            try
            {
                String aux = "http://"; //http://192.168.1.25/secret
                aux += txboxIP.Text.ToString();
                aux += "/";
                aux += txbPassword.Text.ToString();
                System.Diagnostics.Process.Start(aux);
            }
            catch (Exception ex) { MessageBox.Show(ex.Message, "ERROR"); }
        }

        private void btnSiguiente_Click(object sender, EventArgs e)
        {
            horas = 0;
            minutos = 0;
            segundos = 0;
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            server.RequestStop();
            serverThread.Abort();
            // Use the Join method to block the current thread 
            // until the object's thread terminates.
            //serverThread.Join();
        }
    }
}
