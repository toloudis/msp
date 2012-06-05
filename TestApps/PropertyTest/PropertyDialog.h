#pragma once

using namespace System;
//using namespace System::ComponentModel;
//using namespace System::Collections;
//using namespace System::Windows::Forms;
//using namespace System::Data;
//using namespace System::Drawing;
//

namespace PropertyTest
{
	/// <summary> 
	/// Summary for PropertyDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class PropertyDialog : public System::Windows::Forms::Form
	{
	public: 
		PropertyDialog(void)
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
	public: System::Windows::Forms::PropertyGrid *  propertyGrid_data;

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
			this->propertyGrid_data = new System::Windows::Forms::PropertyGrid();
			this->SuspendLayout();
			// 
			// propertyGrid_data
			// 
			this->propertyGrid_data->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->propertyGrid_data->CommandsVisibleIfAvailable = true;
			this->propertyGrid_data->LargeButtons = false;
			this->propertyGrid_data->LineColor = System::Drawing::SystemColors::ScrollBar;
			this->propertyGrid_data->Location = System::Drawing::Point(8, 8);
			this->propertyGrid_data->Name = S"propertyGrid_data";
			this->propertyGrid_data->Size = System::Drawing::Size(352, 300);
			this->propertyGrid_data->TabIndex = 0;
			this->propertyGrid_data->Text = S"PropertyGrid";
			this->propertyGrid_data->ViewBackColor = System::Drawing::SystemColors::Window;
			this->propertyGrid_data->ViewForeColor = System::Drawing::SystemColors::WindowText;
			// 
			// PropertyDialog
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(368, 318);
			this->Controls->Add(this->propertyGrid_data);
			this->Name = S"PropertyDialog";
			this->Text = S"PropertyDialog";
			this->ResumeLayout(false);

		}
	};
}