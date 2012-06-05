using System;
using System.IO;
using System.Windows.Forms;
using EnvDTE;


//============================================================================
//============================================================================
namespace CodeExplorer
{
	//========================================================================
	/// Summary description for CodeHierarchy.
	//========================================================================
	public class CodeHierarchy
	{
		enum NodeImageIndexes
		{
			Solution,
			CSProject, VBProject, VCProject, Project,
			CSFile, VBFile, VCFile, File,
			Namespace, Class, Interface, Delegate,
			Struct, Enum,
			Function, Property, Variable,
			FolderClosed, FolderOpen,
			Unknown,
			VJProject, VJFile,
			HeaderFile
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static public CodeNode Init( _DTE i_App )
		{
			m_ApplicationObject = (_DTE)i_App;
			m_InitReferences++;

			BuildHierarchy();		// build the tree

			return m_SolutionNode;
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static private void BuildHierarchy()
		{
			if ( m_SolutionNode != null )
			{
				// ToDo [rjk] delete the hierarchy and clean things up.
			}
				
			m_SolutionNode = new CodeNode();

			// _DTE.Solution always returns an object, so check
			// for empty string in FullName
			if (m_ApplicationObject.Solution.FullName != String.Empty)
			{
				Solution solution = m_ApplicationObject.Solution;

				// Add Solution
				m_SolutionNode.Tag = solution;
				m_SolutionNode.Text = Path.GetFileNameWithoutExtension(solution.FullName);
				m_SolutionNode.Kind = CodeNode.CodeNodeType.Solution;
				m_SolutionNode.ImageIndex = (int)NodeImageIndexes.Solution;
				m_SolutionNode.SelectedImageIndex = (int)NodeImageIndexes.Solution;

				try
				{
					// Add dummy node if Solution has projects; if using CodeModel,
					// verify that at least one project has one
					if (solution.Projects.Count > 0)
					{
						bool hasChildren = true;

//						if (cmRadioButton.Checked)
//						{
//							hasChildren = false;
//
//							foreach (Project project in solution.Projects)
//							{
//								if (project.CodeModel != null)
//								{
//									hasChildren = true;
//									break;
//								}
//							}
//						}

						if (hasChildren)
						{
							//m_SolutionNode.Nodes.Add(new TreeNode());
							AddSolutionChildren( m_SolutionNode );
						}
					}
				}
				catch
				{
				}
			}
			else
			{
				// No solution
				m_SolutionNode.Text = "(No solution)";
				m_SolutionNode.Kind = CodeNode.CodeNodeType.Solution;
				m_SolutionNode.ImageIndex = (int)NodeImageIndexes.Solution;
				m_SolutionNode.SelectedImageIndex = (int)NodeImageIndexes.Solution;
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static private void AddSolutionChildren(TreeNode solutionNode)
		{
			Solution solution = (Solution)solutionNode.Tag;

			// Add Projects
			foreach (Project project in solution.Projects)
			{
				try
				{
					// If using CodeModel, ignore projects without it (e.g., Solution Items folder)
					if (   (
							//	fcmRadioButton.Checked 
							//&& 
								project.Name != "Solution Items") 
						//|| (cmRadioButton.Checked && project.CodeModel != null)
						)
					{
						CodeNode projectNode = new CodeNode();
						int index = GetProjectImageIndex(project);

						projectNode.Tag = project;
						projectNode.Text = Path.GetFileNameWithoutExtension(project.Name);
						projectNode.Kind = CodeNode.CodeNodeType.Project;
						projectNode.ImageIndex = index;
						projectNode.SelectedImageIndex = index;

						solutionNode.Nodes.Add(projectNode);

						try
						{
							// Add dummy node to each Project that has children
							if (ProjectHasChildren(project))
							{
								//projectNode.Nodes.Add(new TreeNode());
								AddProjectChildren( projectNode );
							}
						}
						catch
						{
						}
					}
				}
				catch
				{
				}
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static private void AddProjectChildren(TreeNode projectNode)
		{
			Project project = (Project)projectNode.Tag;

			//if (fcmRadioButton.Checked) // Use FileCodeModel
			{
				// Add ProjectItems
				foreach (ProjectItem item in project.ProjectItems)
				{
					try
					{
						bool isFolder = IsFolder(item);

						// Ignore project items without a FileCodeModel, unless they're folders
						if (item.FileCodeModel != null || isFolder)
						{	
							CodeNode itemNode = new CodeNode();
							int index = GetProjectItemImageIndex(item);

							itemNode.Tag = item;
							itemNode.Text = item.Name;
							itemNode.Kind = (isFolder) ? CodeNode.CodeNodeType.Folder : CodeNode.CodeNodeType.ProjectItem;
							itemNode.ImageIndex = index;
							itemNode.SelectedImageIndex = index;

							projectNode.Nodes.Add(itemNode);
				
							try
							{
								// Add dummy node to each ProjectItem that has children
								if (isFolder && FolderHasChildren(item))
								{
									//itemNode.Nodes.Add(new TreeNode());
									AddFolderChildren(itemNode);
								}
								else if (item.FileCodeModel.CodeElements.Count > 0)
								{
									//itemNode.Nodes.Add(new TreeNode());
									AddProjectItemChildren(itemNode);
								}
							}
							catch
							{
							}
						}
					}
					catch
					{
					}
				}
			}
//			else // Use CodeModel
//			{
//				CodeModel model = project.CodeModel;
//
//				AddCodeElements(projectNode, model.CodeElements, false);
//			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static private void AddFolderChildren(TreeNode folderNode)
		{
			ProjectItem folder = (ProjectItem)folderNode.Tag;

			// Add ProjectItems
			foreach (ProjectItem item in folder.ProjectItems)
			{
				try
				{
					bool isFolder = IsFolder(item);

					// Ignore project items without a FileCodeModel, unless they're folders
					if (item.FileCodeModel != null || isFolder)
					{	
						CodeNode itemNode = new CodeNode();
						int index = GetProjectItemImageIndex(item);

						itemNode.Tag = item;
						itemNode.Text = item.Name;
						itemNode.Kind = (isFolder) ? CodeNode.CodeNodeType.Folder : CodeNode.CodeNodeType.ProjectItem;
						itemNode.ImageIndex = index;
						itemNode.SelectedImageIndex = index;

						folderNode.Nodes.Add(itemNode);
				
						try
						{
							// Add dummy node to each ProjectItem that has children
							if (isFolder && item.ProjectItems.Count > 0)
							{
								//itemNode.Nodes.Add(new TreeNode());
								AddProjectItemChildren( itemNode );
							}
							else if (item.FileCodeModel.CodeElements.Count > 0)
							{
								//itemNode.Nodes.Add(new TreeNode());
								AddProjectItemChildren( itemNode );
							}
						}
						catch
						{
						}
					}
				}
				catch
				{
				}
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static private void AddProjectItemChildren(TreeNode itemNode)
		{
			ProjectItem item = (ProjectItem)itemNode.Tag;
			FileCodeModel model = item.FileCodeModel;
			
			AddCodeElements(itemNode, model.CodeElements, false);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static private void AddCodeElementChildren(TreeNode elementNode)
		{
			CodeElement element = (CodeElement)elementNode.Tag;
			CodeElements members = GetMembers(element);

			if (members != null)
			{
				AddCodeElements(elementNode, members, false);
			}

			//if (this.childCheckBox.Checked)
			{
				try
				{
					if (element.Children.Count > 0)
					{
						AddCodeElements(elementNode, element.Children, true);
					}
				}
				catch
				{
				}
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static private void SetStartPoints( CodeNode i_CNode )
		{
			CodeElement ce = (CodeElement)i_CNode.Tag;
			if (ce.Kind == vsCMElement.vsCMElementFunction)
			{
				CodeFunction cfunc = (CodeFunction)i_CNode.Tag;

				i_CNode.StartPointHPP = CodeModelLib.System.GetStartPoint(ce,vsCMPart.vsCMPartHeader, CodeModelLib.System.eFileType.Header);
				i_CNode.StartPointCPP = CodeModelLib.System.GetStartPoint(ce,vsCMPart.vsCMPartHeader, CodeModelLib.System.eFileType.Source);
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static private void AddCodeElements(TreeNode rootNode, CodeElements elements, bool children)
		{
			foreach (CodeElement element in elements)
			{
				CodeNode elementNode = new CodeNode();
				int index = GetCodeElementImageIndex(element);

				elementNode.Tag = element;
				elementNode.Text = GetCodeElementName(element, false);
				elementNode.Kind = CodeNode.CodeNodeType.CodeElement;
				elementNode.ImageIndex = index;
				elementNode.SelectedImageIndex = index;

				SetStartPoints( elementNode );

				if (children)
				{
					elementNode.ForeColor = System.Drawing.Color.Red;
				}

				rootNode.Nodes.Add(elementNode);

				CodeElements members = GetMembers(element);

				if (members != null && members.Count > 0)
				{
					// dummy node
					//elementNode.Nodes.Add(new TreeNode());
					//AddCodeElementChildren(elementNode);
				}
				else if (children)
				{
					try
					{
						if (element.Children.Count > 0)
						{
							// dummy node
							//elementNode.Nodes.Add(new TreeNode());
							//AddCodeElementChildren(elementNode);
						}
					}
					catch
					{
					}
				}
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static private CodeElements GetMembers(CodeElement element)
		{
			CodeElements members = null;

			// Retrieve collection for populating child nodes
			if (element.IsCodeType)
			{
				members = ((CodeType)element).Members;
			}
			else if (element.Kind == vsCMElement.vsCMElementNamespace)
			{
				members = ((CodeNamespace)element).Members;
			}
			else if (element.Kind == vsCMElement.vsCMElementFunction)
			{
				members = ((CodeFunction)element).Parameters;
			}

			return members;
		}


		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static private int GetProjectImageIndex(Project project)
		{
			int index = (int)NodeImageIndexes.Project;

			try
			{
				switch (project.CodeModel.Language.ToUpper())
				{
					case CodeModelLanguageConstants.vsCMLanguageCSharp:
						index = (int)NodeImageIndexes.CSProject;
						break;

					case CodeModelLanguageConstants.vsCMLanguageVB:
						index = (int)NodeImageIndexes.VBProject;
						break;

					case "{E6FDF8BF-F3D1-11D4-8576-0002A516ECE8}":  // Visual J#
						index = (int)NodeImageIndexes.VJProject;
						break;

					case CodeModelLanguageConstants.vsCMLanguageVC:
						index = (int)NodeImageIndexes.VCProject;
						break;
				}
			}
			catch
			{
			}

			return index;
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static private int GetProjectItemImageIndex(ProjectItem item)
		{
			int index;

			if (IsFolder(item))
			{
				index = (int)NodeImageIndexes.FolderClosed;
			}
			else
			{
				switch (System.IO.Path.GetExtension(item.Name).ToLower())
				{
					case ".cs":
						index = (int)NodeImageIndexes.CSFile;
						break;

					case ".vb":
						index = (int)NodeImageIndexes.VBFile;
						break;

					case ".jsl":
						index = (int)NodeImageIndexes.VJFile;
						break;

					case ".cpp":
						index = (int)NodeImageIndexes.VCFile;
						break;

					case ".hpp":
					case ".h":
						index = (int)NodeImageIndexes.HeaderFile;
						break;

					default:
						index = (int)NodeImageIndexes.File;
						break;
				}
			}

			return index;
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static private int GetCodeElementImageIndex(CodeElement element)
		{
			int index = (int)NodeImageIndexes.Unknown;

			switch (element.Kind)
			{
				case vsCMElement.vsCMElementNamespace:
					index = (int)NodeImageIndexes.Namespace;
					break;

				case vsCMElement.vsCMElementClass:
					index = (int)NodeImageIndexes.Class;
					break;

				case vsCMElement.vsCMElementInterface:
					index = (int)NodeImageIndexes.Interface;
					break;

				case vsCMElement.vsCMElementDelegate:
					index = (int)NodeImageIndexes.Delegate;
					break;

				case vsCMElement.vsCMElementStruct:
					index = (int)NodeImageIndexes.Struct;
					break;

				case vsCMElement.vsCMElementEnum:
					index = (int)NodeImageIndexes.Enum;
					break;

				case vsCMElement.vsCMElementFunction:
					index = (int)NodeImageIndexes.Function;
					break;

				case vsCMElement.vsCMElementProperty:
					index = (int)NodeImageIndexes.Property;
					break;

				case vsCMElement.vsCMElementVariable:
					index = (int)NodeImageIndexes.Variable;
					break;

				case vsCMElement.vsCMElementParameter:
					index = (int)NodeImageIndexes.Variable;
					break;
			}

			return index;
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static private string GetCodeElementName(CodeElement element, bool includeLabel)
		{
			string name = String.Empty;
			string label = String.Empty;

			switch (element.Kind)
			{
				case vsCMElement.vsCMElementOther:
					label = (includeLabel) ? "[Other] " : String.Empty;
					name = label + element.FullName;
					break;

				case vsCMElement.vsCMElementClass:
					label = (includeLabel) ? "[Class] " : String.Empty;
					name = label + ((CodeClass)element).Name;
					break;

				case vsCMElement.vsCMElementFunction:
					label = (includeLabel) ? "[Function] " : String.Empty;
					name = label + ((CodeFunction)element).Name;
					break;

				case vsCMElement.vsCMElementVariable:
					label = (includeLabel) ? "[Variable] " : String.Empty;
					name = label + ((CodeVariable)element).Name;
					break;

				case vsCMElement.vsCMElementProperty:
					label = (includeLabel) ? "[Property] " : String.Empty;
					name = label + ((CodeProperty)element).Name;
					break;

				case vsCMElement.vsCMElementNamespace:
					label = (includeLabel) ? "[Namespace] " : String.Empty;
					name = label + ((CodeNamespace)element).Name;
					break;

				case vsCMElement.vsCMElementParameter:
					label = (includeLabel) ? "[Parameter] " : String.Empty;
					name = label + ((CodeParameter)element).Name;
					break;

				case vsCMElement.vsCMElementAttribute:
					label = (includeLabel) ? "[Attribute] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementInterface:
					label = (includeLabel) ? "[Interface] " : String.Empty;
					name = label + ((CodeInterface)element).Name;
					break;

				case vsCMElement.vsCMElementDelegate:
					label = (includeLabel) ? "[Delegate] " : String.Empty;
					name = label + ((CodeDelegate)element).Name;
					break;

				case vsCMElement.vsCMElementEnum:
					label = (includeLabel) ? "[Enum] " : String.Empty;
					name = label + ((CodeEnum)element).Name;
					break;

				case vsCMElement.vsCMElementStruct:
					label = (includeLabel) ? "[Struct] " : String.Empty;
					name = label + ((CodeStruct)element).Name;
					break;

				case vsCMElement.vsCMElementUnion:
					label = (includeLabel) ? "[Union] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementLocalDeclStmt:
					label = (includeLabel) ? "[LocalDeclStmt] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementFunctionInvokeStmt:
					label = (includeLabel) ? "[FunctionInvokeStmt] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementPropertySetStmt:
					label = (includeLabel) ? "[PropertySetStmt] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementAssignmentStmt:
					label = (includeLabel) ? "[AssignmentStmt] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementInheritsStmt:
					label = (includeLabel) ? "[InheritsStmt] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementImplementsStmt:
					label = (includeLabel) ? "[ImplementsStmt] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementOptionStmt:
					label = (includeLabel) ? "[OptionStmt] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementVBAttributeStmt:
					label = (includeLabel) ? "[VBAttributeStmt] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementVBAttributeGroup:
					label = (includeLabel) ? "[VBAttributeGroup] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementEventsDeclaration:
					label = (includeLabel) ? "[EventsDeclaration] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementUDTDecl:
					label = (includeLabel) ? "[UDTDecl] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementDeclareDecl:
					label = (includeLabel) ? "[DeclareDecl] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementDefineStmt:
					label = (includeLabel) ? "[DefineStmt] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementTypeDef:
					label = (includeLabel) ? "[TypeDef] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementIncludeStmt:
					label = (includeLabel) ? "[IncludeStmt] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementUsingStmt:
					label = (includeLabel) ? "[UsingStmt] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementMacro:
					label = (includeLabel) ? "[Macro] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementMap:
					label = (includeLabel) ? "[Map] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementIDLImport:
					label = (includeLabel) ? "[IDLImport] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementIDLImportLib:
					label = (includeLabel) ? "[IDLImportLib] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementIDLCoClass:
					label = (includeLabel) ? "[IDLCoClass] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementIDLLibrary:
					label = (includeLabel) ? "[IDLLibrary] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementImportStmt:
					label = (includeLabel) ? "[ImportStmt] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementMapEntry:
					label = (includeLabel) ? "[MapEntry] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementVCBase:
					label = (includeLabel) ? "[VCBase] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementEvent:
					label = (includeLabel) ? "[Event] " : String.Empty;
					name = label + element.Name;
					break;

				case vsCMElement.vsCMElementModule:
					label = (includeLabel) ? "[Module] " : String.Empty;
					name = label + element.Name;
					break;

				default:
					label = (includeLabel) ? "[Unknown] " : String.Empty;
					name = label + element.FullName;
					break;
			}

			return name;
		}


		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static private bool ProjectHasChildren(Project project)
		{
			bool val = false;

			foreach (ProjectItem item in project.ProjectItems)
			{
				if (IsFolder(item) || item.FileCodeModel != null)
				{
					val = true;
					break;
				}
			}

			return val;
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static private bool FolderHasChildren(ProjectItem folder)
		{
			bool val = false;

			foreach (ProjectItem item in folder.ProjectItems)
			{
				if (IsFolder(item) || item.FileCodeModel != null)
				{
					val = true;
					break;
				}
			}

			return val;
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static private bool IsFolder(ProjectItem item)
		{
			bool val = false;
			string[] folderGUIDs = 
				{
					"{6BB5F8EF-4483-11D3-8BCF-00C04F8EC28C}",  // GUID for VB, C#, and J# folders
					"{6BB5F8F0-4483-11D3-8BCF-00C04F8EC28C}"   // GUID for VC folders
				};

			foreach (string guid in folderGUIDs)
			{
				if (item.Kind.ToUpper() == guid)
				{
					val = true;
					break;
				}
			}

			return val;
		}

		//--------------------------------------------------------------------
		//	variables
		//--------------------------------------------------------------------
		static private CodeNode	m_SolutionNode = null;
		static private _DTE		m_ApplicationObject = null;
		static private int		m_InitReferences = 0;
	}
}
