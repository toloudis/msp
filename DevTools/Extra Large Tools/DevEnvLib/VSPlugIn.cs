//****************************************************************************
/// \file VSPlugIn.cs
///
///	base class for the "sub" plug-ins.
///
///	Extra Large Technology
///	Copyright(C) 2005 - All Rights Reserved
//****************************************************************************
using System;
using Microsoft.Office.Core;
using System.Runtime.InteropServices;
using EnvDTE;


//============================================================================
///
//============================================================================
namespace DevEnvLib
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------ 
	public abstract class VSPlugIn
	{ 
		#region implement these abstract properties
		//--------------------------------------------------------------------
		/// this is the fully qualified name that comes back in the exec and query
		/// events. e.g.
		/// return "RefactorAddIn.Connect.MakeProperty";
		//--------------------------------------------------------------------
		public string qualifiedName
		{
			get
			{
				return (m_AddInInstance.ProgID + "." + cmdName);
			}
		}

		//--------------------------------------------------------------------
		/// the short command (last part of the above)
		/// return "MakeProperty";
		//--------------------------------------------------------------------
		public abstract string cmdName
		{
			get ;
		}
		
		//--------------------------------------------------------------------
		/// short description is what the menu will display as its item
		//--------------------------------------------------------------------
		public abstract string shortDescription{get ; }
		
		//--------------------------------------------------------------------
		/// the long description is available for the tool help
		//--------------------------------------------------------------------
		public abstract string longDescription { get ; }
		
		//--------------------------------------------------------------------
		/// set to 1 if you want it to be at the start of the menu
		/// set to -1 if at the end of the menu
		//--------------------------------------------------------------------
		public abstract int position { get ; }

		//--------------------------------------------------------------------
		/// provide a number for the icons, e.g. 54
		//--------------------------------------------------------------------
		public abstract int iconId { get ; }
		#endregion

		#region public methods
		//--------------------------------------------------------------------
		/// 
		//--------------------------------------------------------------------
		public EnvDTE.vsCommandStatus QueryStatus(string cmdName, _DTE applicationObject)
		{
			if (cmdName == qualifiedName)
			{
				try
				{
					getEnvironment();
					return doQueryStatus();
				}
				catch (Exception ex)
				{
					DebugOutput.Message("QueryStatus raised exception {0}", ex.Message);
					DebugOutput.Message("   stacktrace {0}", ex.StackTrace);

					return EnvDTE.vsCommandStatus.vsCommandStatusUnsupported;
				}
			}
			else return EnvDTE.vsCommandStatus.vsCommandStatusUnsupported;
		}

		//--------------------------------------------------------------------
		/// 
		//--------------------------------------------------------------------
		public bool Exec(string cmdName, _DTE applicationObject)
		{
			if (cmdName == qualifiedName)
			{
				DevEnvLib.DebugOutput.Message("Execute {0}", cmdName);

				getEnvironment();
				return doExec();
			}
			return false;
		}

		//--------------------------------------------------------------------
		/// add a command to a command bar
		/// 
		/// \param popup
		/// \param cmd
		/// \param shortDesc
		/// \param longDesc
		/// \param position
		/// \param icon
		/// 
		///	\return a command
		//--------------------------------------------------------------------
		public Command InsertCommand(CommandBar cmdBar, bool deleteIfExists)
		{
			if (cmdBar == null)
			{
				throw new ArgumentNullException("CmdBar", "Commandbar cannot be null");
			}

			getEnvironment();

			Command command = null;

			// Command exists
			//
			if (HasCommand( qualifiedName,out command) && deleteIfExists)
			{
				command.Delete();
				command = null;
			}

			//	if the command doesn't exist then create it.
			//
			if (command == null)
			{
				object []contextGUIDS = new object[] { };
				Commands commands = m_ApplicationObject.Commands;
		
				command = commands.AddNamedCommand(
					m_AddInInstance,
					cmdName,
					shortDescription,
					longDescription,
					true,
					iconId,
					ref contextGUIDS,
					(int)vsCommandStatus.vsCommandStatusSupported + (int)vsCommandStatus.vsCommandStatusEnabled
					);
			}

			//	flush these commands
			//
			flushControls(cmdBar, shortDescription);
			flushControls(cmdBar, string.Empty);		// why?

			//	add the command
			//
			CommandBarControl commandBarControl = command.AddControl(cmdBar, position);
			commandBarControl.Enabled = true;
			commandBarControl.Visible = true;
			return command;
		}
		#endregion

		#region protected methods
		//--------------------------------------------------------------------
		/// return what is currently selected in the class view. 
		/// 
		/// \return null if class view window is not the active window
		//--------------------------------------------------------------------
		protected CodeElement selectedElementInClassView(bool mustBeActive)
		{
			Window ClassView = m_ApplicationObject.Windows.Item(Constants.vsWindowKindClassView) as Window;
			Window active = m_ApplicationObject.ActiveWindow;
			if ((active != ClassView) && mustBeActive)
				return null;

			if (m_ApplicationObject.SelectedItems == null)
				return null;

			SelectionContainer container = m_ApplicationObject.SelectedItems.SelectionContainer;
			if (container == null)
				return null;

			Object obj = container.Item(1);
			if (obj == null) 
				return null;			

			CodeElement e = obj as CodeElement;
			return e;
		}

		//--------------------------------------------------------------------
		/// what is the current project of the active window
		//--------------------------------------------------------------------
		public Project CurrentProject
		{
			get 
			{
				if (projectItem != null)
					return projectItem.ContainingProject;
				else
					return m_ApplicationObject.ActiveWindow.Project;
			}
		}

		//--------------------------------------------------------------------
		/// return the currently selected code element
		/// this kind of code element must be specified.
		/// This function checks first the class view (which must be active)
		/// and then the file code model
		/// 
		/// \param kind the kind of element looked for
		/// 
		/// \return the element or null if no such element is selected
		//--------------------------------------------------------------------
		protected CodeElement selectedElement(vsCMElement kind)
		{
			CodeElement ce = selectedElementInClassView(true);
			if (ce == null)
				ce = selectedElementInFileCodeModel(kind);
			if (ce != null)
				if (ce.Kind == kind)
					return ce;
			return null;
		}

		//--------------------------------------------------------------------
		/// find the currently selected element in the file code model
		/// 
		/// \param kind specify which kind of element is wanted
		/// 
		/// \return 
		//--------------------------------------------------------------------
		protected CodeElement selectedElementInFileCodeModel(vsCMElement kind)
		{
			if (fileCodeModel == null)
				return null;
			if (Start == null)
				return null;
			
			try
			{
				theElement = fileCodeModel.CodeElementFromPoint(Start, kind);
				if (theElement.Kind == kind) 
					return theElement;
				else 
					return null;
			}
			catch (System.Exception /*ex*/)
			{
				//DevEnvLib.DebugOutput.Message("selectedElementInFileCodeModel has exception ({0})", ex.Message);
				return null;
			}
		}

		//--------------------------------------------------------------------
		/// delete all existing controls on the commandbar with this caption
		/// 
		/// Note: will this work with other languages?  Should it use ID?
		/// 
		/// \param cmdBar
		/// \param caption
		//--------------------------------------------------------------------
		protected void flushControls( CommandBar cmdBar, string caption)
		{
			foreach (CommandBarControl control in cmdBar.Controls)
			{
				if (control.Caption == caption)
				{
					control.Delete(System.Reflection.Missing.Value);
				}
			}
		}

		//--------------------------------------------------------------------
		/// 
		//--------------------------------------------------------------------
		protected void getEnvironment()
		{
			//m_ApplicationObject = applicationObject;
			doc = (Document)m_ApplicationObject.ActiveDocument; 
			if (doc != null)
			{
				projectItem = (ProjectItem)doc.ProjectItem ;
				if (projectItem != null)
				{
					fileCodeModel = (FileCodeModel) projectItem.FileCodeModel;
					selection = (TextSelection)m_ApplicationObject.ActiveWindow.Selection;
					if (selection != null)
						_Start = selection.TopPoint.CreateEditPoint();
					else
						_Start = null;
				}
			}
		}

		//--------------------------------------------------------------------
		/// does a command already exist?
		/// 
		/// \param commName
		/// \param command
		/// \return 
		//--------------------------------------------------------------------
		protected bool HasCommand(string commName,out Command command)
		{
			try
			{
				// Get all commands from current application.
				Commands commands = m_ApplicationObject.Commands;
				// Get a reference of the a command based on the command name
				command = commands.Item(commName,-1);
				return true;
			}
			catch (System.Exception)
			{
				//Ignore
			}

			// we must assign the out object before leaving method.
			command = null;
			return false;
		}
		#endregion

		#region implement these abstract methods
		//--------------------------------------------------------------------
		/// 
		//--------------------------------------------------------------------
		protected abstract EnvDTE.vsCommandStatus doQueryStatus();

		//--------------------------------------------------------------------
		/// 
		//--------------------------------------------------------------------
		protected abstract bool doExec();
		#endregion

		#region construction
		//--------------------------------------------------------------------
		/// 
		//--------------------------------------------------------------------
		protected VSPlugIn(_DTE applicationObject, AddIn addInInstance)
		{
			if (addInInstance == null)
				throw new ArgumentNullException("addInInstance", "addInInstance cannot be null");

			m_ApplicationObject = applicationObject;
			m_AddInInstance = addInInstance;
		}
		#endregion

		#region protected  fields
		//--------------------------------------------------------------------
		/// variables
		//--------------------------------------------------------------------
		protected Document doc = null; 
		protected TextSelection selection = null;
		protected ProjectItem projectItem = null;
		protected FileCodeModel fileCodeModel = null;
		private CodeElement theElement = null;

		protected EnvDTE.CodeElement XTheElement
		{
			get {  return theElement; }
			set { theElement = value; }    
		}
	
		private EditPoint _Start  = null;

		protected EnvDTE.EditPoint Start
		{
			get {  return _Start; }
		}
	
		protected _DTE m_ApplicationObject = null;
		protected AddIn m_AddInInstance;
		#endregion
	}
}
