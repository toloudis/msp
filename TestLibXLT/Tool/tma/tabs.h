#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace tmaTest
{
	/// <summary>
	/// Summary for tabs
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class tabs : public System::Windows::Forms::Form
	{
	public:
		tabs(void)
		{
			InitializeComponent();
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
	private: System::Windows::Forms::TabControl *  tabControl1;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container* components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tabControl1 = new System::Windows::Forms::TabControl();
			this->SuspendLayout();
			//
			// tabControl1
			//
			this->tabControl1->Location = System::Drawing::Point(8, 8);
			this->tabControl1->Name = S"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(296, 280);
			this->tabControl1->TabIndex = 0;
			//
			// tabs
			//
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(312, 293);
			this->Controls->Add(this->tabControl1);
			this->Name = S"tabs";
			this->Text = S"tabs";
			this->ResumeLayout(false);

		}
	};
}