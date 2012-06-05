using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace TestStrips
{
    public partial class FormStrips : Form
    {
        public FormStrips()
        {
            InitializeComponent();

            update_statusbar_coords();

            KeysConverter kc = new KeysConverter();
            String keys1 = kc.ConvertToString(Keys.Control | Keys.Alt | Keys.F);
            this.action1ToolStripMenuItem.ShortcutKeys = (Keys)kc.ConvertFromString(keys1);
            String keys2 = kc.ConvertToString(Keys.Control | Keys.Shift | Keys.G);
            this.action2ToolStripMenuItem.ShortcutKeys = (Keys)kc.ConvertFromString(keys2);
        }

        private void update_statusbar_coords()
        {
            String ptstring;
            ptstring = String.Format("tbp width,height({0},{1})",
                this.toolStripContainer1.Width, this.toolStripContainer1.Height);
            this.label_wh.Text = ptstring;
            ptstring = String.Format("tbp bounds({0},{1})",
                this.toolStripContainer1.Bounds.Width, this.toolStripContainer1.Bounds.Height);
            this.label_bounds.Text = ptstring;
            ptstring = String.Format("tbp size({0},{1})",
                this.toolStripContainer1.Size.Width, this.toolStripContainer1.Size.Height);
            this.label_size.Text = ptstring;
            ptstring = String.Format("tbp client size({0},{1})",
                this.toolStripContainer1.ClientSize.Width, this.toolStripContainer1.ClientSize.Height);
            this.label_clientsize.Text = ptstring;
            ptstring = String.Format("tbp client rect({0},{1})",
                this.toolStripContainer1.ClientRectangle.Width, this.toolStripContainer1.ClientRectangle.Height);
            this.label_clientrect.Text = ptstring;
            ptstring = String.Format("tbp top,bot,rt,lt({0},{1},{2},{3})",
                this.toolStripContainer1.Top, this.toolStripContainer1.Bottom,
                this.toolStripContainer1.Right, this.toolStripContainer1.Left);
            this.label_tbrl.Text = ptstring;
            ptstring = String.Format("tbp tb-top[{0}]({1},{2})",
                this.toolStripContainer1.TopToolStripPanelVisible, this.toolStripContainer1.TopToolStripPanel.Width, this.toolStripContainer1.TopToolStripPanel.Height);
            this.label_paneltop.Text = ptstring;
            ptstring = String.Format("tbp tb-bottom[{0}]({1},{2})",
                this.toolStripContainer1.BottomToolStripPanelVisible, this.toolStripContainer1.BottomToolStripPanel.Width, this.toolStripContainer1.BottomToolStripPanel.Height);
            this.label_panelbottom.Text = ptstring;
            ptstring = String.Format("tbp tb-right[{0}]({1},{2})",
                this.toolStripContainer1.RightToolStripPanelVisible, this.toolStripContainer1.RightToolStripPanel.Width, this.toolStripContainer1.RightToolStripPanel.Height);
            this.label_panelright.Text = ptstring;
            ptstring = String.Format("tbp tb-left[{0}]({1},{2})",
                this.toolStripContainer1.LeftToolStripPanelVisible, this.toolStripContainer1.LeftToolStripPanel.Width, this.toolStripContainer1.LeftToolStripPanel.Height);
            this.label_panelleft.Text = ptstring;
            ptstring = String.Format("tbp tb-content({0},{1})",
                this.toolStripContainer1.ContentPanel.Width, this.toolStripContainer1.ContentPanel.Height);
            this.label_panelcontent.Text = ptstring;
        }

        private void redoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            toolStripStatusLabel_pressed.Text = "redo";
        }

        private void undoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            toolStripStatusLabel_pressed.Text = "undo";
        }

        private void helpToolStripMenuItem1_Click(object sender, EventArgs e)
        {
            toolStripStatusLabel_pressed.Text = "help";
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            toolStripStatusLabel_pressed.Text = "open";
        }

        private void closeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            toolStripStatusLabel_pressed.Text = "close";
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            toolStripStatusLabel_pressed.Text = "exit";
        }

        private void hidden1ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            toolStripStatusLabel_pressed.Text = "hidden 1";
        }

        private void hidden2ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            toolStripStatusLabel_pressed.Text = "hidden 2";
        }

        private void toolStripButton_b1_Click(object sender, EventArgs e)
        {
            toolStripStatusLabel_button.Text = "b1";
        }

        private void toolStripButton_b2_Click(object sender, EventArgs e)
        {
            toolStripStatusLabel_button.Text = "b2";
        }

        private void toolStripButton_a1_Click(object sender, EventArgs e)
        {
            toolStripStatusLabel_button.Text = "a1";
        }

        private void toolStripButton_a2_Click(object sender, EventArgs e)
        {
            toolStripStatusLabel_button.Text = "a2";
        }

        private void toolStripContainer1_ContentPanel_Resize(object sender, EventArgs e)
        {
            update_statusbar_coords();
        }

        private void textBox_instructions_TextChanged(object sender, EventArgs e)
        {

        }

    }
}