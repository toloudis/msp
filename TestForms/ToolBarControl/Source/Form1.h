#pragma once


namespace Source
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

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
		Form1(void)
		{
			InitializeComponent();

			CreateButtons( toolBar1,new System::String("..\\test.bmp"),0,0 );
			CreateButtons( toolBar2,new System::String("..\\test.png"),0,0 );
			CreateButtons( toolBar3,new System::String("..\\test.jpg"),10,7 );
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
	private: System::Windows::Forms::ToolBar *  toolBar1;
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::ToolBar *  toolBar2;
	private: System::Windows::Forms::Label *  label2;
	private: System::Windows::Forms::ToolBar *  toolBar3;
	private: System::Windows::Forms::Label *  label3;

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
			this->toolBar1 = new System::Windows::Forms::ToolBar();
			this->label1 = new System::Windows::Forms::Label();
			this->toolBar2 = new System::Windows::Forms::ToolBar();
			this->label2 = new System::Windows::Forms::Label();
			this->toolBar3 = new System::Windows::Forms::ToolBar();
			this->label3 = new System::Windows::Forms::Label();
			this->SuspendLayout();
			// 
			// toolBar1
			// 
			this->toolBar1->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->toolBar1->AutoSize = false;
			this->toolBar1->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->toolBar1->ButtonSize = System::Drawing::Size(48, 48);
			this->toolBar1->CausesValidation = false;
			this->toolBar1->Divider = false;
			this->toolBar1->Dock = System::Windows::Forms::DockStyle::None;
			this->toolBar1->DropDownArrows = true;
			this->toolBar1->Location = System::Drawing::Point(10, 20);
			this->toolBar1->Name = S"toolBar1";
			this->toolBar1->ShowToolTips = true;
			this->toolBar1->Size = System::Drawing::Size(353, 62);
			this->toolBar1->TabIndex = 0;
			this->toolBar1->TextAlign = System::Windows::Forms::ToolBarTextAlign::Right;
			this->toolBar1->Wrappable = false;
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(48, 88);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(288, 24);
			this->label1->TabIndex = 1;
			this->label1->Text = S"48x48 button with 48x48 image";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// toolBar2
			// 
			this->toolBar2->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->toolBar2->AutoSize = false;
			this->toolBar2->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->toolBar2->ButtonSize = System::Drawing::Size(59, 56);
			this->toolBar2->Divider = false;
			this->toolBar2->Dock = System::Windows::Forms::DockStyle::None;
			this->toolBar2->DropDownArrows = true;
			this->toolBar2->Location = System::Drawing::Point(8, 130);
			this->toolBar2->Name = S"toolBar2";
			this->toolBar2->ShowToolTips = true;
			this->toolBar2->Size = System::Drawing::Size(353, 80);
			this->toolBar2->TabIndex = 2;
			this->toolBar2->TextAlign = System::Windows::Forms::ToolBarTextAlign::Right;
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(48, 216);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(288, 24);
			this->label2->TabIndex = 3;
			this->label2->Text = S"bigger button to accomodate a 48x48 image";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// toolBar3
			// 
			this->toolBar3->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->toolBar3->AutoSize = false;
			this->toolBar3->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->toolBar3->ButtonSize = System::Drawing::Size(48, 48);
			this->toolBar3->Divider = false;
			this->toolBar3->Dock = System::Windows::Forms::DockStyle::None;
			this->toolBar3->DropDownArrows = true;
			this->toolBar3->Location = System::Drawing::Point(8, 256);
			this->toolBar3->Name = S"toolBar3";
			this->toolBar3->ShowToolTips = true;
			this->toolBar3->Size = System::Drawing::Size(353, 55);
			this->toolBar3->TabIndex = 4;
			this->toolBar3->TextAlign = System::Windows::Forms::ToolBarTextAlign::Right;
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(40, 320);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(296, 24);
			this->label3->TabIndex = 5;
			this->label3->Text = S"48x48 button with a 48x48 image that is shrunk";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// Form1
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(384, 397);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->toolBar3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->toolBar2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->toolBar1);
			this->Name = S"Form1";
			this->Text = S"Form1";
			this->ResumeLayout(false);

		}	


	
		//	Create a single button
		//
		ToolBarButton * CreateButton(ToolBar* pTB, System::String * pFilename, int width_border, int height_border )
		{
			ToolBarButton * pTBB = new ToolBarButton();

			pTBB->set_Style(ToolBarButtonStyle::PushButton);
			pTBB->Text = "Button Name";
			pTBB->ToolTipText = "ToolTip";

			Image * pImage = Image::FromFile( pFilename );

			//	set up the images
			//
			if ( pTB->ImageList == 0 )
			{
				pTB->ImageList = new ImageList();

				int w,h;
				w = pImage->get_Width() - width_border;
				h = pImage->get_Height() - height_border;
				pTB->ImageList->ImageSize = *(__nogc new System::Drawing::Size(w,h));
				Drawing::Size imagesize = pTB->ImageSize;
			}

			pTB->ImageList->Images->Add( pImage );

			// Assign ImageIndex property of the ToolBarButton.
			pTBB->ImageIndex = pTB->ImageList->Images->Count - 1;

			return pTBB;
		}

		//	Create a toolbar full of buttons
		void CreateButtons(ToolBar* pTB, System::String * pFilename, int width, int height)
		{
			// Add the ToolBarButton controls to the ToolBar.
			//
			pTB->Buttons->Add( CreateButton(pTB, pFilename, width, height));
			pTB->Buttons->Add( CreateButton(pTB, pFilename, width, height));
			pTB->Buttons->Add( CreateButton(pTB, pFilename, width, height));
			pTB->Buttons->Add( CreateButton(pTB, pFilename, width, height));

			Drawing::Size imagesize = pTB->ImageSize;
		}

};
}


