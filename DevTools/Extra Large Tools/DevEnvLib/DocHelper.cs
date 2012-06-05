//****************************************************************************
/// \file DocHelper.cs
///
///	Helper functions for the Documentation object.
///
///	Extra Large Technology
///	Copyright(C) 2005 - All Rights Reserved
//****************************************************************************
using System;
using EnvDTE;
using Microsoft.Office.Core;


//============================================================================
///
//============================================================================
namespace DevEnvLib
{
	//------------------------------------------------------------------------
	/// Summary description for DocHelper.
	//------------------------------------------------------------------------
	public class DocHelper
	{
		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		public DocHelper()
		{
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		public static bool IsHeader( Document i_Doc )
		{
			return ((i_Doc.Name.EndsWith(".h") == true) ||	(i_Doc.Name.EndsWith(".hpp") == true));
		}

		//--------------------------------------------------------------------
		///
		//--------------------------------------------------------------------
		public static void SwitchToHppOrCpp( object i_App, Document i_Doc )
		{
			_DTE App = (_DTE)i_App;

			// Only Process C/C++ Applications
			if (App.ActiveDocument == null) return;
			if (App.ActiveDocument.Language != "C/C++") return;

			string name;
			name = i_Doc.Name;
			if (IsHeader(i_Doc))
			{
				name = name.Replace(".h",".c");
				if (name.EndsWith(".c"))
				{
					name += "pp";
				}
			}
			else
			{
				name = name.Replace(".c",".h");
				//ToDo [rjk] check if .hpp isn't there to find the .h instead
			}

			Project proj = i_Doc.ProjectItem.ContainingProject;

			ProjectItems PIs = proj.ProjectItems;
			foreach (ProjectItem pi in PIs)
			{
				if (pi.Name == name)
				{
					Window win = App.ItemOperations.OpenFile(pi.get_FileNames(0),Constants.vsViewKindCode);
	                //if (win. != 0)
					{
						return;
					}
				}
			}
		}

		private bool checking = false;
		//--------------------------------------------------------------------
		/// check the status of a control for a caption on a commandbar
		/// 
		/// \param cmdBar
		/// \param caption
		//--------------------------------------------------------------------
		private void checkControl(CommandBar cmdBar, string caption)
		{
			if (checking) return;
			try
			{
				checking = true;
				foreach (CommandBarControl control in cmdBar.Controls)
				{
					if (control.Caption == caption)
					{
						DevEnvLib.DebugOutput.Message("control for Cmd {0} in CommandBar {1}, enabled:{2}, visible {3}",
							caption, cmdBar.Name, control.Enabled, control.Visible);
						return;
					}
				}
				DebugOutput.Message("Did not find caption {0} on commandbar {1}", caption, cmdBar.Name); 
			}
			finally
			{
				checking = false;
			}
		}

		//--------------------------------------------------------------------
		/// insert a popup into the code window
		/// 
		/// \param MenuLabel
		/// \return the command bar
		//--------------------------------------------------------------------
		public static CommandBar InsertSubMenu( _DTE i_App, 
												string i_NameOfOwner, 
												string i_Caption)
		{
			Object temporary = true;
			CommandBar codeWindowCB = i_App.CommandBars[i_NameOfOwner];
			CommandBarControl cbCtl	= codeWindowCB.Controls.Add(
																MsoControlType.msoControlPopup,
																System.Reflection.Missing.Value,
																System.Reflection.Missing.Value,
																1,
																temporary);
			cbCtl.Caption = i_Caption;

			CommandBarPopup cbPopup = (CommandBarPopup)cbCtl;
			CommandBar retval = cbPopup.CommandBar;
			return retval;
		}
	}
}
