/*****************************************************************************
**  entImport.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/ent/entImport.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envExceptionX.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/ent/entLODModelTemplate.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace entImport
{
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace
{
	std::vector<entImportImpl*> l_pImplementations;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Initialize()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void DeInitialize()
{
	envSTLHelpers::DeleteContainer(l_pImplementations);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void AddImplementation(entImportImpl *i_pImplementation)
{
	l_pImplementations.push_back(i_pImplementation);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void RemoveImplementation(entImportImpl *i_pImplementation)
{
	envSTLHelpers::DeleteOneValue(l_pImplementations, i_pImplementation);
}

//----------------------------------------------------------------------------
//  LoadGeometry
//----------------------------------------------------------------------------
entModelTemplate* entImport::LoadGeometry( const fsLocator& i_ModelLocator,
											//const fsResourceFinder& i_TextureFinder,
											entFragInfoSink* o_Sink )
{
	//std::string tempstring;
	//fsFileUtil::LocatorToANSIFilename( i_ModelLocator, tempstring );
	//DBG_LOG( "ent geometry path (" << tempstring.c_str() << ")" );

	try
	{
	
		const int nimpl = l_pImplementations.size();
		for (int i=0; i<nimpl; i++)
		{
			entModelTemplate* model_template = l_pImplementations[i]->LoadGeometry(i_ModelLocator, o_Sink);
			if (model_template != NULL)
				return model_template;
		}
		// No implementation handled this file format, set error string and return NULL
		DBG_ERROR("Unrecognized model file format " << i_ModelLocator);
	}
	catch ( const envExceptionX& i_Ex)
	{
		DBG_ERROR("Problem occurred loading model: " << i_Ex.GetErrorMessage());
	}
	catch (const std::bad_alloc&)
	{
		DBG_ERROR("Out of memory while loading model " << i_ModelLocator);
	}
	catch (...)
	{
		DBG_ERROR("General exception while loading model " << i_ModelLocator);
	}

	return NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
scObject* entImport::CreateObject( const entModelTemplate& i_Template,
								   entModelInstance &o_Instance,
								   const fsResourceFinder& i_TextureFinder)
{
	std::vector< shared_ptr<mdlMaterialInfo> > no_material_overrides;
	return CreateObject(i_Template, o_Instance, i_TextureFinder, no_material_overrides);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
scObject* entImport::CreateObject( const entModelTemplate& i_Template,
								   entModelInstance &o_Instance,
								   const fsResourceFinder& i_TextureFinder,
								   const std::vector< shared_ptr<mdlMaterialInfo> > &i_MaterialOverrides )
{
	try
	{
		int nimpl = (int) l_pImplementations.size();
		for (int i=0; i<nimpl; i++)
		{
			scObject* obj = l_pImplementations[i]->CreateObject(i_Template, o_Instance, i_TextureFinder, i_MaterialOverrides);
			if (obj != NULL)
				return obj;
		}
	
		// No implementation handled this, log error and return NULL
		DBG_ERROR("Unrecognized model type");
	}
	catch ( const g2dOutOfSystemMemoryX& )
	{
		// special case for out of system memory
		DBG_ERROR("Out of system memory whiling creating object ");
		throw g2dOutOfSystemMemoryX();

	}
	catch ( const envExceptionX& i_Ex)
	{
		DBG_ERROR("Problem occurred creating object: " << i_Ex.GetErrorMessage());
	}
	catch (const std::bad_alloc&)
	{
		DBG_ERROR("Out of memory while creating object");
	}
	catch (...)
	{
		DBG_ERROR("General exception while creating object");
	}

	// If we got here, there was an error, return NULL
	return NULL;
}


//----------------------------------------------------------------------------
// Load shared animation key info from file
//----------------------------------------------------------------------------
entAnimKeys* entImport::LoadAnimKeys(const fsLocator& i_AnimLocator)
{
	try
	{
		int nimpl = (int) l_pImplementations.size();
		for (int i=0; i<nimpl; i++)
		{
			entAnimKeys* keys = l_pImplementations[i]->LoadAnimKeys(i_AnimLocator);
			if (keys != NULL)
				return keys;
		}

		// No implementation handled this file format, set error string and return NULL
		DBG_ERROR("Unrecognized animation file format " << i_AnimLocator);
	}
	catch ( const envExceptionX& i_Ex)
	{
		DBG_ERROR("Problem occurred loading animation: " << i_Ex.GetErrorMessage());
	}
	catch (const std::bad_alloc&)
	{
		DBG_ERROR("Out of memory while loading animation " << i_AnimLocator);
	}
	catch (...)
	{
		DBG_ERROR("General exception while loading animation " << i_AnimLocator);
	}

	// If we got here, there was an error, return NULL
	return NULL;
}

//----------------------------------------------------------------------------
// Create animation from key data
//----------------------------------------------------------------------------
entAnimation* entImport::CreateAnimation(const entAnimKeys &i_KeyData)
{
	try
	{
		int nimpl = (int) l_pImplementations.size();
		for (int i=0; i<nimpl; i++)
		{
			entAnimation* animation = l_pImplementations[i]->CreateAnimation(i_KeyData);
			if (animation != NULL)
				return animation;
		}

		// No implementation handled this file format, log error and return NULL
		DBG_ERROR("Unrecognized animation type");
	}
	catch ( const envExceptionX& i_Ex)
	{
		DBG_ERROR("Problem occurred creating animation: " << i_Ex.GetErrorMessage());
	}
	catch (const std::bad_alloc&)
	{
		DBG_ERROR("Out of memory while creating animation");
	}
	catch (...)
	{
		DBG_ERROR("General exception while creating animation");
	}

	// If we got here, there was an error, return NULL
	return NULL;
}

}	// end of namespace
