#pragma once

#ifndef CMRA_DRIVERDATASUBJECTINFO_HPP
#include "Systems/Cameras/Drivers/cmraDriverDataSubjectInfo.hpp"
#endif
#ifndef CMRA_OBJECTMGR_HPP
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"
#endif

#ifndef DBG_LOG_HPP
#include "Core/Dbg/dbgLog.hpp"
#endif
#ifndef NAME_MGR_HPP
#include "Core/name/nameMgr.hpp"
#endif
#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef NAME_TYPES_HPP
#include "Core/name/nameTypes.hpp"
#endif
#ifndef TMA_MANAGEDCONVERSIONUTIL_HPP
#include "ToolUIManaged/tma/tmaManagedConversionUtil.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif

#include "Systems/Cameras/Timeline/cmraRefNameForm.h"

#include <algorithm>

#ifdef _MANAGED


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace StudioFramework
{
	/// <summary> 
	/// Summary for cmraDriverDataSubjectForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class cmraDriverDataSubjectForm : public System::Windows::Forms::Form
	{
	public: 
		static cmraDriverDataSubjectForm ^FormInstance = nullptr;
	public: 
		cmraDriverDataSubjectForm(cmraDriverDataSubjectInfo &i_Data)
			: m_Data(i_Data)
		{
			m_bDisableNotify = true;

			InitializeComponent();

			FormInstance = this;

			SetUpComponents();

			m_bDisableNotify = false;
		}

		void UpdateForm()
		{
			SetUpComponents();
			this->Invalidate();
		}
        
	protected: 
		~cmraDriverDataSubjectForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private: cmraDriverDataSubjectInfo &m_Data;
	private: bool m_bDisableNotify;

	private: System::Windows::Forms::TabControl ^  tabControl_target;
	private: System::Windows::Forms::TabPage ^  tabPage_camerasubject;

	private: System::Windows::Forms::Label ^  label_targetobject;
	private: System::Windows::Forms::CheckedListBox ^  checkedListBox_targetlist;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->label_targetobject = gcnew System::Windows::Forms::Label();
			this->tabControl_target = gcnew System::Windows::Forms::TabControl();
			this->tabPage_camerasubject = gcnew System::Windows::Forms::TabPage();
			this->checkedListBox_targetlist = gcnew System::Windows::Forms::CheckedListBox();
			this->tabControl_target->SuspendLayout();
			this->tabPage_camerasubject->SuspendLayout();
			this->SuspendLayout();
			// 
			// label_targetobject
			// 
			this->label_targetobject->Location = System::Drawing::Point(8, 8);
			this->label_targetobject->Name = "label_targetobject";
			this->label_targetobject->Size = System::Drawing::Size(80, 16);
			this->label_targetobject->TabIndex = 0;
			this->label_targetobject->Text = "Target List:";
			// 
			// tabControl_target
			// 
			this->tabControl_target->Controls->Add(this->tabPage_camerasubject);
			this->tabControl_target->Location = System::Drawing::Point(8, 8);
			this->tabControl_target->Name = "tabControl_target";
			this->tabControl_target->SelectedIndex = 0;
			this->tabControl_target->Size = System::Drawing::Size(424, 256);
			this->tabControl_target->TabIndex = 7;
			// 
			// tabPage_camerasubject
			// 
			this->tabPage_camerasubject->Controls->Add(this->checkedListBox_targetlist);
			this->tabPage_camerasubject->Controls->Add(this->label_targetobject);
			this->tabPage_camerasubject->Location = System::Drawing::Point(4, 22);
			this->tabPage_camerasubject->Name = "tabPage_camerasubject";
			this->tabPage_camerasubject->Size = System::Drawing::Size(416, 230);
			this->tabPage_camerasubject->TabIndex = 0;
			this->tabPage_camerasubject->Text = "Subjects";
			this->tabPage_camerasubject->ToolTipText = "Camera Focus Subjects";
			// 
			// checkedListBox_targetlist
			// 
			this->checkedListBox_targetlist->CheckOnClick = true;
			this->checkedListBox_targetlist->Location = System::Drawing::Point(8, 24);
			this->checkedListBox_targetlist->Name = "checkedListBox_targetlist";
			this->checkedListBox_targetlist->Size = System::Drawing::Size(400, 199);
			this->checkedListBox_targetlist->TabIndex = 0;
			this->checkedListBox_targetlist->ItemCheck += gcnew System::Windows::Forms::ItemCheckEventHandler(this, &cmraDriverDataSubjectForm::checkedListBox_targetlist_ItemCheck);
			// 
			// cmraDriverDataSubjectForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(440, 269);
			this->Controls->Add(this->tabControl_target);
			this->Name = "cmraDriverDataSubjectForm";
			this->Text = "Camera Subject Properties";
			this->TopMost = true;
			this->tabControl_target->ResumeLayout(false);
			this->tabPage_camerasubject->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
		//

private: System::Void rebuild_list( int i_ChangedIndex )
		 {
			m_Data.m_ObjectNames.clear();

			int size = checkedListBox_targetlist->Items->Count;
			m_Data.m_ObjectNames.resize( size );

			//DBG_LOG1( "Rebuilding List (size = %d)", size );

			std::string name;
			int i;
			int count = 0;
			for ( i = 0 ; i < size ; i++ )
			{
				if (   ((i != i_ChangedIndex) && (checkedListBox_targetlist->GetItemChecked(i)))
					|| ((i == i_ChangedIndex) && (!checkedListBox_targetlist->GetItemChecked( i_ChangedIndex ))) )
				{
					tmaManagedStringUtils::ManagedStringToStdString( checkedListBox_targetlist->Items[i]->ToString(), name );
					m_Data.m_ObjectNames[count++].SetString(name);

					//checkedListBox_targetlist->SetItemChecked(i, true);

					//DBG_LOG3( "  %02d)+%s (%02d)", i, name.c_str(), count );
				}
				else
				{
					tmaManagedStringUtils::ManagedStringToStdString( checkedListBox_targetlist->Items[i]->ToString(), name );
					//DBG_LOG2( "  %02d) %s", i, name.c_str() );
				}
			}

			m_Data.m_ObjectNames.resize( count );
			//DBG_LOG1( "objects rebuilt %d", count );
		 }

private: System::Void checkedListBox_targetlist_ItemCheck(System::Object ^  sender, System::Windows::Forms::ItemCheckEventArgs ^  e)
		 {
			 if (!m_bDisableNotify)
			 {
				 m_bDisableNotify = true;
				//if (e->NewValue == CheckState::Checked)
				//{
				//	int index = m_Data.m_ObjectNames.size();

				//	m_Data.m_ObjectNames.resize( index + 1 );
				//	std::string name;
				//	tmaManagedStringUtils::ManagedStringToStdString( checkedListBox_targetlist->Items->Item[e->Index]->ToString(), name );

				//	DBG_LOG2( "new checked object size=%d (name %s)", index, name.c_str() );

				//	m_Data.m_ObjectNames[index] = nameMgr::GetObjectByName( nameString( name ) );
				//}
				//else
				{
					//DBG_LOG0( "item unchecked rebuilding list" );

					rebuild_list( e->Index );
				}

				//DBG_LOG0( "data object list" );
				//int size = m_Data.m_ObjectNames.size();
				//for ( int j = 0 ; j < size ; j++ )
				//{
				//	DBG_LOG2( " %02d - (%s)", j, m_Data.m_ObjectNames[j].GetString().c_str() );
				//}
				m_bDisableNotify = false;
			}
		 }

private:
		//
		void SetUpComponents()
		{
			//	build the object list
			//
			int i;
			nameList aNameList;
			nameMgr::GetNameList( aNameList );

			int namelist_size = aNameList.size();
			for ( i = 0 ; i < namelist_size ; i++ )
			{
				checkedListBox_targetlist->Items->Add( gcnew String(aNameList[i]->GetString().c_str()) );
			}

			//	check the appropriate items in the list
			//
			int size = m_Data.m_ObjectNames.size();
			for ( int j = 0 ; j < size ; j++ )
			{
				DBG_LOG2( "item %d - (%s)", j, m_Data.m_ObjectNames[j].GetString().c_str() );
				for ( i = 0 ; i < namelist_size ; i++ )
				{
					std::string tname;
					tmaManagedStringUtils::ManagedStringToStdString( checkedListBox_targetlist->Items[i]->ToString(), tname );
					DBG_LOG2( "    %d (%s)", i, tname.c_str() );
					if ( m_Data.m_ObjectNames[j].GetString() == tname )
					{
						checkedListBox_targetlist->SetItemChecked(i, true);
						break;
					}
				}
			}
		}

	public:
		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_camerasubject;
		}

};
}
#endif // _MANAGED
