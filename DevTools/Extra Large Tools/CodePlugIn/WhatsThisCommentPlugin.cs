//****************************************************************************
/// \file WhatsThisCommentPlugin.cs
///
///	A simple plugin that displays the comment for this codeblock
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
	public class WhatsThisCommentPlugin : DevEnvLib.VSPlugIn
	{
		#region public Attributes
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override string cmdName
		{
			get
			{
				return "WhatIsThisComment";
			}
		}
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override string shortDescription{get { return "What's the Comment";} }
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override string longDescription { get {return "identify what the comment is currently for the block of code"; }}
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
		public WhatsThisCommentPlugin(_DTE applicationDTE, AddIn addInInstance)
			: base(applicationDTE, addInInstance)
		{
		}
		#endregion

		#region public methods
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		protected override EnvDTE.vsCommandStatus doQueryStatus()
		{
			identifySelectedItem();

			if (IsCommentableItem())
			{
				return vsCommandStatus.vsCommandStatusEnabled | vsCommandStatus.vsCommandStatusSupported;
			}
			return vsCommandStatus.vsCommandStatusUnsupported;
		}
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		protected override bool doExec()
		{
			if (!IsCommentableItem())
				return false;

			switch (m_ElementToComment.Kind)
			{
				case vsCMElement.vsCMElementFunction:
					outputComment( m_FunctionToComment.Comment );
					break;
				case vsCMElement.vsCMElementClass:
					outputComment( m_ClassToComment.Comment );
					break;
				case vsCMElement.vsCMElementVariable:
					outputComment( m_VariableToComment.Comment );
					break;
			}
			return true;
		}
		#endregion

		#region worker stuff
		//--------------------------------------------------------------------
		/// determine what it actually is that has been selected.
		//--------------------------------------------------------------------
		private void outputComment( string i_Comment )
		{
			DevEnvLib.DebugOutput.Message( i_Comment );
			DevEnvLib.DebugOutput.Message( "\n" );
		}
		
		//--------------------------------------------------------------------
		/// determine what it actually is that has been selected.
		//--------------------------------------------------------------------
		private void identifySelectedItem()
		{
			m_ElementToComment = null;
			m_ClassToComment = (CodeClass) this.selectedElement(vsCMElement.vsCMElementClass);
			if (m_ClassToComment != null)
			{
				m_ElementToComment = (CodeElement)m_ClassToComment;
			}
			m_FunctionToComment = (CodeFunction) this.selectedElement(vsCMElement.vsCMElementFunction);
			if (m_FunctionToComment != null)
			{
				m_ElementToComment = (CodeElement)m_FunctionToComment;
			}
			m_VariableToComment = (CodeVariable) this.selectedElement(vsCMElement.vsCMElementVariable);
			if (m_VariableToComment != null)
			{
				m_ElementToComment = (CodeElement)m_VariableToComment;
			}
		}

		//--------------------------------------------------------------------
		/// get the detail of the item to move and its target.
		/// 
		/// \return true if circumstances allow moving it
		//--------------------------------------------------------------------
		private bool IsCommentableItem()
		{
			//condition something must be a selectied
			if (m_ElementToComment == null)
				return false;

			return true;
		}

		//--------------------------------------------------------------------
		/// check if a Class is part of the project or not
		/// 
		/// \param parent
		/// 
		/// \return if this class is part of the project
		//--------------------------------------------------------------------
		private bool IsPartOfProject(CodeClass parent)
		{		
			try 
			{
				ProjectItem project = parent.ProjectItem;
			}
			catch (Exception) {return false;} 
			return true;
		}
		#endregion

		#region private vars
		//--------------------------------------------------------------------
		//	variables
		//--------------------------------------------------------------------
		CodeElement		m_ElementToComment = null;
		CodeFunction	m_FunctionToComment = null;
		CodeClass		m_ClassToComment = null;
		CodeVariable	m_VariableToComment = null;
		#endregion
	}
}
