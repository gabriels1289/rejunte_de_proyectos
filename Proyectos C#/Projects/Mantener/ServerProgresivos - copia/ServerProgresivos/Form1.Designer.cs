namespace ServerProgresivos
{
    partial class Form1
    {
        /// <summary>
        /// Variable del diseñador requerida.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Limpiar los recursos que se estén utilizando.
        /// </summary>
        /// <param name="disposing">true si los recursos administrados se deben desechar; false en caso contrario.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Código generado por el Diseñador de Windows Forms

        /// <summary>
        /// Método necesario para admitir el Diseñador. No se puede modificar
        /// el contenido del método con el editor de código.
        /// </summary>
        private void InitializeComponent()
        {
            this.richTextBox_Msg = new System.Windows.Forms.RichTextBox();
            this.richTextBox_IPs = new System.Windows.Forms.RichTextBox();
            this.textBox_IP_filtro = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.checkBox_Filtrar_IP = new System.Windows.Forms.CheckBox();
            this.label2 = new System.Windows.Forms.Label();
            this.checkBox_FormatoMsg = new System.Windows.Forms.CheckBox();
            this.button_Clear_BoxIPs = new System.Windows.Forms.Button();
            this.button_Configuracion = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.button_Clear_BoxMsg = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // richTextBox_Msg
            // 
            this.richTextBox_Msg.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.richTextBox_Msg.Location = new System.Drawing.Point(13, 32);
            this.richTextBox_Msg.Name = "richTextBox_Msg";
            this.richTextBox_Msg.ReadOnly = true;
            this.richTextBox_Msg.Size = new System.Drawing.Size(1092, 744);
            this.richTextBox_Msg.TabIndex = 0;
            this.richTextBox_Msg.Text = "";
            // 
            // richTextBox_IPs
            // 
            this.richTextBox_IPs.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.richTextBox_IPs.Location = new System.Drawing.Point(1134, 32);
            this.richTextBox_IPs.Name = "richTextBox_IPs";
            this.richTextBox_IPs.ReadOnly = true;
            this.richTextBox_IPs.Size = new System.Drawing.Size(280, 744);
            this.richTextBox_IPs.TabIndex = 1;
            this.richTextBox_IPs.Text = "";
            // 
            // textBox_IP_filtro
            // 
            this.textBox_IP_filtro.Location = new System.Drawing.Point(1185, 6);
            this.textBox_IP_filtro.Name = "textBox_IP_filtro";
            this.textBox_IP_filtro.Size = new System.Drawing.Size(86, 20);
            this.textBox_IP_filtro.TabIndex = 2;
            this.textBox_IP_filtro.Text = "192.168.1.254";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(1131, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(48, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "Filtrar IP:";
            // 
            // checkBox_Filtrar_IP
            // 
            this.checkBox_Filtrar_IP.AutoSize = true;
            this.checkBox_Filtrar_IP.Location = new System.Drawing.Point(1277, 8);
            this.checkBox_Filtrar_IP.Name = "checkBox_Filtrar_IP";
            this.checkBox_Filtrar_IP.Size = new System.Drawing.Size(80, 17);
            this.checkBox_Filtrar_IP.TabIndex = 4;
            this.checkBox_Filtrar_IP.Text = "Aplicar filtro";
            this.checkBox_Filtrar_IP.UseVisualStyleBackColor = true;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(10, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(153, 13);
            this.label2.TabIndex = 5;
            this.label2.Text = "Mostrar mensajes formateados:";
            // 
            // checkBox_FormatoMsg
            // 
            this.checkBox_FormatoMsg.AutoSize = true;
            this.checkBox_FormatoMsg.Checked = true;
            this.checkBox_FormatoMsg.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBox_FormatoMsg.Location = new System.Drawing.Point(169, 9);
            this.checkBox_FormatoMsg.Name = "checkBox_FormatoMsg";
            this.checkBox_FormatoMsg.Size = new System.Drawing.Size(15, 14);
            this.checkBox_FormatoMsg.TabIndex = 6;
            this.checkBox_FormatoMsg.UseVisualStyleBackColor = true;
            // 
            // button_Clear_BoxIPs
            // 
            this.button_Clear_BoxIPs.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.button_Clear_BoxIPs.Location = new System.Drawing.Point(1134, 782);
            this.button_Clear_BoxIPs.Name = "button_Clear_BoxIPs";
            this.button_Clear_BoxIPs.Size = new System.Drawing.Size(83, 37);
            this.button_Clear_BoxIPs.TabIndex = 7;
            this.button_Clear_BoxIPs.Text = "Limpiar";
            this.button_Clear_BoxIPs.UseVisualStyleBackColor = false;
            this.button_Clear_BoxIPs.Click += new System.EventHandler(this.button_Clear_BoxIPs_Click);
            // 
            // button_Configuracion
            // 
            this.button_Configuracion.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.button_Configuracion.Location = new System.Drawing.Point(1233, 782);
            this.button_Configuracion.Name = "button_Configuracion";
            this.button_Configuracion.Size = new System.Drawing.Size(83, 37);
            this.button_Configuracion.TabIndex = 8;
            this.button_Configuracion.Text = "Configuración";
            this.button_Configuracion.UseVisualStyleBackColor = false;
            // 
            // button1
            // 
            this.button1.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.button1.Location = new System.Drawing.Point(1331, 782);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(83, 37);
            this.button1.TabIndex = 9;
            this.button1.Text = "Tests";
            this.button1.UseVisualStyleBackColor = false;
            // 
            // button_Clear_BoxMsg
            // 
            this.button_Clear_BoxMsg.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.button_Clear_BoxMsg.Location = new System.Drawing.Point(13, 782);
            this.button_Clear_BoxMsg.Name = "button_Clear_BoxMsg";
            this.button_Clear_BoxMsg.Size = new System.Drawing.Size(83, 37);
            this.button_Clear_BoxMsg.TabIndex = 10;
            this.button_Clear_BoxMsg.Text = "Limpiar";
            this.button_Clear_BoxMsg.UseVisualStyleBackColor = false;
            this.button_Clear_BoxMsg.Click += new System.EventHandler(this.button_Clear_BoxMsg_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.ClientSize = new System.Drawing.Size(1426, 823);
            this.Controls.Add(this.button_Clear_BoxMsg);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.button_Configuracion);
            this.Controls.Add(this.button_Clear_BoxIPs);
            this.Controls.Add(this.checkBox_FormatoMsg);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.checkBox_Filtrar_IP);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.textBox_IP_filtro);
            this.Controls.Add(this.richTextBox_IPs);
            this.Controls.Add(this.richTextBox_Msg);
            this.ForeColor = System.Drawing.SystemColors.ControlLightLight;
            this.Name = "Form1";
            this.Text = "Servidor De Progresivos";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.RichTextBox richTextBox_Msg;
        private System.Windows.Forms.RichTextBox richTextBox_IPs;
        private System.Windows.Forms.TextBox textBox_IP_filtro;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox checkBox_Filtrar_IP;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.CheckBox checkBox_FormatoMsg;
        private System.Windows.Forms.Button button_Clear_BoxIPs;
        private System.Windows.Forms.Button button_Configuracion;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button_Clear_BoxMsg;
    }
}

