using System;
using System.Windows.Forms;
using EnvDTE;


namespace CodeExplorer
{
	/// <summary>
	/// Summary description for CodeNode.
	/// </summary>
	public class CodeNode : TreeNode
	{
		public enum CodeNodeType
		{
			Undefined,
			Solution,
			Project,
			Folder,
			ProjectItem,
			CodeElement,
		}

		private CodeNodeType CMKind = CodeNodeType.Undefined;
		private bool isChild = false;
		private TextPoint m_StartPointHPP;
		public TextPoint StartPointHPP
		{
			get
			{
				return this.m_StartPointHPP;
			}
			set
			{
				this.m_StartPointHPP = value;
			}
		}
		private TextPoint m_StartPointCPP;
		public TextPoint StartPointCPP
		{
			get
			{
				return this.m_StartPointCPP;
			}
			set
			{
				this.m_StartPointCPP = value;
			}
		}

		public CodeNode()
		{
		}

		public CodeNodeType Kind
		{
			get
			{
				return this.CMKind;
			}
			set
			{
				this.CMKind = value;
			}
		}

		public bool IsChild
		{
			get
			{
				return this.isChild;
			}
			set
			{
				this.isChild = value;
			}
		}
	}
}
