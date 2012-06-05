/*****************************************************************************
**	mslMetaSLMgr.cpp
**
**	 see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "mslMetaSLMgr.hpp"
#include "mslCompiler.hpp"
#include "mslEffectCompiler.hpp"
#include "mslErrors.hpp"
#include "mslGraphView.hpp"
#include "mslParameterView.hpp"
#include "mslShaderOutputView.hpp"
#include "mslToolView.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/Eff/effShaderParams.hpp"
#include "Graphics/Mat/matMaterial.hpp"
#include "Graphics/Mat/matMetaFX.hpp"
#include "Graphics/Mat/matMetaFXParser.hpp"
#include "Tool/gui/guiCursor.hpp"
#include "Tool/gui/guiMainWindow.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"


#pragma comment(lib, "mentalmill.lib")


namespace
{
    IMill *m_mill = 0;						//!< Primary mental mill interface
	IMill_gui *m_mill_gui = 0;				//!< mental mill GUI interface
    IMill_graph *m_mill_graph = 0;			//!< mental mill graph interface
	IMill_compiler *m_mill_compiler = 0;	//!< mental mill compiler interface
    IGraph_library_manager *m_toolbox = 0;	//!< mental mill toolbox manager
	IGraph_library *m_graph_library = 0;	//!< mental mill node classes interface
	IGraph_view *m_graph_view = 0;			//!< mental mill graph view interface
    ITool_view *m_tool_view = 0;			//!< mental mill tool view interface
    IParameter_view *m_parameter_view = 0;	//!< mental mill param view interface

	// Our dialogs
	mslGraphView *l_pGraphViewPane = NULL;
	mslToolView *l_pToolViewPane = NULL;
	mslParameterView *l_pParameterViewPane = NULL;
	mslShaderOutputView *l_pShaderOutputView = NULL;

	fsLocator l_TextureDirectory;	// directory of MentalMill textures

	//bga - I am not sure why these ids would be needed...

	//! The id of the graph view.
    static const wxWindowID id_graph_view =   0x100;

    //! The id of the tool view.
    static const wxWindowID id_tool_view =    0x101;
	
	// Load a toolbox into the application
	void LoadToolbox(const char *path)
	{
		if(m_toolbox)
			m_toolbox->release();
		ICompiler_errors * errors = m_mill_compiler->create_errors();
		m_toolbox = m_mill_graph->load_toolbox(path, errors);
		if (errors->count() > 0)
			mslErrors::ReportErrors("Errors while loading toolbox ", errors);
		errors->release();
		if(!m_toolbox) {
			DBG_ERROR("Failed to load toolbox " << path);
			return;
		}
		if(m_graph_view)
			m_graph_view->set_graph_library_manager(m_toolbox);
		//if(m_tool_view) {
		//	m_tool_view->release();
		//	SocketWidget *socket_widget;
		//	Uint64 handle = get_handle(m_tool_window, socket_widget);
		//	m_tool_view = m_mill_gui->create_tool_view(handle,
		//		id_tool_view,
		//		m_toolbox);
		//	m_tool_window->SetView(m_tool_view);
		//	if(m_tool_window) {
		//		m_tool_window->SetView(m_tool_view);
		//		m_tool_window->Resize(m_tool_window->GetSize());
		//		m_frame->Refresh();
		//	}
		//}
	}

	// disconnect a given attachment
	void disconnect_attachment(IGraph_node* i_pNode,
							   IAttachment_iterator *attachment,
							   const std::string &param_name)
	{
		std::string member_name = param_name;
		if (attachment->get_member_name())
		{
			member_name += ".";
			member_name += attachment->get_member_name();
		}
		
		i_pNode->remove_input_attachment(member_name.c_str());

	}

	// disconnect all input attachments to the given node.
	void disconnect_inputs(IGraph_node* i_pNode)
	{
		std::string param_name;
		IParameter_iterator* params = i_pNode->get_parameters( IGraph_node_class::INPUT );
		for (params->to_first(); !params->at_end(); params->to_next())
		{
			param_name = params->get_name();
			IAttachment_iterator *attachments = params->get_attachments();
			for (attachments->to_first(); !attachments->at_end(); attachments->to_next())
			{
				// Have an attachment on this parameter, disconnect it.
				disconnect_attachment(i_pNode, attachments, param_name);
			}
			attachments->release();
		}
		params->release();
	}

	// disconnect all output attachments to the given node.
	void disconnect_outputs(IGraph_node* i_pNode)
	{
		std::string param_name;
		IParameter_iterator* params = i_pNode->get_parameters( IGraph_node_class::OUTPUT );
		for (params->to_first(); !params->at_end(); params->to_next())
		{
			param_name = params->get_name();
			//DBG_LOG("Param: " << param_name);

			IAttachment_iterator *attachments = params->get_attachments();
			for (attachments->to_first(); !attachments->at_end(); attachments->to_next())
			{
				if (attachments->get_target_node())
				{
					//DBG_LOG("Target name: " << attachments->get_target_name());
					// Have an attachment on this parameter, disconnect it.
					attachments->get_target_node()->remove_input_attachment(attachments->get_target_name());
				}
			}

			attachments->release();
		}
		params->release();
	}
}

//--------------------------------------------------------------------
// Initialize
//--------------------------------------------------------------------
bool  mslMetaSLMgr::Initialize(const fsLocator& i_DataDirectory)
{
//   m_graph_event_handler = new MMGuiGraphEventHandler(this);
	
	// get the mental mill interface
    m_mill = initialize_mill();
	if (!m_mill) {
		fprintf(stderr, "Unable to initialize mental mill.\n");
        return false;
	}

	// Have to expressly load the gen_fx plugin in 1.2
	//m_mill->load_library_from_file("gen_fx");

	// Store the texture directory for later use
	l_TextureDirectory = i_DataDirectory;
	l_TextureDirectory.Push("textures");

	// get the application directory so that even if the executable is
	// started from a different directory, mental mill data will be found
 //   std::string data = GetApplicationDirectory() + "/../../../../data";
    std::string data;
	fsFileUtil::LocatorToANSIFilename(i_DataDirectory, data);
    std::string shaders = data + "/shaders";
    std::string textures = data + "/textures";
    std::string icons = data + "/icons";
    std::string toolbox = shaders + "/mill_toolbox.tbx";

	// set paths where mental mill will look for shaders, textures, and icons
    m_mill->set_shader_path(shaders.c_str());
    m_mill->set_texture_path(textures.c_str());
    m_mill->set_icon_path(icons.c_str());

	// get the main GUI interface from the mental mill interface
    m_mill_gui = (IMill_gui *)m_mill->get_interface(I_MILL_GUI);
	if(!m_mill_gui) {
		fprintf(stderr, "Unable to initialize mental mill GUI interface.\n");
        return false;
    }

	// get the main graph interface from the mental mill interface
    m_mill_graph = (IMill_graph *)m_mill->get_interface(I_MILL_GRAPH);
    if(!m_mill_graph) {
		fprintf(stderr, "Unable to initialize mental mill graph interface.\n");
        fprintf(stderr,"failed to instantiate graph\n");
        return false;
    }
    
	// get the main compiler interface from the mental mill interface
	m_mill_compiler = (IMill_compiler *)m_mill->get_interface(I_MILL_COMPILER);
    if(!m_mill_compiler) {
		fprintf(stderr, "Unable to initialize mental mill compiler "
						"interface.\n");
		return false;
	}

	// Log some messages about what back ends are loaded...
	int num_backends = m_mill_compiler->get_back_end_count();
		DBG_LOG("Num Back ends: " << num_backends);
	for (int i=0; i<num_backends; ++i)
	{
		DBG_LOG("Back end named: " << m_mill_compiler->get_back_end_type(i));
	}

	// load the mental mill toolbox
    LoadToolbox(toolbox.c_str());

	l_pGraphViewPane = new mslGraphView(twxSystem::g_pMainForm);
    Uint64 handle = (Uint64) l_pGraphViewPane->GetHWND();

	// create the graph library and set its event handler
    m_graph_library = m_mill_graph->create_graph_library();
    //m_graph_library->set_event_handler(m_graph_event_handler);

 //   // show the frame before creating graph view
 //   SetTopWindow(m_frame);
 //   m_frame->Show(true);

	// create the graph view and set it in the graph window
    m_graph_view = m_mill_gui->create_graph_view(handle,
    //m_graph_view = m_mill_gui->create_graph_view_wx(l_pGraphViewPane,
                                                    id_graph_view,
                                                    m_graph_library);
    l_pGraphViewPane->SetView(m_graph_view);

	// create the tool view and set it in the tool window
	l_pToolViewPane = new mslToolView(twxSystem::g_pMainForm);
    handle = (Uint64) l_pToolViewPane->GetHWND();

    m_tool_view = m_mill_gui->create_tool_view(handle,
    //m_tool_view = m_mill_gui->create_tool_view_wx(l_pToolViewPane,
                                                    id_tool_view,
                                                    m_toolbox);
    l_pToolViewPane->SetView(m_tool_view);
    m_graph_view->set_tool_view(m_tool_view);

	// create the parameter view and setit in the parameter window
	l_pParameterViewPane = new mslParameterView(twxSystem::g_pMainForm);
    handle = (Uint64) l_pParameterViewPane->GetHWND();
    m_parameter_view = m_mill_gui->create_parameter_view(handle,
    //m_parameter_view = m_mill_gui->create_parameter_view_wx(l_pParameterViewPane,
                                                    id_tool_view);
	l_pParameterViewPane->SetView(m_parameter_view);	
    m_graph_view->set_parameter_view(m_parameter_view);

	// Create shader output view
	l_pShaderOutputView = new mslShaderOutputView(twxSystem::g_pMainForm);

	return true;
}

//--------------------------------------------------------------------
// DeInitialize
//--------------------------------------------------------------------
void  mslMetaSLMgr::DeInitialize()
{

	// clean up mental mill interfaces
//    if(m_graph_event_handler)
 //       delete m_graph_event_handler;
    if(m_toolbox)
        m_toolbox->release();
    if(m_graph_library)
	{
		m_graph_library->destroy();
        m_graph_library->release();
	}
    if(m_graph_view)
        m_graph_view->release();
    if(m_tool_view)
        m_tool_view->release();
    if(m_mill_compiler)
        m_mill_compiler->release();
    if(m_mill_graph)
        m_mill_graph->release();
    if(m_mill_gui)
        m_mill_gui->release();
    if(m_mill)
        m_mill->release();
}

//--------------------------------------------------------------------
// Clear out any existing node graph
//--------------------------------------------------------------------
void mslMetaSLMgr::ClearGraph()
{
	if(m_graph_library)
	{
		m_graph_library->release();
		//m_graph_library = NULL;
		m_graph_library = m_mill_graph->create_graph_library();	// create new, empty graph
		if(m_graph_view)
			m_graph_view->set_graph_library(m_graph_library);
	}
}

//--------------------------------------------------------------------
// Create and load a graph into the application
//--------------------------------------------------------------------
bool mslMetaSLMgr::LoadGraph(const fsLocator &i_Locator)
{
    if (m_mill_compiler) {
        ICompiler_errors *errors = m_mill_compiler->create_errors();
		std::string path_str;
		fsFileUtil::LocatorToANSIFilename(i_Locator, path_str);
        IGraph_library *library = m_mill_graph->load_graph_library(path_str.c_str(),m_toolbox,errors);
        if(errors->count()) {
			mslErrors::ReportErrors("Failed to load graph library", errors);
        } else {
			m_graph_library->release();
			m_graph_library = library;
			//m_graph_library_path = path;
			//m_graph_library->set_event_handler(m_graph_event_handler);
			m_graph_view->set_graph_library(m_graph_library);
			m_graph_view->layout_graph();

			itString title = i_Locator.GetLastName();
			guiMainWindow::SetAppTitle(title);

			return true;
        }
    } 
	return false;
}

//--------------------------------------------------------------------
// Save a graph under a given file name in the application
//--------------------------------------------------------------------
void mslMetaSLMgr::SaveGraph(const fsLocator &i_Locator)
{
	std::string path_str;
	fsFileUtil::LocatorToANSIFilename(i_Locator, path_str);
    if (!m_graph_library->save(path_str.c_str())) 
	{
		DBG_ERROR("Failed to save graph to " << path_str);
    }
	else
	{
		itString title = i_Locator.GetLastName();
		guiMainWindow::SetAppTitle(title);
	}
}

//--------------------------------------------------------------------
// Layout graph nodes automatically.
//--------------------------------------------------------------------
void mslMetaSLMgr::LayoutGraph()
{
	m_graph_view->layout_graph();
}

//--------------------------------------------------------------------
// Pan and zoom such that whole graph is visible.
//--------------------------------------------------------------------
void mslMetaSLMgr::ZoomExtents()
{
	m_graph_view->zoom_extents();
}

//--------------------------------------------------------------------
// Move cursor to given source line number in shader output window
//--------------------------------------------------------------------
void mslMetaSLMgr::GotoSourceLine(int i_LineNum)
{
	l_pShaderOutputView->MoveToLine(i_LineNum);
}

//--------------------------------------------------------------------
// Delete selected nodes
//--------------------------------------------------------------------
void mslMetaSLMgr::DeleteSelectedNodes()
{
	// Get selected node from graph view
	int sel_count = m_graph_view->get_selected_node_count();
	if (sel_count > 0)
	{
		for (int i=0; i<sel_count; ++i)
		{
			IGraph_node* pSelNode = m_graph_view->get_selected_node(i);
			disconnect_inputs(pSelNode);
			disconnect_outputs(pSelNode);
			m_graph_library->remove_node(pSelNode);
		}
		l_pGraphViewPane->Refresh();
	}
}

//--------------------------------------------------------------------
// Disconnect inputs or outputs from selected nodes
//--------------------------------------------------------------------
void mslMetaSLMgr::DisconnectInputs()
{
	// Get selected node from graph view
	int sel_count = m_graph_view->get_selected_node_count();
	if (sel_count > 0)
	{
		for (int i=0; i<sel_count; ++i)
		{
			IGraph_node* pSelNode = m_graph_view->get_selected_node(i);
			disconnect_inputs(pSelNode);
		}
		l_pGraphViewPane->Refresh();
	}
}
void mslMetaSLMgr::DisconnectOutputs()
{
	// Get selected node from graph view
	int sel_count = m_graph_view->get_selected_node_count();
	if (sel_count > 0)
	{
		for (int i=0; i<sel_count; ++i)
		{
			IGraph_node* pSelNode = m_graph_view->get_selected_node(i);
			disconnect_outputs(pSelNode);
		}
		l_pGraphViewPane->Refresh();
	}
}

//--------------------------------------------------------------------
// Return directory containing MetaSL textures
//--------------------------------------------------------------------
const fsLocator& mslMetaSLMgr::GetTextureDirectory()
{
	return l_TextureDirectory;
}

//--------------------------------------------------------------------
// Compile node graph into HLSL shader 
// and put into the given material
//--------------------------------------------------------------------
bool mslMetaSLMgr::Compile(matMaterial* i_pShaderMaterial,
						   const fsLocator& i_EffectFilename)
{
	bool bResult = false;

	// Get selected node from graph view
	if (m_graph_view->get_selected_node_count() != 1)
	{
		guiMessageBox::Show("Select root node to export", "Select Root");
	}
	else
	{
		IGraph_node* pSelNode = m_graph_view->get_selected_node(0);

		// See if this node has a color output with the name "result"
		IParameter_iterator *pParamIterator  = pSelNode->get_parameters(IGraph_node_class::OUTPUT);
		pParamIterator->to_next("result");
		if (!pParamIterator->at_end())
		{
			guiCursor::SetWaitCursor();

			// Send to compiler to create shader
			std::string hlsl_shader_str, metasl_shader_str;
			std::string shader_name = pSelNode->get_name();
			if (mslCompiler::Compile(shader_name, m_mill_compiler, m_graph_library, 
									 hlsl_shader_str, metasl_shader_str))
			{
				l_pShaderOutputView->UpdateShaderString(hlsl_shader_str);
				DBG_LOG("Shader compilation successful.");

				// Try to compile shader
				shared_ptr<effShaderSDKData> compiledShaderData;
				matShaderEffect* pShaderEffect = mslEffectCompiler::CompileEffect(hlsl_shader_str, 
						l_TextureDirectory, compiledShaderData);
				DBG_LOG("DX11 compilation: " << ((pShaderEffect) ? "succeeded" : "failed"));

				if (pShaderEffect)
				{
					// Put the pShaderEffect into the i_pShaderMaterial
					shared_ptr<effShaderParams> params(new effShaderParams());
					fsLocator effect_filename = (i_EffectFilename.GetNumNames() > 0) ? i_EffectFilename : fsLocator(itString("CustomShader.mfx"));
					params->SetShaderName(effect_filename, pShaderEffect);
					pShaderEffect->BuildPrtyObject(params.get());
					i_pShaderMaterial->SetShaderParams(0, params);
					bResult = true;
					
					// Write compiled shader to file
					if (compiledShaderData && (i_EffectFilename.GetNumNames() > 0))
					{
						fsLocator effect_loc = i_EffectFilename;
						effect_loc.ReplaceExtension(itString("mfx"));

						// Get shader data as pointer. Ownership maintained by effShaderSDKData class above.
						int shaderSize = 0;
						BYTE * pCompiledShader = NULL;
						compiledShaderData->GetData(&pCompiledShader, &shaderSize);
						
						// Write a multi-format .mfx file (Meta FX)
						matMetaFX meta_fx;
						meta_fx.m_ShaderName = shader_name;
						meta_fx.SetCompiledFXData(pCompiledShader, shaderSize);
						meta_fx.SetHLSLSource(hlsl_shader_str);
						meta_fx.SetMetaSLSource(metasl_shader_str);
						matMetaFXParser::WriteMetaFX(effect_loc, meta_fx);
					}

				}
			}
			else
				l_pShaderOutputView->ClearShaderString();

			
			guiCursor::EndWaitCursor();
		}
		else
		{
			guiMessageBox::Show("Selected root node to compile must return a color named 'result'", "Select Root");
		}
	}
	return bResult;
}
