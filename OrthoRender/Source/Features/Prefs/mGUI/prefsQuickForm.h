/*****************************************************************************
**	prefsQuickForm.h
**
**		Form for the quick commands
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifndef CMA_COMMANDMGR_HPP
#include "Tool/cma/cmaCommandMgr.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif


//============================================================================
//============================================================================
#ifdef _MANAGED

public ref class QuickForm : public System::Windows::Forms::Form
{
public:
	QuickForm()
	:	Form()
	{
		InitializeComponents();
	}

	void BuildButtons(const prefsQuickData& i_Data, const int i_ScreenX, const int i_ScreenY)
	{
		const int lc_BUTTON_WIDTH = 74;
		const int lc_BUTTON_HEIGHT = 38; //23;
		const int lc_BUTTON_GAP = 6;
		const int lc_BUTTON_WIDTH_INC = lc_BUTTON_WIDTH + lc_BUTTON_GAP;
		const int lc_BUTTON_HEIGHT_INC = lc_BUTTON_HEIGHT + lc_BUTTON_GAP;

		//DBG_LOG0("-----Creating Quick Commands-----");
		//DBG_LOG3("user pressed @ (%d,%d) commands=%d", i_ScreenX, i_ScreenY, i_Data.m_Commands.GetNumberOfItems());

		//	build the buttons
		//
		int btncnt = 1;

		this->SuspendLayout();
		for (int j = 0; j < i_Data.m_Commands.GetNumberOfItems(); ++j)
		{
			if (i_Data.m_Commands.GetValueFlag(j) == true)
			{
				System::Windows::Forms::Button^ pButton = gcnew System::Windows::Forms::Button();

				pButton->BackColor = System::Drawing::SystemColors::Control;
				pButton->FlatAppearance->BorderColor = System::Drawing::SystemColors::ControlText;
				pButton->FlatAppearance->BorderSize = 1;
				pButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
				pButton->ForeColor = System::Drawing::SystemColors::ControlText;
				pButton->Name = System::String::Format("button{0}", btncnt);
				pButton->Size = System::Drawing::Size(lc_BUTTON_WIDTH, lc_BUTTON_HEIGHT);
				pButton->TabIndex = j;
				pButton->Text = System::String::Format("{0}", gcnew System::String(i_Data.m_Commands.GetValueText(j).c_str()));
				pButton->UseVisualStyleBackColor = false;
				pButton->Margin = System::Windows::Forms::Padding(1);
				pButton->Location = System::Drawing::Point(0,0);
				pButton->Click += gcnew System::EventHandler(this, &QuickForm::anybutton_click);

				this->Controls->Add(pButton);
				++btncnt;
			}
		}

		// big hack formula!
		int formsizex = (((btncnt/24) > 0) ? 7 : ((btncnt/8) > 0) ? 5 : 3) * (lc_BUTTON_WIDTH_INC);
		int formsizey = (((btncnt/24) > 0) ? 7 : ((btncnt/8) > 0) ? 5 : 3) * (lc_BUTTON_HEIGHT_INC);
		int formlocx = i_ScreenX - (formsizex/2);
		int formlocy = i_ScreenY - (formsizey/2);
		//DBG_LOG5("client size(%d,%d) loc(%d,%d) buttons=%d", formsizex, formsizey, formlocx, formlocy, btncnt);

		//	adjust the screen according to the number of buttons
		this->ClientSize = System::Drawing::Size(formsizex, formsizey);
		this->Location = System::Drawing::Point(formlocx, formlocy);

		//	calculate the button and form sizes/locations
		//
		int section = 0;
		int circle = 1;
		int count = 1;
		int circle_button_count = 1;
		System::Windows::Forms::Button^ pControl;
		System::Collections::IEnumerator^ myEnumerator = this->Controls->GetEnumerator();
		while ( myEnumerator->MoveNext() )
		{
			pControl = dynamic_cast<System::Windows::Forms::Button^>(myEnumerator->Current);

			if (pControl != nullptr)
			{
				int btns_across = (circle * 2 + 1);
				int btns_down = (circle * 2 + 1);
				int startx = (formsizex/2) - (((float)btns_across / 2.0f) * (float)lc_BUTTON_WIDTH  + lc_BUTTON_GAP * (btns_across/2));
				int starty = (formsizey/2) - (((float)btns_down   / 2.0f) * (float)lc_BUTTON_HEIGHT + lc_BUTTON_GAP * (btns_across/2));

				int x, y;
				//int btncircle_num = count - (circle-1)*(((circle-1) * 2 + 1)*2+((circle-1) * 2 - 2)*2);
				if (section == 0)		// top row across
				{
					x = startx + (circle_button_count-1) * ((float)(lc_BUTTON_WIDTH_INC));
					y = starty;

					if (circle_button_count == btns_across)
						section = 1;
				}
				else if (section == 1)	// right column down
				{
					//x is already set from section 0
					y += ((float)(lc_BUTTON_HEIGHT_INC));

					if (circle_button_count == btns_across*2-1)
						section = 2;
				}
				else if (section == 2)	// bottom row across
				{
					//x -= startx + (count - (btns_across*3 - 2)) * ((float)(lc_BUTTON_WIDTH_INC));
					x -= ((float)(lc_BUTTON_WIDTH_INC));
					//y is already set from section 1

					if (circle_button_count == btns_across*3-2)
						section = 3;
				}
				else	// left column up
				{
					//x is already set from section 2
					y -= ((float)(lc_BUTTON_HEIGHT_INC));

					if (circle_button_count == btns_across*4-4)
					{
						++circle;
						section = 0;
						circle_button_count = 0;
					}
				}

				//DBG_LOG9("    circle=%d cnt=%02d (%03d,%03d) btns(%d-%d) sec=%1d start(%03d,%03d)", circle, count, x, y, btns_across, btns_down, section, startx, starty);
				pControl->Location = System::Drawing::Point( x, y );

				++count;
				++circle_button_count;
			}
		}

		this->ResumeLayout(false);
	}

protected:
	/// <summary>
	/// Clean up any resources being used.
	/// </summary>
	~QuickForm()
	{
		if (components)
		{
			delete components;
		}
	}

private:
	/// <summary>
	/// Required designer variable.
	/// </summary>
	System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
	/// <summary>
	/// Required method for Designer support - do not modify
	/// the contents of this method with the code editor.
	/// </summary>
	void InitializeComponents()
	{
		this->SuspendLayout();
		// 
		// QuickForm
		// 
		this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
		this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		this->BackColor = System::Drawing::SystemColors::ControlDark;
		this->ControlBox = false;
		this->ForeColor = System::Drawing::SystemColors::Control;
		this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
		this->KeyPreview = true;
		this->Name = L"QuickCommandsForm";
		this->ShowIcon = false;
		this->ShowInTaskbar = false;
		this->Text = L"QuickCommands";
		this->TopMost = true;
		this->StartPosition = System::Windows::Forms::FormStartPosition::Manual;

		//this->PreviewKeyDown += gcnew System::Windows::Forms::PreviewKeyDownEventHandler(this, &QuickForm::QuickForm_PreviewKeyDown);
		//this->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &QuickForm::QuickForm_KeyPress);
		//this->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &QuickForm::QuickForm_KeyUp);
		this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &QuickForm::QuickForm_KeyDown);
		this->Click += gcnew System::EventHandler(this, &QuickForm::QuickForm_Click);
		this->ResumeLayout(false);
	}
#pragma endregion

protected:	virtual void OnPaintBackground(System::Windows::Forms::PaintEventArgs^ e) override
			{
			}
private: System::Void QuickForm_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) 
		 {
			 if (e->KeyCode == System::Windows::Forms::Keys::Escape)
			 {
				 this->Close();
			 }
		 }
private: System::Void QuickForm_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 this->Close();
		 }
private: System::Void anybutton_click(System::Object^  sender, System::EventArgs^  e) 
	 {
		// Immediately hide the control and let the command execute
		this->Hide();

		//	execute
		std::string tag;
		tmaManagedStringUtils::ManagedStringToStdString( dynamic_cast<System::Windows::Forms::Button^>(sender)->Text, tag );
		cmaCommandMgr::ExecuteCommand(tag, -1);

		//	close this form
		this->Close();
	 }

};

#endif

