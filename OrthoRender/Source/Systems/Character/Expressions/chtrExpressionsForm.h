#pragma once

#ifndef CHTR_OPERATIONS_HPP
#include "Systems/Character/Undo/chtrOperations.hpp"
#endif
#ifndef CHTR_EXPRESSIONDATA_HPP
#include "Systems/Character/Data/chtrExpressionData.hpp"
#endif

#ifdef _MANAGED


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace SystemCharacter
{
	/// <summary> 
	/// Summary for chtrExpressionsForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class chtrExpressionsForm : public System::Windows::Forms::Form
	{
	public:
		static chtrExpressionsForm^ FormInstance = nullptr;

		chtrExpressionsForm()
		{
			InitializeComponent();
		}

		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_Expressions;
		}

	protected: 
		~chtrExpressionsForm()
		{
			// clear instance
			if (chtrExpressionsForm::FormInstance == this)
				chtrExpressionsForm::FormInstance = nullptr;

			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::TabControl ^  tabControl_Expressions;
	private: System::Windows::Forms::TabPage ^  tabPage_Expressions;


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
			this->tabControl_Expressions = gcnew System::Windows::Forms::TabControl();
			this->tabPage_Expressions = gcnew System::Windows::Forms::TabPage();
			this->tabControl_Expressions->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl_Expressions
			// 
			this->tabControl_Expressions->Controls->Add(this->tabPage_Expressions);
			this->tabControl_Expressions->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabControl_Expressions->Location = System::Drawing::Point(0, 0);
			this->tabControl_Expressions->Name = "tabControl_Expressions";
			this->tabControl_Expressions->SelectedIndex = 0;
			this->tabControl_Expressions->Size = System::Drawing::Size(292, 266);
			this->tabControl_Expressions->TabIndex = 0;
			// 
			// tabPage_Expressions
			// 
			this->tabPage_Expressions->AutoScroll = true;
			this->tabPage_Expressions->Location = System::Drawing::Point(4, 22);
			this->tabPage_Expressions->Name = "tabPage_Expressions";
			this->tabPage_Expressions->Size = System::Drawing::Size(284, 240);
			this->tabPage_Expressions->TabIndex = 0;
			this->tabPage_Expressions->Text = "Expressions";
			// 
			// chtrExpressionsForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(292, 266);
			this->Controls->Add(this->tabControl_Expressions);
			this->Name = "chtrExpressionsForm";
			this->Text = "Expressions Form";
			this->tabControl_Expressions->ResumeLayout(false);
			this->ResumeLayout(false);

		}		


	
		
	};
}
#endif // _MANAGED
