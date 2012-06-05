using EnvDTE;
using System;
using System.Windows.Forms;


//============================================================================
//============================================================================
namespace CodeExplorer
{
	//========================================================================
	/// Summary description for HierarchyViewer.
	//========================================================================
	public class HierarchyViewer
	{
		static private bool m_bShow = false;
		static private bool m_bInit = false;
		static private HierarchyViewerForm m_ViewerForm = null;
		static private TreeNode m_RootSolutionNode;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static public void Init( _DTE i_App )
		{
			m_bInit = true;
			m_RootSolutionNode = CodeHierarchy.Init( i_App );
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static public void Show()
		{
			if ( !m_bInit )
				return;
			if ( m_bShow )
				return;
			m_bShow = true;

			m_ViewerForm = new HierarchyViewerForm( ref m_RootSolutionNode );
			m_ViewerForm.Show();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static public void Hide()
		{
			if ( !m_bShow )
				return;
			m_bShow = false;

			m_ViewerForm.Hide();
		}

		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		static public TreeNode GetRootTreeNode()
		{
			return m_RootSolutionNode;
		}
	}
}
