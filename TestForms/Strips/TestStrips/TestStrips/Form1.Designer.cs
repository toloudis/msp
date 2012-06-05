namespace TestStrips
{
    partial class FormStrips
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormStrips));
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.closeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.editToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.redoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.undoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.hidden1ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.hidden2ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.toolStripStatusLabel_info = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStripStatusLabel_button = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStripStatusLabel_pressed = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStrip_a1 = new System.Windows.Forms.ToolStrip();
            this.toolStripButton_a1 = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton_a2 = new System.Windows.Forms.ToolStripButton();
            this.toolStripContainer1 = new System.Windows.Forms.ToolStripContainer();
            this.groupBox_coords = new System.Windows.Forms.GroupBox();
            this.label_wh = new System.Windows.Forms.Label();
            this.label_bounds = new System.Windows.Forms.Label();
            this.label_panelcontent = new System.Windows.Forms.Label();
            this.label_size = new System.Windows.Forms.Label();
            this.label_clientrect = new System.Windows.Forms.Label();
            this.label_clientsize = new System.Windows.Forms.Label();
            this.label_panelright = new System.Windows.Forms.Label();
            this.label_tbrl = new System.Windows.Forms.Label();
            this.label_panelleft = new System.Windows.Forms.Label();
            this.label_paneltop = new System.Windows.Forms.Label();
            this.label_panelbottom = new System.Windows.Forms.Label();
            this.textBox_instructions = new System.Windows.Forms.TextBox();
            this.toolStrip_b1 = new System.Windows.Forms.ToolStrip();
            this.toolStripButton_b1 = new System.Windows.Forms.ToolStripButton();
            this.toolStripButton_b2 = new System.Windows.Forms.ToolStripButton();
            this.unassignedToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.action1ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.action2ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menuStrip1.SuspendLayout();
            this.statusStrip1.SuspendLayout();
            this.toolStrip_a1.SuspendLayout();
            this.toolStripContainer1.ContentPanel.SuspendLayout();
            this.toolStripContainer1.TopToolStripPanel.SuspendLayout();
            this.toolStripContainer1.SuspendLayout();
            this.groupBox_coords.SuspendLayout();
            this.toolStrip_b1.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.editToolStripMenuItem,
            this.helpToolStripMenuItem,
            this.unassignedToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(453, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openToolStripMenuItem,
            this.closeToolStripMenuItem,
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(35, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
            this.openToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.openToolStripMenuItem.Text = "Open";
            this.openToolStripMenuItem.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
            // 
            // closeToolStripMenuItem
            // 
            this.closeToolStripMenuItem.Name = "closeToolStripMenuItem";
            this.closeToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.C)));
            this.closeToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.closeToolStripMenuItem.Text = "Close";
            this.closeToolStripMenuItem.Click += new System.EventHandler(this.closeToolStripMenuItem_Click);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.X)));
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.exitToolStripMenuItem.Text = "Exit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // editToolStripMenuItem
            // 
            this.editToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.redoToolStripMenuItem,
            this.undoToolStripMenuItem,
            this.hidden1ToolStripMenuItem,
            this.hidden2ToolStripMenuItem});
            this.editToolStripMenuItem.Name = "editToolStripMenuItem";
            this.editToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.editToolStripMenuItem.Text = "Edit";
            // 
            // redoToolStripMenuItem
            // 
            this.redoToolStripMenuItem.Name = "redoToolStripMenuItem";
            this.redoToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Y)));
            this.redoToolStripMenuItem.Size = new System.Drawing.Size(195, 22);
            this.redoToolStripMenuItem.Text = "Redo";
            this.redoToolStripMenuItem.Click += new System.EventHandler(this.redoToolStripMenuItem_Click);
            // 
            // undoToolStripMenuItem
            // 
            this.undoToolStripMenuItem.Name = "undoToolStripMenuItem";
            this.undoToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Z)));
            this.undoToolStripMenuItem.Size = new System.Drawing.Size(195, 22);
            this.undoToolStripMenuItem.Text = "Undo";
            this.undoToolStripMenuItem.Click += new System.EventHandler(this.undoToolStripMenuItem_Click);
            // 
            // hidden1ToolStripMenuItem
            // 
            this.hidden1ToolStripMenuItem.Name = "hidden1ToolStripMenuItem";
            this.hidden1ToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift)
                        | System.Windows.Forms.Keys.A)));
            this.hidden1ToolStripMenuItem.Size = new System.Drawing.Size(195, 22);
            this.hidden1ToolStripMenuItem.Text = "Hidden1";
            this.hidden1ToolStripMenuItem.Visible = false;
            this.hidden1ToolStripMenuItem.Click += new System.EventHandler(this.hidden1ToolStripMenuItem_Click);
            // 
            // hidden2ToolStripMenuItem
            // 
            this.hidden2ToolStripMenuItem.Name = "hidden2ToolStripMenuItem";
            this.hidden2ToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift)
                        | System.Windows.Forms.Keys.S)));
            this.hidden2ToolStripMenuItem.Size = new System.Drawing.Size(195, 22);
            this.hidden2ToolStripMenuItem.Text = "Hidden 2";
            this.hidden2ToolStripMenuItem.Visible = false;
            this.hidden2ToolStripMenuItem.Click += new System.EventHandler(this.hidden2ToolStripMenuItem_Click);
            // 
            // helpToolStripMenuItem
            // 
            this.helpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.helpToolStripMenuItem1});
            this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
            this.helpToolStripMenuItem.Size = new System.Drawing.Size(40, 20);
            this.helpToolStripMenuItem.Text = "Help";
            // 
            // helpToolStripMenuItem1
            // 
            this.helpToolStripMenuItem1.Name = "helpToolStripMenuItem1";
            this.helpToolStripMenuItem1.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.H)));
            this.helpToolStripMenuItem1.Size = new System.Drawing.Size(152, 22);
            this.helpToolStripMenuItem1.Text = "Help";
            this.helpToolStripMenuItem1.Click += new System.EventHandler(this.helpToolStripMenuItem1_Click);
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel_info,
            this.toolStripStatusLabel_button,
            this.toolStripStatusLabel_pressed});
            this.statusStrip1.Location = new System.Drawing.Point(0, 343);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(453, 22);
            this.statusStrip1.TabIndex = 1;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // toolStripStatusLabel_info
            // 
            this.toolStripStatusLabel_info.Name = "toolStripStatusLabel_info";
            this.toolStripStatusLabel_info.Size = new System.Drawing.Size(19, 17);
            this.toolStripStatusLabel_info.Text = "...";
            // 
            // toolStripStatusLabel_button
            // 
            this.toolStripStatusLabel_button.Name = "toolStripStatusLabel_button";
            this.toolStripStatusLabel_button.Size = new System.Drawing.Size(39, 17);
            this.toolStripStatusLabel_button.Text = "button";
            // 
            // toolStripStatusLabel_pressed
            // 
            this.toolStripStatusLabel_pressed.Name = "toolStripStatusLabel_pressed";
            this.toolStripStatusLabel_pressed.Size = new System.Drawing.Size(47, 17);
            this.toolStripStatusLabel_pressed.Text = "shortcut";
            // 
            // toolStrip_a1
            // 
            this.toolStrip_a1.Dock = System.Windows.Forms.DockStyle.None;
            this.toolStrip_a1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton_a1,
            this.toolStripButton_a2});
            this.toolStrip_a1.Location = new System.Drawing.Point(92, 0);
            this.toolStrip_a1.Name = "toolStrip_a1";
            this.toolStrip_a1.Size = new System.Drawing.Size(58, 25);
            this.toolStrip_a1.TabIndex = 2;
            this.toolStrip_a1.Text = "toolStrip1";
            // 
            // toolStripButton_a1
            // 
            this.toolStripButton_a1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton_a1.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_a1.Image")));
            this.toolStripButton_a1.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton_a1.Name = "toolStripButton_a1";
            this.toolStripButton_a1.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton_a1.Text = "a1";
            this.toolStripButton_a1.Click += new System.EventHandler(this.toolStripButton_a1_Click);
            // 
            // toolStripButton_a2
            // 
            this.toolStripButton_a2.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton_a2.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_a2.Image")));
            this.toolStripButton_a2.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton_a2.Name = "toolStripButton_a2";
            this.toolStripButton_a2.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton_a2.Text = "toolStripButton1";
            this.toolStripButton_a2.Click += new System.EventHandler(this.toolStripButton_a2_Click);
            // 
            // toolStripContainer1
            // 
            this.toolStripContainer1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            // 
            // toolStripContainer1.ContentPanel
            // 
            this.toolStripContainer1.ContentPanel.Controls.Add(this.groupBox_coords);
            this.toolStripContainer1.ContentPanel.Controls.Add(this.textBox_instructions);
            this.toolStripContainer1.ContentPanel.Size = new System.Drawing.Size(453, 290);
            this.toolStripContainer1.ContentPanel.Resize += new System.EventHandler(this.toolStripContainer1_ContentPanel_Resize);
            this.toolStripContainer1.Location = new System.Drawing.Point(0, 25);
            this.toolStripContainer1.Name = "toolStripContainer1";
            this.toolStripContainer1.Size = new System.Drawing.Size(453, 315);
            this.toolStripContainer1.TabIndex = 3;
            this.toolStripContainer1.Text = "toolStripContainer1";
            // 
            // toolStripContainer1.TopToolStripPanel
            // 
            this.toolStripContainer1.TopToolStripPanel.Controls.Add(this.toolStrip_b1);
            this.toolStripContainer1.TopToolStripPanel.Controls.Add(this.toolStrip_a1);
            // 
            // groupBox_coords
            // 
            this.groupBox_coords.Controls.Add(this.label_wh);
            this.groupBox_coords.Controls.Add(this.label_bounds);
            this.groupBox_coords.Controls.Add(this.label_panelcontent);
            this.groupBox_coords.Controls.Add(this.label_size);
            this.groupBox_coords.Controls.Add(this.label_clientrect);
            this.groupBox_coords.Controls.Add(this.label_clientsize);
            this.groupBox_coords.Controls.Add(this.label_panelright);
            this.groupBox_coords.Controls.Add(this.label_tbrl);
            this.groupBox_coords.Controls.Add(this.label_panelleft);
            this.groupBox_coords.Controls.Add(this.label_paneltop);
            this.groupBox_coords.Controls.Add(this.label_panelbottom);
            this.groupBox_coords.Location = new System.Drawing.Point(12, 58);
            this.groupBox_coords.Name = "groupBox_coords";
            this.groupBox_coords.Size = new System.Drawing.Size(434, 199);
            this.groupBox_coords.TabIndex = 13;
            this.groupBox_coords.TabStop = false;
            this.groupBox_coords.Text = "Toolbar Container Coordinates";
            // 
            // label_wh
            // 
            this.label_wh.AutoSize = true;
            this.label_wh.Location = new System.Drawing.Point(18, 16);
            this.label_wh.Name = "label_wh";
            this.label_wh.Size = new System.Drawing.Size(21, 13);
            this.label_wh.TabIndex = 0;
            this.label_wh.Text = "wh";
            // 
            // label_bounds
            // 
            this.label_bounds.AutoSize = true;
            this.label_bounds.Location = new System.Drawing.Point(18, 76);
            this.label_bounds.Name = "label_bounds";
            this.label_bounds.Size = new System.Drawing.Size(42, 13);
            this.label_bounds.TabIndex = 1;
            this.label_bounds.Text = "bounds";
            // 
            // label_panelcontent
            // 
            this.label_panelcontent.AutoSize = true;
            this.label_panelcontent.Location = new System.Drawing.Point(18, 46);
            this.label_panelcontent.Name = "label_panelcontent";
            this.label_panelcontent.Size = new System.Drawing.Size(43, 13);
            this.label_panelcontent.TabIndex = 10;
            this.label_panelcontent.Text = "content";
            // 
            // label_size
            // 
            this.label_size.AutoSize = true;
            this.label_size.Location = new System.Drawing.Point(18, 106);
            this.label_size.Name = "label_size";
            this.label_size.Size = new System.Drawing.Size(25, 13);
            this.label_size.TabIndex = 2;
            this.label_size.Text = "size";
            // 
            // label_clientrect
            // 
            this.label_clientrect.AutoSize = true;
            this.label_clientrect.Location = new System.Drawing.Point(18, 163);
            this.label_clientrect.Name = "label_clientrect";
            this.label_clientrect.Size = new System.Drawing.Size(53, 13);
            this.label_clientrect.TabIndex = 9;
            this.label_clientrect.Text = "client rect";
            // 
            // label_clientsize
            // 
            this.label_clientsize.AutoSize = true;
            this.label_clientsize.Location = new System.Drawing.Point(18, 136);
            this.label_clientsize.Name = "label_clientsize";
            this.label_clientsize.Size = new System.Drawing.Size(50, 13);
            this.label_clientsize.TabIndex = 3;
            this.label_clientsize.Text = "clientsize";
            // 
            // label_panelright
            // 
            this.label_panelright.AutoSize = true;
            this.label_panelright.Location = new System.Drawing.Point(200, 106);
            this.label_panelright.Name = "label_panelright";
            this.label_panelright.Size = new System.Drawing.Size(56, 13);
            this.label_panelright.TabIndex = 8;
            this.label_panelright.Text = "panel right";
            // 
            // label_tbrl
            // 
            this.label_tbrl.AutoSize = true;
            this.label_tbrl.Location = new System.Drawing.Point(200, 136);
            this.label_tbrl.Name = "label_tbrl";
            this.label_tbrl.Size = new System.Drawing.Size(35, 13);
            this.label_tbrl.TabIndex = 4;
            this.label_tbrl.Text = "TBRL";
            // 
            // label_panelleft
            // 
            this.label_panelleft.AutoSize = true;
            this.label_panelleft.Location = new System.Drawing.Point(201, 76);
            this.label_panelleft.Name = "label_panelleft";
            this.label_panelleft.Size = new System.Drawing.Size(50, 13);
            this.label_panelleft.TabIndex = 7;
            this.label_panelleft.Text = "panel left";
            // 
            // label_paneltop
            // 
            this.label_paneltop.AutoSize = true;
            this.label_paneltop.Location = new System.Drawing.Point(200, 16);
            this.label_paneltop.Name = "label_paneltop";
            this.label_paneltop.Size = new System.Drawing.Size(51, 13);
            this.label_paneltop.TabIndex = 5;
            this.label_paneltop.Text = "panel top";
            // 
            // label_panelbottom
            // 
            this.label_panelbottom.AutoSize = true;
            this.label_panelbottom.Location = new System.Drawing.Point(200, 46);
            this.label_panelbottom.Name = "label_panelbottom";
            this.label_panelbottom.Size = new System.Drawing.Size(68, 13);
            this.label_panelbottom.TabIndex = 6;
            this.label_panelbottom.Text = "panel bottom";
            // 
            // textBox_instructions
            // 
            this.textBox_instructions.Enabled = false;
            this.textBox_instructions.Location = new System.Drawing.Point(12, 12);
            this.textBox_instructions.Multiline = true;
            this.textBox_instructions.Name = "textBox_instructions";
            this.textBox_instructions.Size = new System.Drawing.Size(434, 40);
            this.textBox_instructions.TabIndex = 12;
            this.textBox_instructions.Text = "Status strip will show menu shortcuts being pressed.  CTRL-SHIFT-A and CTRL_SHIFT" +
                "-S are assigned to HIDDEN menu items.";
            this.textBox_instructions.TextChanged += new System.EventHandler(this.textBox_instructions_TextChanged);
            // 
            // toolStrip_b1
            // 
            this.toolStrip_b1.Dock = System.Windows.Forms.DockStyle.None;
            this.toolStrip_b1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton_b1,
            this.toolStripButton_b2});
            this.toolStrip_b1.Location = new System.Drawing.Point(21, 0);
            this.toolStrip_b1.Name = "toolStrip_b1";
            this.toolStrip_b1.Size = new System.Drawing.Size(58, 25);
            this.toolStrip_b1.TabIndex = 0;
            // 
            // toolStripButton_b1
            // 
            this.toolStripButton_b1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton_b1.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_b1.Image")));
            this.toolStripButton_b1.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton_b1.Name = "toolStripButton_b1";
            this.toolStripButton_b1.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton_b1.Text = "b1";
            this.toolStripButton_b1.Click += new System.EventHandler(this.toolStripButton_b1_Click);
            // 
            // toolStripButton_b2
            // 
            this.toolStripButton_b2.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton_b2.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton_b2.Image")));
            this.toolStripButton_b2.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton_b2.Name = "toolStripButton_b2";
            this.toolStripButton_b2.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton_b2.Text = "b2";
            this.toolStripButton_b2.Click += new System.EventHandler(this.toolStripButton_b2_Click);
            // 
            // unassignedToolStripMenuItem
            // 
            this.unassignedToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.action1ToolStripMenuItem,
            this.action2ToolStripMenuItem});
            this.unassignedToolStripMenuItem.Name = "unassignedToolStripMenuItem";
            this.unassignedToolStripMenuItem.Size = new System.Drawing.Size(74, 20);
            this.unassignedToolStripMenuItem.Text = "Unassigned";
            // 
            // action1ToolStripMenuItem
            // 
            this.action1ToolStripMenuItem.Name = "action1ToolStripMenuItem";
            this.action1ToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.action1ToolStripMenuItem.Text = "Action1";
            // 
            // action2ToolStripMenuItem
            // 
            this.action2ToolStripMenuItem.Name = "action2ToolStripMenuItem";
            this.action2ToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.action2ToolStripMenuItem.Text = "Action2";
            // 
            // FormStrips
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(453, 365);
            this.Controls.Add(this.toolStripContainer1);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "FormStrips";
            this.Text = "Menu, Toolbar, Status Strips";
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.toolStrip_a1.ResumeLayout(false);
            this.toolStrip_a1.PerformLayout();
            this.toolStripContainer1.ContentPanel.ResumeLayout(false);
            this.toolStripContainer1.ContentPanel.PerformLayout();
            this.toolStripContainer1.TopToolStripPanel.ResumeLayout(false);
            this.toolStripContainer1.TopToolStripPanel.PerformLayout();
            this.toolStripContainer1.ResumeLayout(false);
            this.toolStripContainer1.PerformLayout();
            this.groupBox_coords.ResumeLayout(false);
            this.groupBox_coords.PerformLayout();
            this.toolStrip_b1.ResumeLayout(false);
            this.toolStrip_b1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStrip toolStrip_a1;
        private System.Windows.Forms.ToolStripContainer toolStripContainer1;
        private System.Windows.Forms.ToolStrip toolStrip_b1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem closeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem editToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem redoToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem undoToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem1;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel_info;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel_button;
        private System.Windows.Forms.ToolStripButton toolStripButton_b1;
        private System.Windows.Forms.ToolStripButton toolStripButton_b2;
        private System.Windows.Forms.ToolStripButton toolStripButton_a1;
        private System.Windows.Forms.ToolStripButton toolStripButton_a2;
        private System.Windows.Forms.ToolStripMenuItem hidden1ToolStripMenuItem;
        private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel_pressed;
        private System.Windows.Forms.ToolStripMenuItem hidden2ToolStripMenuItem;
        private System.Windows.Forms.Label label_wh;
        private System.Windows.Forms.Label label_bounds;
        private System.Windows.Forms.Label label_tbrl;
        private System.Windows.Forms.Label label_clientsize;
        private System.Windows.Forms.Label label_size;
        private System.Windows.Forms.Label label_panelright;
        private System.Windows.Forms.Label label_panelleft;
        private System.Windows.Forms.Label label_panelbottom;
        private System.Windows.Forms.Label label_paneltop;
        private System.Windows.Forms.Label label_clientrect;
        private System.Windows.Forms.Label label_panelcontent;
        private System.Windows.Forms.GroupBox groupBox_coords;
        private System.Windows.Forms.TextBox textBox_instructions;
        private System.Windows.Forms.ToolStripMenuItem unassignedToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem action1ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem action2ToolStripMenuItem;
    }
}

