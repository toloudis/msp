using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace TimelineControls
{
	/// <summary>
	/// Summary description for ClipTimesDialog.
	/// </summary>
	public class ClipTimesDialog : System.Windows.Forms.Form
	{
		bool m_bLocalChange;

		private TerawattManagedControls.FloatEdit floatedit_BeginTime;
		private TerawattManagedControls.FloatEdit floatedit_Duration;
		private System.Windows.Forms.Button buttonOK;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private TerawattManagedControls.FloatEdit floatedit_BlendTime;
		private System.Windows.Forms.ComboBox comboBlend;
		private System.Windows.Forms.Label label_begintime;
		private System.Windows.Forms.Label label_duration;
		private System.Windows.Forms.Label label_blendtime;
		private System.Windows.Forms.Label label_blendtype;
		private System.Windows.Forms.CheckBox checkbox_checkRestore;
		private System.Windows.Forms.Label label_endtime;
		private TerawattManagedControls.FloatEdit floatEdit_EndTime;
		private System.Windows.Forms.TabControl tabControl_properties;
		private System.Windows.Forms.TabPage tabPage_base;

		private ChannelClip m_Clip;

		public ClipTimesDialog(ChannelClip i_Clip)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			m_Clip = i_Clip;
			this.floatedit_BeginTime.Value	= m_Clip.BeginTime;
			this.floatedit_Duration.Value	= m_Clip.Duration;
			this.floatedit_BlendTime.Value	= m_Clip.BlendTime;

			this.comboBlend.SelectedIndex = (int)m_Clip.Blend;
			this.checkbox_checkRestore.Checked = m_Clip.Restore;
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

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.label_begintime = new System.Windows.Forms.Label();
			this.label_duration = new System.Windows.Forms.Label();
			this.floatedit_BeginTime = new TerawattManagedControls.FloatEdit();
			this.floatedit_Duration = new TerawattManagedControls.FloatEdit();
			this.buttonOK = new System.Windows.Forms.Button();
			this.floatedit_BlendTime = new TerawattManagedControls.FloatEdit();
			this.label_blendtime = new System.Windows.Forms.Label();
			this.label_blendtype = new System.Windows.Forms.Label();
			this.comboBlend = new System.Windows.Forms.ComboBox();
			this.checkbox_checkRestore = new System.Windows.Forms.CheckBox();
			this.label_endtime = new System.Windows.Forms.Label();
			this.floatEdit_EndTime = new TerawattManagedControls.FloatEdit();
			this.tabControl_properties = new System.Windows.Forms.TabControl();
			this.tabPage_base = new System.Windows.Forms.TabPage();
			this.tabControl_properties.SuspendLayout();
			this.tabPage_base.SuspendLayout();
			this.SuspendLayout();
			// 
			// label_begintime
			// 
			this.label_begintime.Location = new System.Drawing.Point(16, 16);
			this.label_begintime.Name = "label_begintime";
			this.label_begintime.Size = new System.Drawing.Size(73, 28);
			this.label_begintime.TabIndex = 0;
			this.label_begintime.Text = "Begin Time:";
			// 
			// label_duration
			// 
			this.label_duration.Location = new System.Drawing.Point(16, 48);
			this.label_duration.Name = "label_duration";
			this.label_duration.Size = new System.Drawing.Size(73, 27);
			this.label_duration.TabIndex = 1;
			this.label_duration.Text = "Duration:";
			// 
			// floatedit_BeginTime
			// 
			this.floatedit_BeginTime.Location = new System.Drawing.Point(112, 16);
			this.floatedit_BeginTime.Name = "floatedit_BeginTime";
			this.floatedit_BeginTime.Precision = ((short)(2));
			this.floatedit_BeginTime.Size = new System.Drawing.Size(69, 20);
			this.floatedit_BeginTime.TabIndex = 2;
			// 
			// floatedit_Duration
			// 
			this.floatedit_Duration.Location = new System.Drawing.Point(112, 48);
			this.floatedit_Duration.Name = "floatedit_Duration";
			this.floatedit_Duration.Precision = ((short)(2));
			this.floatedit_Duration.Size = new System.Drawing.Size(69, 20);
			this.floatedit_Duration.TabIndex = 3;
			this.floatedit_Duration.ValueChanged += new System.EventHandler(this.floatedit_Duration_ValueChanged);
			// 
			// buttonOK
			// 
			this.buttonOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.buttonOK.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.buttonOK.Location = new System.Drawing.Point(152, 248);
			this.buttonOK.Name = "buttonOK";
			this.buttonOK.Size = new System.Drawing.Size(87, 25);
			this.buttonOK.TabIndex = 4;
			this.buttonOK.Text = "OK";
			this.buttonOK.Click += new System.EventHandler(this.buttonOK_Click);
			// 
			// floatedit_BlendTime
			// 
			this.floatedit_BlendTime.Location = new System.Drawing.Point(112, 128);
			this.floatedit_BlendTime.Name = "floatedit_BlendTime";
			this.floatedit_BlendTime.Precision = ((short)(2));
			this.floatedit_BlendTime.Size = new System.Drawing.Size(69, 20);
			this.floatedit_BlendTime.TabIndex = 6;
			// 
			// label_blendtime
			// 
			this.label_blendtime.Location = new System.Drawing.Point(16, 128);
			this.label_blendtime.Name = "label_blendtime";
			this.label_blendtime.Size = new System.Drawing.Size(73, 28);
			this.label_blendtime.TabIndex = 5;
			this.label_blendtime.Text = "Blend Time:";
			// 
			// label_blendtype
			// 
			this.label_blendtype.Location = new System.Drawing.Point(16, 88);
			this.label_blendtype.Name = "label_blendtype";
			this.label_blendtype.Size = new System.Drawing.Size(73, 28);
			this.label_blendtype.TabIndex = 7;
			this.label_blendtype.Text = "Blend Type:";
			// 
			// comboBlend
			// 
			this.comboBlend.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboBlend.Items.AddRange(new object[] {
															"No Blend",
															"Previous",
															"Blend Time",
															"Overwrite",
															"Smooth Blend"});
			this.comboBlend.Location = new System.Drawing.Point(112, 88);
			this.comboBlend.Name = "comboBlend";
			this.comboBlend.Size = new System.Drawing.Size(120, 21);
			this.comboBlend.TabIndex = 8;
			// 
			// checkbox_checkRestore
			// 
			this.checkbox_checkRestore.Location = new System.Drawing.Point(16, 160);
			this.checkbox_checkRestore.Name = "checkbox_checkRestore";
			this.checkbox_checkRestore.Size = new System.Drawing.Size(177, 20);
			this.checkbox_checkRestore.TabIndex = 9;
			this.checkbox_checkRestore.Text = "Restore Original Value at End";
			// 
			// label_endtime
			// 
			this.label_endtime.Location = new System.Drawing.Point(208, 16);
			this.label_endtime.Name = "label_endtime";
			this.label_endtime.Size = new System.Drawing.Size(72, 28);
			this.label_endtime.TabIndex = 10;
			this.label_endtime.Text = "End Time:";
			// 
			// floatEdit_EndTime
			// 
			this.floatEdit_EndTime.Location = new System.Drawing.Point(288, 16);
			this.floatEdit_EndTime.Name = "floatEdit_EndTime";
			this.floatEdit_EndTime.Precision = ((short)(2));
			this.floatEdit_EndTime.Size = new System.Drawing.Size(69, 20);
			this.floatEdit_EndTime.TabIndex = 11;
			this.floatEdit_EndTime.ValueChanged += new System.EventHandler(this.floatEdit_EndTime_ValueChanged);
			// 
			// tabControl_properties
			// 
			this.tabControl_properties.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.tabControl_properties.Controls.Add(this.tabPage_base);
			this.tabControl_properties.Location = new System.Drawing.Point(0, 0);
			this.tabControl_properties.Name = "tabControl_properties";
			this.tabControl_properties.SelectedIndex = 0;
			this.tabControl_properties.Size = new System.Drawing.Size(392, 242);
			this.tabControl_properties.TabIndex = 12;
			// 
			// tabPage_base
			// 
			this.tabPage_base.Controls.Add(this.comboBlend);
			this.tabPage_base.Controls.Add(this.label_duration);
			this.tabPage_base.Controls.Add(this.checkbox_checkRestore);
			this.tabPage_base.Controls.Add(this.floatedit_BeginTime);
			this.tabPage_base.Controls.Add(this.floatedit_Duration);
			this.tabPage_base.Controls.Add(this.label_endtime);
			this.tabPage_base.Controls.Add(this.floatEdit_EndTime);
			this.tabPage_base.Controls.Add(this.floatedit_BlendTime);
			this.tabPage_base.Controls.Add(this.label_blendtime);
			this.tabPage_base.Controls.Add(this.label_begintime);
			this.tabPage_base.Controls.Add(this.label_blendtype);
			this.tabPage_base.Location = new System.Drawing.Point(4, 22);
			this.tabPage_base.Name = "tabPage_base";
			this.tabPage_base.Size = new System.Drawing.Size(384, 216);
			this.tabPage_base.TabIndex = 0;
			this.tabPage_base.Text = "Base";
			// 
			// ClipTimesDialog
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(392, 278);
			this.Controls.Add(this.tabControl_properties);
			this.Controls.Add(this.buttonOK);
			this.Name = "ClipTimesDialog";
			this.Text = "Driver Clip Properties";
			this.tabControl_properties.ResumeLayout(false);
			this.tabPage_base.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		private void buttonOK_Click(object sender, System.EventArgs e)
		{
			double begin_time	= floatedit_BeginTime.Value;
			double duration		= floatedit_Duration.Value;

			m_Clip.SetTime(begin_time, duration);
			m_Clip.Blend		= (ChannelClip.BlendType)this.comboBlend.SelectedIndex;
			m_Clip.BlendTime	= floatedit_BlendTime.Value;
			m_Clip.Restore		= checkbox_checkRestore.Checked;

			this.Close();
		}

		private void floatedit_Duration_ValueChanged(object sender, System.EventArgs e)
		{
			if (!m_bLocalChange)
			{
				m_bLocalChange = true;
				floatEdit_EndTime.Value = floatedit_Duration.Value + floatedit_BeginTime.Value;
				this.Invalidate();
				m_bLocalChange = false;
			}
		}

		private void floatEdit_EndTime_ValueChanged(object sender, System.EventArgs e)
		{
			if (!m_bLocalChange)
			{
				m_bLocalChange = true;
				floatedit_Duration.Value = floatEdit_EndTime.Value - floatedit_BeginTime.Value;
				this.Invalidate();
				m_bLocalChange = false;
			}
		}
	}
}
