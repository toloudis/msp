using System;
using System.Diagnostics;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Drawing.Text;
using System.Windows.Forms;

namespace TerawattManagedControls
{
	/// <summary>
	/// Summary description for SplashImage.
	/// </summary>
	public class SplashImage : System.Windows.Forms.Form
	{
		private System.Int16 m_TimerCount;
		private System.Int16 m_FadeInTime;		// milliseconds
		private System.Int16 m_FadeOutTime;		// milliseconds
		private System.Int16 m_VisibleTime;		// milliseconds
		private System.Int16 m_TextX;
		private System.Int16 m_TextY;
		private System.Int16 m_UpdateTime;		// milliseconds

		private enum states
				{
					e_FadeIn = 0,
					e_Visible,
					e_FadeOut,
					e_Exit
				};
		private states m_State;

		private System.Windows.Forms.Panel panel_splash;
		private System.Windows.Forms.Label label_status;
		private System.ComponentModel.IContainer components;
		/// <summary>
		/// Timer used to update the screen at regular intervals.
		/// </summary>
		System.Threading.Timer splashTimer = null;

		/// <summary>
		/// Specifies the filter for file extensions to open
		/// </summary>
		[Bindable(true), Category("Appearance"), 
		DefaultValue("All files (*.*)|*.*"),
		Description("Specifies the filename")]
		public System.String SplashString
		{
			get { return label_status.Text; }
			set	{ label_status.Text = value; }
		}

		/// <summary>
		/// Specifies the TimerCount
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0),
		Description("Specifies the Timer Count")]
		public System.Int16 SplashTimerCount
		{
			get { return m_TimerCount; }
			set
			{
				m_TimerCount = value;
			}
		}

		/// <summary>
		/// Specifies the FadeIn Time
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0),
		Description("Specifies the FadeIn Time")]
		public System.Int16 SplashFadeInTime
		{
			get { return m_FadeInTime; }
			set
			{
				m_FadeInTime = value;
			}
		}

		/// <summary>
		/// Specifies the Visible Time
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0),
		Description("Specifies the Visible Time")]
		public System.Int16 SplashVisibleTime
		{
			get { return m_VisibleTime; }
			set
			{
				m_VisibleTime = value;
			}
		}

		/// <summary>
		/// Specifies the FadeOut Time
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0),
		Description("Specifies the FadeOut Time")]
		public System.Int16 SplashFadeOutTime
		{
			get { return m_FadeOutTime; }
			set
			{
				m_FadeOutTime = value;
			}
		}

		/// <summary>
		/// Specifies the Text X 
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0),
		Description("Specifies the Text X location")]
		public System.Int16 SplashTextX
		{
			get { return m_TextX; }
			set
			{
				m_TextX = value;
			}
		}

		/// <summary>
		/// Specifies the Text Y 
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0),
		Description("Specifies the Text Y location")]
		public System.Int16 SplashTextY
		{
			get { return m_TextY; }
			set
			{
				m_TextY = value;
			}
		}

		/// <summary>
		/// Specifies the Update Time
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0),
		Description("Specifies how often to update")]
		public System.Int16 SplashUpdateTime
		{
			get { return m_UpdateTime; }
			set
			{
				m_UpdateTime = value;
			}
		}

		/// <summary>
		/// Specifies the Text Fore Color 
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0),
		Description("Specifies the Text Fore Color")]
		public System.Drawing.Color SplashTextForeColor
		{
			get { return this.label_status.ForeColor; }
			set
			{
				this.label_status.ForeColor = value;
			}
		}

		/// <summary>
		/// Specifies the Text Back Color 
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0),
		Description("Specifies the Text Back Color")]
		public System.Drawing.Color SplashTextBackColor
		{
			get { return this.label_status.BackColor; }
			set
			{
				this.label_status.BackColor = value;
			}
		}

		public SplashImage(System.String i_BackgroundImageFilename)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//	load the background image
			//
			this.panel_splash.BackgroundImage = new Bitmap(i_BackgroundImageFilename, true);
			Debug.Assert( this.panel_splash.BackgroundImage != null , "Image not found" );

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
			SetupComponents();
		}
		private void SetupComponents()
		{
			m_TimerCount = 0;
			m_FadeInTime = 500;		// milliseconds
			m_FadeOutTime = 1000;	// milliseconds
			m_VisibleTime = -1;		// milliseconds
			m_TextY = 0;
			m_TextX = 0;
			m_UpdateTime = 50;

			//	set the form width to be the image size
			this.Height = this.panel_splash.BackgroundImage.Height;
			this.Width = this.panel_splash.BackgroundImage.Width;

			m_State = states.e_FadeIn;

			this.Opacity = 0.0;
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
			this.panel_splash = new System.Windows.Forms.Panel();
			this.label_status = new System.Windows.Forms.Label();
			this.panel_splash.SuspendLayout();
			this.SuspendLayout();
			// 
			// panel_splash
			// 
			this.panel_splash.BackColor = System.Drawing.Color.Transparent;
			//this.panel_splash.Controls.Add(this.label_status);
			this.panel_splash.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel_splash.Location = new System.Drawing.Point(0, 0);
			this.panel_splash.Name = "panel_splash";
			this.panel_splash.Size = new System.Drawing.Size(256, 256);
			this.panel_splash.TabIndex = 0;
			// 
			// label_status
			// 
			this.label_status.AutoSize = true;
			this.label_status.BackColor = System.Drawing.Color.White;
			this.label_status.ForeColor = System.Drawing.Color.Black;
			this.label_status.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.label_status.Location = new System.Drawing.Point(0, 0);
			this.label_status.Name = "label_status";
			this.label_status.Size = new System.Drawing.Size(54, 16);
			this.label_status.TabIndex = 0;
			this.label_status.Text = "Loading...";
			this.label_status.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// SplashImage
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.BackColor = System.Drawing.Color.White;
			this.ClientSize = new System.Drawing.Size(256, 256);
			this.Controls.Add(this.panel_splash);
			this.Controls.Add(this.label_status);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "SplashImage";
			this.Opacity = 0.8;
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.TopMost = true;
			this.TransparencyKey = System.Drawing.Color.White;
			this.panel_splash.ResumeLayout(false);
			this.ResumeLayout(false);
			this.label_status.BringToFront();
		}
		#endregion

		/// <summary>
		/// Draw the screen.  This is the callback for the timer
		/// </summary>
		/// <param name="state">Not used - timer data</param>
		protected void Draw(Object state)
		{
			Draw();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		private void Draw()
		{
			m_TimerCount++;
			int time_elapsed = m_TimerCount*m_UpdateTime;

			switch (m_State)
			{
				case states.e_FadeIn:
					if (time_elapsed >= m_FadeInTime)
					{
						m_State = states.e_Visible;
						m_TimerCount = 0;
						this.Opacity = 1.0;
					}
					else
					{
						double op = ((double)time_elapsed / (double)m_FadeInTime);
						this.Opacity = op;
						if (this.Opacity > 1.0) this.Opacity = 1.0;
					}
					break;
				default:
				case states.e_Visible:
					//	Note: this will be an infinite loop if value is -1.
					//	It is up to the user to Deactivate it.
					if (   (m_VisibleTime > 0)
						&& (time_elapsed > m_VisibleTime))
					{
						m_State = states.e_FadeOut;
						m_TimerCount = 0;
					}
					break;
				case states.e_FadeOut:
					if (time_elapsed >= m_FadeOutTime)
					{
						m_State = states.e_Exit;
						m_TimerCount = 0;
						this.Opacity = 0.0;
					}
					else
					{
						double op = 1.0 - ((double)time_elapsed / (double)m_FadeInTime);
						this.Opacity = op;
						if (this.Opacity < 0.0) this.Opacity = 0.0;
					}
					break;
				case states.e_Exit:
					this.Hide();
					this.splashTimer.Dispose();
					break;
			}
		}

		public void StartUp()
		{
			m_TimerCount = 0;
			m_State = states.e_FadeIn;
			this.Opacity = 0.0;

			// Start a timer that will call Draw every x ms
			System.Threading.TimerCallback splashDelegate = new System.Threading.TimerCallback(this.Draw);
			this.splashTimer = new System.Threading.Timer(splashDelegate, null, 0, m_UpdateTime);

			this.label_status.Location = new System.Drawing.Point(m_TextX,m_TextY);
			this.BringToFront();
			Show();
		}

		public void ShutDown()
		{
			m_TimerCount = 0;
			m_State = states.e_FadeOut;
		}

		public void SetTextColor( Byte i_FGR, Byte i_FGG, Byte i_FGB, 
			Byte i_BGR, Byte i_BGG, Byte i_BGB )
		{
			this.label_status.BackColor = System.Drawing.Color.FromArgb(((System.Byte)(i_BGR)), ((System.Byte)(i_BGG)), ((System.Byte)(i_BGB)));
			this.label_status.ForeColor = System.Drawing.Color.FromArgb(((System.Byte)(i_FGR)), ((System.Byte)(i_FGG)), ((System.Byte)(i_FGB)));
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------

	}
}
