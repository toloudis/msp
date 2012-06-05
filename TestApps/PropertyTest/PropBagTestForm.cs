using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

using XLT.Windows.Forms;

namespace PropertyBagTester
{
	public class PropBagTestForm : System.Windows.Forms.Form
	{
		//
		private System.Windows.Forms.ListView listView;
		private System.ComponentModel.IContainer components;
		private System.Windows.Forms.ContextMenu contextMenu;
		private System.Windows.Forms.MenuItem menuReset;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.ImageList imageList;
		private System.Windows.Forms.Button button_propgridshow;
		private PropGridDialog m_PropGridDialog = new PropGridDialog();

		public PropBagTestForm()
		{
			InitializeComponent();

			DataCollections.Init();	// set up the actual data

			ListViewItem item1 = new ListViewItem("Bag 1", 0);
			item1.Tag = DataCollections.bag1;
			listView.Items.Add(item1);
//			ListViewItem item2 = new ListViewItem("Bag 2", 0);
//			item2.Tag = DataCollections.bag2;
//			listView.Items.Add(item2);
			ListViewItem item3 = new ListViewItem("Table", 0);
			item3.Tag = DataCollections.bag3;
			listView.Items.Add(item3);
			ListViewItem item4 = new ListViewItem("Table 2", 0);
			item4.Tag = DataCollections.bag4;
			listView.Items.Add(item4);
		}

		protected override void Dispose(bool disposing)
		{
			if(disposing)
				if(components != null) 
					components.Dispose();

			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(PropBagTestForm));
			this.listView = new System.Windows.Forms.ListView();
			this.imageList = new System.Windows.Forms.ImageList(this.components);
			this.contextMenu = new System.Windows.Forms.ContextMenu();
			this.menuReset = new System.Windows.Forms.MenuItem();
			this.label1 = new System.Windows.Forms.Label();
			this.button_propgridshow = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// listView
			// 
			this.listView.HideSelection = false;
			this.listView.LargeImageList = this.imageList;
			this.listView.Location = new System.Drawing.Point(8, 32);
			this.listView.Name = "listView";
			this.listView.Size = new System.Drawing.Size(240, 256);
			this.listView.TabIndex = 0;
			this.listView.SelectedIndexChanged += new System.EventHandler(this.listView_SelectedIndexChanged);
			// 
			// imageList
			// 
			this.imageList.ImageSize = new System.Drawing.Size(32, 32);
			this.imageList.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList.ImageStream")));
			this.imageList.TransparentColor = System.Drawing.Color.Transparent;
			// 
			// contextMenu
			// 
			this.contextMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																						this.menuReset});
			// 
			// menuReset
			// 
			this.menuReset.Index = 0;
			this.menuReset.Text = "Reset";
			this.menuReset.Click += new System.EventHandler(this.menuReset_Click);
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(8, 8);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(448, 23);
			this.label1.TabIndex = 2;
			this.label1.Text = "Select one or multiple items in the list to see those objects in the property gri" +
				"d.";
			// 
			// button_propgridshow
			// 
			this.button_propgridshow.Location = new System.Drawing.Point(272, 48);
			this.button_propgridshow.Name = "button_propgridshow";
			this.button_propgridshow.Size = new System.Drawing.Size(136, 24);
			this.button_propgridshow.TabIndex = 3;
			this.button_propgridshow.Text = "Show PropertyGrid";
			this.button_propgridshow.Click += new System.EventHandler(this.button_propgridshow_Click);
			// 
			// PropBagTestForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 14);
			this.ClientSize = new System.Drawing.Size(464, 301);
			this.Controls.Add(this.button_propgridshow);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.listView);
			this.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "PropBagTestForm";
			this.Text = "PropertyBag Test Form";
			this.ResumeLayout(false);

		}
		#endregion

		[STAThread] static void Main() { Application.Run(new PropBagTestForm()); }

		private void listView_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			ArrayList objs = new ArrayList();
			foreach(ListViewItem item in listView.SelectedItems)
				objs.Add(item.Tag);

			//if (m_PropGridDialog != null)
			{
				m_PropGridDialog.propertyGrid_data.SelectedObjects = objs.ToArray();
			}
		}

		private void menuReset_Click(object sender, System.EventArgs e)
		{
			//if (m_PropGridDialog != null)
			{
				if(m_PropGridDialog.propertyGrid_data.SelectedObject != null &&
					m_PropGridDialog.propertyGrid_data.SelectedGridItem != null)
				{
					m_PropGridDialog.propertyGrid_data.ResetSelectedProperty();
				}
			}
		}

//		// Member variables associated with the properties of bag 1 and
//		// bag 2.  Since events are fired to query these, you could use
//		// any source--variables, contents of a file, a database, etc.
//		private Fruit DataCollections.bag1_Fruit = Fruit.Orange;
//		private Image DataCollections.bag1_Picture = null;
//
//		private Fruit DataCollections.bag2_Fruit = Fruit.Banana;
//		private Font DataCollections.bag2_Typeface = new Font("Tahoma", 8.25f);
//		private bool DataCollections.bag2_SomeBoolean = false;
//
//		// This is a pretty basic way to handle the properties.  Optimally,
//		// you might have some kind of table that indexes into a database
//		// or file where the values are stored.  But for the purposes of this
//		// example, a simple case statement will do.
//		private void DataCollections.bag1_GetValue(object sender, PropertySpecEventArgs e)
//		{
//			switch(e.Property.Name)
//			{
//				case "Fruit":  e.Value = DataCollections.bag1_Fruit;  break;
//				case "Picture":  e.Value = DataCollections.bag1_Picture;  break;
//			}
//		}
//
//		private void DataCollections.bag1_SetValue(object sender, PropertySpecEventArgs e)
//		{
//			switch(e.Property.Name)
//			{
//				case "Fruit":  DataCollections.bag1_Fruit = (Fruit)e.Value;  break;
//				case "Picture":  DataCollections.bag1_Picture = (Image)e.Value;  break;
//			}
//		}
//
//		private void DataCollections.bag2_GetValue(object sender, PropertySpecEventArgs e)
//		{
//			switch(e.Property.Name)
//			{
//				case "Fruit":  e.Value = DataCollections.bag2_Fruit;  break;
//				case "Typeface":  e.Value = DataCollections.bag2_Typeface;  break;
//				case "Some Boolean":  e.Value = DataCollections.bag2_SomeBoolean;  break;
//			}
//		}
//
//		private void DataCollections.bag2_SetValue(object sender, PropertySpecEventArgs e)
//		{
//			switch(e.Property.Name)
//			{
//				case "Fruit":  DataCollections.bag2_Fruit = (Fruit)e.Value;  break;
//				case "Typeface":  DataCollections.bag2_Typeface = (Font)e.Value;  break;
//				case "Some Boolean":  DataCollections.bag2_SomeBoolean = (bool)e.Value;  break;
//			}
//		}

		private void button_propgridshow_Click(object sender, System.EventArgs e)
		{
			//if (this.m_PropGridDialog)
			{
				this.m_PropGridDialog.Show();
			}
		}
	}
}
