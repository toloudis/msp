#pragma once

#ifndef PROP_DRIVERAIMOVEBASE_HPP
#include "propDriverAIMoveBase.hpp"
#endif
#ifndef CHNL_DIALOGUTIL_HPP
#include "chnlDialogUtil.hpp"
#endif
#ifndef DBG_LOG_HPP
#include "dbgLog.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "tmaManagedStringUtils.hpp"
#endif


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


//============================================================================
//============================================================================
namespace SystemProp
{
	/// <summary>
	/// Summary for propDriverAIMoveBaseForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class propDriverAIMoveBaseForm : public System::Windows::Forms::Form
	{
	public:
		propDriverAIMoveBaseForm( propDriverAIMoveBase& i_Driver )
			: m_Driver(i_Driver)
		{
			m_bDisableNotify = true;

			InitializeComponent();

			m_bDisableNotify = false;
		}

	protected:
		~propDriverAIMoveBaseForm()
		{
			if (components)
			{
				delete components;
			}
		}



	private: propDriverAIMoveBase	&m_Driver;
	private: bool m_bDisableNotify;
	private: System::Windows::Forms::TabControl ^  tabControl_aimovebase;
	private: System::Windows::Forms::TabPage ^  tabPage_aimovebase;
	private: System::Windows::Forms::Label ^  label_nothing;

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
			this->tabControl_aimovebase = gcnew System::Windows::Forms::TabControl();
			this->tabPage_aimovebase = gcnew System::Windows::Forms::TabPage();
			this->label_nothing = gcnew System::Windows::Forms::Label();
			this->tabControl_aimovebase->SuspendLayout();
			this->tabPage_aimovebase->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl_aimovebase
			// 
			this->tabControl_aimovebase->Controls->Add(this->tabPage_aimovebase);
			this->tabControl_aimovebase->Location = System::Drawing::Point(8, 8);
			this->tabControl_aimovebase->Name = "tabControl_aimovebase";
			this->tabControl_aimovebase->SelectedIndex = 0;
			this->tabControl_aimovebase->Size = System::Drawing::Size(352, 328);
			this->tabControl_aimovebase->TabIndex = 2;
			// 
			// tabPage_aimovebase
			// 
			this->tabPage_aimovebase->Controls->Add(this->label_nothing);
			this->tabPage_aimovebase->Location = System::Drawing::Point(4, 22);
			this->tabPage_aimovebase->Name = "tabPage_aimovebase";
			this->tabPage_aimovebase->Size = System::Drawing::Size(344, 302);
			this->tabPage_aimovebase->TabIndex = 0;
			this->tabPage_aimovebase->Text = "AI Movement";
			// 
			// label_nothing
			// 
			this->label_nothing->Location = System::Drawing::Point(104, 40);
			this->label_nothing->Name = "label_nothing";
			this->label_nothing->Size = System::Drawing::Size(152, 23);
			this->label_nothing->TabIndex = 0;
			this->label_nothing->Text = "This tab has nothing";
			// 
			// propDriverAIMoveBaseForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(368, 341);
			this->Controls->Add(this->tabControl_aimovebase);
			this->Name = "propDriverAIMoveBaseForm";
			this->Text = "Prop Driver AI Movement Properties";
			this->TopMost = true;
			this->tabControl_aimovebase->ResumeLayout(false);
			this->tabPage_aimovebase->ResumeLayout(false);
			this->ResumeLayout(false);

		}

		//

	public:
		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_aimovebase;
		}

		//	add a tab page
		System::Void AddTabPage( System::Windows::Forms::TabPage^ i_pPage )
		{
			tabControl_aimovebase->Controls->Add( i_pPage );
		}
	};
}