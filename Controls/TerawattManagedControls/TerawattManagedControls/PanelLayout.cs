using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace TerawattManagedControls
{
	/// <summary>
	/// PanelLayout manages four controls assigned by the user and arranges
	/// them in layouts where 1 to 4 of the controls are visible.
	/// </summary>
	public class PanelLayout : System.Windows.Forms.UserControl
	{
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		private int m_Separation = 2;
		/// <summary>
		/// Amount of space to put between panels
		/// </summary>
		public int Separation
		{
			get { return m_Separation; }
			set 
			{ 
				m_Separation = value;
				this.do_layout();
			}
		}

		private Color m_FocusColor = System.Drawing.Color.Blue;
		/// <summary>
		/// Color to highlight the panel with focus
		/// </summary>
		public Color FocusColor
		{
			get { return m_FocusColor; }
			set 
			{ 
				m_FocusColor = value;
				this.Invalidate(); 
			}
		}

		public enum Layouts
		{
			e_SinglePane = 0,
			e_TwoSideBySide,
			e_TwoStacked,
			e_ThreeSplitTop,
			e_ThreeSplitLeft,
			e_ThreeSplitBottom,
			e_ThreeSplitRight,
			e_FourPanes,
			e_NumLayouts
		};
		private Layouts m_Layout = Layouts.e_SinglePane;

		public Layouts LayoutStyle
		{
			get { return this.m_Layout; }
			set
			{
				this.m_Layout = value;
				this.do_layout();
			}
		}

		private System.Windows.Forms.Control [] m_Controls = null;
		public System.Windows.Forms.Control GetControl(int i_Index)
		{
			if (i_Index >= 0 && i_Index < 4)
				return this.m_Controls[i_Index]; 
			else
				return null;
		}
		public void SetControl(int i_Index, System.Windows.Forms.Control i_Control)
		{
			if (i_Index >= 0 && i_Index < 4)
			{
				// need to clear out old control in this slot, if any
				if (this.m_Controls[i_Index] != null)
					this.Controls.Remove(this.m_Controls[i_Index]);

				// Keep track of it in our array
				this.m_Controls[i_Index] = i_Control;

				// add into our components array
				if (i_Control != null)
				{
					this.Controls.Add(i_Control);

					i_Control.Enter +=new EventHandler(Control_Enter);
				}

				// layout out the controls
				this.do_layout();
			}
		}

		public PanelLayout()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			m_Controls = new System.Windows.Forms.Control[4];
			m_Controls[0] = null;
			m_Controls[1] = null;
			m_Controls[2] = null;
			m_Controls[3] = null;
		}

		/// <summary>
		/// Return number of panes that are used in the current layout style
		/// </summary>
		public int GetNumPanelsVisible()
		{
			switch (this.m_Layout)
			{
				default:
				case Layouts.e_SinglePane:
					return 1;
				case Layouts.e_TwoSideBySide:
				case Layouts.e_TwoStacked:
					return 2;
				case Layouts.e_ThreeSplitTop:
				case Layouts.e_ThreeSplitLeft:
				case Layouts.e_ThreeSplitBottom:
				case Layouts.e_ThreeSplitRight:
					return 3;
				case Layouts.e_FourPanes:
					return 4;
			}
		}

		/// <summary>
		/// Return a display string for the name of the panel layout
		/// with the given layout style.
		/// </summary>
		public string GetLayoutString(Layouts i_Style)
		{
			switch (i_Style)
			{
				default:
					return "";
				case Layouts.e_SinglePane:
					return "Single Pane";
				case Layouts.e_TwoSideBySide:
					return "Two Panes Side by Side";
				case Layouts.e_TwoStacked:
					return "Two Panes Stacked";
				case Layouts.e_ThreeSplitTop:
					return "Three Panes Split Top";
				case Layouts.e_ThreeSplitLeft:
					return "Three Panes Split Left";
				case Layouts.e_ThreeSplitBottom:
					return "Three Panes Split Bottom";
				case Layouts.e_ThreeSplitRight:
					return "Three Panes Split Right";
				case Layouts.e_FourPanes:
					return "Four Panes";
			}
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
			// 
			// PanelLayout
			// 
			this.Name = "PanelLayout";
			this.Resize += new System.EventHandler(this.PanelLayout_Resize);

		}
		#endregion


		private void layout_control(int i_Index, int i_X, int i_Y, 
								    int i_Width, int i_Height,
									int i_Offset, int i_Shrink)
		{
			if (m_Controls[i_Index] != null)
			{
				m_Controls[i_Index].Location = new Point(i_X + i_Offset, i_Y + i_Offset);
				m_Controls[i_Index].Size = new Size(i_Width - i_Shrink, i_Height - i_Shrink);
				m_Controls[i_Index].Visible = true;
				m_Controls[i_Index].Enabled = true;
			}
		}
		private void disable_control(int i_Index)
		{
			if (m_Controls[i_Index] != null)
			{
				m_Controls[i_Index].Visible = false;
				m_Controls[i_Index].Enabled = false;
			}
		}

		private void do_layout()
		{
			this.SuspendLayout();

			Size full_size = this.Size;
			int hw = full_size.Width/2;
			int hh = full_size.Height/2;
			int off = this.m_Separation;
			int shrink = off * 2;
			switch (this.m_Layout)
			{
				default:
				case Layouts.e_SinglePane:
					// no offset or shrink when doing single pane
					layout_control(0, 0, 0, full_size.Width, full_size.Height, 0, 0);
					disable_control(1);
					disable_control(2);
					disable_control(3);
					break;
				case Layouts.e_TwoSideBySide:
					layout_control(0, 0, 0, hw, full_size.Height, off, shrink);
					layout_control(1, hw, 0, hw, full_size.Height, off, shrink);
					disable_control(2);
					disable_control(3);
					break;
				case Layouts.e_TwoStacked:
					layout_control(0, 0, 0, full_size.Width, hh, off, shrink);
					layout_control(1, 0, hh, full_size.Width, hh, off, shrink);
					disable_control(2);
					disable_control(3);
					break;
				case Layouts.e_ThreeSplitTop:
					layout_control(0, 0, hh, full_size.Width, hh, off, shrink);
					layout_control(1, 0, 0, hw, hh, off, shrink);
					layout_control(2, hw, 0, hw, hh, off, shrink);
					disable_control(3);
					break;
				case Layouts.e_ThreeSplitLeft:
					layout_control(0, hw, 0, hw, full_size.Height, off, shrink);
					layout_control(1, 0, 0, hw, hh, off, shrink);
					layout_control(2, 0, hh, hw, hh, off, shrink);
					disable_control(3);
					break;
				case Layouts.e_ThreeSplitBottom:
					layout_control(0, 0, 0, full_size.Width, hh, off, shrink);
					layout_control(1, 0, hh, hw, hh, off, shrink);
					layout_control(2, hw, hh, hw, hh, off, shrink);
					disable_control(3);
					break;
				case Layouts.e_ThreeSplitRight:
					layout_control(0, 0, 0, hw, full_size.Height, off, shrink);
					layout_control(1, hw, 0, hw, hh, off, shrink);
					layout_control(2, hw, hh, hw, hh, off, shrink);
					disable_control(3);
					break;
				case Layouts.e_FourPanes:
					layout_control(0, 0, 0, hw, hh, off, shrink);
					layout_control(1, hw, hh, hw, hh, off, shrink);
					layout_control(2, 0, hh, hw, hh, off, shrink);
					layout_control(3, hw, 0, hw, hh, off, shrink);
					break;
			}

			this.ResumeLayout();
			this.Invalidate(); 
		}

		private void PanelLayout_Resize(object sender, System.EventArgs e)
		{
			do_layout();
		}

		private void Control_Enter(object sender, EventArgs e)
		{
			this.Invalidate(); 
		}
	
		protected override void OnPaint(PaintEventArgs e)
		{
			if (this.m_Separation > 0 &&
				this.m_Layout != Layouts.e_SinglePane)
			{
				// Find the control with the focus
				foreach (Control ctrl in this.m_Controls)
				{
					if (ctrl != null && ctrl.Focused)
					{
						Pen pen = new Pen(this.m_FocusColor, this.m_Separation);
						Rectangle bds = ctrl.Bounds;
						int hs = this.m_Separation / 2;
						e.Graphics.DrawRectangle(pen, bds.X-hs, bds.Y-hs, bds.Width+this.m_Separation, bds.Height+this.m_Separation);
					}
				}
			}

			base.OnPaint (e);
		}
	}
}
