//#define USE_PICTUREBOXES 

using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace TestControls
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form1 : TerawattManagedControls.FXForm
	{
		private System.Windows.Forms.TabPage tabPage_other;
		private System.Windows.Forms.Label labelSelected;
		private TerawattManagedControls.ObjectList objectList1;
		private TerawattManagedControls.RadioGroup radioGroup1;
		private System.Windows.Forms.Label labelChoice;
		private System.Windows.Forms.TabPage tabPage_files;
		private System.Windows.Forms.Label labelFilename;
		private System.Windows.Forms.Label label_pickdir;
		private System.Windows.Forms.Label label_browsefile;
		private System.Windows.Forms.Label labelFullpath;
		private TerawattManagedControls.FolderChooser folderChooser1;
		private System.Windows.Forms.Label labelDirectory;
		private TerawattManagedControls.FileChooser fileChooser1;
		private System.Windows.Forms.TabPage tabPage_numbers;
		private System.Windows.Forms.Label labelRange;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label_updownXvalue;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private TerawattManagedControls.Vector3Edit vector3Edit1;
		private TerawattManagedControls.Vector3EditUpDown vector3EditUpDown1;
		private System.Windows.Forms.Label labelFloat;
		private TerawattManagedControls.FloatEdit floatEdit1;
		private TerawattManagedControls.RangedFloat rangedFloat1;
		private System.Windows.Forms.TabPage tabPage_splash;
		private System.Windows.Forms.Button button_splash;
		private System.Windows.Forms.Label label_tempmsg;
		private System.Windows.Forms.Button button_closeSplash;
		private TerawattManagedControls.RangedFloat rangedFloat_slim;
		private System.Windows.Forms.Label label_browsefile_only_path;
		private System.Windows.Forms.Label label_browsefile_only;
        private TerawattManagedControls.FileChooser fileChooser_browsefile_only;
        private TerawattManagedControls.Vector3Edit vector3Edit_2dp;
        private TerawattManagedControls.MultiRangedBox multiRanged1;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private TerawattManagedControls.KeyCombo hotKey_test;
		private System.Windows.Forms.Label label_keycombo;
		private System.Windows.Forms.TabControl tabControl_TMC;
		private System.Windows.Forms.TabPage tabPage_color;
		private TerawattManagedControls.ColorRGBEdit colorRGBEdit1;
		private TerawattManagedControls.ColorRGBAEdit colorRGBAEdit2;
		private TerawattManagedControls.Vector3EditRanged vector3EditRanged1;
		private System.Windows.Forms.Label label_coords;
		private System.Windows.Forms.GroupBox groupBox_splash1;
		private System.Windows.Forms.GroupBox groupBox_loading;
		private System.Windows.Forms.GroupBox groupBox_fade;
		private System.Windows.Forms.TabPage tabPage_PanelLayout;
		private TerawattManagedControls.PanelLayout panelLayout1;
		private System.Windows.Forms.ComboBox comboBox_Layouts;
        private Label label_keycombo_output;
        private GroupBox groupBox_keycombo;
        private Label label_keycombo_number;
        private TabPage tabPage_treeview;
        private Label label_selected;
        private TextBox textBox_selected;
        private Label label_treeviewms;
        private TerawattManagedControls.TreeViewMS treeViewMS_test;
        private Button button_refreshselected;
        private Button button_mstree_selectfew;
        private GroupBox groupBox_treeviewbuttons;
        private Label labelMultiValueY;
        private Label labelMultiValueX;
        private Label label_rf_nobox;
        private CheckBox checkBox_filenameOnly;
        private TerawattManagedControls.SplashImage m_Splash;

		public Form1()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
			this.folderChooser1.Directory = "C:\\Projects";
			this.fileChooser1.Fullpath = "C:\\Projects";
			//this.fileChooser1.Fullpath = "C:\\projects\\Bratz01\\Data\\3W_CLWISH01\\Characters\\General\\Sounds";
			//this.folderChooser1.Directory = "C:\\projects\\Bratz01\\Data\\3W_CLWISH01\\Characters\\General\\Sounds";

			this.vector3EditUpDown1.IncrementX = 0.1M;
			this.vector3EditUpDown1.IncrementY = 0.01M;
			this.vector3EditUpDown1.IncrementZ = 0.001M;

			this.fileChooser_browsefile_only.Fullpath = "C:\\Projects\\MachStudio\\Data\\Splash.png";
			this.fileChooser_browsefile_only.Fullpath = "E:\\Storytime01\\Data\\DawnFlight_000\\Characters\\General\\Sounds\\animation test animatic 3-16-06_AUDIO.wav";
			this.fileChooser_browsefile_only.FileNameOnly = true;

            // Set initial text
            this.label_browsefile_only_path.Text = this.fileChooser_browsefile_only.Fullpath;
            this.labelFullpath.Text = this.fileChooser1.Fullpath;
            this.labelFilename.Text = this.fileChooser1.Filename;

#if USE_PICTUREBOXES
			// Add picture boxes to the PanelLayout control to test
			//	the separation
			Bitmap shared_image = new Bitmap("../../Splash.png", true);
			for (int i=0; i<4; i++)
			{
				PictureBox pictureBox = new System.Windows.Forms.PictureBox();
				pictureBox.BackColor = System.Drawing.Color.Black;
				pictureBox.Image = shared_image;
				pictureBox.Name = System.String.Format("pictureBox{0}", i);
				pictureBox.TabIndex = i+1;
				pictureBox.TabStop = false;
				pictureBox.SizeMode = PictureBoxSizeMode.StretchImage;
				this.panelLayout1.SetControl(i, pictureBox);
			}
#else
			// Add text boxes to test the focus handling
			for (int i=0; i<4; i++)
			{
				TextBox textBox = new System.Windows.Forms.TextBox();
				textBox.Multiline = true;
				this.panelLayout1.SetControl(i, textBox);
			}
#endif
			this.comboBox_Layouts.SelectedIndex = 0;

			this.hotKey_test.KeyChanged += new System.Windows.Forms.KeyEventHandler(hotKey_KeyDown);

            //  fill up the treeviewms
            fillintree_fancy(treeViewMS_test);
            treeViewMS_test.ExpandAll();
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.tabControl_TMC = new System.Windows.Forms.TabControl();
            this.tabPage_numbers = new System.Windows.Forms.TabPage();
            this.label_rf_nobox = new System.Windows.Forms.Label();
            this.labelMultiValueY = new System.Windows.Forms.Label();
            this.labelMultiValueX = new System.Windows.Forms.Label();
            this.label_coords = new System.Windows.Forms.Label();
            this.vector3EditRanged1 = new TerawattManagedControls.Vector3EditRanged();
            this.vector3Edit_2dp = new TerawattManagedControls.Vector3Edit();
            this.rangedFloat_slim = new TerawattManagedControls.RangedFloat();
            this.labelRange = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label_updownXvalue = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.vector3Edit1 = new TerawattManagedControls.Vector3Edit();
            this.vector3EditUpDown1 = new TerawattManagedControls.Vector3EditUpDown();
            this.labelFloat = new System.Windows.Forms.Label();
            this.floatEdit1 = new TerawattManagedControls.FloatEdit();
            this.rangedFloat1 = new TerawattManagedControls.RangedFloat();
            this.multiRanged1 = new TerawattManagedControls.MultiRangedBox();
            this.tabPage_color = new System.Windows.Forms.TabPage();
            this.colorRGBEdit1 = new TerawattManagedControls.ColorRGBEdit();
            this.colorRGBAEdit2 = new TerawattManagedControls.ColorRGBAEdit();
            this.tabPage_splash = new System.Windows.Forms.TabPage();
            this.groupBox_fade = new System.Windows.Forms.GroupBox();
            this.groupBox_loading = new System.Windows.Forms.GroupBox();
            this.groupBox_splash1 = new System.Windows.Forms.GroupBox();
            this.button_closeSplash = new System.Windows.Forms.Button();
            this.label_tempmsg = new System.Windows.Forms.Label();
            this.button_splash = new System.Windows.Forms.Button();
            this.tabPage_other = new System.Windows.Forms.TabPage();
            this.groupBox_keycombo = new System.Windows.Forms.GroupBox();
            this.label_keycombo_number = new System.Windows.Forms.Label();
            this.label_keycombo = new System.Windows.Forms.Label();
            this.label_keycombo_output = new System.Windows.Forms.Label();
            this.hotKey_test = new TerawattManagedControls.KeyCombo();
            this.labelSelected = new System.Windows.Forms.Label();
            this.objectList1 = new TerawattManagedControls.ObjectList();
            this.labelChoice = new System.Windows.Forms.Label();
            this.radioGroup1 = new TerawattManagedControls.RadioGroup();
            this.tabPage_files = new System.Windows.Forms.TabPage();
            this.checkBox_filenameOnly = new System.Windows.Forms.CheckBox();
            this.label_browsefile_only_path = new System.Windows.Forms.Label();
            this.label_browsefile_only = new System.Windows.Forms.Label();
            this.fileChooser_browsefile_only = new TerawattManagedControls.FileChooser();
            this.labelFilename = new System.Windows.Forms.Label();
            this.label_pickdir = new System.Windows.Forms.Label();
            this.label_browsefile = new System.Windows.Forms.Label();
            this.labelFullpath = new System.Windows.Forms.Label();
            this.folderChooser1 = new TerawattManagedControls.FolderChooser();
            this.labelDirectory = new System.Windows.Forms.Label();
            this.fileChooser1 = new TerawattManagedControls.FileChooser();
            this.tabPage_PanelLayout = new System.Windows.Forms.TabPage();
            this.comboBox_Layouts = new System.Windows.Forms.ComboBox();
            this.panelLayout1 = new TerawattManagedControls.PanelLayout();
            this.tabPage_treeview = new System.Windows.Forms.TabPage();
            this.groupBox_treeviewbuttons = new System.Windows.Forms.GroupBox();
            this.button_mstree_selectfew = new System.Windows.Forms.Button();
            this.button_refreshselected = new System.Windows.Forms.Button();
            this.label_selected = new System.Windows.Forms.Label();
            this.textBox_selected = new System.Windows.Forms.TextBox();
            this.label_treeviewms = new System.Windows.Forms.Label();
            this.treeViewMS_test = new TerawattManagedControls.TreeViewMS();
            this.tabControl_TMC.SuspendLayout();
            this.tabPage_numbers.SuspendLayout();
            this.tabPage_color.SuspendLayout();
            this.tabPage_splash.SuspendLayout();
            this.groupBox_splash1.SuspendLayout();
            this.tabPage_other.SuspendLayout();
            this.groupBox_keycombo.SuspendLayout();
            this.tabPage_files.SuspendLayout();
            this.tabPage_PanelLayout.SuspendLayout();
            this.tabPage_treeview.SuspendLayout();
            this.groupBox_treeviewbuttons.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl_TMC
            // 
            this.tabControl_TMC.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl_TMC.Controls.Add(this.tabPage_numbers);
            this.tabControl_TMC.Controls.Add(this.tabPage_color);
            this.tabControl_TMC.Controls.Add(this.tabPage_splash);
            this.tabControl_TMC.Controls.Add(this.tabPage_other);
            this.tabControl_TMC.Controls.Add(this.tabPage_files);
            this.tabControl_TMC.Controls.Add(this.tabPage_PanelLayout);
            this.tabControl_TMC.Controls.Add(this.tabPage_treeview);
            this.tabControl_TMC.ItemSize = new System.Drawing.Size(42, 18);
            this.tabControl_TMC.Location = new System.Drawing.Point(8, 8);
            this.tabControl_TMC.Name = "tabControl_TMC";
            this.tabControl_TMC.SelectedIndex = 0;
            this.tabControl_TMC.Size = new System.Drawing.Size(502, 504);
            this.tabControl_TMC.TabIndex = 31;
            // 
            // tabPage_numbers
            // 
            this.tabPage_numbers.Controls.Add(this.label_rf_nobox);
            this.tabPage_numbers.Controls.Add(this.labelMultiValueY);
            this.tabPage_numbers.Controls.Add(this.labelMultiValueX);
            this.tabPage_numbers.Controls.Add(this.label_coords);
            this.tabPage_numbers.Controls.Add(this.vector3EditRanged1);
            this.tabPage_numbers.Controls.Add(this.vector3Edit_2dp);
            this.tabPage_numbers.Controls.Add(this.rangedFloat_slim);
            this.tabPage_numbers.Controls.Add(this.labelRange);
            this.tabPage_numbers.Controls.Add(this.label3);
            this.tabPage_numbers.Controls.Add(this.label_updownXvalue);
            this.tabPage_numbers.Controls.Add(this.label1);
            this.tabPage_numbers.Controls.Add(this.label2);
            this.tabPage_numbers.Controls.Add(this.vector3Edit1);
            this.tabPage_numbers.Controls.Add(this.vector3EditUpDown1);
            this.tabPage_numbers.Controls.Add(this.labelFloat);
            this.tabPage_numbers.Controls.Add(this.floatEdit1);
            this.tabPage_numbers.Controls.Add(this.rangedFloat1);
            this.tabPage_numbers.Controls.Add(this.multiRanged1);
            this.tabPage_numbers.Location = new System.Drawing.Point(4, 22);
            this.tabPage_numbers.Name = "tabPage_numbers";
            this.tabPage_numbers.Size = new System.Drawing.Size(494, 478);
            this.tabPage_numbers.TabIndex = 2;
            this.tabPage_numbers.Text = "Numbers";
            // 
            // label_rf_nobox
            // 
            this.label_rf_nobox.AutoSize = true;
            this.label_rf_nobox.Location = new System.Drawing.Point(246, 52);
            this.label_rf_nobox.Name = "label_rf_nobox";
            this.label_rf_nobox.Size = new System.Drawing.Size(175, 13);
            this.label_rf_nobox.TabIndex = 38;
            this.label_rf_nobox.Text = "ranged float w/out textbox + smaller";
            // 
            // labelMultiValueY
            // 
            this.labelMultiValueY.AutoSize = true;
            this.labelMultiValueY.Location = new System.Drawing.Point(105, 299);
            this.labelMultiValueY.Name = "labelMultiValueY";
            this.labelMultiValueY.Size = new System.Drawing.Size(62, 13);
            this.labelMultiValueY.TabIndex = 37;
            this.labelMultiValueY.Text = "multiValueY";
            // 
            // labelMultiValueX
            // 
            this.labelMultiValueX.AutoSize = true;
            this.labelMultiValueX.Location = new System.Drawing.Point(105, 263);
            this.labelMultiValueX.Name = "labelMultiValueX";
            this.labelMultiValueX.Size = new System.Drawing.Size(62, 13);
            this.labelMultiValueX.TabIndex = 36;
            this.labelMultiValueX.Text = "multiValueX";
            // 
            // label_coords
            // 
            this.label_coords.Location = new System.Drawing.Point(296, 168);
            this.label_coords.Name = "label_coords";
            this.label_coords.Size = new System.Drawing.Size(160, 96);
            this.label_coords.TabIndex = 34;
            this.label_coords.Text = "Coordinate:";
            // 
            // vector3EditRanged1
            // 
            this.vector3EditRanged1.Exponent = ((short)(1));
            this.vector3EditRanged1.Location = new System.Drawing.Point(8, 160);
            this.vector3EditRanged1.Maximum = 100;
            this.vector3EditRanged1.Name = "vector3EditRanged1";
            this.vector3EditRanged1.NumTicks = ((short)(100));
            this.vector3EditRanged1.Precision = ((short)(0));
            this.vector3EditRanged1.Size = new System.Drawing.Size(264, 86);
            this.vector3EditRanged1.TabIndex = 33;
            this.vector3EditRanged1.ValueX = 25;
            this.vector3EditRanged1.ValueY = 50;
            this.vector3EditRanged1.ValueZ = 30;
            this.vector3EditRanged1.ValueChanged += new System.EventHandler(this.vector3EditRanged1_ValueChanged);
            // 
            // vector3Edit_2dp
            // 
            this.vector3Edit_2dp.Location = new System.Drawing.Point(8, 64);
            this.vector3Edit_2dp.Name = "vector3Edit_2dp";
            this.vector3Edit_2dp.Precision = ((short)(4));
            this.vector3Edit_2dp.Size = new System.Drawing.Size(180, 21);
            this.vector3Edit_2dp.TabIndex = 32;
            this.vector3Edit_2dp.ValueX = 5.3;
            this.vector3Edit_2dp.ValueY = 8.2;
            this.vector3Edit_2dp.ValueZ = 12.4;
            // 
            // rangedFloat_slim
            // 
            this.rangedFloat_slim.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.rangedFloat_slim.Exponent = ((short)(3));
            this.rangedFloat_slim.Location = new System.Drawing.Point(240, 64);
            this.rangedFloat_slim.Name = "rangedFloat_slim";
            this.rangedFloat_slim.NumTicks = ((short)(100));
            this.rangedFloat_slim.Precision = ((short)(4));
            this.rangedFloat_slim.ShowValue = false;
            this.rangedFloat_slim.Size = new System.Drawing.Size(191, 20);
            this.rangedFloat_slim.TabIndex = 31;
            this.rangedFloat_slim.Value = 33.36;
            // 
            // labelRange
            // 
            this.labelRange.Location = new System.Drawing.Point(424, 10);
            this.labelRange.Name = "labelRange";
            this.labelRange.Size = new System.Drawing.Size(40, 16);
            this.labelRange.TabIndex = 13;
            this.labelRange.Text = "range";
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(136, 8);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(53, 20);
            this.label3.TabIndex = 3;
            this.label3.Text = "Z";
            // 
            // label_updownXvalue
            // 
            this.label_updownXvalue.Location = new System.Drawing.Point(8, 128);
            this.label_updownXvalue.Name = "label_updownXvalue";
            this.label_updownXvalue.Size = new System.Drawing.Size(184, 16);
            this.label_updownXvalue.TabIndex = 30;
            this.label_updownXvalue.Text = "x";
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(16, 8);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(54, 20);
            this.label1.TabIndex = 1;
            this.label1.Text = "X";
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(80, 8);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(53, 20);
            this.label2.TabIndex = 2;
            this.label2.Text = "Y";
            // 
            // vector3Edit1
            // 
            this.vector3Edit1.Location = new System.Drawing.Point(8, 32);
            this.vector3Edit1.Name = "vector3Edit1";
            this.vector3Edit1.Precision = ((short)(2));
            this.vector3Edit1.Size = new System.Drawing.Size(180, 21);
            this.vector3Edit1.TabIndex = 16;
            this.vector3Edit1.ValueX = 5.3;
            this.vector3Edit1.ValueY = 8.2;
            this.vector3Edit1.ValueZ = 12.4;
            this.vector3Edit1.ValueChanged += new System.EventHandler(this.vector3Edit1_ValueChanged);
            // 
            // vector3EditUpDown1
            // 
            this.vector3EditUpDown1.Increment = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.vector3EditUpDown1.IncrementX = new decimal(new int[] {
            1,
            0,
            0,
            131072});
            this.vector3EditUpDown1.IncrementY = new decimal(new int[] {
            1,
            0,
            0,
            196608});
            this.vector3EditUpDown1.IncrementZ = new decimal(new int[] {
            1,
            0,
            0,
            262144});
            this.vector3EditUpDown1.Location = new System.Drawing.Point(8, 104);
            this.vector3EditUpDown1.MaximumX = new decimal(new int[] {
            -1,
            -1,
            -1,
            0});
            this.vector3EditUpDown1.MaximumY = new decimal(new int[] {
            -1,
            -1,
            -1,
            0});
            this.vector3EditUpDown1.MaximumZ = new decimal(new int[] {
            -1,
            -1,
            -1,
            0});
            this.vector3EditUpDown1.MinimumX = new decimal(new int[] {
            -1,
            -1,
            -1,
            -2147483648});
            this.vector3EditUpDown1.MinimumY = new decimal(new int[] {
            -1,
            -1,
            -1,
            -2147483648});
            this.vector3EditUpDown1.MinimumZ = new decimal(new int[] {
            -1,
            -1,
            -1,
            -2147483648});
            this.vector3EditUpDown1.Name = "vector3EditUpDown1";
            this.vector3EditUpDown1.Precision = ((short)(5));
            this.vector3EditUpDown1.Size = new System.Drawing.Size(192, 24);
            this.vector3EditUpDown1.TabIndex = 26;
            this.vector3EditUpDown1.ValueX = new decimal(new int[] {
            0,
            0,
            0,
            0});
            this.vector3EditUpDown1.ValueY = new decimal(new int[] {
            0,
            0,
            0,
            0});
            this.vector3EditUpDown1.ValueZ = new decimal(new int[] {
            0,
            0,
            0,
            0});
            this.vector3EditUpDown1.ValueChanged += new System.EventHandler(this.vector3EditUpDown1_ValueChanged);
            // 
            // labelFloat
            // 
            this.labelFloat.Location = new System.Drawing.Point(336, 115);
            this.labelFloat.Name = "labelFloat";
            this.labelFloat.Size = new System.Drawing.Size(36, 20);
            this.labelFloat.TabIndex = 25;
            this.labelFloat.Text = "float";
            // 
            // floatEdit1
            // 
            this.floatEdit1.Location = new System.Drawing.Point(256, 115);
            this.floatEdit1.Name = "floatEdit1";
            this.floatEdit1.Precision = ((short)(3));
            this.floatEdit1.Size = new System.Drawing.Size(67, 20);
            this.floatEdit1.TabIndex = 24;
            this.floatEdit1.Value = 2.3;
            this.floatEdit1.ValueChanged += new System.EventHandler(this.floatEdit1_ValueChanged);
            // 
            // rangedFloat1
            // 
            this.rangedFloat1.Exponent = ((short)(3));
            this.rangedFloat1.Location = new System.Drawing.Point(240, 9);
            this.rangedFloat1.Maximum = 10;
            this.rangedFloat1.Name = "rangedFloat1";
            this.rangedFloat1.NumTicks = ((short)(10));
            this.rangedFloat1.Precision = ((short)(4));
            this.rangedFloat1.ShowValue = true;
            this.rangedFloat1.Size = new System.Drawing.Size(174, 25);
            this.rangedFloat1.TabIndex = 20;
            this.rangedFloat1.Value = 0.125;
            this.rangedFloat1.ValueChanged += new System.EventHandler(this.rangedFloat1_ValueChanged);
            // 
            // multiRanged1
            // 
            this.multiRanged1.BackColor = System.Drawing.SystemColors.Control;
            this.multiRanged1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.multiRanged1.Location = new System.Drawing.Point(19, 263);
            this.multiRanged1.Name = "multiRanged1";
            this.multiRanged1.Size = new System.Drawing.Size(64, 64);
            this.multiRanged1.TabIndex = 35;
            this.multiRanged1.ValueX = -0.4;
            this.multiRanged1.ValueY = 0.4;
            this.multiRanged1.ValueChanged += new System.EventHandler(this.multiRanged1_ValueChanged);
            // 
            // tabPage_color
            // 
            this.tabPage_color.Controls.Add(this.colorRGBEdit1);
            this.tabPage_color.Controls.Add(this.colorRGBAEdit2);
            this.tabPage_color.Location = new System.Drawing.Point(4, 22);
            this.tabPage_color.Name = "tabPage_color";
            this.tabPage_color.Size = new System.Drawing.Size(494, 478);
            this.tabPage_color.TabIndex = 4;
            this.tabPage_color.Text = "color";
            // 
            // colorRGBEdit1
            // 
            this.colorRGBEdit1.Color = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(128)))), ((int)(((byte)(128)))));
            this.colorRGBEdit1.DialogText = "Color Picker 1";
            this.colorRGBEdit1.Location = new System.Drawing.Point(16, 48);
            this.colorRGBEdit1.Name = "colorRGBEdit1";
            this.colorRGBEdit1.Size = new System.Drawing.Size(208, 24);
            this.colorRGBEdit1.TabIndex = 28;
            // 
            // colorRGBAEdit2
            // 
            this.colorRGBAEdit2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.colorRGBAEdit2.Color = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))));
            this.colorRGBAEdit2.DialogText = "Color Picker 2";
            this.colorRGBAEdit2.Location = new System.Drawing.Point(16, 16);
            this.colorRGBAEdit2.Name = "colorRGBAEdit2";
            this.colorRGBAEdit2.Size = new System.Drawing.Size(270, 24);
            this.colorRGBAEdit2.TabIndex = 29;
            // 
            // tabPage_splash
            // 
            this.tabPage_splash.Controls.Add(this.groupBox_fade);
            this.tabPage_splash.Controls.Add(this.groupBox_loading);
            this.tabPage_splash.Controls.Add(this.groupBox_splash1);
            this.tabPage_splash.Location = new System.Drawing.Point(4, 22);
            this.tabPage_splash.Name = "tabPage_splash";
            this.tabPage_splash.Size = new System.Drawing.Size(494, 478);
            this.tabPage_splash.TabIndex = 3;
            this.tabPage_splash.Text = "Splash-Loading-FX";
            // 
            // groupBox_fade
            // 
            this.groupBox_fade.Location = new System.Drawing.Point(8, 224);
            this.groupBox_fade.Name = "groupBox_fade";
            this.groupBox_fade.Size = new System.Drawing.Size(248, 80);
            this.groupBox_fade.TabIndex = 5;
            this.groupBox_fade.TabStop = false;
            this.groupBox_fade.Text = "Fading";
            // 
            // groupBox_loading
            // 
            this.groupBox_loading.Location = new System.Drawing.Point(8, 144);
            this.groupBox_loading.Name = "groupBox_loading";
            this.groupBox_loading.Size = new System.Drawing.Size(248, 72);
            this.groupBox_loading.TabIndex = 4;
            this.groupBox_loading.TabStop = false;
            this.groupBox_loading.Text = "Loading Screen";
            // 
            // groupBox_splash1
            // 
            this.groupBox_splash1.Controls.Add(this.button_closeSplash);
            this.groupBox_splash1.Controls.Add(this.label_tempmsg);
            this.groupBox_splash1.Controls.Add(this.button_splash);
            this.groupBox_splash1.Location = new System.Drawing.Point(8, 8);
            this.groupBox_splash1.Name = "groupBox_splash1";
            this.groupBox_splash1.Size = new System.Drawing.Size(248, 128);
            this.groupBox_splash1.TabIndex = 3;
            this.groupBox_splash1.TabStop = false;
            this.groupBox_splash1.Text = "Splash";
            // 
            // button_closeSplash
            // 
            this.button_closeSplash.Location = new System.Drawing.Point(152, 88);
            this.button_closeSplash.Name = "button_closeSplash";
            this.button_closeSplash.Size = new System.Drawing.Size(80, 23);
            this.button_closeSplash.TabIndex = 2;
            this.button_closeSplash.Text = "CloseSplash";
            this.button_closeSplash.Click += new System.EventHandler(this.button_closeSplash_Click);
            // 
            // label_tempmsg
            // 
            this.label_tempmsg.Location = new System.Drawing.Point(16, 24);
            this.label_tempmsg.Name = "label_tempmsg";
            this.label_tempmsg.Size = new System.Drawing.Size(216, 48);
            this.label_tempmsg.TabIndex = 1;
            this.label_tempmsg.Text = "Put controls here to adjust the splash: Fade times, image, etc";
            // 
            // button_splash
            // 
            this.button_splash.Location = new System.Drawing.Point(16, 88);
            this.button_splash.Name = "button_splash";
            this.button_splash.Size = new System.Drawing.Size(75, 23);
            this.button_splash.TabIndex = 0;
            this.button_splash.Text = "Splash!";
            this.button_splash.Click += new System.EventHandler(this.button_splash_Click);
            // 
            // tabPage_other
            // 
            this.tabPage_other.Controls.Add(this.groupBox_keycombo);
            this.tabPage_other.Controls.Add(this.labelSelected);
            this.tabPage_other.Controls.Add(this.objectList1);
            this.tabPage_other.Controls.Add(this.labelChoice);
            this.tabPage_other.Controls.Add(this.radioGroup1);
            this.tabPage_other.Location = new System.Drawing.Point(4, 22);
            this.tabPage_other.Name = "tabPage_other";
            this.tabPage_other.Size = new System.Drawing.Size(494, 478);
            this.tabPage_other.TabIndex = 0;
            this.tabPage_other.Text = "Other";
            // 
            // groupBox_keycombo
            // 
            this.groupBox_keycombo.Controls.Add(this.label_keycombo_number);
            this.groupBox_keycombo.Controls.Add(this.label_keycombo);
            this.groupBox_keycombo.Controls.Add(this.label_keycombo_output);
            this.groupBox_keycombo.Controls.Add(this.hotKey_test);
            this.groupBox_keycombo.Location = new System.Drawing.Point(16, 164);
            this.groupBox_keycombo.Name = "groupBox_keycombo";
            this.groupBox_keycombo.Size = new System.Drawing.Size(200, 124);
            this.groupBox_keycombo.TabIndex = 25;
            this.groupBox_keycombo.TabStop = false;
            this.groupBox_keycombo.Text = "Key Combos";
            // 
            // label_keycombo_number
            // 
            this.label_keycombo_number.AutoSize = true;
            this.label_keycombo_number.Location = new System.Drawing.Point(8, 94);
            this.label_keycombo_number.Name = "label_keycombo_number";
            this.label_keycombo_number.Size = new System.Drawing.Size(16, 13);
            this.label_keycombo_number.TabIndex = 25;
            this.label_keycombo_number.Text = "...";
            // 
            // label_keycombo
            // 
            this.label_keycombo.Location = new System.Drawing.Point(8, 16);
            this.label_keycombo.Name = "label_keycombo";
            this.label_keycombo.Size = new System.Drawing.Size(136, 23);
            this.label_keycombo.TabIndex = 23;
            this.label_keycombo.Text = "Enter a key combination";
            // 
            // label_keycombo_output
            // 
            this.label_keycombo_output.AutoSize = true;
            this.label_keycombo_output.Location = new System.Drawing.Point(8, 72);
            this.label_keycombo_output.Name = "label_keycombo_output";
            this.label_keycombo_output.Size = new System.Drawing.Size(16, 13);
            this.label_keycombo_output.TabIndex = 24;
            this.label_keycombo_output.Text = "...";
            // 
            // hotKey_test
            // 
            this.hotKey_test.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.hotKey_test.HotkeyValue = 0;
            this.hotKey_test.KeyComboString = "";
            this.hotKey_test.Location = new System.Drawing.Point(8, 40);
            this.hotKey_test.ModifiersValue = 0;
            this.hotKey_test.Name = "hotKey_test";
            this.hotKey_test.Size = new System.Drawing.Size(186, 20);
            this.hotKey_test.TabIndex = 22;
            this.hotKey_test.UseMenuItemKeyCombos = true;
            this.hotKey_test.KeyChanged += new System.Windows.Forms.KeyEventHandler(this.hotKey_test_KeyChanged);
            // 
            // labelSelected
            // 
            this.labelSelected.Location = new System.Drawing.Point(192, 24);
            this.labelSelected.Name = "labelSelected";
            this.labelSelected.Size = new System.Drawing.Size(46, 21);
            this.labelSelected.TabIndex = 10;
            // 
            // objectList1
            // 
            this.objectList1.Location = new System.Drawing.Point(16, 16);
            this.objectList1.Name = "objectList1";
            this.objectList1.SelectedIndex = -1;
            this.objectList1.Size = new System.Drawing.Size(174, 132);
            this.objectList1.TabIndex = 19;
            this.objectList1.NewPressed += new System.EventHandler(this.objectList1_NewPressed);
            this.objectList1.EditPressed += new System.EventHandler(this.objectList1_EditPressed);
            this.objectList1.DeletePressed += new System.EventHandler(this.objectList1_DeletePressed);
            // 
            // labelChoice
            // 
            this.labelChoice.Location = new System.Drawing.Point(255, 8);
            this.labelChoice.Name = "labelChoice";
            this.labelChoice.Size = new System.Drawing.Size(120, 16);
            this.labelChoice.TabIndex = 15;
            this.labelChoice.Text = "choice";
            // 
            // radioGroup1
            // 
            this.radioGroup1.Location = new System.Drawing.Point(244, 27);
            this.radioGroup1.Name = "radioGroup1";
            this.radioGroup1.SelectedIndex = 0;
            this.radioGroup1.Size = new System.Drawing.Size(187, 104);
            this.radioGroup1.TabIndex = 21;
            this.radioGroup1.ChoiceChanged += new System.EventHandler(this.radioGroup1_ChoiceChanged);
            // 
            // tabPage_files
            // 
            this.tabPage_files.Controls.Add(this.checkBox_filenameOnly);
            this.tabPage_files.Controls.Add(this.label_browsefile_only_path);
            this.tabPage_files.Controls.Add(this.label_browsefile_only);
            this.tabPage_files.Controls.Add(this.fileChooser_browsefile_only);
            this.tabPage_files.Controls.Add(this.labelFilename);
            this.tabPage_files.Controls.Add(this.label_pickdir);
            this.tabPage_files.Controls.Add(this.label_browsefile);
            this.tabPage_files.Controls.Add(this.labelFullpath);
            this.tabPage_files.Controls.Add(this.folderChooser1);
            this.tabPage_files.Controls.Add(this.labelDirectory);
            this.tabPage_files.Controls.Add(this.fileChooser1);
            this.tabPage_files.Location = new System.Drawing.Point(4, 22);
            this.tabPage_files.Name = "tabPage_files";
            this.tabPage_files.Size = new System.Drawing.Size(494, 478);
            this.tabPage_files.TabIndex = 1;
            this.tabPage_files.Text = "Files";
            // 
            // checkBox_filenameOnly
            // 
            this.checkBox_filenameOnly.AutoSize = true;
            this.checkBox_filenameOnly.Location = new System.Drawing.Point(243, 103);
            this.checkBox_filenameOnly.Name = "checkBox_filenameOnly";
            this.checkBox_filenameOnly.Size = new System.Drawing.Size(129, 17);
            this.checkBox_filenameOnly.TabIndex = 33;
            this.checkBox_filenameOnly.Text = "Display Filename Only";
            this.checkBox_filenameOnly.UseVisualStyleBackColor = true;
            this.checkBox_filenameOnly.CheckedChanged += new System.EventHandler(this.checkBox_filenameOnly_CheckedChanged);
            // 
            // label_browsefile_only_path
            // 
            this.label_browsefile_only_path.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.label_browsefile_only_path.Location = new System.Drawing.Point(8, 272);
            this.label_browsefile_only_path.Name = "label_browsefile_only_path";
            this.label_browsefile_only_path.Size = new System.Drawing.Size(462, 24);
            this.label_browsefile_only_path.TabIndex = 30;
            this.label_browsefile_only_path.Text = "filepath";
            // 
            // label_browsefile_only
            // 
            this.label_browsefile_only.Location = new System.Drawing.Point(8, 224);
            this.label_browsefile_only.Name = "label_browsefile_only";
            this.label_browsefile_only.Size = new System.Drawing.Size(232, 16);
            this.label_browsefile_only.TabIndex = 32;
            this.label_browsefile_only.Text = "browse for a file -- show only the File Name";
            // 
            // fileChooser_browsefile_only
            // 
            this.fileChooser_browsefile_only.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.fileChooser_browsefile_only.FileNameOnly = true;
            this.fileChooser_browsefile_only.Location = new System.Drawing.Point(8, 240);
            this.fileChooser_browsefile_only.Name = "fileChooser_browsefile_only";
            this.fileChooser_browsefile_only.Size = new System.Drawing.Size(462, 28);
            this.fileChooser_browsefile_only.TabIndex = 31;
            this.fileChooser_browsefile_only.ValueChanged += new System.EventHandler(this.fileChooser_browsefile_only_ValueChanged);
            // 
            // labelFilename
            // 
            this.labelFilename.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.labelFilename.Location = new System.Drawing.Point(8, 152);
            this.labelFilename.Name = "labelFilename";
            this.labelFilename.Size = new System.Drawing.Size(462, 24);
            this.labelFilename.TabIndex = 7;
            this.labelFilename.Text = "filename";
            // 
            // label_pickdir
            // 
            this.label_pickdir.Location = new System.Drawing.Point(8, 16);
            this.label_pickdir.Name = "label_pickdir";
            this.label_pickdir.Size = new System.Drawing.Size(136, 16);
            this.label_pickdir.TabIndex = 28;
            this.label_pickdir.Text = "browse for a directory";
            // 
            // label_browsefile
            // 
            this.label_browsefile.Location = new System.Drawing.Point(8, 104);
            this.label_browsefile.Name = "label_browsefile";
            this.label_browsefile.Size = new System.Drawing.Size(136, 16);
            this.label_browsefile.TabIndex = 29;
            this.label_browsefile.Text = "browse for a file";
            // 
            // labelFullpath
            // 
            this.labelFullpath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.labelFullpath.Location = new System.Drawing.Point(8, 174);
            this.labelFullpath.Name = "labelFullpath";
            this.labelFullpath.Size = new System.Drawing.Size(462, 26);
            this.labelFullpath.TabIndex = 11;
            this.labelFullpath.Text = "fullpath";
            // 
            // folderChooser1
            // 
            this.folderChooser1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.folderChooser1.Location = new System.Drawing.Point(8, 32);
            this.folderChooser1.Name = "folderChooser1";
            this.folderChooser1.Size = new System.Drawing.Size(462, 28);
            this.folderChooser1.TabIndex = 17;
            this.folderChooser1.ValueChanged += new System.EventHandler(this.folderChooser1_ValueChanged);
            // 
            // labelDirectory
            // 
            this.labelDirectory.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.labelDirectory.Location = new System.Drawing.Point(8, 64);
            this.labelDirectory.Name = "labelDirectory";
            this.labelDirectory.Size = new System.Drawing.Size(470, 40);
            this.labelDirectory.TabIndex = 5;
            this.labelDirectory.Text = "directory";
            // 
            // fileChooser1
            // 
            this.fileChooser1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.fileChooser1.Location = new System.Drawing.Point(8, 120);
            this.fileChooser1.Name = "fileChooser1";
            this.fileChooser1.Size = new System.Drawing.Size(462, 28);
            this.fileChooser1.TabIndex = 18;
            this.fileChooser1.ValueChanged += new System.EventHandler(this.fileChooser1_ValueChanged);
            // 
            // tabPage_PanelLayout
            // 
            this.tabPage_PanelLayout.Controls.Add(this.comboBox_Layouts);
            this.tabPage_PanelLayout.Controls.Add(this.panelLayout1);
            this.tabPage_PanelLayout.Location = new System.Drawing.Point(4, 22);
            this.tabPage_PanelLayout.Name = "tabPage_PanelLayout";
            this.tabPage_PanelLayout.Size = new System.Drawing.Size(494, 478);
            this.tabPage_PanelLayout.TabIndex = 5;
            this.tabPage_PanelLayout.Text = "Panels";
            // 
            // comboBox_Layouts
            // 
            this.comboBox_Layouts.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_Layouts.Items.AddRange(new object[] {
            "Single Pane",
            "Two Panes Side by Side",
            "Two Panes Stacked",
            "Three Panes Split Top",
            "Three Panes Split Left",
            "Three Panes Split Bottom",
            "Three Panes Split Right",
            "Four Panes",
            ""});
            this.comboBox_Layouts.Location = new System.Drawing.Point(24, 8);
            this.comboBox_Layouts.MaxDropDownItems = 10;
            this.comboBox_Layouts.Name = "comboBox_Layouts";
            this.comboBox_Layouts.Size = new System.Drawing.Size(200, 21);
            this.comboBox_Layouts.TabIndex = 1;
            this.comboBox_Layouts.SelectedIndexChanged += new System.EventHandler(this.comboBox_Layouts_SelectedIndexChanged);
            // 
            // panelLayout1
            // 
            this.panelLayout1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.panelLayout1.FocusColor = System.Drawing.Color.Blue;
            this.panelLayout1.LayoutStyle = TerawattManagedControls.PanelLayout.Layouts.e_SinglePane;
            this.panelLayout1.Location = new System.Drawing.Point(24, 40);
            this.panelLayout1.Name = "panelLayout1";
            this.panelLayout1.Separation = 2;
            this.panelLayout1.Size = new System.Drawing.Size(438, 416);
            this.panelLayout1.TabIndex = 0;
            // 
            // tabPage_treeview
            // 
            this.tabPage_treeview.Controls.Add(this.groupBox_treeviewbuttons);
            this.tabPage_treeview.Controls.Add(this.button_refreshselected);
            this.tabPage_treeview.Controls.Add(this.label_selected);
            this.tabPage_treeview.Controls.Add(this.textBox_selected);
            this.tabPage_treeview.Controls.Add(this.label_treeviewms);
            this.tabPage_treeview.Controls.Add(this.treeViewMS_test);
            this.tabPage_treeview.Location = new System.Drawing.Point(4, 22);
            this.tabPage_treeview.Name = "tabPage_treeview";
            this.tabPage_treeview.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_treeview.Size = new System.Drawing.Size(494, 478);
            this.tabPage_treeview.TabIndex = 6;
            this.tabPage_treeview.Text = "TreeView";
            this.tabPage_treeview.UseVisualStyleBackColor = true;
            // 
            // groupBox_treeviewbuttons
            // 
            this.groupBox_treeviewbuttons.Controls.Add(this.button_mstree_selectfew);
            this.groupBox_treeviewbuttons.Location = new System.Drawing.Point(261, 261);
            this.groupBox_treeviewbuttons.Name = "groupBox_treeviewbuttons";
            this.groupBox_treeviewbuttons.Size = new System.Drawing.Size(227, 131);
            this.groupBox_treeviewbuttons.TabIndex = 34;
            this.groupBox_treeviewbuttons.TabStop = false;
            this.groupBox_treeviewbuttons.Text = "Special TreeView Buttons";
            // 
            // button_mstree_selectfew
            // 
            this.button_mstree_selectfew.Location = new System.Drawing.Point(15, 29);
            this.button_mstree_selectfew.Name = "button_mstree_selectfew";
            this.button_mstree_selectfew.Size = new System.Drawing.Size(75, 23);
            this.button_mstree_selectfew.TabIndex = 33;
            this.button_mstree_selectfew.Text = "Select Few";
            this.button_mstree_selectfew.UseVisualStyleBackColor = true;
            this.button_mstree_selectfew.Click += new System.EventHandler(this.button_mstree_selectfew_Click);
            // 
            // button_refreshselected
            // 
            this.button_refreshselected.Location = new System.Drawing.Point(339, 211);
            this.button_refreshselected.Name = "button_refreshselected";
            this.button_refreshselected.Size = new System.Drawing.Size(75, 23);
            this.button_refreshselected.TabIndex = 32;
            this.button_refreshselected.Text = "Refresh!";
            this.button_refreshselected.UseVisualStyleBackColor = true;
            this.button_refreshselected.Click += new System.EventHandler(this.button_refreshselected_Click);
            // 
            // label_selected
            // 
            this.label_selected.AutoSize = true;
            this.label_selected.Location = new System.Drawing.Point(258, 10);
            this.label_selected.Name = "label_selected";
            this.label_selected.Size = new System.Drawing.Size(83, 13);
            this.label_selected.TabIndex = 31;
            this.label_selected.Text = "Selected Nodes";
            // 
            // textBox_selected
            // 
            this.textBox_selected.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox_selected.Location = new System.Drawing.Point(258, 28);
            this.textBox_selected.Multiline = true;
            this.textBox_selected.Name = "textBox_selected";
            this.textBox_selected.Size = new System.Drawing.Size(230, 177);
            this.textBox_selected.TabIndex = 30;
            // 
            // label_treeviewms
            // 
            this.label_treeviewms.AutoSize = true;
            this.label_treeviewms.Location = new System.Drawing.Point(6, 12);
            this.label_treeviewms.Name = "label_treeviewms";
            this.label_treeviewms.Size = new System.Drawing.Size(157, 13);
            this.label_treeviewms.TabIndex = 29;
            this.label_treeviewms.Text = "TreeView with multiple selection";
            // 
            // treeViewMS_test
            // 
            this.treeViewMS_test.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.treeViewMS_test.Location = new System.Drawing.Point(9, 28);
            this.treeViewMS_test.Name = "treeViewMS_test";
            this.treeViewMS_test.SelectedNodes = ((System.Collections.ArrayList)(resources.GetObject("treeViewMS_test.SelectedNodes")));
            this.treeViewMS_test.Size = new System.Drawing.Size(234, 444);
            this.treeViewMS_test.TabIndex = 28;
            this.treeViewMS_test.MouseUp += new System.Windows.Forms.MouseEventHandler(this.treeViewMS_test_MouseUp);
            // 
            // Form1
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(518, 518);
            this.Controls.Add(this.tabControl_TMC);
            this.MinimumSize = new System.Drawing.Size(504, 384);
            this.Name = "Form1";
            this.Text = "Form1";
            this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Form1_MouseDown);
            this.tabControl_TMC.ResumeLayout(false);
            this.tabPage_numbers.ResumeLayout(false);
            this.tabPage_numbers.PerformLayout();
            this.tabPage_color.ResumeLayout(false);
            this.tabPage_splash.ResumeLayout(false);
            this.groupBox_splash1.ResumeLayout(false);
            this.tabPage_other.ResumeLayout(false);
            this.groupBox_keycombo.ResumeLayout(false);
            this.groupBox_keycombo.PerformLayout();
            this.tabPage_files.ResumeLayout(false);
            this.tabPage_files.PerformLayout();
            this.tabPage_PanelLayout.ResumeLayout(false);
            this.tabPage_treeview.ResumeLayout(false);
            this.tabPage_treeview.PerformLayout();
            this.groupBox_treeviewbuttons.ResumeLayout(false);
            this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new Form1());
		}

		private void vector3Edit1_ValueChanged(object sender, System.EventArgs e)
		{
			this.label1.Text = this.vector3Edit1.ValueX.ToString();
			this.label2.Text = this.vector3Edit1.ValueY.ToString();
			this.label3.Text = this.vector3Edit1.ValueZ.ToString();
		}

		private void folderChooser1_ValueChanged(object sender, System.EventArgs e)
		{
			this.labelDirectory.Text = this.folderChooser1.Directory;
		}

		private void fileChooser1_ValueChanged(object sender, System.EventArgs e)
		{
			this.labelFullpath.Text = this.fileChooser1.Fullpath;
			this.labelFilename.Text = this.fileChooser1.Filename;
		}

		private void objectList1_NewPressed(object sender, System.EventArgs e)
		{
			this.objectList1.Items.Add("Item");
		}

		private void objectList1_EditPressed(object sender, System.EventArgs e)
		{
			this.labelSelected.Text = this.objectList1.SelectedIndex.ToString();
		}

		private void objectList1_DeletePressed(object sender, System.EventArgs e)
		{
			if (this.objectList1.SelectedIndex >= 0)
				this.objectList1.Items.RemoveAt(this.objectList1.SelectedIndex);
		
		}

		private void rangedFloat1_ValueChanged(object sender, System.EventArgs e)
		{
			this.labelRange.Text = this.rangedFloat1.Value.ToString();
		}

		private void radioGroup1_ChoiceChanged(object sender, System.EventArgs e)
		{
			this.labelChoice.Text = this.radioGroup1.SelectedIndex.ToString();
		}

		private void floatEdit1_ValueChanged(object sender, System.EventArgs e)
		{
			this.labelFloat.Text = this.floatEdit1.Value.ToString();
		}

		private void vector3EditUpDown1_ValueChanged(object sender, System.EventArgs e)
		{
			label_updownXvalue.Text = vector3EditUpDown1.ValueX.ToString() + " "
				+ vector3EditUpDown1.ValueY.ToString() + " "
				+ vector3EditUpDown1.ValueZ.ToString();
		}

		private void button_splash_Click(object sender, System.EventArgs e)
		{
			m_Splash = new TerawattManagedControls.SplashImage("..\\..\\Splash.png");
			m_Splash.SplashFadeInTime = 500;
			m_Splash.SplashFadeOutTime = 1500;
			m_Splash.SplashString = "Splash Splash Splash!!!";
			m_Splash.SetTextColor(255,255,255,0,0,0);
			m_Splash.SplashTextX = 20;
			m_Splash.SplashTextY = 120;
			m_Splash.StartUp();
		}

		private void button_closeSplash_Click(object sender, System.EventArgs e)
		{
			m_Splash.ShutDown();
		}

		private void fileChooser_browsefile_only_ValueChanged(object sender, System.EventArgs e)
		{
			this.label_browsefile_only_path.Text = this.fileChooser_browsefile_only.Fullpath;
        }

        private void checkBox_filenameOnly_CheckedChanged(object sender, EventArgs e)
        {
            this.fileChooser1.FileNameOnly = this.checkBox_filenameOnly.Checked;
        }

		private void hotKey_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
		{
			//	do something here
			int kv = e.KeyValue;
		}

		private void vector3EditRanged1_ValueChanged(object sender, System.EventArgs e)
		{
			label_coords.Text = "Coordinate:\n" 
				+ vector3EditRanged1.ValueX.ToString() + ",\n "
				+ vector3EditRanged1.ValueY.ToString() + ",\n "
				+ vector3EditRanged1.ValueZ.ToString();
		}

		private void comboBox_Layouts_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			int sel_index = this.comboBox_Layouts.SelectedIndex;
			if (sel_index >= 0)
			{
				this.panelLayout1.LayoutStyle = 
					(TerawattManagedControls.PanelLayout.Layouts) sel_index;
			}
		}

        private void hotKey_test_KeyChanged(object sender, KeyEventArgs e)
        {
            label_keycombo_output.Text = this.hotKey_test.KeyComboString;

            KeysConverter kc = new KeysConverter();
            int keycode = (int)kc.ConvertFromString(this.hotKey_test.KeyComboString);
            label_keycombo_number.Text = System.Convert.ToString(keycode);
        }

	    // Updates all child tree nodes recursively.
        private void CheckAllChildNodes(TreeNode treeNode, bool nodeChecked) 
	    {
		    IEnumerator myEnum = treeNode.Nodes.GetEnumerator();
		    while (myEnum.MoveNext()) 
		    {
			    TreeNode node = (TreeNode)(myEnum.Current);

			    node.Checked = nodeChecked;
			    if (node.Nodes.Count > 0) 
			    {
				    // If the current node has child nodes, call the CheckAllChildsNodes method recursively.
				    this.CheckAllChildNodes(node, nodeChecked);
			    }
		    }
	    }

	    // NOTE   This code can be added to the BeforeCheck event handler instead of the AfterCheck event.
	    // After a tree node's Checked property is changed, all its child nodes are updated to the same value.
        private void node_AfterCheck(Object sender, TreeViewEventArgs e) 
	    {
		    // The code only executes if the user caused the checked state to change.
		    if (e.Action != TreeViewAction.Unknown) 
		    {
			    if (e.Node.Nodes.Count > 0) 
			    {
				    // Calls the CheckAllChildNodes method, passing in the current
				    // Checked value of the TreeNode whose checked state changed.
				    this.CheckAllChildNodes(e.Node, e.Node.Checked);
			    }
		    }
	    }

        private void fillintree_fancy(TreeView i_TV)
	    {
		    TreeNode rootnode;
		    TreeNode node;
		    TreeNode subnode;
		    int n;

		    //	set up the tree
		    //
		    i_TV.FullRowSelect = true;
		    i_TV.ShowLines = false;
		    i_TV.Scrollable = true;
		    i_TV.AfterCheck += new System.Windows.Forms.TreeViewEventHandler(this.node_AfterCheck);
		    //i_TV.BackColor = System.Drawing.SystemColors.ControlDark;

		    //	fill it with crap data
		    //
		    // Add nodes to treeView_unchecked.
		    n=0;
		    node = i_TV.Nodes.Add(String.Format("Actors", n++));
		    node.BackColor = System.Drawing.SystemColors.ControlDark;
		    node.ForeColor = System.Drawing.SystemColors.ControlLightLight;
		    subnode = node.Nodes.Add(String.Format("Bob", n++));
		    subnode.BackColor = System.Drawing.SystemColors.ControlLightLight;
		    subnode = node.Nodes.Add(String.Format("Steve", n++));
		    subnode.BackColor = System.Drawing.SystemColors.ControlLightLight;
		    subnode = node.Nodes.Add(String.Format("Joe", n++));
		    subnode.BackColor = System.Drawing.SystemColors.ControlLightLight;
		    subnode = node.Nodes.Add(String.Format("Mike", n++));
		    subnode.BackColor = System.Drawing.SystemColors.ControlLightLight;

		    node = i_TV.Nodes.Add(String.Format("Lights", n++));
		    node.BackColor = System.Drawing.Color.LightGray;
		    node.ForeColor = System.Drawing.Color.Black;
		    subnode = node.Nodes.Add(String.Format("spot", n++));
		    subnode.BackColor = System.Drawing.SystemColors.ControlLightLight;
		    subnode = node.Nodes.Add(String.Format("directional", n++));
		    subnode.BackColor = System.Drawing.Color.Yellow;
		    subnode = node.Nodes.Add(String.Format("point-001", n++));
		    subnode.BackColor = System.Drawing.SystemColors.ControlLightLight;

		    node = i_TV.Nodes.Add(String.Format("Props", n++));
		    node.BackColor = System.Drawing.SystemColors.ControlDark;
		    node.ForeColor = System.Drawing.SystemColors.ControlLightLight;
		    subnode = node.Nodes.Add(String.Format("Chair1", n++));
		    subnode.BackColor = System.Drawing.SystemColors.ControlLightLight;
		    subnode = node.Nodes.Add(String.Format("Chair2", n++));
		    subnode.BackColor = System.Drawing.SystemColors.ControlLightLight;
		    subnode = node.Nodes.Add(String.Format("table", n++));
		    subnode.BackColor = System.Drawing.SystemColors.ControlLightLight;
		    subnode = node.Nodes.Add(String.Format("cup", n++));
		    subnode.BackColor = System.Drawing.SystemColors.ControlLightLight;
	    }

        private void treeViewMS_test_KeyDown(object sender, KeyEventArgs e)
        {
        }

        private void show_selected_nodes()
        {
            if (this.treeViewMS_test.SelectedNode != null)
            {
                int x = this.treeViewMS_test.SelectedNodes.Count;
            }
            if (this.treeViewMS_test.SelectedNodes.Count > 0)
            {
                textBox_selected.Clear();
                IEnumerator myEnum = treeViewMS_test.SelectedNodes.GetEnumerator();
                while (myEnum.MoveNext())
                {
                    TreeNode node = (TreeNode)(myEnum.Current);

                    if (node != null)
                    {
                        textBox_selected.Text += node.Text;
                        textBox_selected.Text += "\r\n";
                    }
                }
            }
        }

        //  MouseUp occurs BEFORE the treeview's AfterSelect, so it is
        //  always behind by one selection.
        private void treeViewMS_test_MouseUp(object sender, MouseEventArgs e)
        {
            show_selected_nodes();
        }

        private void Form1_MouseDown(object sender, MouseEventArgs e)
        {
        }

        private void button_refreshselected_Click(object sender, EventArgs e)
        {
            show_selected_nodes();
        }

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		TreeNode find_treenode( TreeNode i_pTreeNode, String i_pTreeNodeText )
		{
			if (i_pTreeNode != null)
			{
				if ( i_pTreeNode.Text.Equals(i_pTreeNodeText) )
				{
					return i_pTreeNode;
				}

				for (int i = 0; i < i_pTreeNode.Nodes.Count; ++i)
				{
					TreeNode pTN = find_treenode(i_pTreeNode.Nodes[i], i_pTreeNodeText);
					if (pTN != null)
					{
						return pTN;
					}
				}
			}

			return null;
		}

		//--------------------------------------------------------------------
		TreeNode FindTreeNode( TreeNodeCollection i_pTNC, String i_pTreeNodeText )
		{
			IEnumerator  myNodes = (i_pTNC).GetEnumerator();
			try
			{
				while (myNodes.MoveNext())
				{
					TreeNode  pFoundNode = find_treenode((TreeNode)(myNodes.Current),
														  i_pTreeNodeText);
					if (pFoundNode != null)
					{
						return pFoundNode;
					}
				}
			}
			finally
			{
				//IDisposable  disposable = (IDisposable)(myNodes);
				//if (disposable != null) 
				//	delete disposable;
			}

			return null;
		}

        private void button_mstree_selectfew_Click(object sender, EventArgs e)
        {
            TreeNode pTN;
            pTN = FindTreeNode(this.treeViewMS_test.Nodes, "table");
            treeViewMS_test.AddToSelectedNodes(pTN);
            pTN = FindTreeNode(this.treeViewMS_test.Nodes, "spot");
            treeViewMS_test.AddToSelectedNodes(pTN);
            pTN = FindTreeNode(this.treeViewMS_test.Nodes, "Steve");
            treeViewMS_test.AddToSelectedNodes(pTN);
            //pTN = FindTreeNode(this.treeViewMS_test.Nodes, "cup");
            //treeViewMS_test.AddToSelectedNodes(pTN);
        }

        private void multiRanged1_ValueChanged(object sender, EventArgs e)
        {
            this.labelMultiValueX.Text = this.multiRanged1.ValueX.ToString();
            this.labelMultiValueY.Text = this.multiRanged1.ValueY.ToString();
        }

    }
}
