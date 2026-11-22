/*****************************************************************************
**  gltfImport.hpp
**
**      The gltfImport implements importing functionality for the
**	Autodesk FBX SDK.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#pragma once
#ifdef GLTF_IMPORT_HPP
#error gltfImport.hpp multiply included
#endif
#define GLTF_IMPORT_HPP

#ifndef ENT_INPORT_HPP
#include "Graphics/ent/entImport.hpp"
#endif

class gltfImport : public entImportImpl
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	gltfImport();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual ~gltfImport();

	//----------------------------------------------------------------------------
	// Load geometry from file, creating a model template to hold the data
	//----------------------------------------------------------------------------
	virtual entModelTemplate* LoadGeometry( const fsLocator&	i_ModelLocator,
									//const fsResourceFinder& i_TextureFinder,
								    entFragInfoSink* o_Sink = NULL );

	//----------------------------------------------------------------------------
	// Load one node's mesh (i_Part: its identity, gltfSceneObject::m_Identity),
	// creating a model template to hold it
	//----------------------------------------------------------------------------
	virtual entModelTemplate* LoadGeometryPart( const fsLocator& i_ModelLocator,
												const std::string& i_Part );

	//----------------------------------------------------------------------------
	// Create scObject of appropriate animation type from template
	//----------------------------------------------------------------------------
	virtual scObject* CreateObject( const entModelTemplate& i_Template,
								    entModelInstance &o_Instance,
								    const fsResourceFinder& i_TextureFinder,
									const std::vector< shared_ptr<mdlMaterialInfo> > &i_MaterialOverrides );

	//----------------------------------------------------------------------------
	// Load shared animation key info from file,
	// deletion of return value is responsibility of caller
	//----------------------------------------------------------------------------
	virtual entAnimKeys* LoadAnimKeys( const fsLocator& i_AnimLocator );

	//----------------------------------------------------------------------------
	// Load one clip (i_Clip: its identity, gltfAnimRead::ClipIdentities) built
	// for one mesh node (i_Part: its identity, as LoadGeometryPart),
	// deletion of return value is responsibility of caller
	//----------------------------------------------------------------------------
	virtual entAnimKeys* LoadAnimKeysPart( const fsLocator& i_AnimLocator,
										   const std::string& i_Part,
										   const std::string& i_Clip );

	//----------------------------------------------------------------------------
	// Create animation from key data,
	// deletion of return value is responsibility of caller
	//----------------------------------------------------------------------------
	virtual entAnimation* CreateAnimation( const entAnimKeys &i_KeyData );
};
