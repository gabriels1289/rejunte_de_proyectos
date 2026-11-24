using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace AnalizadorDeTramas
{
    public static class MetodosExtendidos
    {
        // metodo para agregar un menu contextual a multiples objetos
        public static void AddContextMenu(this RichTextBox rtb)
        {
            if (rtb.ContextMenuStrip == null)
            {
                ContextMenuStrip cms = new ContextMenuStrip()
                {
                    ShowImageMargin = false
                };

                ToolStripMenuItem tsmiUndo = new ToolStripMenuItem("Atrás");
                tsmiUndo.Click += (sender, e) => { if (rtb.CanUndo) rtb.Undo(); }; //rtb.Undo();
                tsmiUndo.ShortcutKeys = Keys.Z | Keys.Control;
                cms.Items.Add(tsmiUndo);

                ToolStripMenuItem tsmiRedo = new ToolStripMenuItem("Adelante");
                tsmiRedo.Click += (sender, e) => { if (rtb.CanRedo) rtb.Redo(); };//rtb.Redo();
                tsmiRedo.ShortcutKeys = Keys.Y | Keys.Control;
                cms.Items.Add(tsmiRedo);

                cms.Items.Add(new ToolStripSeparator());

                ToolStripMenuItem tsmiCut = new ToolStripMenuItem("Cortar");
                tsmiCut.Click += (sender, e) => rtb.Cut();
                tsmiCut.ShortcutKeys = Keys.X | Keys.Control;
                cms.Items.Add(tsmiCut);

                ToolStripMenuItem tsmiCopy = new ToolStripMenuItem("Copiar");
                tsmiCopy.Click += (sender, e) => rtb.Copy();
                tsmiCopy.ShortcutKeys = Keys.C | Keys.Control;
                cms.Items.Add(tsmiCopy);

                ToolStripMenuItem tsmiPaste = new ToolStripMenuItem("Pegar");
                tsmiPaste.Click += (sender, e) => rtb.Paste();
                tsmiPaste.ShortcutKeys = Keys.V | Keys.Control;
                cms.Items.Add(tsmiPaste);
                
                cms.Items.Add(new ToolStripSeparator());

                ToolStripMenuItem tsmiDelete = new ToolStripMenuItem("Borrar");
                tsmiDelete.Click += (sender, e) => rtb.SelectedText = "";
                cms.Items.Add(tsmiDelete);

                ToolStripMenuItem tsmiSelectAll = new ToolStripMenuItem("Seleccionar todo");
                tsmiSelectAll.Click += (sender, e) => rtb.SelectAll();
                tsmiSelectAll.ShortcutKeys = Keys.A | Keys.Control;
                cms.Items.Add(tsmiSelectAll);

                cms.Items.Add(new ToolStripSeparator());

                ToolStripMenuItem tsmiFont = new ToolStripMenuItem("Fuente");
                tsmiFont.Click += (sender, e) =>
                {
                    FontDialog font = new FontDialog();
                    //se aplica el tipo de fuente del rich textbox al objeto que hemos creado
                    font.Font = rtb.Font;
                    //se hace la condicion para cambiar el tipo de fuente al presionar el boton ok
                    if (font.ShowDialog() == DialogResult.OK)
                    {
                        if (!string.IsNullOrEmpty(rtb.SelectedText))
                        {
                            rtb.SelectionFont = font.Font;
                        }
                        //rtb.Font = font.Font;
                    }
                };
                cms.Items.Add(tsmiFont);

                ToolStripMenuItem tsmiColor = new ToolStripMenuItem("Color");
                tsmiColor.Click += (sender, e) =>
                {
                    //se crea un objeto de tipo colordialog que servira para cabiar color de la fuente 
                    ColorDialog color = new ColorDialog();
                    //se hace la condicion para cambiar el color de la fuente al presionar el boton ok 
                    if (color.ShowDialog() == DialogResult.OK)
                    {
                        if (!string.IsNullOrEmpty(rtb.SelectedText))
                        {
                            rtb.SelectionColor = color.Color;
                        }
                        // rtb.ForeColor = color.Color;
                    }
                };
                cms.Items.Add(tsmiColor);

                ToolStripMenuItem tsmiHighlight = new ToolStripMenuItem("Resaltar");
                tsmiHighlight.Click += (sender, e) =>
                {
                    //se crea un objeto de tipo colordialog que servira para cabiar color del background
                    ColorDialog color = new ColorDialog();
                    //se hace la condicion para cambiar el color de la fuente al presionar el boton ok 
                    if (color.ShowDialog() == DialogResult.OK)
                    {
                        if (!string.IsNullOrEmpty(rtb.SelectedText))
                        {
                            rtb.SelectionBackColor = color.Color;
                        }
                        // rtb.ForeColor = color.Color;
                    }
                };
                cms.Items.Add(tsmiHighlight);

                cms.Items.Add(new ToolStripSeparator());

                ToolStripMenuItem tsmiSaveMyFile = new ToolStripMenuItem("Guardar");
                tsmiSaveMyFile.Click += (sender, e) =>
                {
                    // Create a SaveFileDialog to request a path and file name to save to.
                    SaveFileDialog saveFile1 = new SaveFileDialog();

                    // Set the properties on SaveFileDialog1 so the user is 
                    // prompted to create the file if it doesn't exist 
                    // or overwrite the file if it does exist.
                    /*SaveFileDialog1.CreatePrompt = true;
                    SaveFileDialog1.OverwritePrompt = true;*/

                    // Initialize the SaveFileDialog to specify the RTF extention for the file.
                    saveFile1.DefaultExt = "*.rtf";
                    saveFile1.Filter = "RTF Files|*.rtf";

                    // Determine whether the user selected a file name from the saveFileDialog.
                    if (saveFile1.ShowDialog() == System.Windows.Forms.DialogResult.OK &&
                       saveFile1.FileName.Length > 0)
                    {
                        // Save the contents of the RichTextBox into the file.
                        rtb.SaveFile(saveFile1.FileName);
                    }
                };
                tsmiSaveMyFile.ShortcutKeys = Keys.S | Keys.Control;
                cms.Items.Add(tsmiSaveMyFile);

                ToolStripMenuItem tsmiLoadMyFile = new ToolStripMenuItem("Cargar");
                tsmiLoadMyFile.Click += (sender, e) =>
                {
                    // Create an OpenFileDialog to request a file to open.
                    OpenFileDialog openFile1 = new OpenFileDialog();

                    // Initialize the OpenFileDialog to look for RTF files.
                    openFile1.DefaultExt = "*.rtf";
                    openFile1.Filter = "RTF Files|*.rtf";

                    // Determine whether the user selected a file from the OpenFileDialog.
                    if (openFile1.ShowDialog() == System.Windows.Forms.DialogResult.OK &&
                       openFile1.FileName.Length > 0)
                    {
                        // Load the contents of the file into the RichTextBox.
                        rtb.LoadFile(openFile1.FileName);
                    }
                };
                tsmiLoadMyFile.ShortcutKeys = Keys.O | Keys.Control;
                cms.Items.Add(tsmiLoadMyFile);

                cms.Opening += (sender, e) =>
                {
                    tsmiUndo.Enabled = !rtb.ReadOnly && rtb.CanUndo;
                    tsmiRedo.Enabled = !rtb.ReadOnly && rtb.CanRedo;
                    tsmiCut.Enabled = !rtb.ReadOnly && rtb.SelectionLength > 0;
                    tsmiCopy.Enabled = rtb.SelectionLength > 0;
                    tsmiPaste.Enabled = !rtb.ReadOnly && Clipboard.ContainsText();
                    tsmiDelete.Enabled = !rtb.ReadOnly && rtb.SelectionLength > 0;
                    tsmiSelectAll.Enabled = rtb.TextLength > 0 && rtb.SelectionLength < rtb.TextLength;
                    tsmiColor.Enabled = rtb.SelectionLength > 0;
                    tsmiFont.Enabled = rtb.SelectionLength > 0;
                    tsmiHighlight.Enabled = rtb.SelectionLength > 0;
                    tsmiSaveMyFile.Enabled = rtb.TextLength > 0;
                    tsmiLoadMyFile.Enabled = true;
                };

                rtb.ContextMenuStrip = cms;
            }
        }
    }
}
