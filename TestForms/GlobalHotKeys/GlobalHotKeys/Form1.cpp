#include "stdafx.h"
#include "Form1.h"

#include <string>
#include <windows.h>
#include <vcclr.h>
#include <wchar.h>
#include <cstdlib>


//============================================================================
// Windows API functions and constants
//============================================================================
//#define DLLIMPORT __declspec(dllimport) 
//[DllImport("user32", SetLastError=true)]
//static extern int RegisterHotKey (IntPtr hwnd, int id, int fsModifiers, int vk);
//[DllImport("user32", SetLastError=true)]
//static extern int UnregisterHotKey (IntPtr hwnd, int id);
__declspec( dllimport ) short GlobalAddAtom(std::string* lpString);
//__declspec( dllimport ) ATOM GlobalDeleteAtom(ATOM atom);


//============================================================================
//============================================================================
namespace GlobalHotKeys 
{

//
Form1::Form1(void)
{
	InitializeComponent();
	//
	//TODO: Add the constructor code here
	//
	this->Activate();
}

//
Form1::~Form1()
{
	if (components)
	{
		delete components;
	}
}

//
System::Void Form1::Form1_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e)
{
	//cmaCommand* pCmd = 
	
	if ( this->m_hotkeyID != 0 )
	{
		UnregisterHotKey((HWND)(this->Handle.ToPointer()), m_hotkeyID);
		// clean up the atom list
		GlobalDeleteAtom(m_hotkeyID);
		m_hotkeyID = 0;
	}
}

//
//	This method works as long as the main form has FOCUS.
//
bool Form1::ProcessCmdKey(Message% msg, Keys keyData)  
{
    //const int WM_KEYDOWN;// = 0x100;
    //const int WM_SYSKEYDOWN = 0x104;

    if ((msg.Msg == WM_KEYDOWN) || (msg.Msg == WM_SYSKEYDOWN))
    {
        switch(keyData)
        {
        case Keys::Down:
				this->label_processcmdkey->Text = "DOWN ARROW key captured";
                break;
            
        case Keys::Up:
                this->label_processcmdkey->Text = "UP ARROW key captured";
                break;
        
        case Keys::Tab:
                this->label_processcmdkey->Text = "TAB key captured";
                break;
        
        case Keys::Control | Keys::M:
                this->label_processcmdkey->Text = "CTRL+M key combination captured";
                break;
        
        case Keys::Alt | Keys::Z:
                this->label_processcmdkey->Text = "ALT+Z key combination captured";
                break;
		default:
			String^ modifiers = "";
			String^ keyName;
			int origKeyCode;

			// get the actual integer value of the keystroke
			int keyCode = (int) keyData;
			origKeyCode = keyCode;

			// check to see if the control key is on
			if ((keyCode & (int)Keys::Control) != 0)
			{
				modifiers += "<CTRL>";
			}
			        
			// check to see if the alt key is on
			if ((keyCode & (int)Keys::Alt) != 0)
			{
				modifiers += "<ALT>";
			}

			// check to see if the shift key is on
			if ((keyCode & (int)Keys::Shift) != 0)
			{
				modifiers += "<SHIFT>";
			}    

			// strip off the modifier keys
			keyCode = keyCode & 0xFFFF;

			// attempt to convert the remaining bits to the enum name
			Keys key = (Keys) keyCode;
			if ((key != Keys::Menu) && (key != Keys::ControlKey) && (key != Keys::ShiftKey))
			{
				keyName = key.ToString();
			}
			else
			{
				// the key is ctrl, alt, or shift by itself. Don't repeat the
				// key name since it was already printed above
				keyName = "";
			}
			        
			// display the final code string value and return true to swallow the key
			this->label_processcmdkey->Text = origKeyCode.ToString() + " = " + modifiers + " " + keyName;

			//
			//	try to display a "legal" key combo string
			//
			String^ keycombo;
            KeysConverter^ kc = gcnew KeysConverter();

            keycombo = kc->ConvertToString(origKeyCode);
            //keycombo = String.Format("{0} + ", m_Modifiers.ToString());
            //keycombo = String.Concat(keycombo, m_Hotkey.ToString());

			//	Get the keys
			try
			{
				//	try to convert it and see if it throws an exception
				//
				TypeConverter^ keyconv = TypeDescriptor::GetConverter(origKeyCode.GetType());
				//Shortcut scut = (Shortcut)keyconv->ConvertFromString(keycombo);
			}
			catch (ArgumentNullException^)
			{
				// if not a valid key combo, show nothing.
				keycombo = "";
			}
			catch (ArgumentException^)
			{
				// if not a valid key combo, show nothing.
				keycombo = "";
			}

			this->label_keysconverter->Text = keycombo;

			// Return true to "swallow" the key.
			return true;    
        }               
    }

    return (__super::ProcessCmdKey(msg,keyData));
}

//
bool ManagedStringToStdString(System::String^ i_Source, std::string& o_Dest)
{
	int len = (( i_Source->Length+1) * 2);
	char *ch = new char[ len ];
	bool result ;
	{
	pin_ptr<const wchar_t> wch = PtrToStringChars( i_Source );
	result = wcstombs( ch, wch, len ) != -1;
	}

	o_Dest = ch;
	delete[] ch;
	return result; 
}

//
System::Void Form1::Form1_Load(System::Object^  sender, System::EventArgs^  e)
{
	//	register the hotkey globally
	//
	try
	{
		// use the GlobalAddAtom API to get a unique ID (as suggested by MSDN docs)
		//System::String* atomName = System::Threading::Thread::CurrentThread->ManagedThreadId.ToString("X8") + this->Name;
		System::Guid atomGUID = System::Guid::NewGuid();
		std::string atomstr;
		ManagedStringToStdString(atomGUID.ToString(), atomstr);

		// null-call to get the size
		size_t needed = ::mbstowcs(NULL,atomstr.c_str(),atomstr.size());
		// allocate
		std::wstring output;
		output.resize(needed);
		// real call
		::mbstowcs(&output[0],atomstr.c_str(),atomstr.size());
		// You asked for a pointer
		const wchar_t *pout = output.c_str(); 

		//static const WCHAR foobarW[] = {'f','o','o','b','a','r',0};
//		WCHAR wstr[256] = atomstr.c_str();//L"#1234";
		m_hotkeyID = GlobalAddAtom(pout);//&atomstr);
		if ( m_hotkeyID == 0 )
		{
			//throw new System::Exception("Unable to generate unique hotkey ID. Error code: " +
			//	Marshal::GetLastWin32Error().ToString());
		}

		// register the hotkey, throw if any error
		if (RegisterHotKey((HWND)(this->Handle.ToPointer()), m_hotkeyID, MOD_ALT, 'P'))
		{
			//throw new System::Exception("Unable to register hotkey. Error code: " + Marshal::GetLastWin32Error()
			//	.ToString());
		}
	}
	catch ( System::Exception^ /*e*/ )
	{
	   // clean up if hotkey registration failed
	   UnregisterHotKey((HWND)(this->Handle.ToPointer()),m_hotkeyID);
	}
}

//	I haven't managed this method to get this hooked up
//
void Form1::WndProc(Message* m)
{   
    // Check if the CTRL key is being pressed.
    switch(m->Msg)
    {
		case WM_ACTIVATEAPP:
		{		   
		   // The WParam value identifies what is occurring.
		   int appActive = (int)m->WParam != 0;
		   this->Invalidate();
		   break;
		}
		case WM_HOTKEY:
		{
			int x = 5;
			x++;
			break;
		}
    }

	//Form::WndProc(m);
	this->WndProc(m);
}

//
//	KeyDown will not get called if the Form itself isn't the focus.
//	As soon as a button added to the form and it has focus, this no
//	longer gets called.
//
System::Void Form1::Form1_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e)
{
	this->label_pressed->Text = System::String::Format("{0}{1}{2}{3}",
															(e->Control ? "Control+":""),
															(e->Alt ? "Alt+":""),
															(e->Shift ? "Shift+":""),
															(e->KeyValue.ToString()) );
}

System::Void Form1::button_launchdialog_Click(System::Object^  sender, System::EventArgs^  e) 
{
	System::Windows::Forms::Form^ dialog = gcnew System::Windows::Forms::Form;
	//dialog->ShowDialog();
	dialog->Show();

}

//
//	PreviewKeyDown will not get called if the Form itself isn't the focus.
//	As soon as a button added to the form and it has focus, this no
//	longer gets called.
//
System::Void Form1::Form1_PreviewKeyDown(System::Object^  sender, System::Windows::Forms::PreviewKeyDownEventArgs^  e)
{
	this->label_previewkeydown->Text = System::String::Format("{0}{1}{2}{3}",
																(e->Control ? "Control+":""),
																(e->Alt ? "Alt+":""),
																(e->Shift ? "Shift+":""),
																(e->KeyValue.ToString()) );
}

}
