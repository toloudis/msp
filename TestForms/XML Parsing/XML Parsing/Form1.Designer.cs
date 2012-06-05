namespace XML_Parsing
{
    partial class Form_Xml
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
            this.tabControl_xml = new System.Windows.Forms.TabControl();
            this.tabPage_xml_basic = new System.Windows.Forms.TabPage();
            this.textBox_basic_output = new System.Windows.Forms.TextBox();
            this.button_basic_read = new System.Windows.Forms.Button();
            this.button_basic_write = new System.Windows.Forms.Button();
            this.tabPage_xml_prefs_cfg = new System.Windows.Forms.TabPage();
            this.button_readprefs_text = new System.Windows.Forms.Button();
            this.label_prefscfg = new System.Windows.Forms.Label();
            this.textBox_prefscfg = new System.Windows.Forms.TextBox();
            this.button_prefsout = new System.Windows.Forms.Button();
            this.button_prefscfg = new System.Windows.Forms.Button();
            this.tabPage_res = new System.Windows.Forms.TabPage();
            this.label1 = new System.Windows.Forms.Label();
            this.textBox_res_output = new System.Windows.Forms.TextBox();
            this.button_res_reader = new System.Windows.Forms.Button();
            this.tabControl_xml.SuspendLayout();
            this.tabPage_xml_basic.SuspendLayout();
            this.tabPage_xml_prefs_cfg.SuspendLayout();
            this.tabPage_res.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl_xml
            // 
            this.tabControl_xml.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl_xml.Controls.Add(this.tabPage_xml_basic);
            this.tabControl_xml.Controls.Add(this.tabPage_xml_prefs_cfg);
            this.tabControl_xml.Controls.Add(this.tabPage_res);
            this.tabControl_xml.Location = new System.Drawing.Point(-1, 31);
            this.tabControl_xml.Name = "tabControl_xml";
            this.tabControl_xml.SelectedIndex = 0;
            this.tabControl_xml.Size = new System.Drawing.Size(509, 478);
            this.tabControl_xml.TabIndex = 0;
            // 
            // tabPage_xml_basic
            // 
            this.tabPage_xml_basic.Controls.Add(this.textBox_basic_output);
            this.tabPage_xml_basic.Controls.Add(this.button_basic_read);
            this.tabPage_xml_basic.Controls.Add(this.button_basic_write);
            this.tabPage_xml_basic.Location = new System.Drawing.Point(4, 22);
            this.tabPage_xml_basic.Name = "tabPage_xml_basic";
            this.tabPage_xml_basic.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_xml_basic.Size = new System.Drawing.Size(501, 452);
            this.tabPage_xml_basic.TabIndex = 0;
            this.tabPage_xml_basic.Text = "XML basic";
            this.tabPage_xml_basic.UseVisualStyleBackColor = true;
            // 
            // textBox_basic_output
            // 
            this.textBox_basic_output.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox_basic_output.Location = new System.Drawing.Point(3, 61);
            this.textBox_basic_output.Multiline = true;
            this.textBox_basic_output.Name = "textBox_basic_output";
            this.textBox_basic_output.ReadOnly = true;
            this.textBox_basic_output.Size = new System.Drawing.Size(498, 388);
            this.textBox_basic_output.TabIndex = 2;
            // 
            // button_basic_read
            // 
            this.button_basic_read.Location = new System.Drawing.Point(106, 17);
            this.button_basic_read.Name = "button_basic_read";
            this.button_basic_read.Size = new System.Drawing.Size(75, 23);
            this.button_basic_read.TabIndex = 1;
            this.button_basic_read.Text = "Read";
            this.button_basic_read.UseVisualStyleBackColor = true;
            this.button_basic_read.Click += new System.EventHandler(this.button_basic_read_Click);
            // 
            // button_basic_write
            // 
            this.button_basic_write.Location = new System.Drawing.Point(25, 17);
            this.button_basic_write.Name = "button_basic_write";
            this.button_basic_write.Size = new System.Drawing.Size(75, 23);
            this.button_basic_write.TabIndex = 0;
            this.button_basic_write.Text = "Write";
            this.button_basic_write.UseVisualStyleBackColor = true;
            this.button_basic_write.Click += new System.EventHandler(this.button_basic_write_Click);
            // 
            // tabPage_xml_prefs_cfg
            // 
            this.tabPage_xml_prefs_cfg.Controls.Add(this.button_readprefs_text);
            this.tabPage_xml_prefs_cfg.Controls.Add(this.label_prefscfg);
            this.tabPage_xml_prefs_cfg.Controls.Add(this.textBox_prefscfg);
            this.tabPage_xml_prefs_cfg.Controls.Add(this.button_prefsout);
            this.tabPage_xml_prefs_cfg.Controls.Add(this.button_prefscfg);
            this.tabPage_xml_prefs_cfg.Location = new System.Drawing.Point(4, 22);
            this.tabPage_xml_prefs_cfg.Name = "tabPage_xml_prefs_cfg";
            this.tabPage_xml_prefs_cfg.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_xml_prefs_cfg.Size = new System.Drawing.Size(501, 452);
            this.tabPage_xml_prefs_cfg.TabIndex = 1;
            this.tabPage_xml_prefs_cfg.Text = "Prefs.cfg";
            this.tabPage_xml_prefs_cfg.UseVisualStyleBackColor = true;
            // 
            // button_readprefs_text
            // 
            this.button_readprefs_text.Location = new System.Drawing.Point(99, 14);
            this.button_readprefs_text.Name = "button_readprefs_text";
            this.button_readprefs_text.Size = new System.Drawing.Size(75, 23);
            this.button_readprefs_text.TabIndex = 4;
            this.button_readprefs_text.Text = "TextReader";
            this.button_readprefs_text.UseVisualStyleBackColor = true;
            this.button_readprefs_text.Click += new System.EventHandler(this.button_readprefs_text_Click);
            // 
            // label_prefscfg
            // 
            this.label_prefscfg.AutoSize = true;
            this.label_prefscfg.Location = new System.Drawing.Point(261, 19);
            this.label_prefscfg.Name = "label_prefscfg";
            this.label_prefscfg.Size = new System.Drawing.Size(203, 13);
            this.label_prefscfg.TabIndex = 3;
            this.label_prefscfg.Text = "Read from Prefs.cfg, write to PrefsOut.cfg";
            // 
            // textBox_prefscfg
            // 
            this.textBox_prefscfg.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox_prefscfg.Location = new System.Drawing.Point(0, 50);
            this.textBox_prefscfg.Multiline = true;
            this.textBox_prefscfg.Name = "textBox_prefscfg";
            this.textBox_prefscfg.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBox_prefscfg.Size = new System.Drawing.Size(498, 402);
            this.textBox_prefscfg.TabIndex = 2;
            // 
            // button_prefsout
            // 
            this.button_prefsout.Location = new System.Drawing.Point(180, 14);
            this.button_prefsout.Name = "button_prefsout";
            this.button_prefsout.Size = new System.Drawing.Size(75, 23);
            this.button_prefsout.TabIndex = 1;
            this.button_prefsout.Text = "Write PrefsOut.cfg";
            this.button_prefsout.UseVisualStyleBackColor = true;
            // 
            // button_prefscfg
            // 
            this.button_prefscfg.Location = new System.Drawing.Point(18, 14);
            this.button_prefscfg.Name = "button_prefscfg";
            this.button_prefscfg.Size = new System.Drawing.Size(75, 23);
            this.button_prefscfg.TabIndex = 0;
            this.button_prefscfg.Text = "Reader";
            this.button_prefscfg.UseVisualStyleBackColor = true;
            this.button_prefscfg.Click += new System.EventHandler(this.button_prefscfg_Click);
            // 
            // tabPage_res
            // 
            this.tabPage_res.Controls.Add(this.label1);
            this.tabPage_res.Controls.Add(this.textBox_res_output);
            this.tabPage_res.Controls.Add(this.button_res_reader);
            this.tabPage_res.Location = new System.Drawing.Point(4, 22);
            this.tabPage_res.Name = "tabPage_res";
            this.tabPage_res.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_res.Size = new System.Drawing.Size(501, 452);
            this.tabPage_res.TabIndex = 2;
            this.tabPage_res.Text = "resolutions.cfg";
            this.tabPage_res.UseVisualStyleBackColor = true;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(262, 12);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(132, 13);
            this.label1.TabIndex = 8;
            this.label1.Text = "Read from Resolutions.cfg";
            // 
            // textBox_res_output
            // 
            this.textBox_res_output.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox_res_output.Location = new System.Drawing.Point(1, 43);
            this.textBox_res_output.Multiline = true;
            this.textBox_res_output.Name = "textBox_res_output";
            this.textBox_res_output.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBox_res_output.Size = new System.Drawing.Size(498, 402);
            this.textBox_res_output.TabIndex = 7;
            // 
            // button_res_reader
            // 
            this.button_res_reader.Location = new System.Drawing.Point(19, 7);
            this.button_res_reader.Name = "button_res_reader";
            this.button_res_reader.Size = new System.Drawing.Size(75, 23);
            this.button_res_reader.TabIndex = 5;
            this.button_res_reader.Text = "Reader";
            this.button_res_reader.UseVisualStyleBackColor = true;
            this.button_res_reader.Click += new System.EventHandler(this.button_res_reader_Click);
            // 
            // Form_Xml
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(506, 507);
            this.Controls.Add(this.tabControl_xml);
            this.Name = "Form_Xml";
            this.Text = "XML Parsing Examples";
            this.tabControl_xml.ResumeLayout(false);
            this.tabPage_xml_basic.ResumeLayout(false);
            this.tabPage_xml_basic.PerformLayout();
            this.tabPage_xml_prefs_cfg.ResumeLayout(false);
            this.tabPage_xml_prefs_cfg.PerformLayout();
            this.tabPage_res.ResumeLayout(false);
            this.tabPage_res.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl_xml;
        private System.Windows.Forms.TabPage tabPage_xml_basic;
        private System.Windows.Forms.TabPage tabPage_xml_prefs_cfg;
        private System.Windows.Forms.Button button_basic_write;
        private System.Windows.Forms.Button button_basic_read;
        private System.Windows.Forms.TextBox textBox_basic_output;
        private System.Windows.Forms.Button button_prefscfg;
        private System.Windows.Forms.Button button_prefsout;
        private System.Windows.Forms.TextBox textBox_prefscfg;
        private System.Windows.Forms.Label label_prefscfg;
        private System.Windows.Forms.Button button_readprefs_text;
        private System.Windows.Forms.TabPage tabPage_res;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textBox_res_output;
        private System.Windows.Forms.Button button_res_reader;
    }
}

