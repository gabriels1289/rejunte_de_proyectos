using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ServerProgresivos
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Borra el contenido del TextBox de mensajes
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void button_Clear_BoxMsg_Click(object sender, EventArgs e)
        {
            richTextBox_Msg.Clear();
        }

        /// <summary>
        /// Borra el contenido del TextBox de IPs
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void button_Clear_BoxIPs_Click(object sender, EventArgs e)
        {
            richTextBox_IPs.Clear();
        }
    }
}
