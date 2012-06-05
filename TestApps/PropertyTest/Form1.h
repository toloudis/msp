#pragma once

#include "PropertyDialog.h"
#include "DataCollections.h"

#ifndef XXX_BASEDATA_HPP
#include "xxxBaseData.hpp"
#endif
#ifndef XXX_BASEDATAPROPERTYTABLE_H
#include "xxxBaseDataPropertyTable.h"
#endif
#ifndef XXX_OBJECT_HPP
#include "xxxObject.hpp"
#endif

#ifndef ZZZ_BASEDATA_HPP
#include "zzzBaseData.hpp"
#endif
#ifndef ZZZ_BASEDATAPROPERTYBAG_H
#include "zzzBaseDataPropertyBag.h"
#endif
#ifndef ZZZ_OBJECT_HPP
#include "zzzObject.hpp"
#endif


namespace PropertyTest
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace XLT::Windows::Forms;

	/// <summary> 
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class Form1 : public System::Windows::Forms::Form
	{	
	public:
		static PropertyDialog* m_PropertyDialog = NULL;
		Form1(void)
		{
			InitializeComponent();

			DataCollections::Init();	// set up the actual data

			//
			if (m_PropertyDialog == NULL)
			{
				m_PropertyDialog = new PropertyDialog();
				m_PropertyDialog->Show();
				int x,y;
				x = this->Location.X;
				y = this->Size.Height + this->Location.Y;
				m_PropertyDialog->Location = System::Drawing::Point(x,y);
			}

			//
			m_pZZZObject = new zzzObject();
			ListViewItem* item1 = new ListViewItem("zzzData", 0);
			item1->Tag = new zzzBaseDataBag( m_pZZZObject->m_basedata );
			this->listView_objects->Items->Add(item1);

			m_pXXXObject = new xxxObject();
			ListViewItem* item2 = new ListViewItem("xxxData", 0);
			item2->Tag = new xxxBaseDataTable( m_pXXXObject->m_basedata );
			this->listView_objects->Items->Add(item2);

			ListViewItem* item3 = new ListViewItem("Table", 0);
			item3->Tag = DataCollections::bag3;
			this->listView_objects->Items->Add(item3);

			ListViewItem* item4 = new ListViewItem("Table 2", 0);
			item4->Tag = DataCollections::bag4;
			this->listView_objects->Items->Add(item4);
		}
  
	protected:
		void Dispose(Boolean disposing)
		{
			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}
	private: System::Windows::Forms::Button *  button_propgridshow;
	private: System::Windows::Forms::Label *  label_desc;
	private: System::Windows::Forms::ListView *  listView_objects;

	private: xxxObject* m_pXXXObject;
	private: zzzObject* m_pZZZObject;
	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container * components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->button_propgridshow = new System::Windows::Forms::Button();
			this->label_desc = new System::Windows::Forms::Label();
			this->listView_objects = new System::Windows::Forms::ListView();
			this->SuspendLayout();
			// 
			// button_propgridshow
			// 
			this->button_propgridshow->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right);
			this->button_propgridshow->Location = System::Drawing::Point(256, 48);
			this->button_propgridshow->Name = S"button_propgridshow";
			this->button_propgridshow->Size = System::Drawing::Size(136, 24);
			this->button_propgridshow->TabIndex = 6;
			this->button_propgridshow->Text = S"Show PropertyGrid";
			this->button_propgridshow->Click += new System::EventHandler(this, button_propgridshow_Click);
			// 
			// label_desc
			// 
			this->label_desc->Location = System::Drawing::Point(8, 8);
			this->label_desc->Name = S"label_desc";
			this->label_desc->Size = System::Drawing::Size(448, 23);
			this->label_desc->TabIndex = 5;
			this->label_desc->Text = S"Select one or multiple items in the list to see those objects in the property gri" 
				S"d.";
			// 
			// listView_objects
			// 
			this->listView_objects->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->listView_objects->HideSelection = false;
			this->listView_objects->Location = System::Drawing::Point(8, 32);
			this->listView_objects->Name = S"listView_objects";
			this->listView_objects->Size = System::Drawing::Size(240, 256);
			this->listView_objects->TabIndex = 4;
			this->listView_objects->SelectedIndexChanged += new System::EventHandler(this, listView_SelectedIndexChanged);
			// 
			// Form1
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(400, 302);
			this->Controls->Add(this->button_propgridshow);
			this->Controls->Add(this->label_desc);
			this->Controls->Add(this->listView_objects);
			this->Name = S"Form1";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = S"Test PropertyGrid";
			this->ResumeLayout(false);

		}	
	private: System::Void button_propgridshow_Click(System::Object *  sender, System::EventArgs *  e)
			 {
				if (this->m_PropertyDialog != NULL)
				{
					this->m_PropertyDialog->Show();
				}

				//	can be used to test the data structure got changed.
				//
				//xxxBaseData& xdata = m_pXXXObject->m_basedata;
				//zzzBaseData& zdata = m_pZZZObject->m_basedata;
			 }

	private: System::Void listView_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e)
			 {
				ArrayList* objs = new ArrayList();
				ListViewItem* pItem = NULL;
				for (int i = 0 ; i < listView_objects->SelectedItems->Count ; ++i)
				{
					pItem = listView_objects->SelectedItems->get_Item(i);
					objs->Add( pItem->Tag );
				}

				if (pItem != NULL)
				{
					m_PropertyDialog->propertyGrid_data->SelectedObjects = objs->ToArray();
				}
			 }

	};
}


