/*****************************************************************************
**  entImport.hpp
**
**      The entImport loads files into templates and
**	can create objects from a template.  It provides a
**	base class for implementations.  It can maintain more
**	than one implementation to handle multiple file types
**	at once.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef ENT_IMPORT_HPP
#error entImport.hpp multiply included
#endif
#define ENT_IMPORT_HPP

#ifndef ENV_BOOST_HPP
#include "Core/env/envBoost.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class entAnimation;
class entAnimKeys;
class entModelInstance;
class entModelTemplate;
class fsLocator;
class fsResourceFinder;
class entFragInfoSink;
class scObject;
class mdlMaterialInfo;


//============================================================================
//============================================================================
class entImportImpl
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual ~entImportImpl() { };

	//----------------------------------------------------------------------------
	// Load geometry from file, creating a model template to hold the data
	//----------------------------------------------------------------------------
	virtual entModelTemplate* LoadGeometry( const fsLocator&	i_ModelLocator,
								   //const fsResourceFinder& i_TextureFinder,
								   entFragInfoSink* o_Sink = NULL ) = 0;

	//----------------------------------------------------------------------------
	// Creates scObject as instance of the template. Data to be stored
	// by the instance are returned in the o_Instance argument.
	// A list of material overrides can be passed in to alter the
	// creation of materials for this specific instance.
	//----------------------------------------------------------------------------
	virtual scObject* CreateObject( const entModelTemplate& i_Template,
								    entModelInstance &o_Instance,
								    const fsResourceFinder& i_TextureFinder,
									const std::vector< shared_ptr<mdlMaterialInfo> > &i_MaterialOverrides ) = 0;

	//----------------------------------------------------------------------------
	// Load shared animation key info from file,
	// deletion of return value is responsibility of caller
	//----------------------------------------------------------------------------
	virtual entAnimKeys* LoadAnimKeys( const fsLocator& i_AnimLocator ) = 0;

	//----------------------------------------------------------------------------
	// Create animation from key data,
	// deletion of return value is responsibility of caller
	//----------------------------------------------------------------------------
	virtual entAnimation* CreateAnimation( const entAnimKeys &i_KeyData ) = 0;
};


//============================================================================
//============================================================================
namespace entImport
{
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Initialize();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void DeInitialize();

	//----------------------------------------------------------------------------
	// Provide implementation for reading file formats.  Ownership
	//	passes to entImport.
	//----------------------------------------------------------------------------
	void AddImplementation(entImportImpl *i_pImplementation);

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void RemoveImplementation(entImportImpl *i_pImplementation);

	//----------------------------------------------------------------------------
	// Load geometry from file, creating a model template to hold the data
	//		If there is an error, NULL is returned.
	//----------------------------------------------------------------------------
	entModelTemplate* LoadGeometry( const fsLocator&	i_ModelLocator,
								   //const fsResourceFinder& i_TextureFinder,
								   entFragInfoSink* o_Sink = NULL );

	//----------------------------------------------------------------------------
	// Create scObject of appropriate animation type from template
	//		If there is an error, NULL is returned.
	//----------------------------------------------------------------------------
	scObject* CreateObject( const entModelTemplate& i_Template,
							entModelInstance &o_Instance,
							const fsResourceFinder& i_TextureFinder );
	scObject* CreateObject( const entModelTemplate& i_Template,
						    entModelInstance &o_Instance,
							const fsResourceFinder& i_TextureFinder,
							const std::vector< shared_ptr<mdlMaterialInfo> > &i_MaterialOverrides );

	//----------------------------------------------------------------------------
	// Load shared animation key info from file,
	// deletion of return value is responsibility of caller
	//		If there is an error, NULL is returned.
	//----------------------------------------------------------------------------
	entAnimKeys* LoadAnimKeys( const fsLocator& i_AnimLocator );

	//----------------------------------------------------------------------------
	// Create animation from key data,
	// deletion of return value is responsibility of caller
	//		If there is an error, NULL is returned.
	//----------------------------------------------------------------------------
	entAnimation* CreateAnimation( const entAnimKeys &i_KeyData );
}
