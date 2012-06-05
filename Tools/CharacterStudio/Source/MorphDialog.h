#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#ifndef CHR_LEVEL_HPP
#include "chrLevel.hpp"
#endif

namespace CharacterStudio
{
	/// <summary> 
	/// Summary for MorphDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class MorphDialog : public System::Windows::Forms::Form
	{
	public: 
		static MorphDialog *FormInstance = 0;

		MorphDialog(void) : m_NumItems(0)
		{
			InitializeComponent();
		}
        
		void AddMorphTarget(const char* i_Name)
		{
			int y_pos = m_NumItems * 32 + 8;

			System::Windows::Forms::Label *  label;
			TerawattManagedControls::RangedFloat *  rangedFloat;
			label = new System::Windows::Forms::Label();
			rangedFloat = new TerawattManagedControls::RangedFloat();

			// 
			// label
			// 
			label->Location = System::Drawing::Point(8, y_pos);
			label->Name = S"Label";
			label->Size = System::Drawing::Size(88, 24);
			label->TabIndex = 0;
			label->Text = i_Name;
			// 
			// rangedFloat
			// 
			rangedFloat->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			rangedFloat->Exponent = (System::Int16)1;
			rangedFloat->Location = System::Drawing::Point(104, y_pos);
			rangedFloat->Name = __box(m_NumItems)->ToString();
			rangedFloat->Precision = (System::Int16)2;
			rangedFloat->Size = System::Drawing::Size(304, 24);
			rangedFloat->ValueChanged += new System::EventHandler(this, &CharacterStudio::MorphDialog::rangedFloat_ValueChanged);

			this->panel1->Controls->Add(rangedFloat);
			this->panel1->Controls->Add(label);

			m_NumItems++;
			this->panel1->AutoScrollMinSize.Height = m_NumItems * 32 + 8;
		}

	protected: 
		void Dispose(Boolean disposing)
		{
			// clear instance
			if (disposing && MorphDialog::FormInstance == this)
				MorphDialog::FormInstance = 0;

			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}
	private: System::Windows::Forms::Panel *  panel1;


	private: int m_NumItems;

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
			this->panel1 = new System::Windows::Forms::Panel();
			this->SuspendLayout();
			// 
			// panel1
			// 
			this->panel1->AutoScroll = true;
			this->panel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panel1->Location = System::Drawing::Point(0, 0);
			this->panel1->Name = S"panel1";
			this->panel1->Size = System::Drawing::Size(424, 267);
			this->panel1->TabIndex = 0;
			// 
			// MorphDialog
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(6, 15);
			this->ClientSize = System::Drawing::Size(424, 267);
			this->ControlBox = false;
			this->Controls->Add(this->panel1);
			this->Name = S"MorphDialog";
			this->Text = S"Morph Targets";
			this->ResumeLayout(false);

		}		

		//

		System::Void rangedFloat_ValueChanged(System::Object *  sender, System::EventArgs *  e)
		{
			TerawattManagedControls::RangedFloat* rangedFloat = __try_cast<TerawattManagedControls::RangedFloat *>(sender);
			int index = System::Int32::Parse(rangedFloat->Name);
			if (index >= 0)
				chrLevel::SetMorphTargetWeight(index, (float)rangedFloat->Value);
		}
	};
}