//****************************************************************************
/// \file DebugOutput.cs
///
///	Enables outputting of text strings to the Output Window.
///
///	Extra Large Technology
///	Copyright(C) 2005 - All Rights Reserved
//****************************************************************************
using System;
using EnvDTE;


//============================================================================
///
//============================================================================
namespace DevEnvLib
{
	//------------------------------------------------------------------------
	/// Summary description for DebugOutput.
	//------------------------------------------------------------------------
	public class DebugOutput
	{
		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		public DebugOutput()
		{
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		static public void Init(object i_Application)
		{
			m_Application = (_DTE)i_Application;
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		static public void DeInit()
		{
		}

		//--------------------------------------------------------------------
		/// output a message to the Refactor window
		/// 
		/// \param format
		/// \param args
		//--------------------------------------------------------------------
		static public void Message(string format, params object[] args)
		{
			Message(string.Format(format, args));
		}

		//--------------------------------------------------------------------
		/// output a messager to the output window
		/// 
		/// \param msg
		//--------------------------------------------------------------------
		static public void Message(string msg)
		{
			Window win = m_Application.Windows.Item(EnvDTE.Constants.vsWindowKindOutput);
			string caption = win.Caption;
			string kind = win.Kind;

			OutputWindow ow = win.Object as OutputWindow;
				
			OutputWindowPane owp = null;
			if (ow != null)
			{
				try
				{
					foreach(OutputWindowPane op in ow.OutputWindowPanes)
					{
						string owName = op.Name;
						if (owName == "XLT Utils")
							owp = op;
					}
					
					if (owp == null)
						owp = ow.OutputWindowPanes.Add("XLT Utils");
					if (owp != null)
					{
						owp.OutputString(msg);
						owp.OutputString("\x0d\x0a");
					}
				}
				catch(Exception ex)
				{
					string exmsg = ex.Message;
				}
			}
		}

		//--------------------------------------------------------------------
		// variables
		//--------------------------------------------------------------------
		static private _DTE m_Application;
	}
}
