/****************************************************************************\
**	fbxExport.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "ImportExport/fbx/export/fbxExport.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"

#ifdef USE_FBX_IMPORTEXPORT

#include "EulerAngles.h"

//------------------------------------------------------------------------
//	library pragmas
//------------------------------------------------------------------------
#include "ImportExport/fbx/fbxSdk.hpp"
#include "ImportExport/fbx/fbxSdkManager.hpp"
#include "ImportExport/fbx/export/fbxExportData.hpp"

#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dIndexPtr.hpp"

#ifdef _DEBUG
#pragma comment(lib,"fbxsdk_md2008d.lib")
#else
#pragma comment(lib,"fbxsdk_md2008.lib")
#endif
#pragma comment(lib,"wininet.lib")

typedef unsigned char BYTE; 


//============================================================================
//============================================================================
namespace
{
	KFbxSdkManager* l_SdkManager;
	KFbxScene* l_Scene = NULL;
}

//--------------------------------------------------------------------
// GetTranslationVec()
//--------------------------------------------------------------------
maVector3d GetTranslationVec(maMatrix4x4 i_Mat)
{
	return maVector3d( i_Mat.m_Mat[12] , i_Mat.m_Mat[13], i_Mat.m_Mat[14] );
}

//--------------------------------------------------------------------
// GetScaleVec()
//--------------------------------------------------------------------
maVector3d GetScaleVec(maMatrix4x4 i_Mat)
{
	float Sx = sqrt( i_Mat.m_Mat[0]*i_Mat.m_Mat[0] + i_Mat.m_Mat[1]*i_Mat.m_Mat[1] + i_Mat.m_Mat[2]*i_Mat.m_Mat[2]);
	float Sy = sqrt( i_Mat.m_Mat[4]*i_Mat.m_Mat[4] + i_Mat.m_Mat[5]*i_Mat.m_Mat[5] + i_Mat.m_Mat[6]*i_Mat.m_Mat[6]);
	float Sz = sqrt( i_Mat.m_Mat[8]*i_Mat.m_Mat[8] + i_Mat.m_Mat[9]*i_Mat.m_Mat[9] + i_Mat.m_Mat[10]*i_Mat.m_Mat[10]);
	return maVector3d( Sx, Sy, Sz );
}

//--------------------------------------------------------------------
// GetRotationVec()
//--------------------------------------------------------------------
maVector3d GetRotationVec(maMatrix4x4 i_Mat)
{
	maMatrix4x4 transpose = i_Mat;
	transpose.Transpose();

	HMatrix M;
	for ( int i = 0 ; i < 4 ; i++ )
	{
		for ( int j = 0 ; j < 4 ; j++ )
		{
			M[i][j] = transpose.m_Mat[i*4+j];
		}	
	}

    EulerAngles ea;
    int i,j,k,h,n,s,f;
    EulGetOrd(16,i,j,k,h,n,s,f);
    if (s==EulRepYes) {
	float sy = sqrt(M[i][j]*M[i][j] + M[i][k]*M[i][k]);
	if (sy > 16*FLT_EPSILON) {
	    ea.x = atan2(M[i][j], M[i][k]);
	    ea.y = atan2(sy, M[i][i]);
	    ea.z = atan2(M[j][i], -M[k][i]);
	} else {
	    ea.x = atan2(-M[j][k], M[j][j]);
	    ea.y = atan2(sy, M[i][i]);
	    ea.z = 0;
	}
    } else {
	float cy = sqrt(M[i][i]*M[i][i] + M[j][i]*M[j][i]);
	if (cy > 16*FLT_EPSILON) {
	    ea.x = atan2(M[k][j], M[k][k]);
	    ea.y = atan2(-M[k][i], cy);
	    ea.z = atan2(M[j][i], M[i][i]);
	} else {
	    ea.x = atan2(-M[j][k], M[j][j]);
	    ea.y = atan2(-M[k][i], cy);
	    ea.z = 0;
	}
    }
    if (n==EulParOdd) {ea.x = -ea.x; ea.y = - ea.y; ea.z = -ea.z;}
    if (f==EulFrmR) {float t = ea.x; ea.x = ea.z; ea.z = t;}

	return maVector3d(ea.x,ea.y,ea.z) * maConstants::c_fRadToAngle;
}

// to save a scene to a FBX file
bool SaveScene(KFbxSdkManager* pSdkManager, KFbxDocument* pScene, const char* pFilename, int pFileFormat, bool pEmbedMedia)
{
    if(pSdkManager == NULL) return false;
    if(pScene      == NULL) return false;
    if(pFilename   == NULL) return false;

    bool lStatus = true;

    // Create an exporter.
    KFbxExporter* lExporter = KFbxExporter::Create(pSdkManager, "");

    if( pFileFormat < 0 || pFileFormat >= pSdkManager->GetIOPluginRegistry()->GetWriterFormatCount() )
    {
        // Write in fall back format if pEmbedMedia is true
        pFileFormat = pSdkManager->GetIOPluginRegistry()->GetNativeWriterFormat();

        if (!pEmbedMedia)
        {
            //Try to export in ASCII if possible
            int lFormatIndex, lFormatCount = pSdkManager->GetIOPluginRegistry()->GetWriterFormatCount();

            for (lFormatIndex=0; lFormatIndex<lFormatCount; lFormatIndex++)
            {
                if (pSdkManager->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex))
                {
                    KString lDesc =pSdkManager->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
                    char *lASCII = "ascii";
                    if (lDesc.Find(lASCII)>=0)
                    {
                        pFileFormat = lFormatIndex;
                        break;
                    }
                }
            }
        }
    }

    // Initialize the exporter by providing a filename.
    if(lExporter->Initialize(pFilename, pFileFormat, pSdkManager->GetIOSettings() ) == false)
    {
        return false;
    }

    // Set the export states. By default, the export states are always set to 
    // true except for the option eEXPORT_TEXTURE_AS_EMBEDDED. The code below 
    // shows how to change these states.
    (*(pSdkManager->GetIOSettings())).SetBoolProp(EXP_FBX_MATERIAL,        true);
    (*(pSdkManager->GetIOSettings())).SetBoolProp(EXP_FBX_TEXTURE,         true);
    (*(pSdkManager->GetIOSettings())).SetBoolProp(EXP_FBX_EMBEDDED,        pEmbedMedia);
    (*(pSdkManager->GetIOSettings())).SetBoolProp(EXP_FBX_SHAPE,           true);
    (*(pSdkManager->GetIOSettings())).SetBoolProp(EXP_FBX_GOBO,            true);
    (*(pSdkManager->GetIOSettings())).SetBoolProp(EXP_FBX_ANIMATION,       true);
    (*(pSdkManager->GetIOSettings())).SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

    // Export the scene.
    lStatus = lExporter->Export(pScene);

    // Destroy the exporter.
    lExporter->Destroy();

    return lStatus;
}


//--------------------------------------------------------------------
// BeginExportToFBX()
//--------------------------------------------------------------------
void fbxExport::BeginExportToFBX(fsLocator i_FullFilePath)
{
	//Create an SDK manager for your application
	l_SdkManager = fbxSdkManager::GetManager();

	// Create a scene object
	l_Scene = KFbxScene::Create(l_SdkManager, "");
}


//--------------------------------------------------------------------
// ExportMeshToFBX()
//--------------------------------------------------------------------
void fbxExport::ExportMeshToFBX(fbxGeometryData i_GeometryData, const g3dSceneNode * i_pNode)
{	

	g3dFragment * currFrag = (g3dFragment*)i_pNode->GetFragment();
	if ( currFrag && currFrag->GetFragmentName() != "" )
	{
		std::vector<int> indices;
		std::vector<maPoint3d> vertexParamList;
		std::vector<maPoint3d> normalParamList;
		std::vector<maPoint2d> uvParamList;

		int numFaces = currFrag->GetNumIndices() / 3;

		// if there's a sys mem copy of the vtx or index buffer, we should use it!
		BYTE* pIndexBuffer = currFrag->ReadOnlyLockIndices();

		int maxIndexBufferVal = 0;

		// need to know whether 16 or 32 bit indices.
		bool indexSize32 = g3dIndexPtr::Needs32Bit( currFrag->GetNumVertices() );
		if (indexSize32)
		{
			envType::UInt32 *buffer = reinterpret_cast<envType::UInt32*>(pIndexBuffer);
			for ( int i = 0 ; i < currFrag->GetNumIndices() ; i++ )
			{
				int currIdx = buffer[i];
				indices.push_back( currIdx );
				if ( currIdx >= maxIndexBufferVal )
					maxIndexBufferVal = currIdx;
			}
		}
		else
		{
			envType::UInt16 *buffer = reinterpret_cast<envType::UInt16*>(pIndexBuffer);
			for ( int i = 0 ; i < currFrag->GetNumIndices() ; i++ )
			{
				int currIdx = buffer[i];
				indices.push_back( currIdx );
				if ( currIdx >= maxIndexBufferVal )
					maxIndexBufferVal = currIdx;
			}
		}
		// done with buffer.
		currFrag->ReadOnlyUnlockIndices();

		// now get the vertices.
		BYTE* pVertexBuffer = currFrag->ReadOnlyLock();
		g3dType::BumpTex1Vertex* vBuffer = reinterpret_cast<g3dType::BumpTex1Vertex*>(pVertexBuffer);
		for ( int i = 0 ; i <= maxIndexBufferVal ; i++ )
		{
			vertexParamList.push_back( vBuffer[ i ].m_Vertex );
			normalParamList.push_back( vBuffer[ i ].m_Normal );
			uvParamList.push_back( vBuffer[ i ].m_TexCoord );
		}

		// Strings for future naming
		std::string baseName = i_pNode->GetBakeName();
		std::string meshName = baseName + "_MESH";
		std::string layerName = baseName + "_LYR";
		std::string normalLayerName = baseName + "_NORM_LYR";
		std::string uvLayerName = baseName + "_UV_LYR";

		// done with buffer.
		currFrag->ReadOnlyUnlock();

		// Create a mesh object
		KFbxMesh* lMesh = KFbxMesh::Create(l_SdkManager, meshName.c_str());

		// Get the scene's root node
		KFbxNode* lRootNode = l_Scene->GetRootNode();

		// Create a node for the mesh.
		KFbxNode* lNode = KFbxNode::Create(l_SdkManager, layerName.c_str());

		// Set the node as a child of the scene’s root node.
		lRootNode->AddChild(lNode);

		// Set the mesh as the node attribute of the node
		lNode->SetNodeAttribute(lMesh);

		// set the shading mode to view texture
		lNode->SetShadingMode(KFbxNode::eFLAT_SHADING);

		lNode->SetName( currFrag->GetFragmentName().c_str() );

		// Create layer
		KFbxLayer* lLayer = lMesh->GetLayer(0);
		if (lLayer == NULL)
		{
			lMesh->CreateLayer();
			lLayer = lMesh->GetLayer(0);
		}

		// node transformations
		maVector3d scale = GetScaleVec(i_pNode->GetTotalTransform());
		maVector3d rotation = GetRotationVec(i_pNode->GetTotalTransform());
		maVector3d translation = GetTranslationVec(i_pNode->GetTotalTransform());

		lNode->SetGeometricScaling(KFbxNode::eSOURCE_SET,KFbxVector4(scale.GetX(), scale.GetY(), scale.GetZ()));
		lNode->SetGeometricRotation(KFbxNode::eSOURCE_SET,KFbxVector4(rotation.GetX(), rotation.GetY(), rotation.GetZ()));
		lNode->SetGeometricTranslation(KFbxNode::eSOURCE_SET,KFbxVector4(translation.GetX(), translation.GetY(), translation.GetZ()));

		// Create control points.
		lMesh->InitControlPoints(currFrag->GetNumVertices());

		KFbxVector4* lControlPoints = lMesh->GetControlPoints();

		// Normals
		KFbxLayerElementNormal* lNormalLayer = KFbxLayerElementNormal::Create(lMesh, normalLayerName.c_str());
		lNormalLayer->SetMappingMode(KFbxLayerElement::eBY_CONTROL_POINT);
		lNormalLayer->SetReferenceMode(KFbxLayerElement::eDIRECT);

		// UVs
		KFbxLayerElementUV* lUVDiffuseLayer = KFbxLayerElementUV::Create(lMesh, uvLayerName.c_str());
		lUVDiffuseLayer->SetMappingMode(KFbxLayerElement::eBY_CONTROL_POINT);
		lUVDiffuseLayer->SetReferenceMode(KFbxLayerElement::eDIRECT);

		for ( int i = 0 ; i < currFrag->GetNumVertices() ; i++ )
		{
			lControlPoints[i] = KFbxVector4( vertexParamList[i].GetX() , vertexParamList[i].GetY() , vertexParamList[i].GetZ() );
			lNormalLayer->GetDirectArray().Add( KFbxVector4( normalParamList[i].GetX() , normalParamList[i].GetY() , normalParamList[i].GetZ() ) );
			lUVDiffuseLayer->GetDirectArray().Add( KFbxVector2( uvParamList[i].GetX() , 1-uvParamList[i].GetY() ) );
		}

		for ( int i = 0 ; i < numFaces ; i++ )
		{
			// Make a polygon (triangle)
			lMesh->BeginPolygon(i);
			for(int j = 0; j < 3; j++)
			{
				lMesh->AddPolygon(indices[i*3 + j]);
			}
			lMesh->EndPolygon();
		}

		lLayer->SetNormals(lNormalLayer);
		lLayer->SetUVs(lUVDiffuseLayer);

		if ( i_GeometryData.m_bHasBeenBaked )
		{
			std::string texName = baseName + "_TEX";
			std::string matName = baseName + "_MAT";
			std::string texLayerName = baseName + "_TEX_LYR";
			std::string matLayerName = baseName + "_MAT_LYR";
			std::string texFileName = baseName + "_Baked_Color." + i_GeometryData.m_BakeExt;

			std::string fullBakeTexStr;
			fsLocator fullBakeTexLoc = i_GeometryData.m_BakedPath;
			fullBakeTexLoc.Push(texFileName.c_str());
			fsFileUtil::LocatorToANSIFilename( fullBakeTexLoc , fullBakeTexStr );

			// Material mapping
			KFbxLayerElementMaterial* lMaterialLayer=KFbxLayerElementMaterial::Create(lMesh,matLayerName.c_str());
			lMaterialLayer->SetMappingMode(KFbxLayerElement::eALL_SAME);
			lMaterialLayer->SetReferenceMode(KFbxLayerElement::eINDEX_TO_DIRECT);
			if(lMesh->GetLayer(0)) lMesh->GetLayer(0)->SetMaterials(lMaterialLayer);

			KFbxSurfacePhong* gMaterial = KFbxSurfacePhong::Create(l_Scene, matName.c_str());

			// Romey-specific params
			gMaterial->GetShadingModel().Set("Lambert");
			gMaterial->GetAmbientColor().Set(fbxDouble3(0, 0, 0));
			gMaterial->GetEmissiveColor().Set(fbxDouble3(1, 1, 1));
			gMaterial->GetDiffuseFactor().Set(0);
			lNode->AddMaterial(gMaterial);

			// Texture mapping 
			KFbxLayerElementTexture* lTextureDiffuseLayer = KFbxLayerElementTexture::Create(lMesh, texLayerName.c_str());
			lTextureDiffuseLayer->SetMappingMode(KFbxLayerElement::eALL_SAME);
			lTextureDiffuseLayer->SetReferenceMode(KFbxLayerElement::eINDEX_TO_DIRECT);
			if(lMesh->GetLayer(0)) lMesh->GetLayer(0)->SetTextures(KFbxLayerElement::eDIFFUSE_TEXTURES, lTextureDiffuseLayer);

			KFbxTexture* gTexture = KFbxTexture::Create(l_SdkManager,texName.c_str());
			KString lTexPath = fullBakeTexStr.c_str();
			gTexture->SetFileName(lTexPath.Buffer());
			gTexture->SetTextureUse(KFbxTexture::eSTANDARD);
			gTexture->SetMappingType(KFbxTexture::eUV);
			gTexture->SetMaterialUse(KFbxTexture::eMODEL_MATERIAL);
			lMesh->GetLayer(0)->GetTextures(KFbxLayerElement::eDIFFUSE_TEXTURES)->GetDirectArray().Add(gTexture);

			if (gTexture) gMaterial->GetDiffuseColor().ConnectSrcObject(gTexture);
		}

	}

	const int num_kids = i_pNode->GetNumChildren();
	for (int i=0; i<num_kids; i++)
	{
		ExportMeshToFBX(i_GeometryData, i_pNode->GetChild(i));
	}
	
}

//--------------------------------------------------------------------
// ExportCameraToFBX()
//--------------------------------------------------------------------
void fbxExport::ExportCameraToFBX(fbxCameraData i_CameraData)
{
	std::string baseName = i_CameraData.m_Name;
	std::string camName = baseName + "_cam";
	std::string camNode = baseName + "_camNode";
	std::string marker = baseName + "_marker";
	std::string markerNode = baseName + "_markerNode";

	KFbxCamera* myCamera = KFbxCamera::Create(l_Scene, camName.c_str());

	if ( i_CameraData.m_bIsOrthographic )
	{
		myCamera->ProjectionType.Set( KFbxCamera::eORTHOGONAL );
	}
	else
	{
		myCamera->ProjectionType.Set( KFbxCamera::ePERSPECTIVE );

		if ( i_CameraData.m_bEnableALP )
		{
			myCamera->SetApertureMode( KFbxCamera::eFOCAL_LENGTH );
			myCamera->SetApertureWidth( i_CameraData.m_HorizontalAperture );
			myCamera->FocalLength.Set( i_CameraData.m_FocalLength );
		}
		else
		{
			myCamera->SetApertureMode( KFbxCamera::eHORIZONTAL );
			myCamera->SetApertureWidth( 1.41732283 );
			myCamera->FieldOfView.Set( i_CameraData.m_FOV );
		}
	}
	

	myCamera->SetNearPlane( i_CameraData.m_NearClip );
	myCamera->SetFarPlane( i_CameraData.m_FarClip );

	KFbxNode* myCameraNode = KFbxNode::Create(l_Scene, camNode.c_str());
	myCameraNode->SetName( baseName.c_str() );
	myCameraNode->LclTranslation.Set(KFbxVector4(i_CameraData.m_Position.GetX(), i_CameraData.m_Position.GetY(), i_CameraData.m_Position.GetZ()));
 
	myCameraNode->SetNodeAttribute(myCamera);
	KFbxNode* myRootNode = l_Scene->GetRootNode();
	myRootNode->AddChild(myCameraNode);
	KFbxMarker* myMarker = KFbxMarker::Create(l_Scene, marker.c_str());
	KFbxNode* myMarkerNode = KFbxNode::Create(l_Scene, markerNode.c_str());
	myMarkerNode->SetNodeAttribute(myMarker);
	myRootNode->AddChild(myMarkerNode);

	myMarkerNode->LclTranslation.Set(KFbxVector4(i_CameraData.m_Target.GetX(), i_CameraData.m_Target.GetY(), i_CameraData.m_Target.GetZ()));
	myCameraNode->SetTarget(myMarkerNode);

	//l_Scene->GetGlobalSettings().SetDefaultCamera((char *) myCamera->GetName());
}

//--------------------------------------------------------------------
// EndExportToFBX()
//--------------------------------------------------------------------
void fbxExport::EndExportToFBX(fsLocator i_FullFilePath)
{

	const char* lFBX7Binary = "FBX binary (*.fbx)";
    const char* lFBX7ASCII = "FBX ascii (*.fbx)";
    const char* lFBX6Binary = "FBX 6.0 binary (*.fbx)";
    const char* lFBX6ASCII = "FBX 6.0 ascii (*.fbx)";
    int lFormat = l_SdkManager->GetIOPluginRegistry()->FindWriterIDByDescription(lFBX6Binary);

	std::string pFilename;
	fsFileUtil::LocatorToANSIFilename(i_FullFilePath,pFilename);

    // Save the scene.
	bool exportStatus = SaveScene(l_SdkManager, l_Scene, pFilename.c_str(), lFormat, true);

	l_Scene->Destroy();
	l_Scene = NULL;
}

#endif
