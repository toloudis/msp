//****************************************************************************
///  \file CodePlugIn
///
///	The main plug-in interface
///
///	This plug-in does a variety of things for the programmer.
///
///	Here is a list:
///		- auto-creation of comment blocks
///		- auto-creation of code blocks (switch, for, etc.)
///		- swap from hpp <-> cpp
///		- 
///
///	Extra Large Technology
///	Copyright(C) 2005 - All Rights Reserved
//****************************************************************************


//============================================================================
///	CodePlugIn namespace
//============================================================================
namespace CodePlugIn
{
	using EnvDTE;
	using Extensibility;
	using Microsoft.Office.Core;
	using Microsoft.VisualStudio.VCProjectEngine;
	using System;
	using System.Collections;
	using System.Configuration;
	using System.IO;
	using System.Runtime.InteropServices;
	using System.Text.RegularExpressions;


	#region Read me for Add-in installation and setup information.
	// When run, the Add-in wizard prepared the registry for the Add-in.
	// At a later time, if the Add-in becomes unavailable for reasons such as:
	//   1) You moved this project to a computer other than which is was originally created on.
	//   2) You chose 'Yes' when presented with a message asking if you wish to remove the Add-in.
	//   3) Registry corruption.
	// you will need to re-register the Add-in by building the MyAddin21Setup project 
	// by right clicking the project in the Solution Explorer, then choosing install.
	#endregion
	
	//========================================================================
	/// the main connection for the plug-in.
	//========================================================================
	[GuidAttribute("F7E33483-2422-4299-8067-A8C12A45EB48"), ProgId("CodePlugIn.Connect")]
	public class Connect : Object, Extensibility.IDTExtensibility2, IDTCommandTarget
	{
		#region envDTE connectivity
		//--------------------------------------------------------------------
		///	Implements the constructor for the Add-in object.
		///	Place your initialization code within this method.
		//--------------------------------------------------------------------
		public Connect()
		{
		}

		//--------------------------------------------------------------------
		///	Implements the OnConnection method of the IDTExtensibility2 interface.
		///	Receives notification that the Add-in is being loaded.
		///	
		///	\param application Root object of the host application.
		///	\param connectMode Describes how the Add-in is being loaded.
		///	\param addInInst Object representing this Add-in.
		//--------------------------------------------------------------------
		public void OnConnection(object application, Extensibility.ext_ConnectMode connectMode, object addInInst, ref System.Array custom)
		{
			//
			//	Initialize variables
			//
			m_Application	= (_DTE)application;
			m_AddInInstance	= (AddIn)addInInst;

			//
			//	Initialize Objects
			//
			DevEnvLib.DebugOutput.Init( m_Application );

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			//	Handlers
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			// Hook into the Line Event Change Handler	
			m_TextEditEvents = m_Application.Events.get_TextEditorEvents(null);
			m_TextEditEvents.LineChanged += new _dispTextEditorEvents_LineChangedEventHandler(_TextEditEvents_LineChanged);

			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			//	Configure the GUI
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			//if(connectMode == Extensibility.ext_ConnectMode.ext_cm_UISetup)
			if (   (connectMode == Extensibility.ext_ConnectMode.ext_cm_Startup)
				||
				   (connectMode == Extensibility.ext_ConnectMode.ext_cm_AfterStartup))
			{
				DevEnvLib.DebugOutput.Message("Connect Mode {0}", connectMode.ToString());

				ConfigureGUI();

				CreatePlugIns();
			}
		}

		//--------------------------------------------------------------------
		///	Implements the OnDisconnection method of the IDTExtensibility2 interface.
		///	Receives notification that the Add-in is being unloaded.
		///     
		/// \param disconnectMode Describes how the Add-in is being unloaded.
		/// \param custom Array of parameters that are host application specific.
		//--------------------------------------------------------------------
		public void OnDisconnection(Extensibility.ext_DisconnectMode disconnectMode, ref System.Array custom)
		{
			//	tool menu
			m_XLTCodeCB = (CommandBar)m_Application.CommandBars[c_EXTRALARGE_CODEMENUNAME];
			DeleteCommandFromCommandBar( m_XLTCodeCB );

			//	menu
			m_XLTMenuCB = (CommandBar)m_Application.CommandBars[c_EXTRALARGE_MENUNAME];
			DeleteCommandFromCommandBar( m_XLTMenuCB );
		}

		//--------------------------------------------------------------------
		///	Implements the OnAddInsUpdate method of the IDTExtensibility2 interface.
		///	Receives notification that the collection of Add-ins has changed.
		/// \param custom Array of parameters that are host application specific.
		//--------------------------------------------------------------------
		public void OnAddInsUpdate(ref System.Array custom)
		{
		}

		//--------------------------------------------------------------------
		///	Implements the OnStartupComplete method of the IDTExtensibility2 interface.
		///	Receives notification that the host application has completed loading.
		///	
		/// \param custom Array of parameters that are host application specific.
		//--------------------------------------------------------------------
		public void OnStartupComplete(ref System.Array custom)
		{
			try
			{
				CreateEditLineEvents();
			}
			catch(System.Exception e)
			{
				DevEnvLib.DebugOutput.Message("Exception {0}", e.ToString() );
				//using (StreamWriter sw = new StreamWriter(@"C:\VSRefactorAddinError.Txt"))
				//{
				//	sw.WriteLine("--------- {0}------", DateTime.Now);
				//	sw.WriteLine(e.Message);
				//	sw.WriteLine(e.StackTrace);
				//}
			}
		}

		//--------------------------------------------------------------------
		///	Implements the OnBeginShutdown method of the IDTExtensibility2 interface.
		///	Receives notification that the host application is being unloaded.
		///	
		/// \param custom Array of parameters that are host application specific.
		//--------------------------------------------------------------------
		public void OnBeginShutdown(ref System.Array custom)
		{
		}

		//--------------------------------------------------------------------
		///	Implements the QueryStatus method of the IDTCommandTarget interface.
		///	This is called when the command's availability is updated
		///	
		/// \param commandName The name of the command to determine state for.
		/// \param neededText Text that is needed for the command.
		/// \param status The state of the command in the user interface.
		/// \param commandText Text requested by the neededText parameter.
		//--------------------------------------------------------------------
		public void QueryStatus(string cmdName, EnvDTE.vsCommandStatusTextWanted i_NeededText, ref EnvDTE.vsCommandStatus status, ref object commandText)
		{
			if (i_NeededText == EnvDTE.vsCommandStatusTextWanted.vsCommandStatusTextWantedNone)
			{
				//message("Querying Status for command{0}", cmdName);
				//checkControl(m_Application.CommandBars["Class View Item"], "Generate Collection Class");

				foreach (DevEnvLib.VSPlugIn plugin in m_Plugins)
				{
					if (plugin.qualifiedName.ToUpper() == cmdName.ToUpper())
					{
						status = plugin.QueryStatus(plugin.qualifiedName, m_Application);
						//message("Query Status for command{0} returned {1}", cmdName, status);
						return;
					} 				
				}

				status = vsCommandStatus.vsCommandStatusUnsupported;
				DevEnvLib.DebugOutput.Message("Query status for command {0} not supported", cmdName);
				DevEnvLib.DebugOutput.Message("    needed text {0}", i_NeededText);
			}
		}

		//--------------------------------------------------------------------
		///	Implements the Exec method of the IDTCommandTarget interface.
		///	This is called when the command is invoked.
		///	
		/// \param commandName The name of the command to execute.
		/// \param executeOption Describes how the command should be run.
		/// \param varIn Parameters passed from the caller to the command handler.
		/// \param varOut Parameters passed from the command handler to the caller.
		/// \param handled Informs the caller if the command was handled or not.
		//--------------------------------------------------------------------
		public void Exec(string commandName, EnvDTE.vsCommandExecOption executeOption, ref object varIn, ref object varOut, ref bool handled)
		{
			handled = false;

			if (executeOption == EnvDTE.vsCommandExecOption.vsCommandExecOptionDoDefault)
			{
				foreach (DevEnvLib.VSPlugIn plugin in m_Plugins)
				{
					if (plugin.qualifiedName == commandName)
					{
						handled = plugin.Exec(commandName, m_Application);

						DevEnvLib.DebugOutput.Message("Exec Completed for {0}", commandName); 
						return;
					}
				}

				DevEnvLib.DebugOutput.Message("No Exec for {0}", commandName); 
			}
		}
		#endregion

		#region handler functions
		//--------------------------------------------------------------------
		/// Line has changed in the TextEditor
		/// 
		/// \param StartPointThe starting point of the edit
		/// \param EndPointThe end point of the edit
		/// \param HintHint code
		//--------------------------------------------------------------------
		private void _TextEditEvents_LineChanged(TextPoint StartPoint, TextPoint EndPoint, int Hint)
		{
			// Only process if edit event occured
			if (Hint == 0) return;

			// Only Process C/C++ Applications
			if (m_Application.ActiveDocument == null) return;
			if (m_Application.ActiveDocument.Language != "C/C++") return;

			// Only process if this is a header file
			//if (DocHelper.IsHeader(m_Application.ActiveDocument) != true) return;

			// Get handle to current cursor position
			TextSelection ts = (TextSelection) m_Application.ActiveWindow.Selection;
			EditPoint ep = ts.ActivePoint.CreateEditPoint();	        

			ep.StartOfLine();
			ep.LineUp(1);
			ep.StartOfLine();

			foreach (DevEnvLib.EditLineEvent ELEvent in m_EditLineEvents)
			{
				if (ELEvent.HandleEvent( ref ep, ref ts ) == true)
				{
					//	handled the event
					return;
				} 				
			}
		}
		#endregion

		#region utility functions
		//--------------------------------------------------------------------
		/// Delete the commands from a commandbar
		//--------------------------------------------------------------------
		private void DeleteCommandFromCommandBar( CommandBar i_CBar )
		{
			CommandBarPopup cbPopup = (CommandBarPopup)i_CBar;

			foreach (CommandBarControl control in cbPopup.CommandBar.Controls)
			{
				control.Delete(System.Reflection.Missing.Value);
			}
		}

		//--------------------------------------------------------------------
		/// Configure the menus, toolbars, etc.
		//--------------------------------------------------------------------
		private void ConfigureGUI()
		{
			//	set-up the tools sub-menu
			//	tool menu
			m_XLTMenuCB = DevEnvLib.DocHelper.InsertSubMenu(m_Application, "Tools", c_EXTRALARGE_MENUNAME);
			CommandBarPopup cbarpopup = (CommandBarPopup)m_XLTMenuCB.Parent;

			cbarpopup.Tag = c_EXTRALARGE_MENUNAME;
			cbarpopup.DescriptionText = "XLT Tools Description";
			cbarpopup.BeginGroup = true;
			cbarpopup.Visible	= true;

			//	set-up the tools sub-menu
			//
			m_XLTCodeCB = DevEnvLib.DocHelper.InsertSubMenu(m_Application, "Code Window", c_EXTRALARGE_CODEMENUNAME);

			//	debugging only
			//DevEnvLib.DebugOutput.Message("Application CommandBars");
			//foreach( CommandBar cb in m_Application.CommandBars )
			//{
			//	DevEnvLib.DebugOutput.Message("    {0}", cb.Name.ToString());
			//}
		}

		//--------------------------------------------------------------------
		///	create the "sub" plug-ins for this plugin
		//--------------------------------------------------------------------
		private void CreatePlugIns()
		{
			DevEnvLib.VSPlugIn plugin;
			//	menu plug-ins
			//
			plugin = new TestPlugin(m_Application, m_AddInInstance);
			m_Plugins.Add(plugin);
			plugin.InsertCommand(m_XLTMenuCB, false);

			//	code plug-ins
			//
			plugin = new WhatsThisCommentPlugin(m_Application, m_AddInInstance);
			m_Plugins.Add(plugin);
			plugin.InsertCommand(m_XLTCodeCB, false);

			plugin = new WhatsThisPlugin(m_Application, m_AddInInstance);
			m_Plugins.Add(plugin);
			plugin.InsertCommand(m_XLTCodeCB, false);

			plugin = new UpdateCommentPlugIn(m_Application, m_AddInInstance);
			m_Plugins.Add(plugin);
			plugin.InsertCommand(m_XLTCodeCB, false);

			plugin = new TestCommentsMatchPlugIn(m_Application, m_AddInInstance);
			m_Plugins.Add(plugin);
			plugin.InsertCommand(m_XLTCodeCB, false);

			plugin = new GetSetPlugin(m_Application, m_AddInInstance);
			m_Plugins.Add(plugin);
			plugin.InsertCommand(m_XLTCodeCB, false);

			plugin = new SwapCppHppPlugin(m_Application, m_AddInInstance);
			m_Plugins.Add(plugin);
			plugin.InsertCommand(m_XLTCodeCB, false);
		}
		
		//--------------------------------------------------------------------
		///	create the EditLine events
		//--------------------------------------------------------------------
		private void CreateEditLineEvents()
		{
			DevEnvLib.EditLineEvent ELEvent;

			ELEvent = new DevEnvLib.SwitchEditLineEvent();
			m_EditLineEvents.Add( ELEvent );

			ELEvent = new DevEnvLib.ForEditLineEvent();
			m_EditLineEvents.Add( ELEvent );

			//ELEvent = new CommentBuilder.CommentEditLineEvent();
			//m_EditLineEvents.Add( ELEvent );
		}
		#endregion

		//--------------------------------------------------------------------
		//	variables
		//--------------------------------------------------------------------
		private const string c_CMDQUALIFIER = "CodePlugIn.Connect.";
		private const string c_EXTRALARGE_CODEMENUNAME = "XLT Code Utils";
		private const string c_EXTRALARGE_MENUNAME = "Extra Large Tools";

		private EnvDTE.TextEditorEvents m_TextEditEvents;

		private ArrayList m_Plugins = new ArrayList();			/// contains all the plug-in objects
		private ArrayList m_EditLineEvents = new ArrayList();	/// contains all the EditLine events
		private string m_privateVarCase = string.Empty;

		private	CommandBar m_XLTCodeCB;
		private	CommandBar m_XLTMenuCB;

		private AddIn m_AddInInstance;
		private _DTE m_Application;
	}
}