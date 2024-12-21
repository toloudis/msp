/*****************************************************************************
**	api3dImport.cpp
**
**		Imports objects from files
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Tool/api3d/api3dImport.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceFinderDir.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Graphics/ent/entEntity.hpp"
#include "Graphics/ent/entImport.hpp"
#include "Graphics/Ent/entModelInstance.hpp"
#include "Graphics/ent/entModelTemplate.hpp"
#include "Graphics/sc/scObject.hpp"
#include "Tool/api3d/api3dObjectEntity.hpp"
#include "Tool/api3d/api3dObjectGeom.hpp"
#include "Tool/api3d/api3dResourceFinder.hpp"
#include "Tool/api3d/api3dSharedModelMgr.hpp"


//============================================================================
//============================================================================
namespace api3dImport
{
	namespace
	{
		fsLocator find_texture_path(const fsLocator &i_Locator)
		{
			// just return directory from filename for now
			fsLocator dir = i_Locator;
			dir.Pop();

			fsLocator tex_dir = dir;
			tex_dir.Pop();
			tex_dir.Push(gfPaths::GetSubPath(gfPaths::e_Textures));
			if (fsFileUtil::DirectoryExists(tex_dir))
				return tex_dir;

			return dir;
		}

		api3dObject * load_geom_object(const fsLocator &i_Locator)
		{
			// Geometry
			//
			fsLocator texture_path = find_texture_path(i_Locator);

			//std::string tempstring;
			//fsFileUtil::LocatorToANSIFilename( i_Locator, tempstring );
			//DBG_LOG( "api3d texture path (" << tempstring.c_str() << ")" );

			fsResourceFinderDir finder(texture_path);
			std::unique_ptr<entModelTemplate> ent_template(
						//entImport::LoadGeometry(i_Locator) );
						api3dSharedModelMgr::LoadModelTemplate(i_Locator) );

			std::unique_ptr<entModelInstance> ent_instance( new entModelInstance() );

			if (ent_template.get() != NULL)
			{
				//bga - Sending everything through api3dObjectEntity now, though
				// we could have a "IsAnimatable" flag on the template and decide from that.

				// pass ownership of pointers to new object
				//
				//if (dynamic_cast<entEntityTemplate*>(ent_template.get()))
				{
					// Entity object
					scObject *pObject = entImport::CreateObject( *ent_template.get(),
																 *ent_instance.get(), 
																 finder); 
					entEntity *entity_ptr = new entEntity(pObject);
					return new api3dObjectEntity(ent_template.release(), ent_instance.release(), entity_ptr);
				}
				//else
				//{
				//	// Static geometry
				//	scObject* obj_ptr = entImport::CreateObject( *ent_template );
				//	return new api3dObjectGeom(ent_template.release(), obj_ptr);
				//}
			}
			return NULL;
		}

		//--------------------------------------------------------------------
		//	load a model template for the passed in file.
		//--------------------------------------------------------------------
		entModelTemplate* load_model_template( const fsLocator& i_LODFileLocator, const std::string& i_ModelFile )
		{
			//fsLocator rootLoc( i_LODFileLocator );
			//rootLoc.Pop();
			//rootLoc.Pop();
			//api3dResourceFinder resourceFinder( rootLoc );

			entModelTemplate* model_template = 0;
			fsLocator modelLoc;
			fsFileUtil::ANSIFilenameToLocator( i_ModelFile, modelLoc );

			//DBG_LOG( "load_model_template (" << i_ModelFile.c_str() << ")" );

			if ( modelLoc.GetNumNames() > 0 )
			{
				//DBG_LOG( "api3d model path (" << i_ModelFile.c_str() << ")" );
				model_template = entImport::LoadGeometry( modelLoc );
			}

			return model_template;
		}

	}

	//--------------------------------------------------------------------
	//	load a model template for the passed in file.
	//--------------------------------------------------------------------
	entModelTemplate* LoadModelTemplate( const fsLocator& i_LODFileLocator, const std::string& i_ModelFile )
	{
		return load_model_template( i_LODFileLocator, i_ModelFile );
	}

	//--------------------------------------------------------------------
	//  Loads data from file and creates object of appropriate type
	//--------------------------------------------------------------------
	api3dObject * LoadObject(const fsLocator &i_Locator)
	{
		//bga - Removed compound object types here (.edf, .lod)
		api3dObject* pObj = load_geom_object( i_Locator );
		return pObj;
	}

	//--------------------------------------------------------------------
	//  Loads data from file and creates a particle generator based on
	//	a TPR file
	//--------------------------------------------------------------------
	api3dParticleGenerator* CreateParticleGenerator(const fsLocator &i_Locator)
	{
		return 0;
	}

}	// end of namespace
