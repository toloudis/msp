/*****************************************************************************
**  fbxImport.hpp
**
**      The fbxImport implements importing functionality for the
**	Autodesk FBX SDK.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef FBX_IMPORT_HPP
#error fbxImport.hpp multiply included
#endif
#define FBX_IMPORT_HPP

#ifndef ENT_INPORT_HPP
#include "Graphics/ent/entImport.hpp"
#endif

class fbxImport : public entImportImpl
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	fbxImport();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual ~fbxImport();

	//----------------------------------------------------------------------------
	// Load geometry from file, creating a model template to hold the data
	//----------------------------------------------------------------------------
	virtual entModelTemplate* LoadGeometry( const fsLocator&	i_ModelLocator,
									//const fsResourceFinder& i_TextureFinder,
								    entFragInfoSink* o_Sink = NULL );

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
	// Create animation from key data,
	// deletion of return value is responsibility of caller
	//----------------------------------------------------------------------------
	virtual entAnimation* CreateAnimation( const entAnimKeys &i_KeyData );
};
