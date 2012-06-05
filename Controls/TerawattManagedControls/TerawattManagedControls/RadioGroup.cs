using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace TerawattManagedControls
{
	/// <summary>
	/// Group box with choices of radio buttons to
	/// represent an enumeration.
	/// </summary>
	[ToolboxItem(true)]
	[ToolboxBitmap(typeof(RadioGroup))]
	public class RadioGroup : System.Windows.Forms.UserControl
	{
		private System.Collections.ArrayList radioButtons = null;
		private System.Collections.ArrayList stringList = null;
		private System.Int32 selectedIndex = -1;

		/// <summary>
		/// Callback when choice changes
		/// </summary>
		[Category("Property Changed"), 
		Description("Callback when choice changes")]
		public event EventHandler ChoiceChanged;
		
		/// <summary>
		/// List of Items to display as choices
		/// </summary>
		[Category("Data"), 
		Description("List of Items to display as choices")]
		public System.Collections.ArrayList Items
		{
			 get {return this.stringList; }
		}


		/// <summary>
		/// Exposes SelectedIndex in ComboBox
		/// </summary>
		[Category("Data"), 
		DefaultValue(-1),
		Description("Exposes SelectedIndex in ComboBox")]
		public System.Int32 SelectedIndex
		{
			get { return this.selectedIndex; }
			set {
				this.selectedIndex = value; 
				this.LayoutButtons();
				this.SelectRadio(value);
			}
		}
		/// <summary>
		/// Exposes Text in GroupBox
		/// </summary>
		[Category("Appearance"), 
		DefaultValue("Choices"),
		Description("Exposes Text in GroupBox")]
		public override string Text
		{
			get {return this.groupBox1.Text; }
			set {this.groupBox1.Text = value; }
		}

		private System.Windows.Forms.GroupBox groupBox1;
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public RadioGroup()
		{
			// Create array of radio buttons
			stringList = new System.Collections.ArrayList();
			radioButtons = new System.Collections.ArrayList();

			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();
		}

		private System.Windows.Forms.RadioButton AddRadio()
		{
			int num = this.radioButtons.Count;
			RadioButton new_radio = new RadioButton();
			new_radio.Location = new System.Drawing.Point(16, 32+num*32);
			new_radio.Name = "my radio";
			new_radio.Size = new System.Drawing.Size(168, 24);
			new_radio.CheckedChanged += new System.EventHandler(this.radioButton1_CheckedChanged);
			radioButtons.Add(new_radio);
			this.groupBox1.Controls.Add(new_radio);
			return new_radio;
		}

		private void LayoutButtons()
		{
			if (this.radioButtons == null) return;

			int num_items = this.Items.Count;
			int num_radio = this.radioButtons.Count;

			for (int i=num_radio; i<num_items; i++)
				this.AddRadio();

			for (int i=0; i<num_items; i++)
			{
				((RadioButton)this.radioButtons[i]).Text = this.Items[i].ToString();
			}

			this.groupBox1.Height = 32+num_items*32;
		}

		private void SelectRadio(int i)
		{
			if (i>=0 && i<this.radioButtons.Count)
				((RadioButton)this.radioButtons[i]).Select();
		}

		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.SuspendLayout();
			// 
			// groupBox1
			// 
			this.groupBox1.Location = new System.Drawing.Point(8, 8);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(208, 64);
			this.groupBox1.TabIndex = 0;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Choices";
			// 
			// RadioGroup
			// 
			this.Controls.Add(this.groupBox1);
			this.Name = "RadioGroup";
			this.Size = new System.Drawing.Size(232, 232);
			this.SizeChanged += new System.EventHandler(this.RadioGroup_SizeChanged);
			this.ResumeLayout(false);

		}
		#endregion

		private void radioButton1_CheckedChanged(object sender, System.EventArgs e)
		{
			RadioButton radio = (RadioButton)sender;
			if (radio.Checked)
			{
				for (int i=0; i<this.radioButtons.Count; i++)
				{
					//if (radio.Text == this.radioButtons[i].ToString())
					if (radio == this.radioButtons[i])
					{
						this.selectedIndex = i;
						if (this.ChoiceChanged != null)
							this.ChoiceChanged(this, e);
						break;
					}
				}
			}
		}

		private void RadioGroup_SizeChanged(object sender, System.EventArgs e)
		{
			this.groupBox1.Width = this.Width - 16;
		}

	}
}
