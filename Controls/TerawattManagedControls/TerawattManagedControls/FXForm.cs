using System;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace TerawattManagedControls
{
	/// <summary>
	/// FXForm adds some flashy effects to forms like fading.
	/// </summary>
	public class FXForm : System.Windows.Forms.Form
	{
		const float c_OPACITY_INC = 0.2F;

		#region Constructor
		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		public FXForm()
		{
 			InitializeComponents();
		}
		public FXForm(bool disposeAtEnd)
		{
			m_bDisposeAtEnd = disposeAtEnd;
			InitializeComponents();
		}

		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		void InitializeComponents()
		{
			this.components = new System.ComponentModel.Container();
			this.m_clock =  new Timer(this.components);
			this.m_clock.Interval = 100;
			this.SuspendLayout();
			this.m_clock.Tick += new EventHandler(Animate);

			this.Load += new EventHandler(FXForm_Load);
			this.Closing += new CancelEventHandler(FXForm_Closing);
			this.ResumeLayout(false);
			this.PerformLayout();
		}
		#endregion

		#region Event handlers
		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		private void FXForm_Load(object sender, EventArgs e)
		{
			//this.Opacity = 0.0;
			m_bShowing = true;

			m_clock.Start();
		}

		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		private void FXForm_Closing(object sender, CancelEventArgs e)
		{
			if (!m_bForceClose)
			{
				m_origDialogResult = this.DialogResult;
				e.Cancel = true;
				m_bShowing = false;
				m_clock.Start();
			}
			else
			{
				this.DialogResult = m_origDialogResult;
			}
		}
		#endregion

		#region Private methods
		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		private void Animate(object sender, EventArgs e)
		{
			if (m_bShowing)
			{
//				if (this.Opacity < 1)
//				{
//					this.Opacity += c_OPACITY_INC;
//				}
//				else
				{
					m_clock.Stop();
				}
			}
			else
			{
//				if (this.Opacity > 0)
//				{
//					this.Opacity -= c_OPACITY_INC;
//				}
//				else
				{
					m_clock.Stop();
					m_bForceClose = true;
					this.Close();
					if (m_bDisposeAtEnd)
						this.Dispose();
				}
			}
		}
		#endregion

		#region overrides
		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}
		#endregion

		#region private variables
		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		private System.ComponentModel.IContainer components = null;
		private Timer m_clock;
		private bool m_bShowing = true;
		private bool m_bForceClose = false;
		private DialogResult m_origDialogResult;
		private bool m_bDisposeAtEnd = false;
		#endregion // private variables
	}
}
