/****************************************************************************\
**  sgpuModelExportScene.cpp
**
**      sgpuModelExportScene.hpp defines the sgpuModelExportScene class
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "sgpuModelExportScene.hpp"
#include "sgpuMaterial.hpp"
#include "sgpuMaterialImpl.hpp"
#include "sgpuNode.hpp"
#include "sgpuNodeImpl.hpp"
#include "sgpuUtilsImpl.hpp"
#include "sgpuException.hpp"
#include "sgpuStringImpl.hpp"

#include "Core/Env/envExceptionX.hpp"
#include "Core/Env/envString.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/mdl/mdlWriter.hpp"
#include "Graphics/mdl/mdlReader.hpp"
#include "Graphics/mdl/mdlNodeUtil.hpp"
#include <sstream>


//========================================================================
// There is version information here and version information
// in the Resources sgpuExportLib.rc and in the ReadMe.txt
//========================================================================

//========================================================================
//========================================================================
struct sgpuModelExportSceneImpl
{
	shared_ptr<mdlNodeInfo> m_NodeGraph;
	mdlMatInfoTable m_MaterialTable;
	std::map<std::string, shared_ptr<effPhongData> > m_PhongData;

	sgpuModelExportSceneImpl()	: m_NodeGraph(new mdlNodeInfo) {}
	void Reset() 
	{
		m_NodeGraph.reset( new mdlNodeInfo );
		m_PhongData.clear();
		m_MaterialTable.clear();
	}
	void DescribeSceneHierarchyRec( std::string &o_desc , shared_ptr< mdlNodeInfo > &i_Node, const std::string &i_Prefix );
};

//========================================================================
//========================================================================
namespace
{
	//------------------------------------------------------------------------
	// Make sure all normals are normalized and non-zero
	//------------------------------------------------------------------------
	void confirm_normals(std::vector<maVector3d> &io_Normals)
	{
		std::vector<maVector3d>::iterator it;
		for (it = io_Normals.begin(); it != io_Normals.end(); ++it)
		{
			if (!it->Normalize())
				it->Set(0,1,0); // Give zero-length normals a new unit vector value
		}
	}

	//------------------------------------------------------------------------
	// Look for meshes in scene and prepare data for exporting
	//------------------------------------------------------------------------
	void prepare_meshes(shared_ptr<mdlNodeInfo> &io_NodeGraph)
	{
		if (io_NodeGraph->m_MeshInfo)
		{
			// Check the normals and make sure all are normalized
			confirm_normals(io_NodeGraph->m_MeshInfo->m_Normals);

			// Create basis vectors now to write into file and save time on import
			mdlFragUtil::CreateBasisVectors(*io_NodeGraph->m_MeshInfo);
		}

		// Recurse on children
		const int num_kids = io_NodeGraph->m_Children.size();
		for  (int i=0; i<num_kids; ++i)
			prepare_meshes(io_NodeGraph->m_Children[i]);
	}

	//------------------------------------------------------------------------
	// Actual write of scene, takes fsLocator that was converted from
	// ANSI or UNICODE strings earlier.
	//------------------------------------------------------------------------
	bool write_scene(const fsLocator& i_Locator,
		const char* i_VersionString,
		shared_ptr<mdlNodeInfo> &i_NodeGraph,
		mdlMatInfoTable &i_MaterialTable,
		std::map<std::string, shared_ptr<effPhongData> > &i_PhongData)
	{
		try
		{
			// Prepare all of the materials
			mdlMatInfoTable::iterator mat_it;
			std::map<std::string, shared_ptr<effPhongData> >::iterator phong_it = i_PhongData.begin();
			for (mat_it = i_MaterialTable.begin(); mat_it != i_MaterialTable.end(); ++mat_it, ++phong_it)
			{
				shared_ptr<effShaderParams> phong_params(new effShaderParams()); 
				if (phong_it->second->m_FullpathDiffuse.GetNumNames() == 0)
					phong_params->SetShaderName(itString("Simple.fx"));
				else
					phong_params->SetShaderName(itString("Phong.fx"));
				phong_it->second->AddToParams(*phong_params);

				mat_it->second->m_Info.SetShaderParams(phong_params);
			}

			// Prepare meshes also (make S and T vectors, confirm normals, etc.)
			prepare_meshes(i_NodeGraph);

			// Combine our info with user info to make version string
			std::string version_string(i_VersionString);
			version_string += "\nsgpuExportLib version ";
			version_string += c_ExportLib_Version;

			// Do actual write
			mdlWriter::WriteHierarchicalModel(i_Locator, 
				i_NodeGraph, 
				i_MaterialTable,
				version_string);

			return true;
		}
		catch (envExceptionX&)
		{
			// catch all custom exceptions that could be thrown by our library code
			return false;
		}
	}

} // end of namespace

//========================================================================
//	default constructor
//========================================================================
sgpuModelExportScene::sgpuModelExportScene()
: m_pImpl(new sgpuModelExportSceneImpl)
{

}

//========================================================================
//	disallowed copy constructor
//========================================================================

sgpuModelExportScene::sgpuModelExportScene( const sgpuModelExportScene &)
: m_pImpl(NULL)
{
	std::stringstream ss;
	ss << "copy constructor for sgpuModelExportScene not allowed";
	DBG_ASSERT( false, ss.str().c_str() );
}

//========================================================================
//	disallowed assignment operator
//========================================================================

sgpuModelExportScene & sgpuModelExportScene::operator=( const sgpuModelExportScene &)
{
	std::stringstream ss;
	ss << "assignment operator for sgpuModelExportScene not allowed";
	DBG_ASSERT( false, ss.str().c_str() );
	return *this;
}
//========================================================================
//	destructor
//========================================================================
sgpuModelExportScene::~sgpuModelExportScene()
{
	delete m_pImpl;
}

//========================================================================
// Write the scene's contents to the given filename, which should
// have the extension ".gxb". Version string is written to 
// file to track how the geometry file was generated.
//========================================================================
bool sgpuModelExportScene::WriteScene(const sgpuString& i_Filename,
		const sgpuString& i_VersionString)
{
	std::string sDataUTF8 = i_Filename.m_pImpl->m_Data;
	std::wstring wData;
	UTF8ToUnicode( sDataUTF8.c_str(), wData );
	std::wstring wVersionString;
	UTF8ToUnicode( i_VersionString.m_pImpl->m_Data.c_str(), wVersionString );
	bool bRet = WriteScene( wData.c_str(), wVersionString.c_str() );
	return bRet;
}
bool sgpuModelExportScene::WriteScene(const wchar_t* i_Filename,
						const wchar_t* i_VersionString)
{
	itString it_version( (itString::CharType*) i_VersionString);
	std::string version_string = itStringUtil::GetStdString(it_version);

	itString it_filename( (itString::CharType*) i_Filename);

	fsLocator file_loc;
	fsFileUtil::UnicodeStringToLocator(it_filename, file_loc);
	return write_scene(file_loc,
		version_string.c_str(),
		m_pImpl->m_NodeGraph,
		m_pImpl->m_MaterialTable,
		m_pImpl->m_PhongData);
}

bool sgpuModelExportScene::ReadScene( const sgpuString& i_Filename)
{
	std::string sDataUTF8 = i_Filename.m_pImpl->m_Data;
	std::wstring wData;
	UTF8ToUnicode( sDataUTF8.c_str(), wData );
	bool bRet = ReadScene( wData.c_str() );
	return bRet;
}

bool sgpuModelExportScene::ReadScene( const wchar_t* i_Filename)
{
#if defined(NEVER)
	m_pImpl->Reset();
	itString it_filename( (itString::CharType*) i_Filename);
	fsLocator file_loc;
	fsFileUtil::UnicodeStringToLocator(it_filename, file_loc);
	m_pImpl->m_NodeGraph.reset();
	shared_ptr< mdlNodeInfo > &nodeGraph = m_pImpl->m_NodeGraph;
	mdlMatInfoTable &mtlTable = m_pImpl->m_MaterialTable;	
	mdlReader::ReadHierarchicalModel( file_loc, nodeGraph, mtlTable );
	mdlMatInfoTable::const_iterator mit;
	shared_ptr< mdlMatInfo > mtlInfo;
	for( mit = mtlTable.begin(); mit != mtlTable.end(); ++mit )
	{
		const std::string &sMtlName( mit->first );
		mtlInfo = mit->second;
		shared_ptr<effShaderParams> shaderParams = mtlInfo->m_Info.GetShaderParams();
		shared_ptr< effPhongData > phongData( new effPhongData() );
		std::pair< std::string, shared_ptr< effPhongData > > valuePair( sMtlName, phongData );
		m_pImpl->m_PhongData.insert( valuePair );
		phongData->AddFromParams( *shaderParams );
	}
#endif
	return true;
}
//========================================================================
// Returns handle to the root node that is built into scene. With this 
// node, you can then build a hieararchy and add meshes. 
//========================================================================
sgpuNode sgpuModelExportScene::GetRootNode()
{
	// Return new public handle for the root node we have in our implementation
	sgpuNode root_node;
	root_node.m_pImpl->m_Node = this->m_pImpl->m_NodeGraph;
	return root_node;
}

//========================================================================
// Materials are shared in the scene and must be uniquely named.
// Create materials here and then assign them to sgpuMesh objects.
// If you call this function with the same name twice, it will return
// a handle to the first material instead of creating a new material.
//========================================================================
sgpuMaterial sgpuModelExportScene::CreateMaterial(const sgpuString& i_MaterialName)
{
	sgpuMaterial material;
	const std::string sMaterialName ( i_MaterialName.m_pImpl->m_Data );
	mdlMatInfoTable::iterator it = m_pImpl->m_MaterialTable.find(i_MaterialName.m_pImpl->m_Data);
	if (it == m_pImpl->m_MaterialTable.end())
	{
		// Create new material
		material.m_pImpl->m_MatInfo.reset(new mdlMatInfo);
		material.m_pImpl->m_MatInfo->m_Info.SetMaterialName( sMaterialName );
		m_pImpl->m_MaterialTable[ sMaterialName ] = material.m_pImpl->m_MatInfo;
		material.m_pImpl->m_PhongData.reset(new effPhongData);
		material.m_pImpl->m_PhongData->m_ColorSpecular.Set(0,0,0,0); // default specular off
		m_pImpl->m_PhongData[ sMaterialName ] = material.m_pImpl->m_PhongData;
		material.m_pImpl->m_Name = sMaterialName;
	}
	else
	{
		// Return existing material
		material.m_pImpl->m_Name = sMaterialName;
		material.m_pImpl->m_MatInfo = it->second;
		material.m_pImpl->m_PhongData = m_pImpl->m_PhongData[ sMaterialName ];
	}
	return material;
}

sgpuMaterial sgpuModelExportScene::GetMaterial(const sgpuString &i_MaterialName)const
{
	sgpuMaterial material;
	const std::string &sMaterialName ( i_MaterialName.m_pImpl->m_Data );
	mdlMatInfoTable::iterator it = m_pImpl->m_MaterialTable.find( sMaterialName );
	ITER_END_EXCEPTION( it, m_pImpl->m_MaterialTable.end(), "Material", sMaterialName)
	material.m_pImpl->m_Name = sMaterialName;
	material.m_pImpl->m_MatInfo = it->second;
	material.m_pImpl->m_PhongData = m_pImpl->m_PhongData[ sMaterialName ];
	return material;
}

sgpuMaterial sgpuModelExportScene::GetMaterial( int i_MtlIdx )const
{	
	sgpuMaterial material;
	INVALID_RANGE_EXCEPTION( i_MtlIdx, m_pImpl->m_MaterialTable.size(), "sgpuModelExportScene::GetMaterial" )
	mdlMatInfoTable::const_iterator mit = m_pImpl->m_MaterialTable.begin();
	std::advance( mit, i_MtlIdx );
	static char sMtlIdx[256];
	sscanf_s( sMtlIdx, "%d", i_MtlIdx );
	ITER_END_EXCEPTION( mit, m_pImpl->m_MaterialTable.end(), "Material", sMtlIdx )
	material.m_pImpl->m_Name = mit->first;
	material.m_pImpl->m_MatInfo = mit->second;
	material.m_pImpl->m_PhongData = m_pImpl->m_PhongData[ mit->first ];
	return material;
}

bool sgpuModelExportScene::IsMaterial( const sgpuString &i_MaterialName )const
{
	const std::string &sMaterialName ( i_MaterialName.m_pImpl->m_Data );
	mdlMatInfoTable::iterator it = m_pImpl->m_MaterialTable.find( sMaterialName );
	return (it != m_pImpl->m_MaterialTable.end());
}



int sgpuModelExportScene::GetNumMaterials() const
{
	return  static_cast< int > ( m_pImpl->m_MaterialTable.size() );
}

void sgpuModelExportScene::MakeErrorMaterial( )
{		
	if( !IsMaterial( sgpuString( SGPU_ERROR_MATERIAL_NAME ) ) )
	{
		sgpuMaterial sgpuMtl = CreateMaterial( sgpuString( SGPU_ERROR_MATERIAL_NAME ) );
		sgpuMtl.SetDiffuseColor( 0.5f, 0.5f, 0.5f );
		sgpuMtl.SetSpecularColor(0,0,0 );
		sgpuMtl.SetShininess( 1.0f );
	}
}


void FlattenSceneRec( shared_ptr< mdlNodeInfo > &i_Node , const maMatrix4x4 &i_CumulativeTm )
{
	maMatrix4x4 cumulativeTm = i_Node->m_Transform * i_CumulativeTm;
	i_Node->m_Transform.Identity();	
	if( NULL != i_Node->m_MeshInfo )
	{
		shared_ptr< mdlFragInfo > mesh = i_Node->m_MeshInfo;	
		std::vector<maPoint3d>::iterator vit;
		for( vit = mesh->m_Vertices.begin(); vit != mesh->m_Vertices.end(); ++vit)
		{
			maPoint3d &p = *vit;
			p = cumulativeTm * p;
		}
		std::vector<maPoint3d>::iterator nit;
		for( nit = mesh->m_Normals.begin(); nit != mesh->m_Normals.end(); ++nit )
		{
			maVector3d &n = *nit;
			cumulativeTm.TransformDir(n);	
		}
	} else if ( i_Node->m_SubdivInfo )
	{
		shared_ptr< mdlSubdivInfo > subdiv = i_Node->m_SubdivInfo;
		std::vector<maPoint3d>::iterator vit;
		for( vit = subdiv->m_Vertices.begin(); vit != subdiv->m_Vertices.end(); ++vit)
		{
			maPoint3d &p = *vit;
			p = cumulativeTm * p;
		}
	} else if ( i_Node->m_InstanceInfo )
	{
		i_Node->m_Transform = cumulativeTm;
	}
	std::vector< shared_ptr<mdlNodeInfo> >::iterator chit;
	for( chit = i_Node->m_Children.begin(); chit != i_Node->m_Children.end(); ++chit )
	{
		shared_ptr<mdlNodeInfo> ch = *chit;		
		FlattenSceneRec( ch, cumulativeTm );
	}
}


void ComputeBasisVectorsForMeshRec( shared_ptr< mdlNodeInfo > &i_Node )
{
	shared_ptr< mdlFragInfo > mesh = i_Node->m_MeshInfo;
	if( NULL != mesh )
	{
		mdlFragUtil::CreateBasisVectors( *mesh );
	}
	
	std::vector< shared_ptr<mdlNodeInfo> >::iterator chit;
	for( chit = i_Node->m_Children.begin(); chit != i_Node->m_Children.end(); ++chit )
	{
		shared_ptr<mdlNodeInfo> ch = *chit;		
		ComputeBasisVectorsForMeshRec( ch );
	}
}

void sgpuModelExportScene::FlattenScene( )
{
	shared_ptr< mdlNodeInfo > rootNode = m_pImpl->m_NodeGraph;
	maMatrix4x4 ident;
	ident.Identity();
	FlattenSceneRec( rootNode, ident );
	
}
//========================================================================
//	Output formatted scene hierarchy into the 'o_Hierarchy' string
//========================================================================

void sgpuModelExportSceneImpl::DescribeSceneHierarchyRec( std::string &o_Hierarchy, shared_ptr< mdlNodeInfo > & i_Node, const std::string &i_Prefix )
{
	std::stringstream ss;
	std::string prefix = i_Prefix + "    ";
	ss  << prefix <<  "<node>: " << i_Node->m_NodeName << "\n";
	if( i_Node->m_MeshInfo )
	{
		ss << prefix << "    <mesh>: " << i_Node->m_MeshInfo->m_Name << "\n";
	} else if ( i_Node->m_SubdivInfo )
	{
		ss << prefix << "    <subd>: " << i_Node->m_SubdivInfo->m_Name << "\n";
	} else if ( i_Node->m_InstanceInfo )
	{
		ss << prefix << "    <instance>: " << i_Node->m_InstanceInfo->GetPathAsString("|") << "\n";
	}
	o_Hierarchy = o_Hierarchy.append( ss.str() );
	
	std::vector< shared_ptr<mdlNodeInfo> >::iterator chit;
	for( chit = i_Node->m_Children.begin(); chit != i_Node->m_Children.end(); ++chit )
	{
		shared_ptr<mdlNodeInfo> ch = *chit;		
		DescribeSceneHierarchyRec( o_Hierarchy, ch, prefix );
	}

}

//========================================================================
//	Output scene hierarchy and material table into the 'o_Hierarchy' string
//========================================================================
void sgpuModelExportScene::Describe( sgpuString &o_Hierarchy )
{
	std::string output("SceneHierarchy:\n");
	m_pImpl->DescribeSceneHierarchyRec( output, m_pImpl->m_NodeGraph, std::string( "" ) );
	//material table output
	std::stringstream ss;
	ss  << output;
	ss << "Material Table:\n";
	int nMtls = GetNumMaterials();
	for( int i=0; i < nMtls; ++i )
	{
		sgpuMaterial mtl = GetMaterial( i );
		ss << mtl.m_pImpl->m_Name << "\n";
		sgpuString sTex = mtl.GetDiffuseTexture();
		if( !sTex.m_pImpl->m_Data.empty() )
		{
			ss << "    <tex>:" << sTex.m_pImpl->m_Data << "\n"; 
		}
	}
	o_Hierarchy.m_pImpl->m_Data = ss.str();
}
//MergeMaterials
void sgpuModelExportScene::MergeByMaterials( sgpuString &i_Prefix, int &o_numMeshesMerged, int &o_numMergeResults )
{
	ComputeBasisVectorsForMeshRec( GetRootNode().m_pImpl->m_Node );
	mdlNodeUtil::DoMerge( i_Prefix.m_pImpl->m_Data, GetRootNode().m_pImpl->m_Node, 30000, o_numMeshesMerged, o_numMergeResults);
}

#if defined( SGPU_SUPPORT_1200)
sgpuMaterial sgpuModelExportScene::CreateMaterial(const char* i_MaterialName)
{
	REPORT_OBSOLETE( "sgpuModelExportScene::CreateMaterial" )
	return CreateMaterial( sgpuString( i_MaterialName ) );
}

sgpuMaterial sgpuModelExportScene::CreateMaterial(const wchar_t* i_MaterialName)
{
	return CreateMaterial( sgpuString( i_MaterialName ) );
}

bool sgpuModelExportScene::WriteScene(const char* i_Filename,
						   const char* i_VersionString)
{
	return WriteScene( sgpuString( i_Filename ), sgpuString( i_VersionString ) );
}
#endif
