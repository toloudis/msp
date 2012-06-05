/*****************************************************************************
**  MtlExporter.cpp
**
**	Exports materials
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "MtlExporter.hpp"

#include "MaxCommon.hpp"
#include "ExportDoc.hpp"
#include "MaxExportUtils.hpp"
#ifndef ENV_STRING_HPP
#include "Core/Env/envString.hpp"
#endif 
#include "Core/fs/fsLocator.hpp"
#include "Core/ch/chBinWriter.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsXMLWriter.hpp"

#include "Graphics/mdl/mdlMaterialInfo.hpp"
#include "Graphics/mdl/mdlMatInfo.hpp"
#include "Graphics/Eff/effPhongData.hpp"

#include <sstream>
using namespace std;


namespace MaxExp
{

	//=============================================================================
	// Get StandardID of a map From Channel Id
	//
	// Each standard material in max can have a number of different
	// type of maps  (egs: ambient, diffuse, reflection, displacement etc)
	// Each of this map has a standard id
	// 
	// Get the standard id from a  channel id
	//=============================================================================
	int StandardIdFromChannelId( StdMat2 *i_pStdMtl, int i_nChannelId )
	{	
		int stdId = -1;
		for (int j = 0; j < NTEXMAPS; j++)
		{		
			if (i_pStdMtl->StdIDToChannel( j )  == i_nChannelId)
			{
				stdId = j;
				break;
			}
		}
		return stdId ;
	}


	//=============================================================================
	// Return the channel id of a standard material,	 
	// that corresponds to a diffuse  texture map
	// 
	//=============================================================================
	int GetDiffuseTexMapIndex( StdMat2 *i_pStdMtl )
	{
		int numSubTexMaps = i_pStdMtl->NumSubTexmaps();
		int diffuseIdx = -1;
		for (int i = 0; i < numSubTexMaps; i++)
		{
			int stdId = StandardIdFromChannelId( i_pStdMtl, i);
			if( stdId == ID_DI)
			{
				diffuseIdx = i;
			}
		}
		return diffuseIdx;
	}

	//=============================================================================
	// Describe the properties of the material in the log
	//=============================================================================
	void MtlExporter::LogMaterial( Mtl *i_pMtl ) const
	{
		StdMat2 *std = dynamic_cast< StdMat2*> (i_pMtl);  
		string sMtlName ( SgpuConvert::MaterialName(i_pMtl ) ); 
		const string &sMtlClassName = ClassDescs::Get().ClassName( i_pMtl->ClassID() );
		ExportLogger::WriteStartElementWrap explogWrap( "Material Description: %s", sMtlName.c_str() ); 
		if( std == NULL)
		{ 
			EXPLOG.WriteInfo("Material is not standardMaterial" );
			return;
		}
		Shader* shader =  std->GetShader();
		if (shader)
		{
			Class_ID shaderClassId = shader->ClassID();   
			const string &sClassName = ClassDescs::Get().ClassName( shaderClassId );
			ExportLogger::WriteStartElementWrap explogWrap( "Shader: %s", sClassName.c_str() );
			Color col = shader->GetAmbientClr( static_cast<TimeValue>(OPTS.m_StartTime) );
			EXPLOG.WriteInfo( "Ambient %g %g %g", col[0], col[1], col[2] );
			col = shader->GetDiffuseClr( static_cast<TimeValue>(OPTS.m_StartTime) );
			EXPLOG.WriteInfo( "Diffuse %g %g %g", col[0], col[1], col[2] );
			col = shader->GetSpecularClr( static_cast<TimeValue>(OPTS.m_StartTime) );
			EXPLOG.WriteInfo( "Diffuse %g %g %g", col[0], col[1], col[2] );
			float glossiness = shader->GetGlossiness( static_cast<TimeValue>(OPTS.m_StartTime) );
			float specularLevel = shader->GetSpecularLevel( static_cast<TimeValue>(OPTS.m_StartTime) );
			EXPLOG.WriteInfo("Glossiness %g, Specular Level %g", glossiness, specularLevel );
		}
		Color col = std->GetAmbient( static_cast<TimeValue>(OPTS.m_StartTime) );
		EXPLOG.WriteInfo( "Ambient %g %g %g", col[0], col[1], col[2] );   
		col = std->GetDiffuse( static_cast<TimeValue>(OPTS.m_StartTime) );
		EXPLOG.WriteInfo( "Diffuse %g %g %g", col[0], col[1], col[2] );  
		col = std->GetSpecular( static_cast<TimeValue>(OPTS.m_StartTime) );
		EXPLOG.WriteInfo( "Specular %g %g %g", col[0], col[1], col[2] );
		float f = std->GetOpacity( static_cast<TimeValue>(OPTS.m_StartTime) );
		EXPLOG.WriteInfo( "Opacity %g", f );
		EXPLOG.WriteInfo("Faceted %s", SGPU_BOOL_STRING( (std->IsFaceted() == TRUE ) ) );  
		EXPLOG.WriteInfo("DoubleSided %s", SGPU_BOOL_STRING( (std->GetTwoSided() == TRUE ) ) );
		EXPLOG.WriteInfo("Wire %s", SGPU_BOOL_STRING( (std->GetWire() == TRUE ) ) );
		EXPLOG.WriteInfo("FaceMap %s", SGPU_BOOL_STRING( (std->GetFaceMap() == TRUE ) ) );
	}

	//=============================================================================
	// Describe the properties of all the materials stored in  m_MtlHashForLogging
	//=============================================================================
	void MtlExporter::LogMaterials( ) const
	{
		stdext::hash_set< Mtl * >::const_iterator cit;
		for( cit = m_MtlHashForLogging.begin(); cit != m_MtlHashForLogging.end(); ++cit )
		{
			Mtl * const mtl = *cit;
			LogMaterial ( mtl );
		}
	}
	//=============================================================================
	//get the submtl name
	//(In case of an erroneous submtl, this retirns "SgpuErrorMtl")	
	//=============================================================================
	std::string MtlExporter::GetSubMtlName( Mtl *i_pSubMtl ) const
	{
		string sMtlName ( SgpuConvert::MaterialName( i_pSubMtl ) );
		if( IsErrorMaterial(  i_pSubMtl ) )
		{
			sMtlName = "SgpuErrorMtl";
		}
		return sMtlName;
	}

	//=============================================================================
	// Given a node material and the face material id, get the sub material info
	// The sub material info includes the submtl and the corrected face mat id
	//=============================================================================
	void MtlExporter::GetSubMtlFromFaceMatId( int i_nFaceMatId, Mtl *i_pNodeMtl, SubMtlAux &o_SubMtlAux ) const
	{
		Mtl *subMtl = NULL;
		size_t nSubMtls = 1;
		string sNodeMtlName ( SgpuConvert::MaterialName( i_pNodeMtl ) );
		if( i_pNodeMtl && i_pNodeMtl->IsMultiMtl() )
		{
			nSubMtls = i_pNodeMtl->NumSubMtls();
			if( nSubMtls <= 0)
			{
				EXPLOG.WriteWarning("multimaterial '%s' doesnt have any sub materials!", sNodeMtlName.c_str() );
				nSubMtls = 1;
			}
		}

		int correctedFaceMatId = i_nFaceMatId % nSubMtls;
		if( i_pNodeMtl && i_pNodeMtl->IsMultiMtl())
		{		
			subMtl = i_pNodeMtl->GetSubMtl( correctedFaceMatId );
		} else {
			subMtl = i_pNodeMtl;
		}
		//note that o_SubMtlAux.m_nFaceMatId may be different
		//than i_nFaceMatId;
		o_SubMtlAux.m_nFaceMatId = correctedFaceMatId;
		o_SubMtlAux.m_pSubMtl = subMtl;
	}

	//=============================================================================
	// Is the max material an error material
	// 
	//=============================================================================
	bool MtlExporter::IsErrorMaterial(Mtl *i_pMtl)const
	{
		string sMtlName ( SgpuConvert::MaterialName( i_pMtl ) );
		bool bErrorMtl =   sMtlName.empty()|| (i_pMtl->ClassID() != Class_ID(DMTL_CLASS_ID, 0) );  //either it is not a stdmtl
			//or its name is empty
		return bErrorMtl;
	}

	//=============================================================================
	// Make error material	 
	// 
	//=============================================================================
	void MtlExporter::MakeErrorMatInfo( mdlMaterialInfo &o_MatInfo) const
	{		
		o_MatInfo.SetMaterialName( "SgpuErrorMaterial" );
		shared_ptr<effPhongData> phong_data(new effPhongData());
		phong_data->m_ColorDiffuse.Set(0.5f, 0.5f, 0.5f, 1.0f);
		phong_data->m_ColorAmbient.Set(1,1,1,1);
		phong_data->m_ColorEmissive.Set(0,0,0,1);
		phong_data->m_ColorSpecular.Set(0,0,0,1);
		phong_data->m_SpecularPower = 1.0f;


		shared_ptr<effShaderParams> phong_params(new effShaderParams());
		if (phong_data->m_FullpathDiffuse.GetNumNames() == 0)
			phong_params->SetShaderName(itString("Simple.fx"));
		else
			phong_params->SetShaderName(itString("Phong.fx"));
		phong_data->AddToParams(*phong_params);
		o_MatInfo.SetShaderParams(phong_params);

	}

	//=============================================================================
	// Public function to do the export	 
	// 
	//=============================================================================
	void MtlExporter::Export( Mtl *i_pSubMtl )
	{
		string sMtlName ( GetSubMtlName(  i_pSubMtl ) );
		assert(NULL !=   m_pExportDoc->m_pMdlMatInfoTable );
		mdlMatInfoTable &matInfoTable = *m_pExportDoc->m_pMdlMatInfoTable;			

		//If this material is not yet registered in the materal info table
		//register it
		if( matInfoTable.end() == matInfoTable.find( sMtlName ) )
		{
			shared_ptr<mdlMatInfo> matInfo( new mdlMatInfo());
			//get the mtl data into matInfo
			ExportCore(i_pSubMtl, matInfo->m_Info);
			matInfoTable.insert( mdlMatInfoTable::value_type( sMtlName, matInfo ) );	
			if( !IsErrorMaterial( i_pSubMtl ) )
		 {
			 m_MtlHashForLogging.insert( i_pSubMtl );
			 size_t sz = m_MtlHashForLogging.size();
			 sz = sz;
			}	
		}
	}


	//=============================================================================
	// Workhorse function to do the export	 
	// 
	// If we  cannot export  the material, then put a standby material called  "SgpuErrorMtl"
	// If we can export, then extract the phong data and the texture name
	//=============================================================================

	void MtlExporter::ExportCore( Mtl *i_pSubMtl, mdlMaterialInfo &o_MatInfo)
	{			
		string sMtlName ( SgpuConvert::MaterialName(i_pSubMtl ) );
		if( IsErrorMaterial( i_pSubMtl ) )
		{
			MakeErrorMatInfo( o_MatInfo );
			return;
		}
		assert( !sMtlName.empty() );
		assert(i_pSubMtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0));
		StdMat2 *std = static_cast< StdMat2*> (i_pSubMtl);
		o_MatInfo.SetMaterialName( sMtlName.c_str() );
		shared_ptr<effPhongData> phong_data(new effPhongData());
		//extract the material diffuse data
		phong_data->m_ColorDiffuse = SgpuConvert::ColorRGBA( std->GetDiffuse( static_cast<TimeValue>(OPTS.m_StartTime) ) );
		phong_data->m_ColorAmbient.Set(1,1,1,1);
		phong_data->m_ColorEmissive.Set(0,0,0,1);
		phong_data->m_ColorSpecular.Set(0,0,0,1);
		phong_data->m_SpecularPower = 1.0f;

		Shader* shader =  std->GetShader();
		if (shader)
		{
			Class_ID shaderClassId = shader->ClassID();   
			const string &sClassName = ClassDescs::Get().ClassName( shaderClassId );
			//Both blinn and phong sehaders get full export
			if ( ( shaderClassId == Class_ID(PHONGClassID, 0) ) || ( shaderClassId == Class_ID(BLINNClassID, 0) ) )
			{			
				float specularLevel = shader->GetSpecularLevel( static_cast<TimeValue>(OPTS.m_StartTime) );
				phong_data->m_ColorDiffuse = SgpuConvert::ColorRGBA( shader->GetDiffuseClr( static_cast<TimeValue>(OPTS.m_StartTime) ) );
				phong_data->m_ColorAmbient = SgpuConvert::ColorRGBA( shader->GetAmbientClr( static_cast<TimeValue>(OPTS.m_StartTime) ) );
				//specular color is  modulated by the specular level
				phong_data->m_ColorSpecular = SgpuConvert::ColorRGBA( shader->GetSpecularClr( static_cast<TimeValue>(OPTS.m_StartTime) )  * specularLevel ) ;
				phong_data->m_SpecularPower =  shader->GetGlossiness( static_cast<TimeValue>(OPTS.m_StartTime) );
				phong_data->m_Transparency = SgpuConvert::Clamp( std->GetOpacity( static_cast<TimeValue>(OPTS.m_StartTime) ) );
				float glossiness =  shader->GetGlossiness( static_cast<TimeValue>(OPTS.m_StartTime) );
			} else
			{ //other wise only the diffuse color is exported    
				phong_data->m_ColorDiffuse = SgpuConvert::ColorRGBA( shader->GetDiffuseClr( static_cast<TimeValue>(OPTS.m_StartTime) ) );    
				phong_data->m_ColorAmbient = SgpuConvert::ColorRGBA( shader->GetAmbientClr( static_cast<TimeValue>(OPTS.m_StartTime) ) );    
				phong_data->m_Transparency = SgpuConvert::Clamp( std->GetOpacity( static_cast<TimeValue>(OPTS.m_StartTime) ) );
				const string &sClassName = ClassDescs::Get().ClassName( shaderClassId );
				EXPLOG.WriteWarning( "shader class '%s' of  material '%s' is interpreted as a specular-less one", sClassName.c_str(), sMtlName.c_str());
			}
		}

		int diffuseIdx =  GetDiffuseTexMapIndex( std );
		if( diffuseIdx >= 0)
		{
			Texmap* map = std->GetSubTexmap(diffuseIdx);
			if( NULL != map )
			{
				if (map->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00))
				{
					BitmapTex* baseBitmap = (BitmapTex*) map;
					if (baseBitmap != NULL)
					{
						const MCHAR *szMbcsBitmapName = baseBitmap->GetMapName();
						fsLocator texLoc;
						std::wstring wBitmapName;
						MbcsToUnicode( szMbcsBitmapName, wBitmapName );
						itString itBitmapName ( reinterpret_cast<const itString::CharType * > ( wBitmapName.c_str() ) );
						fsFileUtil::UnicodeStringToLocator( itBitmapName,   texLoc  );
						WarnIfTextureDoesntExists( texLoc );    

						phong_data->m_FullpathDiffuse = texLoc;
						phong_data->m_ColorDiffuse.Set(1,1,1,1);
					} else

					{

						EXPLOG.WriteError( "cannot convert texture map of diffuse channel of mtl '%s' to bitmap", sMtlName.c_str() );

					}
				} else 
				{ 
					const string &sClassName = ClassDescs::Get().ClassName( map->ClassID() );
					EXPLOG.WriteError( "texture map of class '%s' not recognized  for diffuse channel of material '%s'", sClassName.c_str(), sMtlName.c_str() );
				}
			} else
			{
				EXPLOG.WriteWarning( "diffuse texture of  material '%s' is missing", sMtlName.c_str());
			}
		} else
		{
			EXPLOG.WriteWarning( "material '%s' doesnt have a diffuse texture map", sMtlName.c_str() );
		}


		shared_ptr<effShaderParams> phong_params(new effShaderParams());
		if (phong_data->m_FullpathDiffuse.GetNumNames() == 0)
			phong_params->SetShaderName(itString("Simple.fx"));
		else
			phong_params->SetShaderName(itString("Phong.fx"));
		phong_data->AddToParams(*phong_params);
		o_MatInfo.SetShaderParams(phong_params);

	}


	//=============================================================================
	// If the texture doesnt exist, emit a warning
	// 
	// If the texture  file name is a relative file path, 
	// resolve it.
	//=============================================================================

	void MtlExporter::WarnIfTextureDoesntExists( const fsLocator &texLoc )
	{
		fsLocator absFilePath( texLoc );
		if ( texLoc.GetNumNames() > 0 &&  ! fsFileUtil::IsFileNameAbsolute(  texLoc ) )
		{
			// the texture file name is relative, so resolve it			
			itString itMaxFilePath( reinterpret_cast< const itString::CharType * > ( OPTS.m_wsCurMaxFilepath.c_str()  ));
			fsLocator  tempFilePath;
			fsFileUtil::UnicodeStringToLocator( itMaxFilePath,   tempFilePath );
			// pop the filename part 
			tempFilePath.Pop();
			//add the texture file path
			tempFilePath.Push( texLoc);
			absFilePath = tempFilePath ;
		}
		//now we have the absolute path of the texture name.
		if ( !fsFileUtil::FileExists( absFilePath ) )
		{
			itString itMaxFilePath;
			fsFileUtil::LocatorToUnicodeString( absFilePath, itMaxFilePath );	
			std::string utfMaxFilePath = envString::WideCharToUTF8( itMaxFilePath.GetString( ), itMaxFilePath.GetLength() );
			EXPLOG.WriteWarning( "texture filename %s doesnt exist", utfMaxFilePath.c_str() );
		}
	}

}// namespace MaxExp