/*****************************************************************************
**  mspCommands.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "mspCommands.hpp"
#include "mspModel.hpp"
#include "mspVersion.hpp"
#include "mspViewSettings.hpp"
#include "msl/mslMetaSLMgr.hpp"
#include "wxGUI/wxPropertyPanel.hpp"


#include "Core/fs/fsFileUtil.hpp"
#include "Core/prty/prtyInt32.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Graphics/Mat/matMaterial.hpp"
#include "Graphics/Mat/matTextureMgr.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#include "Graphics/mtr/mtrMaterialSaver.hpp"
#include "Graphics/mtr/mtrThumbnailData.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiPropertyDialog.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"

#include <sstream>

//============================================================================
//
//============================================================================
namespace
{
#ifdef USE_WXWIDGETS
	#define MENU_NAME(mName, wxName) wxName
#else
	#define MENU_NAME(mName, wxName) mName
#endif

	//============================================================================
	//============================================================================
	fsLocator l_ShaderFilename;	// Name of FX file compiled and saved, in order to use in MTL file

	//============================================================================
	// Callback to handle loading textures into materials for test application.
	// Only handles filename textures for now.
	//============================================================================
	class LoadTextureCallback : public prtyPropertyCallback
	{
	public:
		effParamTexture* m_pTextureParam;
		matTexture *m_pTexture;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		LoadTextureCallback(effParamTexture* i_pTextureParam)
			: m_pTextureParam(i_pTextureParam), m_pTexture(NULL) {}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~LoadTextureCallback()
		{
			if (m_pTexture)
				matTextureMgr::ReleaseTexture(m_pTexture);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
		{
			if (prtyTextureFileName* pTextureProperty = dynamic_cast<prtyTextureFileName*>(i_pProperty))
			{
				matTexture *old_texture = m_pTextureParam->GetTexture();
				matTexture *new_texture = NULL;
				fsLocator filename = pTextureProperty->GetValue();
				if (filename.GetNumNames() > 0)
					new_texture = matTextureMgr::LoadTexture(filename);
				m_pTextureParam->SetTexture(new_texture);
				m_pTexture = new_texture; // take ownership of texture
				if (old_texture)
					matTextureMgr::ReleaseTexture(old_texture);
			}
		}
	};

	//
	//	Command Functions
	//

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_GraphView()
	{
#ifdef USE_WXWIDGETS
		twxPaneMgr::Show("GraphView");
#endif
	}
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_RenderView()
	{
#ifdef USE_WXWIDGETS
		twxPaneMgr::Show("RenderArea");
#endif
	}
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_ToolView()
	{
#ifdef USE_WXWIDGETS
		twxPaneMgr::Show("ToolView");
#endif
	}
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_ParameterView()
	{
#ifdef USE_WXWIDGETS
		twxPaneMgr::Show("ParameterView");
#endif
	}
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_ShaderOutput()
	{
#ifdef USE_WXWIDGETS
		twxPaneMgr::Show("ShaderOutput");
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_MaterialProperties()
	{
#ifdef USE_WXWIDGETS
		twxPaneMgr::Show("MaterialProperties");
#endif
	}


	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_GotoLine()
	{
		prtyInt32 line_num("Line number");
		prtyPropertyUIInfoContainer nodeInfo;
		nodeInfo.Add(new prtyFloatEditUIInfo(&line_num));
		if (guiPropertyDialog::ShowModal("Goto Line", 
										 nodeInfo, 
										 "Goto line number:") == guiPropertyDialog::e_OK)
		{
			mslMetaSLMgr::GotoSourceLine(line_num.GetValue());
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_About()
	{	
		// the about box
		std::ostringstream str;
		str << c_AssemblyTitle << " " << c_AssemblyVersion;
		str << "\n\n";
		str << "Copyright (c) 2010, Studio GPU\n";
		str <<  "All rights reserved\n";

		guiMessageBox::Show( str.str().c_str(), c_AssemblyTitle, guiMessageBox::e_OKOnly );
	}


	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	bool Do_Compile(const fsLocator i_ShaderFilename)
	{	
		matMaterial* pShaderMaterial = mspModel::GetMaterialByName("Shader");
		wxPropertyPanel::DialogInstance->ClearProperties();
		if (mslMetaSLMgr::Compile(pShaderMaterial, i_ShaderFilename))
		{
			if (pShaderMaterial && pShaderMaterial->GetShaderParams()->m_pPrtyUI)
			{
				//int property_count = pShaderMaterial->GetShaderParams()->m_pPrtyUI->GetList().size();
				//DBG_LOG("Property count: " << property_count);
				if (wxPropertyPanel::DialogInstance)
					wxPropertyPanel::DialogInstance->SetProperties(pShaderMaterial->GetShaderParams()->m_pPrtyUI->GetListContainer());

				std::vector<effParamTexture*> vTextures;
				pShaderMaterial->GetShaderParams()->GetAllTextureParams(vTextures);
				const int num_textures = vTextures.size();
				for (int i=0; i<num_textures; ++i)
				{
					vTextures[i]->Property().AddCallback( new LoadTextureCallback(vTextures[i]) );

					fsLocator resource_name = vTextures[i]->GetShaderResourceName();
					if (resource_name.GetNumNames() > 0)
					{
						// locate texture in MetaSL directory if not already fullpath
						fsLocator full_path;
						if (resource_name.GetNumNames() == 1) 
							full_path = mslMetaSLMgr::GetTextureDirectory();
						full_path.Push(resource_name);

						// Set filename into property
						prtyTextureFileData tex_data;
						tex_data.m_TextureLocator = full_path;
						vTextures[i]->Property().SetValue(tex_data);
					}
				}
				return true;
			}
		}
		return false;
	}
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Execute_Compile()
	{	
		//fsLocator default_filename(itString("shader_compiled.mfx"));
		fsLocator default_filename; // empty locator so no temporary file is written
		Do_Compile(default_filename);
	}
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Export_MFX_Shader()
	{	
		fsLocator shader_filename;
		const char* c_MFXFilter = "MFX Shader Files (*.mfx)|*.mfx|All Files (*.*)|*.*";
		if (guiFileDialogUtils::GetSaveFileName(c_MFXFilter, shader_filename))
		{
			shader_filename.ReplaceExtension("mfx");
			if (Do_Compile(shader_filename))
			{
				l_ShaderFilename = shader_filename;
			}
		}
	}
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Export_MTL_File()
	{	
		if (l_ShaderFilename.GetNumNames() == 0)
		{
			guiMessageBox::Show("Please compile and save an FX shader file before saving a material file.", "Need FX Shader");
		}
		else
		{
			fsLocator material_filename;
			const char* c_MTLFilter = "Material files (*.mtl)|*.mtl|All files (*.*)|*.*";
			if (guiFileDialogUtils::GetOpenFileName(c_MTLFilter, "Materials", material_filename))
			{
				material_filename.ReplaceExtension("mtl");

				mdlMaterialInfo material_info;

				matMaterial* pShaderMaterial = mspModel::GetMaterialByName("Shader");
				if (pShaderMaterial)
				{
					shared_ptr<effShaderParams> shader_params(pShaderMaterial->GetShaderParams()->Clone());
					shader_params->SetShaderName( l_ShaderFilename );
					material_info.SetShaderParams(shader_params);

					mtrThumbnailData thumbnail;
					//mtrlIconUtil::createMtrlIcon(i_MatInfo, thumbnail);
					mtrMaterialSaver::WriteSingleMaterial(material_filename, material_info, thumbnail);
				}
				else
				{
					DBG_ERROR("Could not access shader material in order to save to MTL file.");
				}
			}
		}
	}
}

//--------------------------------------------------------------------
// SetupMenu
//--------------------------------------------------------------------
void mspCommands::SetupMenu()
{
	//
	//	commands
	//
	int menu_id;
	cmaCommand* pCmd;
	
	guiMenuMgr::AddMenu("Edit", "");
	guiMenuMgr::AddMenu("View", "");
#ifdef USE_WXWIDGETS
	guiMenuMgr::AddMenu("Window", "");
#endif
	guiMenuMgr::AddMenu("Help", "");

	//	COMMAND: Compile Shader
	pCmd = new cmaCommandSimple("Compile Shader", 
								"Edit", 
								"Compile node graph into HLSL shader",
								&Execute_Compile );
	menu_id = guiMenuMgr::AddMenuItem( "Edit", 
		MENU_NAME("Compile Shader", "Compile Shader \tF7"));
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Export FX Shader
	pCmd = new cmaCommandSimple("Export MFX Shader", 
								"Edit", 
								"Compile node graph into HLSL shader and save to file.",
								&Export_MFX_Shader );
	menu_id = guiMenuMgr::AddMenuItem( "Edit", "Export MFX Shader" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );


	//	COMMAND: Export MTL Material
	pCmd = new cmaCommandSimple("Export MTL Material", 
								"Edit", 
								"Save MachStudio material file that uses the compiled shader.",
								&Export_MTL_File );
	menu_id = guiMenuMgr::AddMenuItem( "Edit", "Export MTL Material" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	guiMenuMgr::AddSeparator("Edit");

	//	COMMAND: Delete Node
	pCmd = new cmaCommandSimple("Delete Node", 
								"Edit", 
								"Delete selected nodse from node graph",
								&mslMetaSLMgr::DeleteSelectedNodes );
	menu_id = guiMenuMgr::AddMenuItem( "Edit", "Delete Node");
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );	

	//	COMMAND: Disconnect Inputs
	pCmd = new cmaCommandSimple("Disconnect Inputs", 
								"Edit", 
								"Disconnect inputs for the selected nodes from node graph",
								&mslMetaSLMgr::DisconnectInputs );
	menu_id = guiMenuMgr::AddMenuItem( "Edit", "Disconnect Inputs");
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );	

	//	COMMAND: Disconnect Outputs
	pCmd = new cmaCommandSimple("Disconnect Outputs", 
								"Edit", 
								"Disconnect outputs for the selected nodes from node graph",
								&mslMetaSLMgr::DisconnectOutputs );
	menu_id = guiMenuMgr::AddMenuItem( "Edit", "Disconnect Outputs");
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );	
	
	//bga - this command wasn't working...
	//	COMMAND: Goto Line
	//pCmd = new cmaCommandSimple("Goto Line", 
	//							"Edit", 
	//							"Go to line number in shader source output",
	//							&Execute_GotoLine );
	//menu_id = guiMenuMgr::AddMenuItem( "Edit", 
	//	MENU_NAME("Goto Line", "Goto Line \tCtrl-G"));
	//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Focus Camera
	pCmd = new cmaCommandSimple("Focus Camera", 
								"View", 
								"Focus camera on model",
								&mspModel::FocusCamera );
	menu_id = guiMenuMgr::AddMenuItem( "View", 
		MENU_NAME("Focus Camera", "Focus Camera \tCtrl-F"));
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Zoom extents
	pCmd = new cmaCommandSimple("Zoom extents", 
								"View", 
								"View whole scene graph",
								&mslMetaSLMgr::ZoomExtents );
	menu_id = guiMenuMgr::AddMenuItem( "View", "Zoom extents");
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Layout graph
	pCmd = new cmaCommandSimple("Layout graph", 
								"View", 
								"Layout nodes in graph automatically",
								&mslMetaSLMgr::LayoutGraph );
	menu_id = guiMenuMgr::AddMenuItem( "View", "Layout graph");
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );


	//	COMMAND: View Wireframe
	//pCmd = new cmaCommandToggle("Wireframe", 
	//							"View", 
	//							"Toggle Wireframe Visibility",
	//							&mspViewSettings::SetViewWireframe,
	//							&mspViewSettings::GetViewWireframe );
	//menu_id = guiMenuMgr::AddCheckableMenuItem( "View", 
	//	MENU_NAME("Wireframe", "Wireframe \tCtrl-W"));
	//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Shader Graph
	//pCmd = new cmaCommandSimple("Shader Graph", 
	//							"Window", 
	//							"Show Shader Graph pane",
	//							&Execute_GraphView );
	//menu_id = guiMenuMgr::AddMenuItem( "Window", "Shader Graph" );
	//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Render View
	pCmd = new cmaCommandSimple("Render View", 
								"Window", 
								"Show Render View pane",
								&Execute_RenderView );
	menu_id = guiMenuMgr::AddMenuItem( "Window", "Render View" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Tool Palette
	pCmd = new cmaCommandSimple("Tool Palette", 
								"Window", 
								"Show Tool Palette pane",
								&Execute_ToolView );
	menu_id = guiMenuMgr::AddMenuItem( "Window", "Tool Palette" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Parameters
	pCmd = new cmaCommandSimple("Parameters", 
								"Window", 
								"Show Parameters pane",
								&Execute_ParameterView );
	menu_id = guiMenuMgr::AddMenuItem( "Window", "Parameters" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Shader Output
	pCmd = new cmaCommandSimple("Shader Output", 
								"Window", 
								"Show Shader Output pane",
								&Execute_ShaderOutput );
	menu_id = guiMenuMgr::AddMenuItem( "Window", "Shader Output" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: Material Properties
	pCmd = new cmaCommandSimple("Material Properties", 
								"Window", 
								"Show Material Properties pane",
								&Execute_MaterialProperties );
	menu_id = guiMenuMgr::AddMenuItem( "Window", "Material Properties" );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	//	COMMAND: About dialog
	pCmd = new cmaCommandSimple("About", 
								"Help", 
								"About dialog",
								&Execute_About );
	menu_id = guiMenuMgr::AddMenuItem( "Help", pCmd->GetTag().c_str() );
	guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
}
