namespace Sistema_Tarjetas_Lavadero
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
            this.btn_guardar = new System.Windows.Forms.Button();
            this.btn_Borrar = new System.Windows.Forms.Button();
            this.listBox_info = new System.Windows.Forms.ListBox();
            this.cmbBox_puertos = new System.Windows.Forms.ComboBox();
            this.btn_conectar = new System.Windows.Forms.Button();
            this.btn_escanear = new System.Windows.Forms.Button();
            this.listBox_eventos = new System.Windows.Forms.ListBox();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.btn_guardar);
            this.panel1.Controls.Add(this.btn_Borrar);
            this.panel1.Controls.Add(this.listBox_info);
            this.panel1.Controls.Add(this.cmbBox_puertos);
            this.panel1.Controls.Add(this.btn_conectar);
            this.panel1.Controls.Add(this.btn_escanear);
            this.panel1.Location = new System.Drawing.Point(7, 8);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(968, 87);
            this.panel1.TabIndex = 0;
            // 
            // btn_guardar
            // 
            this.btn_guardar.Location = new System.Drawing.Point(312, 4);
            this.btn_guardar.Name = "btn_guardar";
            this.btn_guardar.Size = new System.Drawing.Size(143, 35);
            this.btn_guardar.TabIndex = 6;
            this.btn_guardar.Text = "Guardar";
            this.btn_guardar.UseVisualStyleBackColor = true;
            this.btn_guardar.Click += new System.EventHandler(this.btn_guardar_Click);
            // 
            // btn_Borrar
            // 
            this.btn_Borrar.Enabled = false;
            this.btn_Borrar.Location = new System.Drawing.Point(154, 47);
            this.btn_Borrar.Name = "btn_Borrar";
            this.btn_Borrar.Size = new System.Drawing.Size(133, 36);
            this.btn_Borrar.TabIndex = 5;
            this.btn_Borrar.Text = "Borrar";
            this.btn_Borrar.UseVisualStyleBackColor = true;
            this.btn_Borrar.Click += new System.EventHandler(this.btn_Borrar_Click);
            // 
            // listBox_info
            // 
            this.listBox_info.FormattingEnabled = true;
            this.listBox_info.Location = new System.Drawing.Point(461, 1);
            this.listBox_info.Name = "listBox_info";
            this.listBox_info.Size = new System.Drawing.Size(503, 82);
            this.listBox_info.TabIndex = 4;
            // 
            // cmbBox_puertos
            // 
            this.cmbBox_puertos.FormattingEnabled = true;
            this.cmbBox_puertos.Location = new System.Drawing.Point(154, 11);
            this.cmbBox_puertos.Name = "cmbBox_puertos";
            this.cmbBox_puertos.Size = new System.Drawing.Size(130, 21);
            this.cmbBox_puertos.TabIndex = 2;
            this.cmbBox_puertos.SelectedIndexChanged += new System.EventHandler(this.cmbBox_puertos_SelectedIndexChanged);
            // 
            // btn_conectar
            // 
            this.btn_conectar.Enabled = false;
            this.btn_conectar.Location = new System.Drawing.Point(13, 48);
            this.btn_conectar.Name = "btn_conectar";
            this.btn_conectar.Size = new System.Drawing.Size(135, 36);
            this.btn_conectar.TabIndex = 1;
            this.btn_conectar.Text = "Conectar";
            this.btn_conectar.UseVisualStyleBackColor = true;
            this.btn_conectar.Click += new System.EventHandler(this.btn_conectar_Click);
            // 
            // btn_escanear
            // 
            this.btn_escanear.Location = new System.Drawing.Point(13, 4);
            this.btn_escanear.Name = "btn_escanear";
            this.btn_escanear.Size = new System.Drawing.Size(135, 32);
            this.btn_escanear.TabIndex = 0;
            this.btn_escanear.Text = "Escanear";
            this.btn_escanear.UseVisualStyleBackColor = true;
            this.btn_escanear.Click += new System.EventHandler(this.btn_escanear_Click);
            // 
            // listBox_eventos
            // 
            this.listBox_eventos.FormattingEnabled = true;
            this.listBox_eventos.Location = new System.Drawing.Point(7, 103);
            this.listBox_eventos.Name = "listBox_eventos";
            this.listBox_eventos.Size = new System.Drawing.Size(964, 277);
            this.listBox_eventos.TabIndex = 1;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(983, 391);
            this.Controls.Add(this.listBox_eventos);
            this.Controls.Add(this.panel1);
            this.Name = "Form1";
            this.Text = "Sistema de Tarjetas para Lavadero";
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.ListBox listBox_info;
        private System.Windows.Forms.ComboBox cmbBox_puertos;
        private System.Windows.Forms.Button btn_conectar;
        private System.Windows.Forms.Button btn_escanear;
        private System.Windows.Forms.ListBox listBox_eventos;
        private System.Windows.Forms.Button btn_Borrar;
        private System.Windows.Forms.Button btn_guardar;
    }
}

