//****************************************************************************
/// \file WhatsThisPlugin.cs
///
///	A simple plugin that displays what type of code block is selected
///
///	Extra Large Technology
///	Copyright(C) 2005 - All Rights Reserved
//****************************************************************************
using System;
using Microsoft.Office.Core;
using Extensibility;
using System.Runtime.InteropServices;
using EnvDTE;
using System.Collections;
using System.Globalization;


//============================================================================
///
//============================================================================
namespace CodePlugIn
{
	//========================================================================
	/// 
	//========================================================================
	public class WhatsThisPlugin : DevEnvLib.VSPlugIn
	{
		#region public Attributes
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override string cmdName	{ get {return "WhatIsThis";} }
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override string shortDescription{get { return "What is this";} }
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override string longDescription { get {return "identify what this codefragment might be"; }}
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override int position { get {return  1;} }
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override int iconId { get {return 54;} }
		#endregion

		#region construction
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public WhatsThisPlugin(_DTE applicationDTE, AddIn addInInstance)
			: base(applicationDTE, addInInstance)
		{
		}
		#endregion

		#region public methods
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		protected override EnvDTE.vsCommandStatus doQueryStatus()
		{
			return 	(vsCommandStatus)(vsCommandStatus.vsCommandStatusEnabled | vsCommandStatus.vsCommandStatusSupported);
		}
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		protected override bool doExec()
		{
			return whatIsThis();
		}
		#endregion

		#region private methods
		//--------------------------------------------------------------------
		/// tell me what this is
		/// 
		/// \return whether the code knows what type of object this is.
		//--------------------------------------------------------------------
		private bool whatIsThis()
		{
			TextSelection selection = (TextSelection)m_ApplicationObject.ActiveWindow.Selection;
			EditPoint Start = selection.TopPoint.CreateEditPoint();
			EditPoint End = selection.BottomPoint.CreateEditPoint();
			if ((End.AbsoluteCharOffset - Start.AbsoluteCharOffset) > 3)
			{
				DevEnvLib.DebugOutput.Message("'{0}'", selection.Text);
			}
			// get the element under the cursor
			FileCodeModel fileCodeModel;
			try
			{
				Document doc = (Document)m_ApplicationObject.ActiveDocument; 
				ProjectItem projectItem = (ProjectItem)doc.ProjectItem;
				fileCodeModel = (FileCodeModel) projectItem.FileCodeModel;
			}
			catch (Exception ex)
			{
				string msg = ex.Message;
				System.Windows.Forms.MessageBox.Show(ex.Message, "Exception when trying to determine what this is"); 
				DevEnvLib.DebugOutput.Message(msg);
				return false;
			}			

			DevEnvLib.DebugOutput.Message("start={0} end={1}", Start.Line, End.Line);
			CodeElement pCE = DevEnvLib.FCMHelper.GetElementFromPoint(fileCodeModel, Start);
			//vsCMElement pCME = (vsCMElement)(pCE);
			if ( pCE != null )
			{
				showInfo(pCE);
			}
			return true;
		}

		//--------------------------------------------------------------------
		/// show what an element is made of
		/// 
		/// \param element
		//--------------------------------------------------------------------
		private void showInfo (CodeElement element)
		{
			DevEnvLib.DebugOutput.Message("--language {0}", m_ApplicationObject.ActiveDocument.Language);

			string result = string.Format(CultureInfo.InvariantCulture, 
				"Name: {0}, Kind: {1}", element.FullName, element.Kind);
			DevEnvLib.DebugOutput.Message(result);
			DevEnvLib.DebugOutput.Message("  IsCodeType: {0}", element.IsCodeType);
			try
			{
				if (element.Children != null)
				{
					DevEnvLib.DebugOutput.Message("  Children:  {0}", element.Children.Count);

					CodeElements children = element.Children;
					IEnumerator enumChild = children.GetEnumerator();
					int idx=0;
					while (enumChild.MoveNext())
					{
						CodeElement child =  enumChild.Current as CodeElement;
						DevEnvLib.DebugOutput.Message("  Child[{0}]: {1}", idx++, child.FullName);
					}
				}
			} 
			catch (NotImplementedException )
			{
				//DevEnvLib.DebugOutput.Message("No Children");
			}
		}
		#endregion
	}
}
