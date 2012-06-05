#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::IO;

#include "EditExpressionDialog.h"
#include "GroupExpressionsDialog.h"

#ifndef CHR_LEVEL_HPP
#include "nonGUI/chrLevel.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif


namespace CharacterStudio
{
	
	enum ExpressionType
	{
		e_Single = 0,
		e_Pair,
		e_Four
	};

	/// <summary> 
	/// Summary for ExpressionsDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class ExpressionsDialog : public System::Windows::Forms::Form
	{
	public: 
		static ExpressionsDialog ^FormInstance = nullptr;

		ExpressionsDialog(void) : m_NumItems(0)
		{
			this->expressionGroups = gcnew ArrayList();

			InitializeComponent();

			this->FillExpressions();
		}
		
	private:
		void ReFillExpressions()
		{
			ClearExpressions();
			FillExpressions();
		}
		void ClearExpressions()
		{
			for (int i=0; i<expressionGroups->Count; i++)
			{
				ExpressionGroup ^group = safe_cast<ExpressionGroup^> (expressionGroups[i]);
				group->Detach();
			}
			expressionGroups->Clear();
			m_NumItems = 0;
		}
		void FillExpressions()
		{
			int nFours = chrLevel::GetNumGroupFours();
			if (nFours > 0)
			{
				for (int i=0; i<nFours; i++)
				{
					float weight1 = 0, weight2 = 0;
					chrLevel::GetGroupFourWeight(i, weight1, weight2);
					this->AddFourGroup(chrLevel::GetGroupFourName(i).c_str(), 
						weight1, weight2, i);
				}
			}
			int nPairs = chrLevel::GetNumGroupPairs();
			if (nPairs > 0)
			{
				for (int i=0; i<nPairs; i++)
					this->AddExpression(chrLevel::GetGroupPairName(i).c_str(), 
						chrLevel::GetGroupPairWeight(i), i, true);
			}
			int nTargets = chrLevel::GetNumExpressions();
			if (nTargets > 0)
			{
				for (int i=0; i<nTargets; i++)
				{
					if (!chrLevel::IsExpressionGrouped(i))
						this->AddExpression(chrLevel::GetExpressionName(i).c_str(), 
							chrLevel::GetExpressionWeight(i), i, false);
				}
			}
		}
		void AddExpression(const char* i_Name, float i_Value, int i_Index, bool i_bPair)
		{
			int y_pos = m_NumItems * 32 + 8 + m_NumFours * 64;
			ExpressionGroup ^group = gcnew ExpressionGroup(i_Name, i_Value, 0, this->panel1, 
				i_bPair ? e_Pair : e_Single, 
				i_Index, y_pos);
			expressionGroups->Add(group);

			m_NumItems++;
			this->panel1->AutoScrollMinSize.Height = y_pos + 32 + 16;
		}
		void AddFourGroup(const char* i_Name, float i_Value1, float i_Value2, 
							int i_Index)
		{
			int y_pos = m_NumFours * 64 + 8;
			ExpressionGroup ^group = gcnew ExpressionGroup(i_Name, i_Value1, i_Value2,
					this->panel1, e_Four, i_Index, y_pos);
			expressionGroups->Add(group);

			m_NumFours++;
			this->panel1->AutoScrollMinSize.Height = m_NumFours * 64 + 16;
		}

	protected: 
		~ExpressionsDialog()
		{
			// clear instance
			if (ExpressionsDialog::FormInstance == this)
				ExpressionsDialog::FormInstance = nullptr;

			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button ^  buttonCreateExpression;

	private: System::Collections::ArrayList ^ expressionGroups;
	private: int m_NumItems;
	private: int m_NumFours;
	private: System::Windows::Forms::Panel ^  panel1;
	private: System::Windows::Forms::Button ^  buttonDeleteExpression;
	private: System::Windows::Forms::Button ^  buttonEditExpression;
	private: System::Windows::Forms::Button ^  buttonLoadMultiple;
	private: System::Windows::Forms::Button^  buttonGroupExpressions;
	private: System::Windows::Forms::Button^  buttonUngroup;



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
			this->buttonCreateExpression = (gcnew System::Windows::Forms::Button());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->buttonDeleteExpression = (gcnew System::Windows::Forms::Button());
			this->buttonEditExpression = (gcnew System::Windows::Forms::Button());
			this->buttonLoadMultiple = (gcnew System::Windows::Forms::Button());
			this->buttonGroupExpressions = (gcnew System::Windows::Forms::Button());
			this->buttonUngroup = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// buttonCreateExpression
			// 
			this->buttonCreateExpression->Location = System::Drawing::Point(16, 7);
			this->buttonCreateExpression->Name = L"buttonCreateExpression";
			this->buttonCreateExpression->Size = System::Drawing::Size(120, 28);
			this->buttonCreateExpression->TabIndex = 0;
			this->buttonCreateExpression->Text = L"Create Expression...";
			this->buttonCreateExpression->Click += gcnew System::EventHandler(this, &ExpressionsDialog::buttonCreateExpression_Click);
			// 
			// panel1
			// 
			this->panel1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->panel1->AutoScroll = true;
			this->panel1->Location = System::Drawing::Point(7, 42);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(572, 197);
			this->panel1->TabIndex = 1;
			// 
			// buttonDeleteExpression
			// 
			this->buttonDeleteExpression->Location = System::Drawing::Point(328, 8);
			this->buttonDeleteExpression->Name = L"buttonDeleteExpression";
			this->buttonDeleteExpression->Size = System::Drawing::Size(64, 28);
			this->buttonDeleteExpression->TabIndex = 2;
			this->buttonDeleteExpression->Text = L"Delete";
			this->buttonDeleteExpression->Click += gcnew System::EventHandler(this, &ExpressionsDialog::buttonDeleteExpression_Click);
			// 
			// buttonEditExpression
			// 
			this->buttonEditExpression->Location = System::Drawing::Point(256, 8);
			this->buttonEditExpression->Name = L"buttonEditExpression";
			this->buttonEditExpression->Size = System::Drawing::Size(64, 28);
			this->buttonEditExpression->TabIndex = 3;
			this->buttonEditExpression->Text = L"Edit...";
			this->buttonEditExpression->Click += gcnew System::EventHandler(this, &ExpressionsDialog::buttonEditExpression_Click);
			// 
			// buttonLoadMultiple
			// 
			this->buttonLoadMultiple->Location = System::Drawing::Point(144, 8);
			this->buttonLoadMultiple->Name = L"buttonLoadMultiple";
			this->buttonLoadMultiple->Size = System::Drawing::Size(104, 28);
			this->buttonLoadMultiple->TabIndex = 4;
			this->buttonLoadMultiple->Text = L"Load Multiple...";
			this->buttonLoadMultiple->Click += gcnew System::EventHandler(this, &ExpressionsDialog::buttonLoadMultiple_Click);
			// 
			// buttonGroupExpressions
			// 
			this->buttonGroupExpressions->Location = System::Drawing::Point(398, 8);
			this->buttonGroupExpressions->Name = L"buttonGroupExpressions";
			this->buttonGroupExpressions->Size = System::Drawing::Size(64, 28);
			this->buttonGroupExpressions->TabIndex = 5;
			this->buttonGroupExpressions->Text = L"Group...";
			this->buttonGroupExpressions->Click += gcnew System::EventHandler(this, &ExpressionsDialog::buttonGroupExpressions_Click);
			// 
			// buttonUngroup
			// 
			this->buttonUngroup->Location = System::Drawing::Point(468, 8);
			this->buttonUngroup->Name = L"buttonUngroup";
			this->buttonUngroup->Size = System::Drawing::Size(64, 28);
			this->buttonUngroup->TabIndex = 6;
			this->buttonUngroup->Text = L"Ungroup";
			this->buttonUngroup->Click += gcnew System::EventHandler(this, &ExpressionsDialog::buttonUngroup_Click);
			// 
			// ExpressionsDialog
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(582, 241);
			this->Controls->Add(this->buttonUngroup);
			this->Controls->Add(this->buttonGroupExpressions);
			this->Controls->Add(this->buttonLoadMultiple);
			this->Controls->Add(this->buttonEditExpression);
			this->Controls->Add(this->buttonDeleteExpression);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->buttonCreateExpression);
			this->Name = L"ExpressionsDialog";
			this->Text = L"Expressions";
			this->ResumeLayout(false);

		}		

		///

		// private class for handling a group of controls for an expression
		ref class ExpressionGroup
		{
		public:

			ExpressionGroup(const char* i_Name, 
							float i_Value1,
							float i_Value2,
							System::Windows::Forms::Panel^ i_Panel,
							ExpressionType i_Type,
							int i_Number,
							int i_YPos)
			: m_Panel(i_Panel), m_bIsPair(i_Type == e_Pair), m_bIsFour(i_Type == e_Four), 
				rangedFloat(nullptr), multiRanged(nullptr)
			{
				label = gcnew System::Windows::Forms::Label();
				radioButton = gcnew System::Windows::Forms::RadioButton();

				// 
				// label
				// 
				label->Location = System::Drawing::Point(24, i_YPos);
				label->Name = "Label";
				label->Size = System::Drawing::Size(104, 24);
				label->TabIndex = 0;
				label->Text = gcnew System::String(i_Name);
				// 
				// radioButton
				// 
				radioButton->Location = System::Drawing::Point(8, i_YPos);
				radioButton->Name = "RadioButton";
				radioButton->Size = System::Drawing::Size(16, 16);
				radioButton->Text = "";
				if (i_Type == e_Four)
				{
					// 
					// multiRanged
					// 
					multiRanged = gcnew TerawattManagedControls::MultiRangedBox();
					multiRanged->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left);
					multiRanged->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
					multiRanged->Location = System::Drawing::Point(144, i_YPos);
					System::Int32 number = i_Number;
					multiRanged->Name = number.ToString();
					multiRanged->Size = System::Drawing::Size(56, 56);
					multiRanged->ValueX = i_Value1;
					multiRanged->ValueY = i_Value2;
					multiRanged->ValueChanged += gcnew System::EventHandler(this, &CharacterStudio::ExpressionsDialog::ExpressionGroup::multiRanged_ValueChanged);
					
					i_Panel->Controls->Add(multiRanged);
				}
				else
				{
					// 
					// rangedFloat
					// 
					rangedFloat = gcnew TerawattManagedControls::RangedFloat();
					rangedFloat->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
						| System::Windows::Forms::AnchorStyles::Right);
					rangedFloat->Exponent = (System::Int16)1;
					rangedFloat->Location = System::Drawing::Point(144, i_YPos);
					System::Int32 number = i_Number;
					rangedFloat->Name = number.ToString();
					rangedFloat->Precision = (System::Int16)2;
					//rangedFloat->Size = System::Drawing::Size(224, 24);
					rangedFloat->Size = System::Drawing::Size(m_Panel->Width - 180, 24);
					if (m_bIsPair)
					{
						rangedFloat->Minimum = -1;
					}
					rangedFloat->Value = i_Value1;
					rangedFloat->ValueChanged += gcnew System::EventHandler(this, &CharacterStudio::ExpressionsDialog::ExpressionGroup::rangedFloat_ValueChanged);
					
					i_Panel->Controls->Add(rangedFloat);
				}

				i_Panel->Controls->Add(label);
				i_Panel->Controls->Add(radioButton);
			}

			//void Reposition(int i_Number,
			//				int i_YPos)
			//{
			//	label->Location = System::Drawing::Point(24, i_YPos);
			//	rangedFloat->Location = System::Drawing::Point(104, i_YPos);
			//	System::Int32 number = i_Number;
			//	rangedFloat->Name = number.ToString();
			//	radioButton->Location = System::Drawing::Point(8, i_YPos);
			//}

			void SetName(System::String^ i_Name)
			{
				label->Text = i_Name;
			}

			bool IsSelected()
			{
				return this->radioButton->Checked;
			}

			void Detach()
			{
				if (rangedFloat != nullptr)
					m_Panel->Controls->Remove(rangedFloat);
				if (multiRanged != nullptr)
					m_Panel->Controls->Remove(multiRanged);

				m_Panel->Controls->Remove(label);
				m_Panel->Controls->Remove(radioButton);
			}

			bool IsSingle()
			{
				return (!this->m_bIsPair && !this->m_bIsFour);
			}
			bool IsPair()
			{
				return this->m_bIsPair;
			}
			bool IsFour()
			{
				return this->m_bIsFour;
			}

			int GetIndex()
			{
				if (rangedFloat != nullptr)
					return System::Int32::Parse(this->rangedFloat->Name);
				else if (multiRanged != nullptr)
					return System::Int32::Parse(this->multiRanged->Name);
				else
					return -1;
			}

		private:
			System::Windows::Forms::Panel^ m_Panel;
			System::Windows::Forms::Label ^  label;
			TerawattManagedControls::RangedFloat ^  rangedFloat;
			TerawattManagedControls::MultiRangedBox ^ multiRanged;
			System::Windows::Forms::RadioButton ^  radioButton;
			bool m_bIsPair;
			bool m_bIsFour;

			System::Void rangedFloat_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
			{
				TerawattManagedControls::RangedFloat^ rangedFloat = safe_cast<TerawattManagedControls::RangedFloat ^>(sender);
				int index = System::Int32::Parse(rangedFloat->Name);
				if (index >= 0)
				{
					if (m_bIsPair)
						chrLevel::SetGroupPairWeight(index, (float)rangedFloat->Value);
					else
						chrLevel::SetExpressionWeight(index, (float)rangedFloat->Value);
				}
			}
			System::Void multiRanged_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
			{
				TerawattManagedControls::MultiRangedBox^ multiRanged = safe_cast<TerawattManagedControls::MultiRangedBox ^>(sender);
				int index = System::Int32::Parse(multiRanged->Name);
				if (index >= 0)
				{
					chrLevel::SetGroupFourWeight(index, (float)multiRanged->ValueX, (float)multiRanged->ValueY);
				}
			}
		};

		///

		ExpressionGroup ^ get_selected_group(int &o_Index)
		{
			for (int i=0; i<expressionGroups->Count; i++)
			{
				ExpressionGroup ^group = safe_cast<ExpressionGroup^> (expressionGroups[i]);
				if (group->IsSelected())
				{
					o_Index = group->GetIndex();
					return group;
				}
			}
			o_Index = -1;
			return nullptr;
		}

		//void reposition_groups()
		//{
		//	int count = 0;
		//	// Put groups at top
		//	for (int i=0; i<expressionGroups->Count; i++)
		//	{
		//		ExpressionGroup ^group = safe_cast<ExpressionGroup^> (expressionGroups[i]);
		//		if (group->IsPair())
		//			group->Reposition(count, count++ * 32 + 8);
		//	}
		//	int num_pairs = count;

		//	// groups to remove from expressions dialog
		//	std::vector<int> to_remove;

		//	// Then regular expressions
		//	for (int i=0; i<expressionGroups->Count; i++)
		//	{
		//		ExpressionGroup ^group = safe_cast<ExpressionGroup^> (expressionGroups[i]);
		//		if (!group->IsPair())
		//		{
		//			int exp_index = group->GetIndex();
		//			if (chrLevel::IsExpressionGrouped(exp_index))
		//				to_remove.push_back(i);
		//			else
		//			{
		//				// This algorithm is failing. The indices 
		//				// are out of date because things are shifting.
		//				DBG_ASSERT2(exp_index == count - num_pairs, "Expression slider indices are out of whack, %d should be %d", exp_index, count - num_pairs);
		//				group->Reposition(count - num_pairs, count++ * 32 + 8);
		//			}
		//		}
		//	}
		//	
		//	m_NumItems = count;

		//	// Reverse order in order to not change indices as we delete
		//	for (int i=to_remove.size()-1; i>=0; i--)
		//	{
		//		ExpressionGroup ^group = safe_cast<ExpressionGroup^> (expressionGroups[to_remove[i]]);
		//		group->Detach();
		//		expressionGroups->Remove(group);
		//	}
		//}

		///

		System::Void buttonCreateExpression_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			EditExpressionDialog ^dialog = gcnew EditExpressionDialog();
			if (dialog->ShowDialog() == ::DialogResult::OK)
			{
				fsLocator anim_loc;
				tmaManagedStringUtils::ManagedStringToLocator(dialog->GetFilename(), anim_loc);
				std::string name;
				tmaManagedStringUtils::ManagedStringToStdString(dialog->GetName(), name);
				chrLevel::AddExpression(anim_loc, name);
				this->AddExpression(name.c_str(), 0, chrLevel::GetNumExpressions()-1, false);
			}
		}

		System::Void buttonLoadMultiple_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			OpenFileDialog ^dialog = gcnew OpenFileDialog();
			dialog->Filter = "Character Animation (*.cha)|*.cha|All files (*.*)|*.*";
			dialog->Multiselect = true;
			if (dialog->ShowDialog() == ::DialogResult::OK)
			{
				int num_files = dialog->FileNames->Length;
				for (int i=0; i<num_files; i++)
				{
					// The files are generally in the order that you click on
					// them, except it puts the last one you clicked on first.
					int index = (i+1) % num_files;

					String ^fullpath = safe_cast<String^>(dialog->FileNames[index]);
					cli::array<String ^> ^strings = fullpath->Split( System::String("\\").ToCharArray() );
					String ^fname = strings[strings->Length-1];
					String ^name = fname;
					if (fname->EndsWith(".cha"))
					{
						name = fname->Substring(0, fname->Length - 4);
					}

					
					fsLocator anim_loc;
					tmaManagedStringUtils::ManagedStringToLocator(fullpath, anim_loc);
					std::string name_str;
					tmaManagedStringUtils::ManagedStringToStdString(name, name_str);
					chrLevel::AddExpression(anim_loc, name_str);
					this->AddExpression(name_str.c_str(), 0, chrLevel::GetNumExpressions()-1, false);
				}
			}
		}

		System::Void buttonDeleteExpression_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			int index = -1;
			ExpressionGroup ^group = get_selected_group(index);
			if (group)
			{
				group->Detach();
				expressionGroups->Remove(group);

				if (group->IsFour())
					chrLevel::DeleteGroupFour(index, true); // delete expressions in pair also
				else if (group->IsPair())
					chrLevel::DeleteGroupPair(index, true); // delete expressions in pair also
				else
					chrLevel::DeleteExpression(index);
					
				this->ReFillExpressions();
			}
		}

		System::Void buttonEditExpression_Click(System::Object ^  sender, System::EventArgs ^  e)
		{
			int index = -1;
			ExpressionGroup ^group = get_selected_group(index);
			if (group && group->IsSingle())
			{
				System::String^ str_name = gcnew System::String(chrLevel::GetExpressionName(index).c_str());
				EditExpressionDialog ^dialog = gcnew EditExpressionDialog(str_name);
				if (dialog->ShowDialog() == ::DialogResult::OK)
				{
					std::string name;
					tmaManagedStringUtils::ManagedStringToStdString(dialog->GetName(), name);
					chrLevel::SetExpressionName(index, name);
					group->SetName(dialog->GetName());
				}
				delete dialog;
			}
		}

		 System::Void buttonGroupExpressions_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 std::vector<std::string> names;
			 chrLevel::GetUngroupedNames(names);

			 GroupExpressionsDialog ^dialog = gcnew GroupExpressionsDialog(names);
			 if (dialog->ShowDialog() == ::DialogResult::OK)
			 {
				 std::string group_name = dialog->GetGroupName();
				 if (dialog->IsFourGroup())
				 {
					 chrLevel::AddExpressionFour(group_name, 
						 dialog->GetLeftExpression(), dialog->GetRightExpression(),
						 dialog->GetDownExpression(), dialog->GetUpExpression());
				 }
				 else
				 {
					 chrLevel::AddExpressionPair(group_name, 
						 dialog->GetLeftExpression(), dialog->GetRightExpression());
				 }
					 
				this->ReFillExpressions();
			 }
			 delete dialog;
		 }
		 System::Void buttonUngroup_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			int index = -1;
			ExpressionGroup ^group = get_selected_group(index);
			if (group)
			{
				if (group->IsPair())
				{
					chrLevel::DeleteGroupPair(index, false); // do not delete expressions in pair	
					this->ReFillExpressions();
				}
				else if (group->IsFour())
				{
					chrLevel::DeleteGroupFour(index, false); // do not delete expressions in four	
					this->ReFillExpressions();
				}
			}
		 }
};
}