namespace ServidorDeImagen
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
            this.panel1 = new System.Windows.Forms.Panel();
            this.txbSegundos = new System.Windows.Forms.TextBox();
            this.lblSeparadorMMss = new System.Windows.Forms.Label();
            this.txbMinutos = new System.Windows.Forms.TextBox();
            this.lblSeparadorHHmm = new System.Windows.Forms.Label();
            this.panel5 = new System.Windows.Forms.Panel();
            this.txbPassword = new System.Windows.Forms.TextBox();
            this.lblPass = new System.Windows.Forms.Label();
            this.txboxIP = new System.Windows.Forms.TextBox();
            this.lblIP = new System.Windows.Forms.Label();
            this.btnIDcliente = new System.Windows.Forms.Button();
            this.txbHora = new System.Windows.Forms.TextBox();
            this.lblTimeLeft = new System.Windows.Forms.Label();
            this.lblTiempoSiguiente = new System.Windows.Forms.Label();
            this.lblTiempoEntre = new System.Windows.Forms.Label();
            this.btnCargarCarpeta = new System.Windows.Forms.Button();
            this.txbAlto = new System.Windows.Forms.TextBox();
            this.lblAlto = new System.Windows.Forms.Label();
            this.lblX = new System.Windows.Forms.Label();
            this.txbAncho = new System.Windows.Forms.TextBox();
            this.lblAncho = new System.Windows.Forms.Label();
            this.linklblDirectorioModificadas = new System.Windows.Forms.LinkLabel();
            this.pictBoxActual = new System.Windows.Forms.PictureBox();
            this.panel2 = new System.Windows.Forms.Panel();
            this.lstboxPropiedadesImagen = new System.Windows.Forms.ListBox();
            this.lstboxImagenes = new System.Windows.Forms.ListBox();
            this.lblPropiedadesBMP = new System.Windows.Forms.Label();
            this.lblListaImagenes = new System.Windows.Forms.Label();
            this.panel3 = new System.Windows.Forms.Panel();
            this.lstboxIPs = new System.Windows.Forms.ListBox();
            this.lblIPs = new System.Windows.Forms.Label();
            this.panel4 = new System.Windows.Forms.Panel();
            this.btnSiguiente = new System.Windows.Forms.Button();
            this.pictBoxSiguiente = new System.Windows.Forms.PictureBox();
            this.lblImagenSiguiente = new System.Windows.Forms.Label();
            this.openFD = new System.Windows.Forms.OpenFileDialog();
            this.panel1.SuspendLayout();
            this.panel5.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictBoxActual)).BeginInit();
            this.panel2.SuspendLayout();
            this.panel3.SuspendLayout();
            this.panel4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictBoxSiguiente)).BeginInit();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.txbSegundos);
            this.panel1.Controls.Add(this.lblSeparadorMMss);
            this.panel1.Controls.Add(this.txbMinutos);
            this.panel1.Controls.Add(this.lblSeparadorHHmm);
            this.panel1.Controls.Add(this.panel5);
            this.panel1.Controls.Add(this.txbHora);
            this.panel1.Controls.Add(this.lblTimeLeft);
            this.panel1.Controls.Add(this.lblTiempoSiguiente);
            this.panel1.Controls.Add(this.lblTiempoEntre);
            this.panel1.Controls.Add(this.btnCargarCarpeta);
            this.panel1.Controls.Add(this.txbAlto);
            this.panel1.Controls.Add(this.lblAlto);
            this.panel1.Controls.Add(this.lblX);
            this.panel1.Controls.Add(this.txbAncho);
            this.panel1.Controls.Add(this.lblAncho);
            this.panel1.Controls.Add(this.linklblDirectorioModificadas);
            this.panel1.Controls.Add(this.pictBoxActual);
            this.panel1.Location = new System.Drawing.Point(356, 12);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(602, 628);
            this.panel1.TabIndex = 0;
            // 
            // txbSegundos
            // 
            this.txbSegundos.Location = new System.Drawing.Point(270, 576);
            this.txbSegundos.Name = "txbSegundos";
            this.txbSegundos.Size = new System.Drawing.Size(38, 20);
            this.txbSegundos.TabIndex = 20;
            this.txbSegundos.Text = "30";
            this.txbSegundos.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.txbSegundos.TextChanged += new System.EventHandler(this.txbSegundos_TextChanged);
            // 
            // lblSeparadorMMss
            // 
            this.lblSeparadorMMss.AutoSize = true;
            this.lblSeparadorMMss.Location = new System.Drawing.Point(254, 579);
            this.lblSeparadorMMss.Name = "lblSeparadorMMss";
            this.lblSeparadorMMss.Size = new System.Drawing.Size(10, 13);
            this.lblSeparadorMMss.TabIndex = 19;
            this.lblSeparadorMMss.Text = ":";
            // 
            // txbMinutos
            // 
            this.txbMinutos.Location = new System.Drawing.Point(210, 576);
            this.txbMinutos.Name = "txbMinutos";
            this.txbMinutos.Size = new System.Drawing.Size(38, 20);
            this.txbMinutos.TabIndex = 18;
            this.txbMinutos.Text = "00";
            this.txbMinutos.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.txbMinutos.TextChanged += new System.EventHandler(this.txbMinutos_TextChanged);
            // 
            // lblSeparadorHHmm
            // 
            this.lblSeparadorHHmm.AutoSize = true;
            this.lblSeparadorHHmm.Location = new System.Drawing.Point(194, 579);
            this.lblSeparadorHHmm.Name = "lblSeparadorHHmm";
            this.lblSeparadorHHmm.Size = new System.Drawing.Size(10, 13);
            this.lblSeparadorHHmm.TabIndex = 17;
            this.lblSeparadorHHmm.Text = ":";
            // 
            // panel5
            // 
            this.panel5.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel5.Controls.Add(this.txbPassword);
            this.panel5.Controls.Add(this.lblPass);
            this.panel5.Controls.Add(this.txboxIP);
            this.panel5.Controls.Add(this.lblIP);
            this.panel5.Controls.Add(this.btnIDcliente);
            this.panel5.Location = new System.Drawing.Point(422, 542);
            this.panel5.Name = "panel5";
            this.panel5.Size = new System.Drawing.Size(167, 78);
            this.panel5.TabIndex = 15;
            // 
            // txbPassword
            // 
            this.txbPassword.Location = new System.Drawing.Point(49, 53);
            this.txbPassword.Name = "txbPassword";
            this.txbPassword.Size = new System.Drawing.Size(100, 20);
            this.txbPassword.TabIndex = 9;
            this.txbPassword.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // lblPass
            // 
            this.lblPass.AutoSize = true;
            this.lblPass.Location = new System.Drawing.Point(20, 57);
            this.lblPass.Name = "lblPass";
            this.lblPass.Size = new System.Drawing.Size(36, 13);
            this.lblPass.TabIndex = 8;
            this.lblPass.Text = "Pass: ";
            // 
            // txboxIP
            // 
            this.txboxIP.Location = new System.Drawing.Point(49, 27);
            this.txboxIP.Name = "txboxIP";
            this.txboxIP.Size = new System.Drawing.Size(100, 20);
            this.txboxIP.TabIndex = 7;
            this.txboxIP.Text = "192.168.2.253";
            this.txboxIP.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // lblIP
            // 
            this.lblIP.AutoSize = true;
            this.lblIP.Location = new System.Drawing.Point(20, 31);
            this.lblIP.Name = "lblIP";
            this.lblIP.Size = new System.Drawing.Size(23, 13);
            this.lblIP.TabIndex = 6;
            this.lblIP.Text = "IP: ";
            // 
            // btnIDcliente
            // 
            this.btnIDcliente.AutoSize = true;
            this.btnIDcliente.Location = new System.Drawing.Point(20, 0);
            this.btnIDcliente.Name = "btnIDcliente";
            this.btnIDcliente.Size = new System.Drawing.Size(129, 23);
            this.btnIDcliente.TabIndex = 5;
            this.btnIDcliente.Text = "Corroborar ID de cliente";
            this.btnIDcliente.UseVisualStyleBackColor = true;
            this.btnIDcliente.Click += new System.EventHandler(this.btnIDcliente_Click);
            // 
            // txbHora
            // 
            this.txbHora.Location = new System.Drawing.Point(150, 576);
            this.txbHora.Name = "txbHora";
            this.txbHora.Size = new System.Drawing.Size(38, 20);
            this.txbHora.TabIndex = 14;
            this.txbHora.Text = "00";
            this.txbHora.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.txbHora.TextChanged += new System.EventHandler(this.txbHora_TextChanged);
            // 
            // lblTimeLeft
            // 
            this.lblTimeLeft.AutoSize = true;
            this.lblTimeLeft.Location = new System.Drawing.Point(201, 604);
            this.lblTimeLeft.Name = "lblTimeLeft";
            this.lblTimeLeft.Size = new System.Drawing.Size(49, 13);
            this.lblTimeLeft.TabIndex = 13;
            this.lblTimeLeft.Text = "00:05:00";
            // 
            // lblTiempoSiguiente
            // 
            this.lblTiempoSiguiente.AutoSize = true;
            this.lblTiempoSiguiente.Location = new System.Drawing.Point(31, 604);
            this.lblTiempoSiguiente.Name = "lblTiempoSiguiente";
            this.lblTiempoSiguiente.Size = new System.Drawing.Size(165, 13);
            this.lblTiempoSiguiente.TabIndex = 12;
            this.lblTiempoSiguiente.Text = "Tiempo para la siguiente imagen: ";
            // 
            // lblTiempoEntre
            // 
            this.lblTiempoEntre.AutoSize = true;
            this.lblTiempoEntre.Location = new System.Drawing.Point(31, 579);
            this.lblTiempoEntre.Name = "lblTiempoEntre";
            this.lblTiempoEntre.Size = new System.Drawing.Size(123, 13);
            this.lblTiempoEntre.TabIndex = 10;
            this.lblTiempoEntre.Text = "Tiempo entre imágenes: ";
            // 
            // btnCargarCarpeta
            // 
            this.btnCargarCarpeta.AutoSize = true;
            this.btnCargarCarpeta.Location = new System.Drawing.Point(293, 542);
            this.btnCargarCarpeta.Name = "btnCargarCarpeta";
            this.btnCargarCarpeta.Size = new System.Drawing.Size(88, 23);
            this.btnCargarCarpeta.TabIndex = 9;
            this.btnCargarCarpeta.Text = "Cargar Imagen";
            this.btnCargarCarpeta.UseVisualStyleBackColor = true;
            this.btnCargarCarpeta.Click += new System.EventHandler(this.btnCargarCarpeta_Click);
            // 
            // txbAlto
            // 
            this.txbAlto.Location = new System.Drawing.Point(204, 545);
            this.txbAlto.Name = "txbAlto";
            this.txbAlto.Size = new System.Drawing.Size(60, 20);
            this.txbAlto.TabIndex = 8;
            this.txbAlto.Text = "272";
            this.txbAlto.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // lblAlto
            // 
            this.lblAlto.AutoSize = true;
            this.lblAlto.Location = new System.Drawing.Point(167, 548);
            this.lblAlto.Name = "lblAlto";
            this.lblAlto.Size = new System.Drawing.Size(31, 13);
            this.lblAlto.TabIndex = 7;
            this.lblAlto.Text = "Alto: ";
            // 
            // lblX
            // 
            this.lblX.AutoSize = true;
            this.lblX.Location = new System.Drawing.Point(147, 548);
            this.lblX.Name = "lblX";
            this.lblX.Size = new System.Drawing.Size(14, 13);
            this.lblX.TabIndex = 6;
            this.lblX.Text = "X";
            // 
            // txbAncho
            // 
            this.txbAncho.Location = new System.Drawing.Point(81, 544);
            this.txbAncho.Name = "txbAncho";
            this.txbAncho.Size = new System.Drawing.Size(60, 20);
            this.txbAncho.TabIndex = 5;
            this.txbAncho.Text = "480";
            this.txbAncho.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // lblAncho
            // 
            this.lblAncho.AutoSize = true;
            this.lblAncho.Location = new System.Drawing.Point(31, 547);
            this.lblAncho.Name = "lblAncho";
            this.lblAncho.Size = new System.Drawing.Size(44, 13);
            this.lblAncho.TabIndex = 4;
            this.lblAncho.Text = "Ancho: ";
            // 
            // linklblDirectorioModificadas
            // 
            this.linklblDirectorioModificadas.AutoSize = true;
            this.linklblDirectorioModificadas.Location = new System.Drawing.Point(207, 7);
            this.linklblDirectorioModificadas.Name = "linklblDirectorioModificadas";
            this.linklblDirectorioModificadas.Size = new System.Drawing.Size(178, 13);
            this.linklblDirectorioModificadas.TabIndex = 3;
            this.linklblDirectorioModificadas.TabStop = true;
            this.linklblDirectorioModificadas.Text = "Directorio De Imagenes Modificadas";
            this.linklblDirectorioModificadas.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linklblDirectorioModificadas_LinkClicked);
            // 
            // pictBoxActual
            // 
            this.pictBoxActual.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.pictBoxActual.Location = new System.Drawing.Point(34, 24);
            this.pictBoxActual.Name = "pictBoxActual";
            this.pictBoxActual.Size = new System.Drawing.Size(555, 512);
            this.pictBoxActual.TabIndex = 1;
            this.pictBoxActual.TabStop = false;
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.lstboxPropiedadesImagen);
            this.panel2.Controls.Add(this.lstboxImagenes);
            this.panel2.Controls.Add(this.lblPropiedadesBMP);
            this.panel2.Controls.Add(this.lblListaImagenes);
            this.panel2.Location = new System.Drawing.Point(13, 12);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(337, 628);
            this.panel2.TabIndex = 1;
            // 
            // lstboxPropiedadesImagen
            // 
            this.lstboxPropiedadesImagen.FormattingEnabled = true;
            this.lstboxPropiedadesImagen.HorizontalScrollbar = true;
            this.lstboxPropiedadesImagen.Location = new System.Drawing.Point(12, 395);
            this.lstboxPropiedadesImagen.Name = "lstboxPropiedadesImagen";
            this.lstboxPropiedadesImagen.SelectionMode = System.Windows.Forms.SelectionMode.None;
            this.lstboxPropiedadesImagen.Size = new System.Drawing.Size(312, 225);
            this.lstboxPropiedadesImagen.TabIndex = 16;
            // 
            // lstboxImagenes
            // 
            this.lstboxImagenes.FormattingEnabled = true;
            this.lstboxImagenes.HorizontalScrollbar = true;
            this.lstboxImagenes.Location = new System.Drawing.Point(12, 24);
            this.lstboxImagenes.Name = "lstboxImagenes";
            this.lstboxImagenes.SelectionMode = System.Windows.Forms.SelectionMode.None;
            this.lstboxImagenes.Size = new System.Drawing.Size(312, 342);
            this.lstboxImagenes.TabIndex = 1;
            // 
            // lblPropiedadesBMP
            // 
            this.lblPropiedadesBMP.AutoSize = true;
            this.lblPropiedadesBMP.Location = new System.Drawing.Point(76, 379);
            this.lblPropiedadesBMP.Name = "lblPropiedadesBMP";
            this.lblPropiedadesBMP.Size = new System.Drawing.Size(164, 13);
            this.lblPropiedadesBMP.TabIndex = 15;
            this.lblPropiedadesBMP.Text = "Propiedades de la imagen actual:";
            // 
            // lblListaImagenes
            // 
            this.lblListaImagenes.AutoSize = true;
            this.lblListaImagenes.Location = new System.Drawing.Point(76, 8);
            this.lblListaImagenes.Name = "lblListaImagenes";
            this.lblListaImagenes.Size = new System.Drawing.Size(177, 13);
            this.lblListaImagenes.TabIndex = 0;
            this.lblListaImagenes.Text = "Lista de imágenes redimensionadas:";
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.lstboxIPs);
            this.panel3.Controls.Add(this.lblIPs);
            this.panel3.Location = new System.Drawing.Point(992, 12);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(202, 270);
            this.panel3.TabIndex = 2;
            // 
            // lstboxIPs
            // 
            this.lstboxIPs.FormattingEnabled = true;
            this.lstboxIPs.HorizontalScrollbar = true;
            this.lstboxIPs.Location = new System.Drawing.Point(12, 24);
            this.lstboxIPs.Name = "lstboxIPs";
            this.lstboxIPs.SelectionMode = System.Windows.Forms.SelectionMode.None;
            this.lstboxIPs.Size = new System.Drawing.Size(179, 238);
            this.lstboxIPs.TabIndex = 1;
            // 
            // lblIPs
            // 
            this.lblIPs.AutoSize = true;
            this.lblIPs.Location = new System.Drawing.Point(9, 7);
            this.lblIPs.Name = "lblIPs";
            this.lblIPs.Size = new System.Drawing.Size(131, 13);
            this.lblIPs.TabIndex = 0;
            this.lblIPs.Text = "Direcciones IP Asociadas:";
            // 
            // panel4
            // 
            this.panel4.Controls.Add(this.btnSiguiente);
            this.panel4.Controls.Add(this.pictBoxSiguiente);
            this.panel4.Controls.Add(this.lblImagenSiguiente);
            this.panel4.Location = new System.Drawing.Point(992, 289);
            this.panel4.Name = "panel4";
            this.panel4.Size = new System.Drawing.Size(202, 351);
            this.panel4.TabIndex = 3;
            // 
            // btnSiguiente
            // 
            this.btnSiguiente.AutoSize = true;
            this.btnSiguiente.Location = new System.Drawing.Point(51, 320);
            this.btnSiguiente.Name = "btnSiguiente";
            this.btnSiguiente.Size = new System.Drawing.Size(99, 23);
            this.btnSiguiente.TabIndex = 2;
            this.btnSiguiente.Text = "Siguiente Imagen";
            this.btnSiguiente.UseVisualStyleBackColor = true;
            this.btnSiguiente.Click += new System.EventHandler(this.btnSiguiente_Click);
            // 
            // pictBoxSiguiente
            // 
            this.pictBoxSiguiente.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.pictBoxSiguiente.Location = new System.Drawing.Point(12, 23);
            this.pictBoxSiguiente.Name = "pictBoxSiguiente";
            this.pictBoxSiguiente.Size = new System.Drawing.Size(179, 292);
            this.pictBoxSiguiente.TabIndex = 1;
            this.pictBoxSiguiente.TabStop = false;
            // 
            // lblImagenSiguiente
            // 
            this.lblImagenSiguiente.AutoSize = true;
            this.lblImagenSiguiente.Location = new System.Drawing.Point(9, 7);
            this.lblImagenSiguiente.Name = "lblImagenSiguiente";
            this.lblImagenSiguiente.Size = new System.Drawing.Size(92, 13);
            this.lblImagenSiguiente.TabIndex = 0;
            this.lblImagenSiguiente.Text = "Imagen Siguiente:";
            // 
            // openFD
            // 
            this.openFD.Multiselect = true;
            this.openFD.Title = "Cargar Imagen";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1206, 644);
            this.Controls.Add(this.panel4);
            this.Controls.Add(this.panel3);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.Name = "Form1";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Servidor De Imagenes";
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.panel5.ResumeLayout(false);
            this.panel5.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictBoxActual)).EndInit();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.panel3.ResumeLayout(false);
            this.panel3.PerformLayout();
            this.panel4.ResumeLayout(false);
            this.panel4.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictBoxSiguiente)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.LinkLabel linklblDirectorioModificadas;
        private System.Windows.Forms.PictureBox pictBoxActual;
        private System.Windows.Forms.TextBox txbAlto;
        private System.Windows.Forms.Label lblAlto;
        private System.Windows.Forms.Label lblX;
        private System.Windows.Forms.TextBox txbAncho;
        private System.Windows.Forms.Label lblAncho;
        private System.Windows.Forms.Button btnCargarCarpeta;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Label lblListaImagenes;
        private System.Windows.Forms.ListBox lstboxImagenes;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.ListBox lstboxIPs;
        private System.Windows.Forms.Label lblIPs;
        private System.Windows.Forms.Panel panel4;
        private System.Windows.Forms.Label lblTimeLeft;
        private System.Windows.Forms.Label lblTiempoSiguiente;
        private System.Windows.Forms.Label lblTiempoEntre;
        private System.Windows.Forms.TextBox txbHora;
        private System.Windows.Forms.ListBox lstboxPropiedadesImagen;
        private System.Windows.Forms.Label lblPropiedadesBMP;
        private System.Windows.Forms.OpenFileDialog openFD;
        private System.Windows.Forms.Panel panel5;
        private System.Windows.Forms.TextBox txboxIP;
        private System.Windows.Forms.Label lblIP;
        private System.Windows.Forms.Button btnIDcliente;
        private System.Windows.Forms.Label lblSeparadorHHmm;
        private System.Windows.Forms.TextBox txbSegundos;
        private System.Windows.Forms.Label lblSeparadorMMss;
        private System.Windows.Forms.TextBox txbMinutos;
        private System.Windows.Forms.TextBox txbPassword;
        private System.Windows.Forms.Label lblPass;
        private System.Windows.Forms.Button btnSiguiente;
        private System.Windows.Forms.PictureBox pictBoxSiguiente;
        private System.Windows.Forms.Label lblImagenSiguiente;
    }
}

