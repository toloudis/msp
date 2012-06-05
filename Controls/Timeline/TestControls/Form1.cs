using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

using TimelineControls;

namespace TestControls
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form1 : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Panel panel1;
		private TimelineControls.ChannelControl channelControl1;
		private TimelineControls.ChannelControl channelControl2;
		private TimelineControls.ChannelControl channelControl3;
		private TimelineControls.TimeLabel timeLabel1;
		private TimelineControls.TimeSlider timeSlider1;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public Form1()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
			//this.channelControl1.TotalTime = 100.0;
			this.channelControl1.AddClip( new ChannelClip("Clip1", 2.0, 30.0) );
			this.channelControl2.AddClip( new ChannelClip("Idle", 20.0, 30.0) );
			this.channelControl2.AddClip( new ChannelClip("Walk", 3.0, 13.0) );
			ChannelClip clip = new ChannelClip("ColorCycle", 20.0, 50.0);
			clip.Blend = ChannelClip.BlendType.e_Previous;
			clip.Category = "Color";
			this.channelControl3.AddClip( clip );
			this.channelControl3.AddClip( new ChannelClip("Split", 40.0, 60.0) );
			this.channelControl3.MultiChannel = true;
			this.channelControl3.Expanded = true;

			this.timeSlider1.AddTimeTick(2.0f);
			this.timeSlider1.AddTimeTick(12.0f);
			this.timeSlider1.AddTimeTick(22.0f);
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
			this.panel1 = new System.Windows.Forms.Panel();
			this.timeSlider1 = new TimelineControls.TimeSlider();
			this.timeLabel1 = new TimelineControls.TimeLabel();
			this.channelControl3 = new TimelineControls.ChannelControl();
			this.channelControl2 = new TimelineControls.ChannelControl();
			this.channelControl1 = new TimelineControls.ChannelControl();
			this.panel1.SuspendLayout();
			this.SuspendLayout();
			// 
			// panel1
			// 
			this.panel1.AutoScroll = true;
			this.panel1.Controls.Add(this.timeSlider1);
			this.panel1.Controls.Add(this.timeLabel1);
			this.panel1.Controls.Add(this.channelControl3);
			this.panel1.Controls.Add(this.channelControl2);
			this.panel1.Controls.Add(this.channelControl1);
			this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel1.Location = new System.Drawing.Point(0, 0);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(553, 241);
			this.panel1.TabIndex = 0;
			// 
			// timeSlider1
			// 
			this.timeSlider1.CurTime = 1;
			this.timeSlider1.Location = new System.Drawing.Point(7, 49);
			this.timeSlider1.Name = "timeSlider1";
			this.timeSlider1.Size = new System.Drawing.Size(526, 13);
			this.timeSlider1.TabIndex = 4;
			this.timeSlider1.TabStop = false;
			// 
			// timeLabel1
			// 
			this.timeLabel1.Location = new System.Drawing.Point(7, 21);
			this.timeLabel1.Name = "timeLabel1";
			this.timeLabel1.Size = new System.Drawing.Size(526, 28);
			this.timeLabel1.TabIndex = 3;
			this.timeLabel1.TabStop = false;
			// 
			// channelControl3
			// 
			this.channelControl3.Expanded = false;
			this.channelControl3.Location = new System.Drawing.Point(7, 139);
			this.channelControl3.MultiChannel = true;
			this.channelControl3.Name = "channelControl3";
			this.channelControl3.Size = new System.Drawing.Size(526, 27);
			this.channelControl3.TabIndex = 2;
			this.channelControl3.TabStop = false;
			this.channelControl3.ClipSelected += new TimelineControls.ClipSelectedEventHandler(this.channelControl_ClipSelected);
			this.channelControl3.ClipMoveFinished += new System.EventHandler(this.channelControl_ClipMoveFinished);
			this.channelControl3.ClipMoved += new TimelineControls.MouseMoveEventHandler(this.channelControl_ClipMoved);
			// 
			// channelControl2
			// 
			this.channelControl2.Expanded = false;
			this.channelControl2.Location = new System.Drawing.Point(7, 104);
			this.channelControl2.Name = "channelControl2";
			this.channelControl2.Size = new System.Drawing.Size(526, 28);
			this.channelControl2.TabIndex = 1;
			this.channelControl2.TabStop = false;
			this.channelControl2.ClipSelected += new TimelineControls.ClipSelectedEventHandler(this.channelControl_ClipSelected);
			this.channelControl2.ClipMoveFinished += new System.EventHandler(this.channelControl_ClipMoveFinished);
			this.channelControl2.ClipMoved += new TimelineControls.MouseMoveEventHandler(this.channelControl_ClipMoved);
			// 
			// channelControl1
			// 
			this.channelControl1.Expanded = false;
			this.channelControl1.Location = new System.Drawing.Point(7, 69);
			this.channelControl1.Name = "channelControl1";
			this.channelControl1.Size = new System.Drawing.Size(526, 28);
			this.channelControl1.TabIndex = 0;
			this.channelControl1.TabStop = false;
			this.channelControl1.ClipSelected += new TimelineControls.ClipSelectedEventHandler(this.channelControl_ClipSelected);
			this.channelControl1.ClipMoveFinished += new System.EventHandler(this.channelControl_ClipMoveFinished);
			this.channelControl1.ClipMoved += new TimelineControls.MouseMoveEventHandler(this.channelControl_ClipMoved);
			// 
			// Form1
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(553, 241);
			this.Controls.Add(this.panel1);
			this.Name = "Form1";
			this.Text = "Form1";
			this.panel1.ResumeLayout(false);
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

		private void channelControl_ClipMoved(object sender, TimelineControls.MouseMoveEventArgs e)
		{
			channelControl1.InteractMoveSelectedClips(e.TimeDelta);
			channelControl2.InteractMoveSelectedClips(e.TimeDelta);
			channelControl3.InteractMoveSelectedClips(e.TimeDelta);
		}

		private void channelControl_ClipMoveFinished(object sender, System.EventArgs e)
		{
			channelControl1.FinishInteraction();
			channelControl2.FinishInteraction();
			channelControl3.FinishInteraction();
		}

		private void channelControl_ClipSelected(object sender, TimelineControls.ClipSelectedEventArgs e)
		{
			// If we are clearing the selection, or not appending to 
			// the selection, then clear selection from other channels
			if (e.SelectedClip == null || e.Appended == false)
			{
				if (sender != channelControl1)
					channelControl1.ClearSelection();
				if (sender != channelControl2)
					channelControl2.ClearSelection();
				if (sender != channelControl3)
					channelControl3.ClearSelection();
			}
			
			// Safe to call on channels that do not have selected clips
			channelControl1.BeginInteraction();
			channelControl2.BeginInteraction();
			channelControl3.BeginInteraction();
		}  
	}
}
