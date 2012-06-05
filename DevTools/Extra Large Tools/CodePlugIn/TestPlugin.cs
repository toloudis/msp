//****************************************************************************
/// \file TestPlugin.cs
///
///	A simple plugin
///
///	Extra Large Technology
///	Copyright(C) 2005 - All Rights Reserved
//****************************************************************************
using EnvDTE;
using Extensibility;
using Microsoft.Office.Core;
using System;
using System.Collections;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Windows.Forms;


//============================================================================
///
//============================================================================
namespace CodePlugIn
{
	//========================================================================
	/// 
	//========================================================================
	public class TestPlugin : DevEnvLib.VSPlugIn
	{
		#region public Attributes
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override string cmdName
		{
			get
			{
				return "Test97";
			}
		}
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override string shortDescription{get { return "test97a";}}
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override string longDescription {get {return "looooooong description"; }}
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override int position { get {return  1;} }
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public override int iconId { get {return 6743;} }
		#endregion

		#region construction
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		public TestPlugin(_DTE applicationDTE, AddIn addInInstance)
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
			doTestExec();
			return true;
		}
		#endregion

		#region private methods
		//--------------------------------------------------------------------
		/// do the actual execution code here.
		/// 
		/// \return true or false
		//--------------------------------------------------------------------
		private bool doTestExec()
		{
			//
			//TESTING code ONLY!!
			//
			//1)
			//CommentBuilder.CommentBatch.RebuildCommentsForActiveSolution( m_ApplicationObject );
			//CommentBatch.RebuildCommentsForActiveFile( m_ApplicationObject );

			//2)
			CodeModelLib.System.Init( m_ApplicationObject );
			CodeModelLib.FCMHierarchy.BuildTreeForActiveSolution();

			CodeModelLib.FCMHierarchy.DebugTree( CodeModelLib.FCMHierarchy.GetRootNode() );

			//CodeModelLib.FCMHierarchy.GetComments( CodeModelLib.FCMHierarchy.GetRootNode() );

			//3)
//			CodeExplorer.HierarchyViewer.Init( m_ApplicationObject );
//			CodeExplorer.HierarchyViewer.Show();
//
//			TreeNode rootNode = CodeExplorer.HierarchyViewer.GetRootTreeNode();
//
//			CodeExplorer.CodeNodeHelper.Traverse( ref rootNode );
			return true;
		}

		//--------------------------------------------------------------------
		/// show information
		/// 
		/// \param element
		//--------------------------------------------------------------------
		private void showInfo (CodeElement element)
		{
		}
		#endregion
	}
}
