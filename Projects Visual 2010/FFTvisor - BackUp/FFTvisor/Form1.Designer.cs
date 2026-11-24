namespace FFTvisor
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea2 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend2 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series2 = new System.Windows.Forms.DataVisualization.Charting.Series();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.cmbBox_handshake = new System.Windows.Forms.ComboBox();
            this.cmbBox_stopBits = new System.Windows.Forms.ComboBox();
            this.cmbBox_parity = new System.Windows.Forms.ComboBox();
            this.cmbBox_dataBits = new System.Windows.Forms.ComboBox();
            this.cmbBox_baudRate = new System.Windows.Forms.ComboBox();
            this.cmbBox_puerto = new System.Windows.Forms.ComboBox();
            this.btn_escanear = new System.Windows.Forms.Button();
            this.btn_conectar = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.listBox_eventos = new System.Windows.Forms.ListBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.chart_fft = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.listBox_datos = new System.Windows.Forms.ListBox();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.btn_borrar = new System.Windows.Forms.Button();
            this.btn_save = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.chart_fft)).BeginInit();
            this.groupBox4.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.cmbBox_handshake);
            this.groupBox1.Controls.Add(this.cmbBox_stopBits);
            this.groupBox1.Controls.Add(this.cmbBox_parity);
            this.groupBox1.Controls.Add(this.cmbBox_dataBits);
            this.groupBox1.Controls.Add(this.cmbBox_baudRate);
            this.groupBox1.Controls.Add(this.cmbBox_puerto);
            this.groupBox1.Controls.Add(this.btn_escanear);
            this.groupBox1.Controls.Add(this.btn_conectar);
            this.groupBox1.Location = new System.Drawing.Point(13, 13);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(505, 95);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Settings";
            // 
            // cmbBox_handshake
            // 
            this.cmbBox_handshake.FormattingEnabled = true;
            this.cmbBox_handshake.Location = new System.Drawing.Point(371, 63);
            this.cmbBox_handshake.Name = "cmbBox_handshake";
            this.cmbBox_handshake.Size = new System.Drawing.Size(121, 21);
            this.cmbBox_handshake.TabIndex = 7;
            this.cmbBox_handshake.Text = "Handshake";
            // 
            // cmbBox_stopBits
            // 
            this.cmbBox_stopBits.FormattingEnabled = true;
            this.cmbBox_stopBits.Location = new System.Drawing.Point(243, 63);
            this.cmbBox_stopBits.Name = "cmbBox_stopBits";
            this.cmbBox_stopBits.Size = new System.Drawing.Size(121, 21);
            this.cmbBox_stopBits.TabIndex = 6;
            this.cmbBox_stopBits.Text = "Stop Bits";
            // 
            // cmbBox_parity
            // 
            this.cmbBox_parity.FormattingEnabled = true;
            this.cmbBox_parity.Location = new System.Drawing.Point(116, 63);
            this.cmbBox_parity.Name = "cmbBox_parity";
            this.cmbBox_parity.Size = new System.Drawing.Size(121, 21);
            this.cmbBox_parity.TabIndex = 5;
            this.cmbBox_parity.Text = "Parity";
            // 
            // cmbBox_dataBits
            // 
            this.cmbBox_dataBits.FormattingEnabled = true;
            this.cmbBox_dataBits.Location = new System.Drawing.Point(372, 20);
            this.cmbBox_dataBits.Name = "cmbBox_dataBits";
            this.cmbBox_dataBits.Size = new System.Drawing.Size(121, 21);
            this.cmbBox_dataBits.TabIndex = 4;
            this.cmbBox_dataBits.Text = "Data Bits";
            // 
            // cmbBox_baudRate
            // 
            this.cmbBox_baudRate.FormattingEnabled = true;
            this.cmbBox_baudRate.Location = new System.Drawing.Point(244, 20);
            this.cmbBox_baudRate.Name = "cmbBox_baudRate";
            this.cmbBox_baudRate.Size = new System.Drawing.Size(121, 21);
            this.cmbBox_baudRate.TabIndex = 3;
            this.cmbBox_baudRate.Text = "Baud Rate";
            // 
            // cmbBox_puerto
            // 
            this.cmbBox_puerto.FormattingEnabled = true;
            this.cmbBox_puerto.Location = new System.Drawing.Point(116, 20);
            this.cmbBox_puerto.Name = "cmbBox_puerto";
            this.cmbBox_puerto.Size = new System.Drawing.Size(121, 21);
            this.cmbBox_puerto.TabIndex = 2;
            this.cmbBox_puerto.Text = "Puerto";
            this.cmbBox_puerto.SelectedIndexChanged += new System.EventHandler(this.cmbBox_puerto_SelectedIndexChanged);
            // 
            // btn_escanear
            // 
            this.btn_escanear.Location = new System.Drawing.Point(7, 63);
            this.btn_escanear.Name = "btn_escanear";
            this.btn_escanear.Size = new System.Drawing.Size(103, 23);
            this.btn_escanear.TabIndex = 1;
            this.btn_escanear.Text = "Escanear";
            this.btn_escanear.UseVisualStyleBackColor = true;
            this.btn_escanear.Click += new System.EventHandler(this.btn_escanear_Click);
            // 
            // btn_conectar
            // 
            this.btn_conectar.Enabled = false;
            this.btn_conectar.Location = new System.Drawing.Point(7, 20);
            this.btn_conectar.Name = "btn_conectar";
            this.btn_conectar.Size = new System.Drawing.Size(103, 23);
            this.btn_conectar.TabIndex = 0;
            this.btn_conectar.Text = "Conectar";
            this.btn_conectar.UseVisualStyleBackColor = true;
            this.btn_conectar.Click += new System.EventHandler(this.btn_conectar_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.listBox_eventos);
            this.groupBox2.Location = new System.Drawing.Point(535, 13);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(300, 95);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Visor de eventos";
            // 
            // listBox_eventos
            // 
            this.listBox_eventos.FormattingEnabled = true;
            this.listBox_eventos.Location = new System.Drawing.Point(6, 20);
            this.listBox_eventos.Name = "listBox_eventos";
            this.listBox_eventos.Size = new System.Drawing.Size(282, 69);
            this.listBox_eventos.TabIndex = 0;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.chart_fft);
            this.groupBox3.Location = new System.Drawing.Point(256, 115);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(1000, 477);
            this.groupBox3.TabIndex = 2;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Visor";
            // 
            // chart_fft
            // 
            chartArea2.Name = "ChartArea1";
            this.chart_fft.ChartAreas.Add(chartArea2);
            legend2.Name = "Legend1";
            this.chart_fft.Legends.Add(legend2);
            this.chart_fft.Location = new System.Drawing.Point(7, 19);
            this.chart_fft.Name = "chart_fft";
            series2.ChartArea = "ChartArea1";
            series2.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.FastLine;
            series2.Legend = "Legend1";
            series2.Name = "FFT";
            this.chart_fft.Series.Add(series2);
            this.chart_fft.Size = new System.Drawing.Size(987, 447);
            this.chart_fft.TabIndex = 0;
            this.chart_fft.Text = "FFT";
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.listBox_datos);
            this.groupBox4.Location = new System.Drawing.Point(13, 115);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(237, 477);
            this.groupBox4.TabIndex = 3;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Datos recibidos";
            // 
            // listBox_datos
            // 
            this.listBox_datos.FormattingEnabled = true;
            this.listBox_datos.Location = new System.Drawing.Point(7, 20);
            this.listBox_datos.Name = "listBox_datos";
            this.listBox_datos.Size = new System.Drawing.Size(224, 446);
            this.listBox_datos.TabIndex = 0;
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.btn_save);
            this.groupBox5.Controls.Add(this.btn_borrar);
            this.groupBox5.Location = new System.Drawing.Point(841, 12);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(415, 95);
            this.groupBox5.TabIndex = 4;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Utilidades";
            // 
            // btn_borrar
            // 
            this.btn_borrar.Location = new System.Drawing.Point(7, 20);
            this.btn_borrar.Name = "btn_borrar";
            this.btn_borrar.Size = new System.Drawing.Size(75, 23);
            this.btn_borrar.TabIndex = 0;
            this.btn_borrar.Text = "Borrar";
            this.btn_borrar.UseVisualStyleBackColor = true;
            this.btn_borrar.Click += new System.EventHandler(this.btn_borrar_Click);
            // 
            // btn_save
            // 
            this.btn_save.Enabled = false;
            this.btn_save.Location = new System.Drawing.Point(7, 62);
            this.btn_save.Name = "btn_save";
            this.btn_save.Size = new System.Drawing.Size(75, 23);
            this.btn_save.TabIndex = 1;
            this.btn_save.Text = "Guardar";
            this.btn_save.UseVisualStyleBackColor = true;
            this.btn_save.Click += new System.EventHandler(this.btn_save_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1268, 604);
            this.Controls.Add(this.groupBox5);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Name = "Form1";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "FFT VISOR";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.chart_fft)).EndInit();
            this.groupBox4.ResumeLayout(false);
            this.groupBox5.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ComboBox cmbBox_baudRate;
        private System.Windows.Forms.ComboBox cmbBox_puerto;
        private System.Windows.Forms.Button btn_escanear;
        private System.Windows.Forms.Button btn_conectar;
        private System.Windows.Forms.ComboBox cmbBox_dataBits;
        private System.Windows.Forms.ComboBox cmbBox_parity;
        private System.Windows.Forms.ComboBox cmbBox_handshake;
        private System.Windows.Forms.ComboBox cmbBox_stopBits;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.ListBox listBox_eventos;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.ListBox listBox_datos;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart_fft;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.Button btn_borrar;
        private System.Windows.Forms.Button btn_save;
    }
}

