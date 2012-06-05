//****************************************************************************
/// \file UpdateCommentPlugin
///
///	Update the comment for the selected code block
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
// TODO: finish this plugin
//============================================================================
namespace CodePlugIn
{
	//========================================================================
	/// Move a selected function to the superclass
	//========================================================================
	public class UpdateCommentPlugIn : DevEnvLib.VSPlugIn
	{
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public UpdateCommentPlugIn(_DTE applicationObject, AddIn addInInstance)
			: base(applicationObject, addInInstance)
		{
		}

		#region overrides
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override string longDescription
		{
			get
			{
				return "Update or Create the comment for this code block";
			}
		}
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override string cmdName
		{
			get
			{
				return "UpdateComment";
			}
		}
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override string shortDescription
		{
			get
			{
				return "Update the Comment";
			}
		}
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override int iconId
		{
			get
			{
				return 54;
			}
		}
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override int position
		{
			get
			{
				return 1;
			}
		}
		#endregion

		#region connect calls
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		protected override bool doExec()
		{
			if (!IsCommentableItem())
				return false;

			switch (m_ElementToComment.Kind)
			{
				case vsCMElement.vsCMElementFunction:
					CommentBuilder.CommentBatch.RebuildCommentsForClassFunction(m_ElementToComment);
					break;
				case vsCMElement.vsCMElementClass:
					CommentBuilder.CommentBatch.RebuildCommentsForClass(m_ElementToComment);
					break;
				case vsCMElement.vsCMElementVariable:
					//copyVariable();
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
			identifySelectedItem();

			if (IsCommentableItem())
			{
				return vsCommandStatus.vsCommandStatusEnabled | vsCommandStatus.vsCommandStatusSupported;
			}
			return vsCommandStatus.vsCommandStatusUnsupported;
		}
		#endregion

		#region worker stuff
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
