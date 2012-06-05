
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
	public class Form_TimelineMockUp : System.Windows.Forms.Form
	{
		private TimelineControls.TimeLabel timeLabel1;
		private TimelineControls.TimeSlider timeSlider1;
        private TimelineControls.MarkerDisplayBar markerBar1;
        private Panel panel_channels;
		private System.Windows.Forms.ComboBox comboBox_filter;
		private System.Windows.Forms.Label labelTime;
		private System.Windows.Forms.Panel panel_names;
		private System.Windows.Forms.Panel panel_darkline;
		private System.Windows.Forms.Label label_lock;
        private System.Windows.Forms.Button button_addmarker;
		private System.Windows.Forms.Button button_paste;
		private System.Windows.Forms.Button button_copy;
		private System.Windows.Forms.Button button_delete;
		private System.Windows.Forms.Button button_add;
		private System.Windows.Forms.Panel panel_timeline;
		private System.Windows.Forms.Panel panel_whole;
		private System.Windows.Forms.Button button_split;
		private System.Windows.Forms.Button button_clearmarkers;

		private System.Collections.ArrayList m_ChannelCountList; // number of channels per object
		private System.Collections.ArrayList channelList;
		private System.Collections.ArrayList channelLockList;
		private System.Collections.ArrayList labelList;
		private System.Collections.ArrayList labelObjectList;

		int m_GuideLineIndexForMarker;
		int m_GuideLineIndexForTime;

		const int c_NamePanelOffsetY	= 0;
		const int c_NamePanelOffsetX	= 0;
		const int c_NameLabelWidth	= 112;	// must be less than width of panel_names
		const int c_LabelWidth		= 64;
		const int c_ChannelHeight	= 32;
		const int c_ChannelOffsetX	= 8;
		const int c_ScalePanelWidthExtra	= 100;
		private System.Windows.Forms.CheckBox checkBox_snap;
        private Button button_addnote;
        private Label label_marker;
        private Label label_note;
        private TerawattManagedControls.RangedFloat rangedFloat_Zoom;
        private Label label_pos;

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public Form_TimelineMockUp()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
			SetUpComponents();
		}

		void AddChannelsForObject(System.String i_ObjectName, int i_NumChannels)
		{
			int slot_num = this.channelList.Count + labelObjectList.Count;

			// Create label for object
			System.Windows.Forms.Label nameLabel = new System.Windows.Forms.Label();
			nameLabel.Text = i_ObjectName; 
			nameLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			nameLabel.BackColor = System.Drawing.SystemColors.ActiveCaption;
			nameLabel.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
			int offset = this.panel_channels.AutoScrollPosition.X;
			nameLabel.Location = new System.Drawing.Point(c_NamePanelOffsetX + offset, c_NamePanelOffsetY + c_ChannelHeight * slot_num);
			nameLabel.Size = new System.Drawing.Size(c_NameLabelWidth, c_ChannelHeight);
			this.panel_names.Controls.Add(nameLabel);
			labelObjectList.Add(nameLabel);
			slot_num++;

			// Add Channels for object
			int num_channels = i_NumChannels;
			m_ChannelCountList.Add(num_channels);
			for (int i=0; i<num_channels; i++)
			{
				System.String chName = System.String.Format("channel{0}", i);
				ChannelControl pChannelCtrl = this.CreateChannel(chName, i+slot_num, false );
				pChannelCtrl.MultiChannel = false;
			}
		}

	ChannelControl CreateChannel(System.String i_Name, int i_Index, bool i_bLocked)
	{
		int offset = this.panel_channels.AutoScrollPosition.X;

		//	create lock checkbox for each channel
		System.Windows.Forms.CheckBox checkBox = new System.Windows.Forms.CheckBox();

		//	location "+ 84" is to put the checkbox at the end of the panel.
		//	location "+ 6" is to drop down the checkbox to line up with the text
		checkBox.Location = new System.Drawing.Point(8 + 84 + offset, 6 + c_NamePanelOffsetY + c_ChannelHeight * i_Index);
		checkBox.Name = System.String.Format("checkBox{0}", i_Index);
		checkBox.Size = new System.Drawing.Size(16, 16);
		checkBox.TabIndex = 0;
//		checkBox.CheckedChanged += new System.EventHandler(checkBox_CheckedChanged);
		checkBox.Checked = i_bLocked;
		this.panel_names.Controls.Add(checkBox);
		this.channelLockList.Add(checkBox);

		//	create the channel label
		System.Windows.Forms.Label channelLabel = new System.Windows.Forms.Label();
		channelLabel.Name = System.String.Format("channelLabel{0}", i_Index);
		channelLabel.Text = i_Name;
		channelLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
		channelLabel.Location = new System.Drawing.Point(8 + offset, c_NamePanelOffsetY + c_ChannelHeight * i_Index);
		channelLabel.Size = new System.Drawing.Size(c_LabelWidth, c_ChannelHeight);

		this.panel_names.Controls.Add(channelLabel);
		this.labelList.Add(channelLabel);

		// create the channel control
		ChannelControl channelControl = new ChannelControl();
		channelControl.Location = new System.Drawing.Point(offset + c_ChannelOffsetX, c_NamePanelOffsetY + c_ChannelHeight * i_Index);
		channelControl.Name = System.String.Format("channelControl{0}", i_Index);
		channelControl.Size = new System.Drawing.Size(this.timeSlider1.Width, c_ChannelHeight);
		channelControl.TimeScale		= this.timeLabel1.TimeScale;
		channelControl.TotalTime		= this.timeLabel1.TotalTime;

		channelControl.ClipSelected		+= new TimelineControls.ClipSelectedEventHandler( this.ClipSelected );
		channelControl.ClipMoved		+= new TimelineControls.MouseMoveEventHandler( this.ClipMoved );
		channelControl.ClipResized		+= new System.EventHandler( this.ClipResized );
		channelControl.ClipMoveFinished	+= new System.EventHandler( this.ClipMoveFinished );
        channelControl.SizeChanged += new System.EventHandler(this.OnSizeChanged);

		this.panel_channels.Controls.Add(channelControl);
		this.channelList.Add(channelControl);
		return channelControl;
	}
	
private void SetUpComponents()
			{
				channelList			= new ArrayList();
				channelLockList		= new ArrayList();
				labelList			= new ArrayList();
				labelObjectList		= new ArrayList();
				m_ChannelCountList	= new ArrayList();

				this.timeLabel1.TotalTime = this.timeSlider1.TotalTime;
				this.timeLabel1.TimeScale = this.timeSlider1.TimeScale;

				// add the channels automatically
				AddChannelsForObject("ObjectOne", 30);
				AddChannelsForObject("ObjectTwo", 10);

				// add the clips manually
				ChannelControl channel;
				channel = (ChannelControl)(channelList[0]);
				channel.AddClip( new ChannelClip("Clip1", 2.0, 30.0) );
				channel = (ChannelControl)(channelList[1]);
				channel.AddClip( new ChannelClip("Idle", 20.0, 30.0) );
				channel.AddClip( new ChannelClip("Walk", 3.0, 13.0) );
				ChannelClip clip = new ChannelClip("ColorCycle", 20.0, 50.0);
				clip.Blend = ChannelClip.BlendType.e_Previous;
				clip.Category = "Color";
				channel = (ChannelControl)(channelList[2]);
				channel.AddClip( clip );
				channel.AddClip( new ChannelClip("Split", 40.0, this.timeSlider1.TotalTime) );
				channel.MultiChannel = true;
				channel.Expanded = true;

				//  
				m_GuideLineIndexForTime = TimeGuideLineMgr.CreateGuideLine();
				TimeGuideLine guide_line = TimeGuideLineMgr.GetGuideLine(m_GuideLineIndexForTime);
				this.panel_channels.Controls.Add( guide_line );
				guide_line.Color = System.Drawing.SystemColors.HotTrack;
				guide_line.TimeScale = this.timeSlider1.TimeScale;
				guide_line.Height = this.panel_channels.DisplayRectangle.Height;
				guide_line.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left))); 
				guide_line.BringToFront();

				m_GuideLineIndexForMarker = TimeGuideLineMgr.CreateGuideLine();
				guide_line = TimeGuideLineMgr.GetGuideLine(m_GuideLineIndexForMarker);
				this.panel_channels.Controls.Add( guide_line );
				guide_line.Color = System.Drawing.SystemColors.HotTrack;
				guide_line.TimeScale = this.timeSlider1.TimeScale;
				guide_line.Height = this.panel_channels.DisplayRectangle.Height;
				guide_line.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left))); 
				//guide_line.Enabled = false;
				guide_line.Visible = false;
				guide_line.BringToFront();
    
				// TODO remove ticks and do it automatically
				this.timeSlider1.AddTimeTick(2.0f);
				this.timeSlider1.AddTimeTick(12.0f);
				this.timeSlider1.AddTimeTick(22.0f);

				timeSlider1.TimeChanged +=new EventHandler(timeSlider1_TimeChanged);
                //markerBar1.MarkersChanged += new EventHandler(markerBar1_MarkersChanged);
                //markerBar1.NotesChanged += new EventHandler(markerBar1_MarkersChanged);

                //
                //panel_channels.HScroll = true;
                ////panel_channels.ScrollControlIntoView = true;
                //HScrollProperties hsp;
                //hsp.Enabled = true;
                //panel_channels.HorizontalScroll = hsp;
                //VScrollProperties vsp;
                //vsp.Enabled = true;
                //panel_channels.VerticalScroll = vsp;
                ////panel_channels.Scroll
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
            this.panel_whole = new System.Windows.Forms.Panel();
            this.button_addnote = new System.Windows.Forms.Button();
            this.panel_timeline = new System.Windows.Forms.Panel();
            this.panel_darkline = new System.Windows.Forms.Panel();
            this.label_lock = new System.Windows.Forms.Label();
            this.button_addmarker = new System.Windows.Forms.Button();
            this.button_paste = new System.Windows.Forms.Button();
            this.comboBox_filter = new System.Windows.Forms.ComboBox();
            this.labelTime = new System.Windows.Forms.Label();
            this.button_copy = new System.Windows.Forms.Button();
            this.button_delete = new System.Windows.Forms.Button();
            this.button_add = new System.Windows.Forms.Button();
            this.panel_channels = new System.Windows.Forms.Panel();
            this.panel_names = new System.Windows.Forms.Panel();
            this.button_split = new System.Windows.Forms.Button();
            this.button_clearmarkers = new System.Windows.Forms.Button();
            this.checkBox_snap = new System.Windows.Forms.CheckBox();
            this.label_note = new System.Windows.Forms.Label();
            this.label_marker = new System.Windows.Forms.Label();
            this.rangedFloat_Zoom = new TerawattManagedControls.RangedFloat();
            this.timeLabel1 = new TimelineControls.TimeLabel();
            this.timeSlider1 = new TimelineControls.TimeSlider();
            this.markerBar1 = new TimelineControls.MarkerDisplayBar();
            this.label_pos = new System.Windows.Forms.Label();
            this.panel_whole.SuspendLayout();
            this.panel_timeline.SuspendLayout();
            this.panel_darkline.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.timeLabel1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.timeSlider1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.markerBar1)).BeginInit();
            this.SuspendLayout();
            // 
            // panel_whole
            // 
            this.panel_whole.BackColor = System.Drawing.SystemColors.Control;
            this.panel_whole.Controls.Add(this.label_pos);
            this.panel_whole.Controls.Add(this.rangedFloat_Zoom);
            this.panel_whole.Controls.Add(this.label_marker);
            this.panel_whole.Controls.Add(this.label_note);
            this.panel_whole.Controls.Add(this.button_addnote);
            this.panel_whole.Controls.Add(this.panel_darkline);
            this.panel_whole.Controls.Add(this.button_addmarker);
            this.panel_whole.Controls.Add(this.button_paste);
            this.panel_whole.Controls.Add(this.comboBox_filter);
            this.panel_whole.Controls.Add(this.labelTime);
            this.panel_whole.Controls.Add(this.button_copy);
            this.panel_whole.Controls.Add(this.button_delete);
            this.panel_whole.Controls.Add(this.button_add);
            this.panel_whole.Controls.Add(this.panel_channels);
            this.panel_whole.Controls.Add(this.panel_names);
            this.panel_whole.Controls.Add(this.button_split);
            this.panel_whole.Controls.Add(this.button_clearmarkers);
            this.panel_whole.Controls.Add(this.checkBox_snap);
            this.panel_whole.Controls.Add(this.panel_timeline);
            this.panel_whole.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel_whole.Location = new System.Drawing.Point(0, 0);
            this.panel_whole.Name = "panel_whole";
            this.panel_whole.Size = new System.Drawing.Size(823, 345);
            this.panel_whole.TabIndex = 0;
            // 
            // button_addnote
            // 
            this.button_addnote.Location = new System.Drawing.Point(152, 34);
            this.button_addnote.Name = "button_addnote";
            this.button_addnote.Size = new System.Drawing.Size(16, 16);
            this.button_addnote.TabIndex = 28;
            this.button_addnote.Text = "+";
            this.button_addnote.Click += new System.EventHandler(this.button_addnote_Click);
            // 
            // panel_timeline
            // 
            this.panel_timeline.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.panel_timeline.AutoScroll = true;
            this.panel_timeline.BackColor = System.Drawing.SystemColors.ActiveBorder;
            this.panel_timeline.Controls.Add(this.timeLabel1);
            this.panel_timeline.Controls.Add(this.timeSlider1);
            this.panel_timeline.Controls.Add(this.markerBar1);
            this.panel_timeline.Location = new System.Drawing.Point(168, 0);
            this.panel_timeline.Name = "panel_timeline";
            this.panel_timeline.Padding = new System.Windows.Forms.Padding(10, 0, 0, 0);
            this.panel_timeline.Size = new System.Drawing.Size(652, 89);
            this.panel_timeline.TabIndex = 6;
            this.panel_timeline.Scroll += new System.Windows.Forms.ScrollEventHandler(this.panel_timeline_Scroll);
            // 
            // panel_darkline
            // 
            this.panel_darkline.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.panel_darkline.BackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.panel_darkline.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel_darkline.Controls.Add(this.label_lock);
            this.panel_darkline.Location = new System.Drawing.Point(48, 72);
            this.panel_darkline.Name = "panel_darkline";
            this.panel_darkline.Size = new System.Drawing.Size(772, 14);
            this.panel_darkline.TabIndex = 6;
            // 
            // label_lock
            // 
            this.label_lock.Location = new System.Drawing.Point(95, 0);
            this.label_lock.Name = "label_lock";
            this.label_lock.Size = new System.Drawing.Size(24, 16);
            this.label_lock.TabIndex = 23;
            this.label_lock.Text = "lock";
            // 
            // button_addmarker
            // 
            this.button_addmarker.Location = new System.Drawing.Point(152, 51);
            this.button_addmarker.Name = "button_addmarker";
            this.button_addmarker.Size = new System.Drawing.Size(16, 16);
            this.button_addmarker.TabIndex = 25;
            this.button_addmarker.Text = "+";
            this.button_addmarker.Click += new System.EventHandler(this.button_addmarker_Click);
            // 
            // button_paste
            // 
            this.button_paste.Location = new System.Drawing.Point(4, 192);
            this.button_paste.Name = "button_paste";
            this.button_paste.Size = new System.Drawing.Size(40, 23);
            this.button_paste.TabIndex = 16;
            this.button_paste.Text = "paste";
            // 
            // comboBox_filter
            // 
            this.comboBox_filter.Items.AddRange(new object[] {
            "Selected",
            "System",
            "All"});
            this.comboBox_filter.Location = new System.Drawing.Point(5, 40);
            this.comboBox_filter.Name = "comboBox_filter";
            this.comboBox_filter.Size = new System.Drawing.Size(80, 21);
            this.comboBox_filter.TabIndex = 11;
            this.comboBox_filter.Text = "Selected";
            // 
            // labelTime
            // 
            this.labelTime.Font = new System.Drawing.Font("Microsoft Sans Serif", 8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelTime.Location = new System.Drawing.Point(100, 4);
            this.labelTime.Name = "labelTime";
            this.labelTime.Size = new System.Drawing.Size(63, 19);
            this.labelTime.TabIndex = 10;
            this.labelTime.Text = "88:88:88";
            // 
            // button_copy
            // 
            this.button_copy.Location = new System.Drawing.Point(4, 168);
            this.button_copy.Name = "button_copy";
            this.button_copy.Size = new System.Drawing.Size(40, 23);
            this.button_copy.TabIndex = 9;
            this.button_copy.Text = "copy";
            // 
            // button_delete
            // 
            this.button_delete.Location = new System.Drawing.Point(4, 144);
            this.button_delete.Name = "button_delete";
            this.button_delete.Size = new System.Drawing.Size(40, 23);
            this.button_delete.TabIndex = 8;
            this.button_delete.Text = "delete";
            // 
            // button_add
            // 
            this.button_add.Location = new System.Drawing.Point(4, 120);
            this.button_add.Name = "button_add";
            this.button_add.Size = new System.Drawing.Size(40, 23);
            this.button_add.TabIndex = 7;
            this.button_add.Text = "add";
            // 
            // panel_channels
            // 
            this.panel_channels.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.panel_channels.AutoScroll = true;
            this.panel_channels.BackColor = System.Drawing.SystemColors.ActiveBorder;
            this.panel_channels.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel_channels.Location = new System.Drawing.Point(168, 84);
            this.panel_channels.Name = "panel_channels";
            this.panel_channels.Size = new System.Drawing.Size(652, 261);
            this.panel_channels.TabIndex = 5;
            this.panel_channels.Scroll += new System.Windows.Forms.ScrollEventHandler(this.panel_channels_Scroll);
            this.panel_channels.Paint += new System.Windows.Forms.PaintEventHandler(this.panel_channels_Paint);
            // 
            // panel_names
            // 
            this.panel_names.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)));
            this.panel_names.AutoScroll = true;
            this.panel_names.BackColor = System.Drawing.SystemColors.ActiveBorder;
            this.panel_names.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel_names.Location = new System.Drawing.Point(48, 84);
            this.panel_names.Name = "panel_names";
            this.panel_names.Size = new System.Drawing.Size(144, 245);
            this.panel_names.TabIndex = 1;
            this.panel_names.Scroll += new System.Windows.Forms.ScrollEventHandler(this.panel_names_Scroll);
            // 
            // button_split
            // 
            this.button_split.Location = new System.Drawing.Point(4, 96);
            this.button_split.Name = "button_split";
            this.button_split.Size = new System.Drawing.Size(40, 23);
            this.button_split.TabIndex = 27;
            this.button_split.Text = "split";
            this.button_split.Click += new System.EventHandler(this.button_split_Click);
            // 
            // button_clearmarkers
            // 
            this.button_clearmarkers.Location = new System.Drawing.Point(135, 51);
            this.button_clearmarkers.Name = "button_clearmarkers";
            this.button_clearmarkers.Size = new System.Drawing.Size(16, 16);
            this.button_clearmarkers.TabIndex = 1;
            this.button_clearmarkers.Text = "---";
            this.button_clearmarkers.Click += new System.EventHandler(this.button_clearmarkers_Click);
            // 
            // checkBox_snap
            // 
            this.checkBox_snap.Location = new System.Drawing.Point(4, 68);
            this.checkBox_snap.Name = "checkBox_snap";
            this.checkBox_snap.Size = new System.Drawing.Size(52, 24);
            this.checkBox_snap.TabIndex = 0;
            this.checkBox_snap.Text = "Snap";
            // 
            // label_note
            // 
            this.label_note.AutoSize = true;
            this.label_note.Location = new System.Drawing.Point(105, 36);
            this.label_note.Name = "label_note";
            this.label_note.Size = new System.Drawing.Size(28, 13);
            this.label_note.TabIndex = 29;
            this.label_note.Text = "note";
            // 
            // label_marker
            // 
            this.label_marker.AutoSize = true;
            this.label_marker.Location = new System.Drawing.Point(105, 52);
            this.label_marker.Name = "label_marker";
            this.label_marker.Size = new System.Drawing.Size(30, 13);
            this.label_marker.TabIndex = 30;
            this.label_marker.Text = "mark";
            // 
            // rangedFloat_Zoom
            // 
            this.rangedFloat_Zoom.Exponent = ((short)(1));
            this.rangedFloat_Zoom.Location = new System.Drawing.Point(3, 8);
            this.rangedFloat_Zoom.Maximum = 500;
            this.rangedFloat_Zoom.Name = "rangedFloat_Zoom";
            this.rangedFloat_Zoom.NumTicks = ((short)(100));
            this.rangedFloat_Zoom.Precision = ((short)(2));
            this.rangedFloat_Zoom.ShowValue = false;
            this.rangedFloat_Zoom.Size = new System.Drawing.Size(91, 24);
            this.rangedFloat_Zoom.TabIndex = 0;
            this.rangedFloat_Zoom.Value = 80;
            this.rangedFloat_Zoom.ValueChanged += new System.EventHandler(this.rangedFloat_Zoom_ValueChanged);
            // 
            // timeLabel1
            // 
            this.timeLabel1.BackColor = System.Drawing.SystemColors.InactiveBorder;
            this.timeLabel1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.timeLabel1.Location = new System.Drawing.Point(10, 4);
            this.timeLabel1.Name = "timeLabel1";
            this.timeLabel1.Size = new System.Drawing.Size(634, 28);
            this.timeLabel1.TabIndex = 3;
            this.timeLabel1.TabStop = false;
            // 
            // timeSlider1
            // 
            this.timeSlider1.BackColor = System.Drawing.SystemColors.Window;
            this.timeSlider1.CurTime = 1;
            this.timeSlider1.Location = new System.Drawing.Point(10, 36);
            this.timeSlider1.Name = "timeSlider1";
            this.timeSlider1.Size = new System.Drawing.Size(634, 13);
            this.timeSlider1.TabIndex = 4;
            this.timeSlider1.TabStop = false;
            // 
            // markerBar1
            // 
            this.markerBar1.BackColor = System.Drawing.SystemColors.ControlLight;
            this.markerBar1.CurTime = 5;
            this.markerBar1.Location = new System.Drawing.Point(10, 52);
            this.markerBar1.Name = "markerBar1";
            this.markerBar1.Size = new System.Drawing.Size(634, 13);
            this.markerBar1.TabIndex = 0;
            this.markerBar1.TabStop = false;
            // 
            // label_pos
            // 
            this.label_pos.AutoSize = true;
            this.label_pos.Location = new System.Drawing.Point(0, 329);
            this.label_pos.Name = "label_pos";
            this.label_pos.Size = new System.Drawing.Size(24, 13);
            this.label_pos.TabIndex = 0;
            this.label_pos.Text = "pos";
            // 
            // Form_TimelineMockUp
            // 
            this.ClientSize = new System.Drawing.Size(823, 345);
            this.Controls.Add(this.panel_whole);
            this.MinimumSize = new System.Drawing.Size(440, 344);
            this.Name = "Form_TimelineMockUp";
            this.Text = "Timeline";
            this.panel_whole.ResumeLayout(false);
            this.panel_whole.PerformLayout();
            this.panel_timeline.ResumeLayout(false);
            this.panel_darkline.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.timeLabel1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.timeSlider1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.markerBar1)).EndInit();
            this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new Form_TimelineMockUp());
		}

        private void update_time_label()
        {
            double time = this.timeSlider1.CurTime;

            int min, sec, frames;
            min = (int)(((int)(time)) / 60.0f);
            sec = (int)(((int)time) - (float)min * 60.0f);
            const float FPS = 24.0f;    // TODO 24fps is hardcoded.
            frames = (int)(((float)(time - ((int)time))) * FPS);

            string desc;
            desc = string.Format("{0:00}:{1:00}.{2:00} ", (min), (sec), (frames));
            labelTime.Text = desc;
        }

		private void TimeChanged(System.Object sender, System.EventArgs e)
		{
			float time = (float)this.timeSlider1.CurTime;

			//	update the Time GuideLine
			TimeGuideLineMgr.GetGuideLine(m_GuideLineIndexForTime).Time = time;
			this.panel_channels.Invalidate();

            //  update the time
            update_time_label();
		}

		private void ClipSelected(System.Object sender, TimelineControls.ClipSelectedEventArgs e)
		{
			this.button_delete.Enabled = true;
            ChannelClip cc = e.SelectedClip;
            if (cc != null)
                cc.UseHighlightFill = true;

			ChannelControl picked = (ChannelControl)(sender);
			for (int i=0; i<this.channelList.Count; i++)
			{
				ChannelControl channel = (ChannelControl)(channelList[i]);
				if (channel != picked)
					channel.ClearSelection();
			}
		}

		private bool TimesMatch( float i_Time1, float i_Time2 )
		{
			float c_TIMEDIFF = 0.008f;
			if (this.checkBox_snap.Checked)
			{
				c_TIMEDIFF = 0.5f;
			}

			return (   (i_Time1 >= (i_Time2 - c_TIMEDIFF))
				&& (i_Time1 <= (i_Time2 + c_TIMEDIFF)));
		}
		private void ClipMoveFinished(System.Object sender, System.EventArgs e)
		{
			//	turn off the guidelines
			TimeGuideLineMgr.GetGuideLine(m_GuideLineIndexForMarker).Visible = false;
		}

		// Clip resized
		//
		private void ClipResized(System.Object sender, System.EventArgs  e)
		{
		}

		// Clip moved
		//
		private void ClipMoved(System.Object   sender, TimelineControls.MouseMoveEventArgs   e)
		{
			float delta = e.TimeDelta;
			float closest_time = 0;
			int closest_type = -1;
			bool bMatch = false;
			bool bBeginTimeMatch = false;

			ChannelControl pickedchannel = (ChannelControl)(sender);
            if (pickedchannel.SelectedClip < 0)
                return;
			ChannelClip pickedclip = pickedchannel.Clips[pickedchannel.SelectedClip];
			float pickstart = (float)pickedclip.BeginTime + delta;
			float pickend	= (float)pickedclip.EndTime + delta;

			//	check the clips of each channel and see if there is a match anywhere.
			//
			for (int i=0; i < this.channelList.Count; i++)
			{
				ChannelControl channel = (ChannelControl)(channelList[i]);

				for (int j=0; j < channel.Clips.Count; ++j)
				{
					ChannelClip clip = (ChannelClip)(channel.Clips[j]);
					if ( clip != pickedclip )
					{
						float clipstart = (float)clip.BeginTime;
						float clipend	= (float)clip.EndTime;

						if (TimesMatch(pickstart, clipstart))
						{
							bMatch = true;
							bBeginTimeMatch = true;
							closest_time = clipstart;
							closest_type = 0;
						}
						else if (TimesMatch(pickstart, clipend))
						{
							bMatch = true;
							bBeginTimeMatch = true;
							closest_time = clipend;
							closest_type = 0;
						}
						else if (TimesMatch(pickend, clipstart))
						{
							bMatch = true;
							bBeginTimeMatch = false;
							closest_time = clipstart;
							closest_type = 1;
						}
						else if (TimesMatch(pickend, clipend))
						{
							bMatch = true;
							bBeginTimeMatch = false;
							closest_time = clipend;
							closest_type = 1;
						}
					}
				}
			}

			//	check the markers to see if they match the clip start/end
			//
            //for (int k = 0; k < this.markerBar1.MarkerCount; ++k)
            //{
            //    float mtime = (float)this.markerBar1.Marker(k).Time;
            //    if (TimesMatch(pickstart, mtime))
            //    {
            //        bMatch = true;
            //        bBeginTimeMatch = true;
            //        closest_time = mtime;
            //        closest_type = 2;
            //    }
            //    else if (TimesMatch(pickend, mtime) )
            //    {
            //        bMatch = true;
            //        bBeginTimeMatch = false;
            //        closest_time = mtime;
            //        closest_type = 2;
            //    }
            //}

			//	check current time
			//
			float curtime = (float)this.timeSlider1.CurTime;
			if (TimesMatch(pickstart, curtime))
			{
				bMatch = true;
				bBeginTimeMatch = true;
				closest_time = curtime;
				closest_type = 3;
			}
			else if (TimesMatch(pickend, curtime))
			{
				bMatch = true;
				bBeginTimeMatch = false;
				closest_time = curtime;
				closest_type = 3;
			}

			//	found a match
			//
			if (closest_type != -1)
			{
				TimeGuideLineMgr.GetGuideLine(m_GuideLineIndexForMarker).Time = closest_time;
				if (this.checkBox_snap.Checked)
				{
					if (bBeginTimeMatch)
					{
						 pickedclip.InteractStart = closest_time;
					}
					else
					{
						pickedclip.InteractEnd = closest_time;
					}
				}
				switch (closest_type)
				{
					default:
					case 0:
					case 1:
						TimeGuideLineMgr.GetGuideLine(m_GuideLineIndexForMarker).Color = System.Drawing.Color.Green;
						break;
					case 2:
					case 3:
						TimeGuideLineMgr.GetGuideLine(m_GuideLineIndexForMarker).Color = System.Drawing.Color.DarkGreen;
						break;
				}
			}
			TimeGuideLineMgr.GetGuideLine(m_GuideLineIndexForMarker).Visible = bMatch;
            TimeGuideLineMgr.GetGuideLine(m_GuideLineIndexForMarker).BringToFront();
		}

		private void OnSizeChanged(System.Object sender, System.EventArgs e)
		{
			int offset = this.panel_channels.AutoScrollPosition.Y;
			int height = c_NamePanelOffsetY + offset;
			int ch_cnt = 0, obj_cnt = 0;
			for (int i=0; i<this.channelList.Count; i++)
			{	
				// Update name label when needed
				if (ch_cnt == 0)
				{
					Label label = (Label)(labelObjectList[obj_cnt]);
					label.Location = new System.Drawing.Point(label.Location.X, height );
					height += label.Height;	
					System.Int32 int_ptr = (System.Int32)(this.m_ChannelCountList[obj_cnt]);
					ch_cnt = (int_ptr);
					obj_cnt++;
				}
				ch_cnt--;

				// Offset channel control and channel label
				ChannelControl channel = (ChannelControl)(channelList[i]);
				channel.Location = new System.Drawing.Point(channel.Location.X, height );
				Label label1 = (Label)(labelList[i]);
				label1.Location = new System.Drawing.Point(label1.Location.X, height );
				height += channel.Height;
			}
		}

		private void panel_channels_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
		{
			System.Drawing.Point point = this.panel_names.AutoScrollPosition;
			point.Y = -panel_channels.AutoScrollPosition.Y;
			panel_names.AutoScrollPosition = point;

			point = this.panel_timeline.AutoScrollPosition;
			point.X = -panel_channels.AutoScrollPosition.X;
			panel_timeline.AutoScrollPosition = point;

			// paint the line
			// Draw color triangle at current time
			double time, timescale;
			timescale	= this.timeSlider1.TimeScale;
			time		= this.timeSlider1.CurTime;

			// temporary
			TimeGuideLine tgl = TimeGuideLineMgr.GetGuideLine(m_GuideLineIndexForTime);

			//	normally this would be called ONLY when the scale changes
			TimeGuideLineMgr.UpdateScale( timescale );
			this.Invalidate();
		}

		private void button_addmarker_Click(object sender, System.EventArgs e)
		{
			this.markerBar1.AddMarker( this.timeSlider1.CurTime );
		}

		private void timeSlider1_TimeChanged(object sender, EventArgs e)
		{
			double time = this.timeSlider1.CurTime;
			
            TimeGuideLineMgr.GetGuideLine(m_GuideLineIndexForTime).Time = time;
			//TimeGuideLineMgr.GetGuideLine(m_GuideLineIndexForTime).Location.X = this.timeSlider1.Location.X;
			this.panel_channels.Invalidate();

            //
            update_time_label();
		}

		private void markerBar1_MarkersChanged(object sender, EventArgs e)
		{
			// get the marker data
		}

		public int SplitSelectedClips( ChannelControl i_ChannelControl, double i_SplitTime )
		{
			if (i_ChannelControl.SelectedClip != -1)
			{
				ChannelClip oldclip = (ChannelClip)i_ChannelControl.Clips.Clips[i_ChannelControl.SelectedClip];
				if (   i_SplitTime > oldclip.BeginTime
					&& i_SplitTime < oldclip.EndTime )
				{
					ChannelClip newclip = oldclip.Clone();

					// TODO: [rjk] come up with a better new clip name
					newclip.Name += "+";

					oldclip.EndTime		= i_SplitTime;
					newclip.BeginTime	= i_SplitTime;

					int index = i_ChannelControl.AddClip( newclip );
					return index;
				}
			}
			return 0;
		}

		private void button_split_Click(object sender, System.EventArgs e)
		{
			for (int i = 0 ; i < this.channelList.Count; ++i )
			{
				int index;
				index = SplitSelectedClips( (ChannelControl)(this.channelList[i]), this.timeSlider1.CurTime );
				((ChannelControl)(this.channelList[i])).Invalidate();
			}
			this.panel_channels.Invalidate();
		}

		private void button_clearmarkers_Click(object sender, System.EventArgs e)
		{
			markerBar1.ClearMarkers();
			markerBar1.Invalidate();
		}

        private void button_addnote_Click(object sender, EventArgs e)
        {
            this.markerBar1.AddNote(this.timeSlider1.CurTime);
        }

		//	based on the length of the panel, calculate the zoom so the whole timeline
		//	is visible on screen.
		//
	    public void CalculateMaxZoom()
		{
			double totaltime = tmlnTimeLine.GetMaximum();
			int label_width = timeLabel1.Width + c_ScalePanelWidthExtra;	// FIX: [rjk] what is this c_ScalePanelWidthExtra for? make it a constant with an explaination
			int panel_width = panel_timeline.Width - (c_ScalePanelWidthExtra / 2);
			double zoom = panel_width / totaltime;

			if (zoom < rangedFloat_Zoom.Minimum)
				zoom = (double)rangedFloat_Zoom.Minimum;
			else if (zoom > rangedFloat_Zoom.Maximum)
				zoom = (double)rangedFloat_Zoom.Maximum;
			this.rangedFloat_Zoom.Value = zoom;
		}
        public void TimelineZoomIn()
        {
	        double newvalue = (double)rangedFloat_Zoom.Value;
	        newvalue++;
	        if (newvalue > rangedFloat_Zoom.Maximum) 
		        newvalue = (double)rangedFloat_Zoom.Maximum;
	        rangedFloat_Zoom.Value = newvalue;
	        //center_view();
        }

        public void TimelineZoomOut()
        {
	        double newvalue = (double)rangedFloat_Zoom.Value;
	        newvalue--;
	        if (newvalue < rangedFloat_Zoom.Minimum) 
		        newvalue = (double)rangedFloat_Zoom.Minimum;
	        rangedFloat_Zoom.Value = newvalue;
	        //center_view();
        }

	    // TimeScale changes the zoom left/right
	    public void SetTimeScale(double i_Scale)
	    {
		    this.timeLabel1.TimeScale	= i_Scale;
		    this.timeSlider1.TimeScale	= i_Scale;
		    this.markerBar1.TimeScale	= i_Scale;

		    int width = timeLabel1.Width + c_ScalePanelWidthExtra;	// FIX: [rjk] what is this c_ScalePanelWidthExtra for? make it a constant with an explaination
            //this.panel_timeline.Width = width;
            //this.panel_channels.Width = width;
            // why can't these be set?
		    //this.panel_channels.AutoScrollMinSize.Width = width;
		    //this.panel_timeline.AutoScrollMinSize.Width = width;

		    //DBG_LOG4( "label width (%d) panel width (%d)  scale (%6.3f) total time(%6.3f)", width, panel_timeline.Width, i_Scale, tmlnTimeLine::GetMaximum());

		    for (int i=0; i < channelList.Count; i++)
		    {	
			    ChannelControl channel = (ChannelControl)(channelList[i]);
			    channel.TimeScale = i_Scale;
		    }

		    //	normally this would be called ONLY when the scale changes
		    //TimeGuideLineMgr::UpdateScale( i_Scale );

		    //	adjust the timeline position based on the selected object or current time.
		    //
		    //center_view();
	    }

        private void rangedFloat_Zoom_ValueChanged(object sender, EventArgs e)
        {
            this.SetTimeScale((double)this.rangedFloat_Zoom.Value);
        }

        private void panel_channels_Scroll(object sender, ScrollEventArgs e)
        {
            panel_names.AutoScrollPosition = new Point(-panel_channels.AutoScrollPosition.X,
                                                        -panel_channels.AutoScrollPosition.Y);
            panel_timeline.AutoScrollPosition = new Point(-panel_channels.AutoScrollPosition.X,
                                                        -panel_channels.AutoScrollPosition.Y);

            String newtxt = String.Format("{0},{1}", -panel_channels.AutoScrollPosition.X, -panel_channels.AutoScrollPosition.Y);
            label_pos.Text = newtxt;
        }

        private void panel_names_Scroll(object sender, ScrollEventArgs e)
        {
            //panel_channels.AutoScrollPosition = new Point(-panel_names.AutoScrollPosition.X,
            //                                            -panel_names.AutoScrollPosition.Y);
        }

        private void panel_timeline_Scroll(object sender, ScrollEventArgs e)
        {

        }
	
	}
}
