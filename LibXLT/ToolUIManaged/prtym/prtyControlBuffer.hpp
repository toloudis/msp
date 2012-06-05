#error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyControlBuffer.hpp
//**
//**		Template class for managing controls. 
//**
//**	StudioGPU
//**	Copyright(C) 2008 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_CONTROLBUFFER_HPP
//#error prtyControlBuffer.hpp multiply included
//#endif
//#define PRTY_CONTROLBUFFER_HPP
//
//#ifdef _MANAGED
//
////#define RECYCLE_CONTROLS
//
////============================================================================
////============================================================================
//template<class C>
//public ref class prtyControlBuffer
//{
//	public:
//		//--------------------------------------------------------------------
//		//--------------------------------------------------------------------
//		static C^ CreateControl()
//		{
//#ifdef RECYCLE_CONTROLS
//			if (m_InactiveControls->Count > 0)
//			{
//				C^ pControl = safe_cast<C^>(m_InactiveControls->Pop());
//				return pControl;
//			}
//			else
//			{
//				return gcnew C();
//			}
//#else
//			return gcnew C();
//#endif
//		}
//		//--------------------------------------------------------------------
//		//--------------------------------------------------------------------
//		static void  ReleaseControl(C^ i_pControl)
//		{
//#ifdef RECYCLE_CONTROLS
//			if (!i_pControl->IsDisposed)
//				m_InactiveControls->Push(i_pControl);
//			else
//			{
//				bool break_point = true;
//			}
//
//#else
//			delete i_pControl;
//#endif
//		}
//
//		//--------------------------------------------------------------------
//		//--------------------------------------------------------------------
//		static void  ClearAll()
//		{
//#ifdef RECYCLE_CONTROLS
//			while (m_InactiveControls->Count > 0)
//			{
//				C^ pControl = safe_cast<C^>(m_InactiveControls->Pop());
//				delete pControl;
//			}
//#endif
//		}
//
//	private:
//		static System::Collections::Stack^ m_InactiveControls = gcnew System::Collections::Stack();
//
//};
//
//typedef  prtyControlBuffer<System::Windows::Forms::Button> prtyButtonControlBuffer;
//typedef  prtyControlBuffer<System::Windows::Forms::CheckBox> prtyCheckBoxControlBuffer;
//typedef  prtyControlBuffer<System::Windows::Forms::CheckedListBox> prtyCheckedListBoxControlBuffer;
//typedef  prtyControlBuffer<System::Windows::Forms::ComboBox> prtyComboBoxControlBuffer;
//typedef  prtyControlBuffer<System::Windows::Forms::Label> prtyLabelControlBuffer;
//typedef  prtyControlBuffer<System::Windows::Forms::ListBox> prtyListBoxControlBuffer;
//typedef  prtyControlBuffer<System::Windows::Forms::NumericUpDown> prtyNumericUpDownControlBuffer;
//typedef  prtyControlBuffer<System::Windows::Forms::TextBox> prtyTextBoxControlBuffer;
//typedef  prtyControlBuffer<TerawattManagedControls::ColorRGBEdit> prtyColorRGBControlBuffer;
//typedef  prtyControlBuffer<TerawattManagedControls::ColorRGBAEdit> prtyColorRGBAControlBuffer;
//typedef  prtyControlBuffer<TerawattManagedControls::FloatEdit> prtyFloatEditControlBuffer;
//typedef  prtyControlBuffer<TerawattManagedControls::FileChooser> prtyFileChooserControlBuffer;
//typedef  prtyControlBuffer<TerawattManagedControls::FolderChooser> prtyFolderChooserControlBuffer;
//typedef  prtyControlBuffer<TerawattManagedControls::KeyCombo> prtyKeyComboControlBuffer;
//typedef  prtyControlBuffer<TerawattManagedControls::RangedFloat> prtyRangedFloatControlBuffer;
//typedef  prtyControlBuffer<TerawattManagedControls::Vector3Edit> prtyVector3EditControlBuffer;
//typedef  prtyControlBuffer<TerawattManagedControls::Vector3EditRanged> prtyVector3RangedControlBuffer;
//typedef  prtyControlBuffer<TerawattManagedControls::Vector3EditUpDown> prtyVector3UpDownControlBuffer;
//
////--------------------------------------------------------------------
//// Clean up function to free up the reusable control buffers.
//// Should only be called when shutting down and all prtyControl
//// classes have already been disposed.
////--------------------------------------------------------------------
//extern void ClearAllControlBuffers();
//
//#endif // _MANAGED
