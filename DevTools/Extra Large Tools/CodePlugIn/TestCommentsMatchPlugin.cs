//****************************************************************************
/// \file TestCommentsMatchPlugin
///
///	Test if the .hpp and .cpp comments match
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
	/// Move a selected function to the superclass
	//========================================================================
	public class TestCommentsMatchPlugIn : DevEnvLib.VSPlugIn
	{
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public TestCommentsMatchPlugIn(_DTE applicationObject, AddIn addInInstance)
			: base(applicationObject, addInInstance)
		{
		}

		#region overrides
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override string longDescription
		{
			get	{return "Test to make sure the header and source comments match";}
		}
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override string cmdName
		{
			get	{return "TestCommentsMatch";}
		}
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override string shortDescription
		{
			get	{return "Test Comment Matching";}
		}
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override int iconId
		{
			get	{return 54;}
		}
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override int position
		{
			get	{return 1;}
		}
		#endregion

		#region connect calls
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		protected override bool doExec()
		{
			TextPoint tp;
			switch (m_CodeElement.Kind)
			{
				case vsCMElement.vsCMElementClass:
				{
					TextSelection selection = (TextSelection)m_ApplicationObject.ActiveWindow.Selection;
					EditPoint Start = selection.TopPoint.CreateEditPoint();

					DevEnvLib.DebugOutput.Message("start={0}", Start.Line);
					CodeElement pCE = DevEnvLib.FCMHelper.GetElementFromPoint(fileCodeModel, Start);
					if ( pCE != null )
					{
						DevEnvLib.DebugOutput.Message(" kind={0}", pCE.Kind.ToString());
					}

					tp = CodeModelLib.System.GetStartPoint( m_CodeElement, vsCMPart.vsCMPartHeader, CodeModelLib.System.eFileType.Header );
					DevEnvLib.DebugOutput.Message( String.Format(" {0} line {1}", m_CodeElement.Name, tp.Line.ToString()) );
					tp = CodeModelLib.System.GetStartPoint( m_CodeElement, vsCMPart.vsCMPartHeader, CodeModelLib.System.eFileType.Source );
					DevEnvLib.DebugOutput.Message( String.Format(" {0} line {1}", m_CodeElement.Name, tp.Line.ToString()) );

					//CommentBuilder.CommentBatch.RebuildCommentsForClass(m_CodeElement);
					break;
				}
				case vsCMElement.vsCMElementFunction:
				{
					tp = CodeModelLib.System.GetStartPoint( m_CodeElement, vsCMPart.vsCMPartHeader, CodeModelLib.System.eFileType.Header );
					DevEnvLib.DebugOutput.Message( String.Format(" {0} line {1}", m_CodeElement.Name, tp.Line.ToString()) );
					tp = CodeModelLib.System.GetStartPoint( m_CodeElement, vsCMPart.vsCMPartHeader, CodeModelLib.System.eFileType.Source );
					DevEnvLib.DebugOutput.Message( String.Format(" {0} line {1}", m_CodeElement.Name, tp.Line.ToString()) );

					//CommentBuilder.CommentBatch.RebuildCommentsForClass(m_CodeElement);
					break;
				}
				default:
					break;
			}
			return true;
		}

		//--------------------------------------------------------------------
		/// can we do this?
		/// 
		/// \return
		//--------------------------------------------------------------------
		protected override EnvDTE.vsCommandStatus doQueryStatus()
		{
			if (m_ApplicationObject == null)
				return vsCommandStatus.vsCommandStatusUnsupported;
			if (m_ApplicationObject.ActiveDocument == null)
				return vsCommandStatus.vsCommandStatusUnsupported;
			if (m_ApplicationObject.ActiveDocument.Language != "C/C++")
				return vsCommandStatus.vsCommandStatusUnsupported;
			if (!DevEnvLib.DocHelper.IsHeader(m_ApplicationObject.ActiveDocument))
				return vsCommandStatus.vsCommandStatusUnsupported;

			m_CodeElement = (CodeElement)this.selectedElement(vsCMElement.vsCMElementClass);
			if (m_CodeElement == null)
			{
				m_CodeElement = (CodeElement)this.selectedElement(vsCMElement.vsCMElementFunction);
				if (m_CodeElement == null)
				{
					m_CodeElement = (CodeElement)this.selectedElement(vsCMElement.vsCMElementNamespace);
					if (m_CodeElement == null)
					{
						return vsCommandStatus.vsCommandStatusUnsupported;
					}
				}
			}

			return vsCommandStatus.vsCommandStatusEnabled | vsCommandStatus.vsCommandStatusSupported;
		}
		#endregion

		#region private vars
		//--------------------------------------------------------------------
		//	variables
		//--------------------------------------------------------------------
		CodeElement		m_CodeElement = null;
		#endregion
	}
}
