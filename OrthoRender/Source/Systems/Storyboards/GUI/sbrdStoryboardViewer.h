#pragma once

#ifndef DBG_LOG_HPP
#include "Core/dbg/dbgLog.hpp"
#endif
#ifndef TMA_DIALOGMEMORY_HPP
#include "ToolUIManaged/tma/tmaDialogMemory.hpp"
#endif
#ifndef TMA_IMAGELIST_HPP
#include "ToolUIManaged/tma/tmaImageList.hpp"
#endif

#ifdef _MANAGED


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace SystemStoryboards
{
	/// <summary> 
	/// Summary for sbrdStoryboardViewer
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class sbrdStoryboardViewer : public System::Windows::Forms::Form
	{
	public: 
		sbrdStoryboardViewer(tmaImageList^  i_pImageList, System::Int32 i_Index)
		{
			m_pImageList = i_pImageList;
			m_Index = i_Index;
			if (m_Index < 0) 
				m_Index = 0;

			InitializeComponent();
			SetupComponents();

			m_bResized = false;	// do this before display_image() -- SetupComponents() triggers resize
			display_image();
		}

	protected: 
		~sbrdStoryboardViewer()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::Button ^  button_prev;
	private: System::Windows::Forms::Button ^  button_next;
	private: System::Windows::Forms::PictureBox ^  pictureBox_storyboard;

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
			this->pictureBox_storyboard = gcnew System::Windows::Forms::PictureBox();
			this->button_prev = gcnew System::Windows::Forms::Button();
			this->button_next = gcnew System::Windows::Forms::Button();
			this->SuspendLayout();
			// 
			// pictureBox_storyboard
			// 
			this->pictureBox_storyboard->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->pictureBox_storyboard->BackColor = System::Drawing::SystemColors::ControlDarkDark;
			this->pictureBox_storyboard->Location = System::Drawing::Point(0, 0);
			this->pictureBox_storyboard->Name = "pictureBox_storyboard";
			this->pictureBox_storyboard->Size = System::Drawing::Size(352, 211);
			this->pictureBox_storyboard->TabIndex = 0;
			this->pictureBox_storyboard->TabStop = false;
			// 
			// button_prev
			// 
			this->button_prev->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->button_prev->Location = System::Drawing::Point(156, 187);
			this->button_prev->Name = "button_prev";
			this->button_prev->Size = System::Drawing::Size(16, 16);
			this->button_prev->TabIndex = 1;
			this->button_prev->Text = "<";
			this->button_prev->Click += gcnew System::EventHandler(this, &sbrdStoryboardViewer::button_prev_Click);
			// 
			// button_next
			// 
			this->button_next->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->button_next->Location = System::Drawing::Point(180, 187);
			this->button_next->Name = "button_next";
			this->button_next->Size = System::Drawing::Size(16, 16);
			this->button_next->TabIndex = 2;
			this->button_next->Text = ">";
			this->button_next->Click += gcnew System::EventHandler(this, &sbrdStoryboardViewer::button_next_Click);
			// 
			// sbrdStoryboardViewer
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(352, 209);
			this->Controls->Add(this->button_next);
			this->Controls->Add(this->button_prev);
			this->Controls->Add(this->pictureBox_storyboard);
			this->Name = "sbrdStoryboardViewer";
			this->Text = "Storyboard Viewer";
			this->Resize += gcnew System::EventHandler(this, &sbrdStoryboardViewer::sbrdStoryboardViewer_Resize);
			this->ResumeLayout(false);

		}		


	private:
		//---------------------------------------------------------------------------
		//---------------------------------------------------------------------------
		void SetupComponents()
		{
			// Dialog memory remembers size, location, visiblity of dialog 
			m_pMemory = gcnew tmaDialogMemory( this );
		}

	private:
		//---------------------------------------------------------------------------
		//	data
		//---------------------------------------------------------------------------
		tmaDialogMemory^	m_pMemory;
		tmaImageList^		m_pImageList;
		System::Int32		m_Index;
		bool				m_bResized;

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	private: System::Void button_prev_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				if (m_Index > 0)
					--m_Index;

				display_image();
			 }

	private: System::Void button_next_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 if (m_Index < (m_pImageList->Count()-1))
					++m_Index;

				 display_image();
			 }

private:
	//---------------------------------------------------------------------------
	//	Display the storyboard
	//---------------------------------------------------------------------------
	void display_image()
	{
		System::Drawing::Image^ pImage = m_pImageList->Get(m_Index);

		if (!m_bResized)
		{
			this->pictureBox_storyboard->SizeMode = PictureBoxSizeMode::Normal;

			//this->pictureBox_storyboard->Size = pImage->Size;
			this->ClientSize = pImage->Size;
		}
		this->pictureBox_storyboard->Image = pImage;

		//DBG_LOG2("Storyboard VIEWER image size (%d, %d)", pImage->Width, pImage->Height);
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
private: System::Void sbrdStoryboardViewer_Resize(System::Object ^  sender, System::EventArgs ^  e)
		 {
			//	when the user resizes the view, stop resizing the viewer to the image size
			//
			m_bResized = true;
			this->pictureBox_storyboard->SizeMode = PictureBoxSizeMode::StretchImage;
		 }

};
}
#endif // _MANAGED
