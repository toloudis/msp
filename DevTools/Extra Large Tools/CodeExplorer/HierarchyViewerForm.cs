using EnvDTE;
using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

	
//============================================================================
//============================================================================
namespace CodeExplorer
{
	//========================================================================
	/// Summary description for HierarchyViewerForm.
	//========================================================================
	public class HierarchyViewerForm : System.Windows.Forms.Form
	{
		private System.ComponentModel.Container components = null;
//		private ImageList imgList;
		private System.Windows.Forms.TreeView treeView_code;

		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		public HierarchyViewerForm( ref TreeNode i_RootNode )
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
//			try
//			{
//				// Load tree view images from assembly
//				this.imgList = new ImageList();
//				this.imgList.ImageSize = new Size(16, 16);
//
//				System.IO.Stream stream = System.Reflection.Assembly.GetExecutingAssembly().GetManifestResourceStream("Elements.bmp"); 
//				//System.IO.Stream stream = System.Reflection.Assembly.GetExecutingAssembly().GetManifestResourceStream(".//..//..//Elements.bmp"); 
//				System.Drawing.Bitmap bmp = new System.Drawing.Bitmap(stream); 
//
//				this.imgList.Images.AddStrip(bmp);
//				treeView_code.ImageList = this.imgList;
//			}
//			catch (System.Exception ex)
//			{
//				DevEnvLib.DebugOutput.Message("HiearchyViewerForm excepetion {0}", ex.ToString() );
//			}

			InitializeTreeView(ref i_RootNode);
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
			treeView_code = new System.Windows.Forms.TreeView();
			this.SuspendLayout();
			// 
			// treeView_code
			// 
			treeView_code.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			treeView_code.ImageIndex = -1;
			treeView_code.Location = new System.Drawing.Point(8, 8);
			treeView_code.Name = "treeView_code";
			treeView_code.SelectedImageIndex = -1;
			treeView_code.Size = new System.Drawing.Size(352, 392);
			treeView_code.TabIndex = 0;
			// 
			// HierarchyViewerForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(368, 469);
			this.Controls.Add(treeView_code);
			this.Name = "HierarchyViewerForm";
			this.Text = "Code Hierarchy";
			this.ResumeLayout(false);
		}
		#endregion

		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		public void InitializeTreeView(ref TreeNode i_RootNode)
		{
			//this.SuspendLayout();
			//treeView_code.SuspendLayout();
			treeView_code.BeginUpdate();

			treeView_code.Nodes.Clear();

			treeView_code.Nodes.Add( i_RootNode );

			treeView_code.EndUpdate();
			//treeView_code.ResumeLayout(true);
			//this.ResumeLayout(true);
		}
	}
}
