#include "stdafx.h"

#include "resource.h"
#include "SGPUExporter.h"
#include "LocalizedStrings.h"

#pragma warning(disable:4996)

SGPUExporter::
SGPUExporter():m_scene(), m_def_mat(0), m_InheritanceManager(*this)
{
}

SGPUExporter::
~SGPUExporter()
{
	delete m_def_mat;
}

bool SGPUExporter::
DoExport(const wchar_t* fileName, IUnknown* activeDocument, IProgressCB* pCB)
{
	m_document = (ISkpDocument*) activeDocument;
    m_progressBar = pCB;
	m_filename = fileName;

	if( m_document==NULL )
    {
        //printf("The document interface is not set\n");
        return false;
    }

	m_isOk = false;

	if (!StartExport())
		return false;

	if (!ExportScene())
		return false;

	if (!FinilizeExport())
		return false;

	m_isOk = true;
    
	return true;
}

bool SGPUExporter::
StartExport()
{
	sgpuNode root_node = m_scene.GetRootNode();
	root_node.SetNodeName(MakeUniqueName(L"RootNode", false).c_str());

	sgpuMatrix matrix;
	matrix.MakeRotate( (float)(-3.14159265*0.5), 1.f, 0.f, 0.f );
	sgpuMatrix matrix1;
	matrix1.MakeRotate( (float)(3.14159265), 0.f, 1.f, 0.f );
	matrix = matrix*matrix1;
	root_node.SetTransformationMatrix(matrix);
	
	if (m_progressBar)
    {
        m_progressBar->SetPercentDone(0.0);
        //m_progressBar->SetProgressMessage(LoadLocalizedString(IDS_WRITING_TEXTURE_FILES));
	}

	wchar_t drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
	_wsplitpath( m_filename.c_str(), drive, dir, fname, ext );

	std::wstring logname = drive;
	logname += dir;
	logname += fname;
	logname += L".log";
	
	m_log.StartLog( logname.c_str(), L"Export into StudioGPU format" );

	return true;
}

bool SGPUExporter::
ExportScene()
{
	m_currentName = L"UnderRoot";
	if (m_progressBar)
        m_progressBar->SetProgressMessage(LoadLocalizedString(IDS_PROCESSING_MATERIALS));
	LoadMaterials();
	
	if (m_progressBar)
    {
        m_progressBar->SetPercentDone(10.0);
        m_progressBar->SetProgressMessage(LoadLocalizedString(IDS_PROCESSING_SCENE));
    }

	return WriteGeometry();
}

bool SGPUExporter::
FinilizeExport()
{
	try
	{
		if (m_progressBar)
		{
			m_progressBar->SetProgressMessage(LoadLocalizedString(IDS_WRITING_FILE));
			m_progressBar->SetPercentDone(90.0);
		}
		return m_scene.WriteScene( m_filename.c_str(), TEXT("SketchUp export plug-in. v 1.0") );
	}
	catch (...)
	{
		m_log.WriteLog( L"Cannot write GXB file\n" );
		return false;
	}

	if (m_progressBar)
	{
		m_progressBar->SetProgressMessage(LoadLocalizedString(IDS_EXPORT_COMPLETED));
		m_progressBar->SetPercentDone(100.0);
	}

	return true;
}

std::wstring SGPUExporter::
MakeUniqueName(std::wstring name, bool isMaterial)
{
	if (!name.length())
		name = TEXT("Unnamed");

	std::wstring::reverse_iterator it;
	std::wstring num, nm;
	wchar_t buff[64];

	//Get last digits
	for ( it=name.rbegin(); it!=name.rend() && (*it>=TEXT('0') && *it<=TEXT('9')); it++ )
		num = *it + num;
	nm.assign( name, 0, name.length()-num.length() );

	//Search exist names and generate new one, if necessary
	std::set<std::wstring>& list = isMaterial?m_matnames:m_onames;
	__int64 n = _wtoi64( num.c_str() );

	while (list.find(name)!=list.end())
	{
		_i64tow_s( ++n, buff, 64, 10 );
		name = nm + buff;
	}

	list.insert( name );
	return name;
}

long SGPUExporter::
GetEntityId(IUnknown* pUnk)
{
    long entityId = 0;
    CComPtr<ISkpEntity> pEntity = NULL;
    if( SUCCEEDED(pUnk->QueryInterface(IID_ISkpEntity, (void**)&pEntity)) )
    {
        pEntity->get_Id(&entityId);
        return entityId;
    }

    return -1;
}

void SGPUExporter::
LoadMaterials()
{
	HRESULT hr;

	/*
	CComPtr<ISkpMaterials> pMats;

	hr = m_document->get_Materials(&pMats);
	long count = 0L;
	hr = pMats->get_Count(&count);

	for(long i=0; i<count; i++)
	{
		CComPtr<ISkpMaterial> pMat;
		hr = pMats->get_Item(i, &pMat);
		LoadMaterial(pMat);
	}
	*/

	//Prepare texture Operations
	
    // Get the ISkpApplication from the ISkpDocument
    CComPtr<ISkpApplication> pApp;
    hr = m_document->get_Application(&pApp);

    // Get the ISkpTextureWriter2 interface from ISkpApplication.  You
    // first have to get the ISkpTextureWriter, then get the ISkpTextureWriter2
    // extension from that.
    CComPtr<ISkpTextureWriter> pTW;
    hr = pApp->CreateTextureWriter(&pTW);
    hr = pTW->QueryInterface(IID_ISkpTextureWriter2, (void**)&m_pTextureWriter);

}

void SGPUExporter::
CheckLoadMaterial(CComPtr<ISkpMaterial> pMaterial, long matID)
{
	if (m_materials.find(matID)!=m_materials.end())
		return;
	LoadMaterial(pMaterial);
}

void SGPUExporter::
LoadMaterial(CComPtr<ISkpMaterial> pMaterial)
{
    if(pMaterial == NULL)
        return;

    HRESULT hr;
    BSTR materialName;

    hr = pMaterial->get_Name(&materialName);
    COLE2T matName1(materialName);
    SysFreeString(materialName);
	std::wstring matName(matName1);

	std::wstring name = MakeUniqueName( matName, true );
	if (matName == L"")
		m_log.WriteLog( L"(no name) material renamed to '%s'\n", name.c_str() );
	else if (matName != name)
		m_log.WriteLog( L"'%s' material renamed to '%s'\n", matName.c_str(), name.c_str() );

	sgpuMaterial * mat;
	m_materials[GetEntityId(pMaterial)] = AutoPtr<sgpuMaterial>( mat = new sgpuMaterial( m_scene.CreateMaterial(name.c_str()) ) );

    //Color
    BOOL isColor;
    hr = pMaterial->get_IsColor(&isColor);
    if ( isColor )
    {
        OLE_COLOR color;
        hr = pMaterial->get_Color(&color);
        mat->SetDiffuseColor( (color&0x0000ff)/255.f, ((color&0x00ff00)>>8)/255.f, ((color&0xff0000)>>16)/255.f );
    }
	else
		mat->SetDiffuseColor( 1.0f, 1.0f, 1.0f );

    // Alpha
    BOOL usesAlpha;
    hr = pMaterial->get_UsesAlpha(&usesAlpha);
    if ( usesAlpha )
    {
        double alpha = 0;
        hr = pMaterial->get_Alpha(&alpha);
        mat->SetOpacity( (float)alpha );
    }
	else
		mat->SetOpacity( 1.0f );

    // See if it has a texture
    BOOL isTexture = FALSE;
    hr = pMaterial->get_IsTexture(&isTexture);
    if( isTexture )
    {
        CComPtr<ISkpTexture> pTexture = NULL;
        hr = pMaterial->get_Texture(&pTexture);
        if(pTexture != NULL)
        {
            BSTR texturePath = NULL;
            hr = pTexture->get_Filename(&texturePath);
			//hr = pTexture->get_Fullname(&texturePath);
            COLE2T fname(texturePath);
			SysFreeString(texturePath);
			std::wstring filename = fname;
			
			if (filename.length())
			{

				wchar_t drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
				_wsplitpath( filename.c_str(), drive, dir, fname, ext );

				if ( (drive[0] == 0) && ((dir[0] != L'\\') || (dir[1] != L'\\')) )
				{
					//Local path
					filename = L".";
					if ( dir[0] != L'\\' )
						filename += L"\\";
					filename += dir;
					filename += fname;
					filename += ext;
				}

				mat->SetDiffuseTexture(filename.c_str());
			}
    
            double width = 0;
            double height = 0;
            hr = pTexture->get_XScale(&width);
            hr = pTexture->get_YScale(&height);
			//!!!???
        }
    }
}

sgpuMaterial& SGPUExporter::
GetDefMat()
{
	if (!m_def_mat)
		m_def_mat = new DefMat( m_scene.CreateMaterial(MakeUniqueName( L"DefaultMaterial", true ).c_str()) );

	return m_def_mat->m_def_mat;
}

sgpuMaterial& SGPUExporter::
GetMaterial(long id)
{
	MatMap::iterator it = m_materials.find(id);
	if (it == m_materials.end())
		return GetDefMat();
	else
		return *it->second;
}

bool SGPUExporter::
WriteGeometry()
{
    HRESULT hr;
	bool b = true;

    CComPtr<ISkpEntityProvider> pEntProvider;
    hr = m_document->QueryInterface(IID_ISkpEntityProvider, (void**) &pEntProvider);
	
	//Count objects
	m_ocount = m_icount = m_ncount = m_vcount = m_tcount = m_ecount = 0;
	bool ret;

	m_used.clear();
	ComputeHierachy(pEntProvider, 0, ret);

	//Write scene
	try
	{
		WriteHierachy(pEntProvider, 0);
	}
	catch(int a)
	{
		//Cancelled
		if (a==1)
			m_log.WriteLog( L"Export cancelled!\n" );

		b = false;
	}
	catch(...)
	{
		b = false;
	}

	m_InheritanceManager.Clear();

	return b;
}

bool SGPUExporter::
IsVisible( IUnknown* obj )
{
	HRESULT hr;
	
	//CComQIPtr<ISkpDrawingElement> elem(obj);
	CComQIPtr<ISkpDrawingElement> elem(obj);
	if (elem)
	{
		BOOL vis;
		hr = elem->get_IsVisible( &vis );
		if (!vis)
			return false;
		CComPtr<ISkpLayer> layer;
		if (SUCCEEDED(hr = elem->get_Layer(&layer)))
		{
			if (layer)
			{
				layer->get_IsVisible( &vis );
				return vis!=0;
			}
		}
	}

	return true;
}

bool SGPUExporter::
IsVisibleN( IUnknown* obj )
{
	if (!IsVisible(obj))
		return false;

	CComQIPtr<ISkpComponentInstance> inst(obj);
	if (inst)
	{
		CComPtr<ISkpComponentDefinition> def;
		inst->get_ComponentDefinition(&def);
		return IsVisible(def);
	}

	return true;
}

void SGPUExporter::
ComputeHierachy(CComPtr<ISkpEntityProvider> pEntProvider, int level, bool& used)
{
    HRESULT hr;
    long nElements, i;
	bool ret;

	used = false;

	if (!IsVisible(pEntProvider))
		return;

    //Recurse all the instances
    CComPtr<ISkpComponentInstances> pInstances = NULL;
    hr = pEntProvider->get_ComponentInstances(&pInstances);
    hr = pInstances->get_Count(&nElements);

	for(i=0; i<nElements; i++)
    {
        CComPtr<ISkpComponentInstance> pInstance;
        hr = pInstances->get_Item(i, &pInstance);

		if (!IsVisibleN(pInstance))
			continue;

        CComPtr<ISkpComponentDefinition> pDef;
        hr = pInstance->get_ComponentDefinition(&pDef);

        CComQIPtr<ISkpEntityProvider> pEntProvider(pDef);

        //Push Transform, Material and Layer
		ComputeHierachy(pEntProvider, level+1, ret);
		used |= ret;
    }

    //Recurse all the groups
    CComPtr<ISkpGroups> pGroups = NULL;
    hr = pEntProvider->get_Groups(&pGroups);
    hr = pGroups->get_Count(&nElements);

    for(i=0; i<nElements; i++)
    {
        CComPtr<ISkpGroup> pGroup;
        hr = pGroups->get_Item(i, &pGroup);

        CComPtr<ISkpEntityProvider> pEntProvider;
        hr = pGroup->QueryInterface(IID_ISkpEntityProvider, (void**) &pEntProvider);

		ComputeHierachy(pEntProvider, level+1, ret);
		used |= ret;
    }

    //Recurse all the images
    CComPtr<ISkpImages> pImages = NULL;
    hr = pEntProvider->get_Images(&pImages);
    hr = pImages->get_Count(&nElements);

    for(i=0; i<nElements; i++)
    {
        CComPtr<ISkpImage> pImage;
        hr = pImages->get_Item(i, &pImage);

        CComPtr<ISkpEntityProvider> pEntProvider;
        hr = pImage->QueryInterface(IID_ISkpEntityProvider, (void**) &pEntProvider);
		if( pEntProvider )
		{
			ComputeHierachy(pEntProvider, level+1, ret);
			used |= ret;
		}
    }

    //Write all the faces
    CComPtr<ISkpFaces> pFaces = NULL;
    hr = pEntProvider->get_Faces(&pFaces);
    hr = pFaces->get_Count(&nElements);

	//m_ocount += nElements;
	if (nElements)
	{
		//used = true;
		for(i=0; i<nElements; i++)
		{
			CComPtr<ISkpFace> pFace;
			hr = pFaces->get_Item(i, &pFace);

			if ((hr==S_OK) && pFace)
			{
				if (IsVisible( pFace ))
				{
					m_ocount++;
					used = true;
				}
			}
		}
	}

	if (used)
		m_ncount++;

	if (level)
	{
		CComQIPtr<ISkpEntity> entity(pEntProvider);
		if (!entity)
		{
			m_used[0] = used;
		}
		else
		{
			long id;
			hr = entity->get_Id( &id );
			m_used[id] = used;
		}
	}
}

void SGPUExporter::
WriteHierachy(CComPtr<ISkpEntityProvider> pEntProvider, int level)
{
    HRESULT hr;
    long nElements, i;

	if (!IsVisible(pEntProvider))
		return;
	
	if (level)
	{
		CComQIPtr<ISkpEntity> entity(pEntProvider);
		if (!entity)
		{
			if (!m_used[0])
				return;
		}
		else
		{
			long id;
			hr = entity->get_Id( &id );
			if (!m_used[id])
				return;
		}
	}
	
    //Recurse all the instances
    CComPtr<ISkpComponentInstances> pInstances = NULL;
    hr = pEntProvider->get_ComponentInstances(&pInstances);
    hr = pInstances->get_Count(&nElements);

	for(i=0; i<nElements; i++)
    {
        CComPtr<ISkpComponentInstance> pInstance;
        hr = pInstances->get_Item(i, &pInstance);

		if (!IsVisibleN(pInstance))
			continue;

        CComPtr<ISkpComponentDefinition> pDef;
        hr = pInstance->get_ComponentDefinition(&pDef);

        CComQIPtr<ISkpEntityProvider> pEntProvider(pDef);
        
        //Push Transform, Material and Layer
		m_InheritanceManager.PushElement(pInstance);

		//Setup current instance name
		std::wstring tmpName = m_currentName;
		BSTR instanceName = NULL;
		hr = pInstance->get_Name(&instanceName);
		COLE2T iname(instanceName);
		SysFreeString(instanceName);
		m_currentName = iname;

		WriteHierachy(pEntProvider, level+1);

		m_currentName = tmpName;

		//Pop Transform, Material and Layer
		m_InheritanceManager.PopElement();
	}

    //Recurse all the groups
    CComPtr<ISkpGroups> pGroups = NULL;
    hr = pEntProvider->get_Groups(&pGroups);
    hr = pGroups->get_Count(&nElements);

    for(i=0; i<nElements; i++)
    {
        CComPtr<ISkpGroup> pGroup;
        hr = pGroups->get_Item(i, &pGroup);

        CComPtr<ISkpEntityProvider> pEntProvider;
        hr = pGroup->QueryInterface(IID_ISkpEntityProvider, (void**) &pEntProvider);

		//Push Transform, Material and Layer
		m_InheritanceManager.PushElement(pGroup);

		WriteHierachy(pEntProvider, level+1);

		//Pop Transform, Material and Layer
		m_InheritanceManager.PopElement();
	}

    //Recurse all the images
    CComPtr<ISkpImages> pImages = NULL;
    hr = pEntProvider->get_Images(&pImages);
    hr = pImages->get_Count(&nElements);

    for(i=0; i<nElements; i++)
    {
        CComPtr<ISkpImage> pImage;
        hr = pImages->get_Item(i, &pImage);

        CComPtr<ISkpEntityProvider> pEntProvider;
        hr = pImage->QueryInterface(IID_ISkpEntityProvider, (void**) &pEntProvider);

		//Push Transform, Material and Layer
		m_InheritanceManager.PushElement(pImage);

		WriteHierachy(pEntProvider, level+1);

		//Pop Transform, Material and Layer
		m_InheritanceManager.PopElement();
	}

    //Write all the faces
    CComPtr<ISkpFaces> pFaces = NULL;
    hr = pEntProvider->get_Faces(&pFaces);
    hr = pFaces->get_Count(&nElements);

	if (nElements)
	{
		try
		{
			if (!level)
				m_InheritanceManager.PushElement();

			ClearMeshes();

#ifndef MERGE_FACES
			m_meshes.reserve( 2*nElements );
#endif

			for(i=0; i<nElements; i++)
			{
				CComPtr<ISkpFace> pFace;
				hr = pFaces->get_Item(i, &pFace);

				

				if ((hr==S_OK) && pFace)
				{
					if (!IsVisible( pFace ))
						continue;

					//Push Transform, Material and Layer
					m_InheritanceManager.PushElement(pFace);
					
					//Write face
					WriteFace(pFace);
					m_progressBar->SetPercentDone(10. + 80.*++m_icount/m_ocount);

					//Cancel check
					BOOL cancelled;
					m_progressBar->HasBeenCancelled(&cancelled);
					if (cancelled)
						throw 1;
					
					//Pop Transform, Material and Layer
					m_InheritanceManager.PopElement();
				}
			}

			WriteMeshes();

			if (!level)
				m_InheritanceManager.PopElement();
		}
		catch(int a)
		{
			throw a;
		}
		catch(...)
		{
			m_log.WriteLog( L"Node '%s' export failed!\n", m_currentName.c_str() );
			throw 0;
		}
	}
}

void SGPUExporter::
WriteFace(CComPtr<ISkpFace> pFace)
{
	HRESULT hr;

	BOOL hasFrontTexture = false;
	BOOL hasBackTexture = false;
	long fMatId=-1, bMatId=-1;

	
	//Get material
	CComPtr<ISkpMaterial> pFrontMaterial = m_InheritanceManager.GetCurrentFrontMaterial();
	if (pFrontMaterial)
	{
		hr = pFrontMaterial->get_IsTexture(&hasFrontTexture);
		fMatId = GetEntityId(pFrontMaterial);
		CheckLoadMaterial(pFrontMaterial, fMatId);
	}

	CComPtr<ISkpMaterial> pBackMaterial = m_InheritanceManager.GetCurrentBackMaterial();
	if (pBackMaterial)
	{
		hr = pBackMaterial->get_IsTexture(&hasBackTexture);
		bMatId = GetEntityId(pBackMaterial);
		CheckLoadMaterial(pBackMaterial, bMatId);
	}


	BOOL hasTexture = hasFrontTexture | hasBackTexture;
	CComPtr<ISkpUVHelper> pUVHelper = NULL;

	//If the face has a texture(s) applied to it, then create a UVHelper class so we can output the uv
	//coordinates at each vertex.
	if (hasTexture)
	{
		CComPtr<ISkpCorrectPerspective> pCorrectPerspective;
		hr = m_pTextureWriter->QueryInterface(IID_ISkpCorrectPerspective, (void**)&pCorrectPerspective); 

		hr = pFace->GetUVHelper(hasFrontTexture, hasBackTexture, pCorrectPerspective, &pUVHelper);
	}

	//If this is a complex face with one or more holes in it
	//we tesselate it into triangles using the polygon mesh class, then
	//export each triangle as a face.

	CComPtr<ISkpPolygonMesh> pMesh;

	if (hasTexture)
		pFace->CreateMeshWithUVHelper(PolygonMeshPoints | PolygonMeshUVQFront | PolygonMeshUVQBack | PolygonMeshNormals, pUVHelper, &pMesh);
	else
		pFace->CreateMesh(PolygonMeshPoints | PolygonMeshNormals, NULL, &pMesh);

	if (pFrontMaterial || (!pFrontMaterial && !pBackMaterial) )
		AddMesh( fMatId, pMesh, pUVHelper, true, !m_InheritanceManager.IsDetPositive(), false );
	if (pBackMaterial)
		AddMesh( bMatId, pMesh, pUVHelper, false, !m_InheritanceManager.IsDetPositive(), false );
}

void SGPUExporter::
AddMesh( long matId, ISkpPolygonMesh* mesh, ISkpUVHelper* uvHelper, bool isFront, bool inverTri, bool invertNormals )
{
	HRESULT hr;
	long i, nPolys, nPoints;

#ifdef MERGE_FACES
	MeshPart& mp = m_meshes[matId];
#else
	m_meshes.push_back( MeshPart() );
	MeshPart& mp = m_meshes.back();
#endif

	mp.matId = matId;

	long currIndex = (long)mp.pos.size()/3;

	//Write positions, normals, UVs
	CTransform transform = m_InheritanceManager.GetCurrentTransform();
	double u = 0.0, v = 0.0, q, p[3], n[3];
	hr = mesh->get_NumPoints( &nPoints );

	mp.pos.reserve( mp.pos.size() + nPoints*3 );
	mp.norm.reserve( mp.norm.size() + nPoints*3 );
	mp.uv.reserve( mp.uv.size() + nPoints*2 );

	for ( i=1; i<=nPoints; i++ )
	{
		hr = mesh->_GetPoint( i, p );
		hr = mesh->_GetVertexNormal( i, n );

		
		if (uvHelper)
		{
			//CPoint3d point(p[0], p[1], p[2]);
			//CPoint3d worldPoint = transform * point;
			//uvHelper->GetFrontUVQ(worldPoint.X(), worldPoint.Y(), worldPoint.Z(), &u, &v, &q);//??? WORLD ???

			ISkpPoint3d * point;
			hr = isFront?mesh->get_FrontUVPoint(i, &point):mesh->get_BackUVPoint(i, &point);
			point->Get( &u, &v, &q );
			point->Release();
		}

		mp.pos.push_back((float)p[0]);
		mp.pos.push_back((float)p[1]);
		mp.pos.push_back((float)p[2]);

		if (isFront^invertNormals)
		{
			mp.norm.push_back((float)n[0]);
			mp.norm.push_back((float)n[1]);
			mp.norm.push_back((float)n[2]);
		}
		else
		{
			mp.norm.push_back((float)-n[0]);
			mp.norm.push_back((float)-n[1]);
			mp.norm.push_back((float)-n[2]);
		}

		mp.uv.push_back((float)(u));
		mp.uv.push_back((float)(1.0-v));
	}


	//Write indexes
	hr = mesh->get_NumPolygons(&nPolys);

	mp.indices.reserve( 3*nPolys + mp.indices.size() );

	for (i=1;i<=nPolys;i++)
	{
		long nP;

		//The mesh is 1 based
		hr = mesh->CountPolygonPoints(i, &nP);

		if (nP!=3)
		{
			ASSERT(nP==3);
			continue;
		}

		/*
		for (long j=1;j<=nP;j++)
		{
			//CComPtr<ISkpPoint3d> skpPoint;
			//hr = mesh->get_PolygonPoint(i, j, &skpPoint);
			long index;
			hr = mesh->GetPolygonPointIndex ( i, j, &index );
			ASSERT(index!=0);
			mp.indices.push_back( abs(index) - 1 + currIndex );
		}
		*/

		long index0, index1, index2;
		
		hr = mesh->GetPolygonPointIndex ( i, 1, &index0 );
		ASSERT(index0!=0);
		hr = mesh->GetPolygonPointIndex ( i, 2, &index1 );
		ASSERT(index1!=0);
		hr = mesh->GetPolygonPointIndex ( i, 3, &index2 );
		ASSERT(index2!=0);

		mp.indices.push_back( abs(index0) - 1 + currIndex );
		if (isFront^inverTri)
		{
			mp.indices.push_back( abs(index1) - 1 + currIndex );
			mp.indices.push_back( abs(index2) - 1 + currIndex );
		}
		else
		{
			mp.indices.push_back( abs(index2) - 1 + currIndex );
			mp.indices.push_back( abs(index1) - 1 + currIndex );
		}
	}
}

void SGPUExporter::
WriteMeshes()
{
	//Process names
	std::wstring shapeName, name;//iname;
	
	name = MakeUniqueName( m_currentName, false );
		
	if (m_currentName == L"")
		m_log.WriteLog( L"(no name) node renamed to '%s'\n", name.c_str() );
	else if (m_currentName != name)
		m_log.WriteLog( L"'%s' node renamed to '%s'\n", m_currentName.c_str(), name.c_str() );

	sgpuNode& pnode = m_InheritanceManager.Node();
	pnode.SetNodeName(name.c_str());
	
	MeshMap::iterator it;

	for ( it=m_meshes.begin(); it!=m_meshes.end(); ++it )
	{
		sgpuNode node = pnode.AddChildNode();

		name = MakeUniqueName( name + L"_Sub", false );
		node.SetNodeName(name.c_str());

		shapeName = MakeUniqueName( name + L"_Shape", false );
		sgpuMesh gpu_mesh = node.CreateTriangleMesh();
		gpu_mesh.SetMeshName(shapeName.c_str());

#ifdef MERGE_FACES
		MeshPart& mp = it->second;
#else
		MeshPart& mp = *it;
#endif
		

		//Create map for final vertices array (without unused vertics)
		int i, j, n, cnt, cntu;
		std::vector<bool> s( n = (int)mp.pos.size()/3, false );
		std::vector<int> vmap(n), vmap2(n);

		//Create set of used vertices
		for ( i=0, n=(int)mp.indices.size(); i<n; i++ )
			s[ mp.indices[i] ] = true;
		
		for (i=0, cnt = n = (int)mp.pos.size()/3; i<cnt; i++)
		{
			if (!s[i])
			{
				for ( j=cnt-1; j>i; j-- )
				{
					if (s[j])
					{
						vmap[i] = j;
						vmap2[j] = i;
						break;
					}
				}
				cnt = j;
			}
			else
			{
				vmap[i] = i;
				vmap2[i] = i;
			}
		}


		//Set vertices
		gpu_mesh.SetNumVertices( n = cnt );

		for ( i=0, j=0; i<n; i++ )
		{
			cnt = vmap[i] * 3;
			cntu = vmap[i] * 2;

			gpu_mesh.SetPosition( j, mp.pos[cnt  ], mp.pos[cnt+1], mp.pos[cnt+2] );
			gpu_mesh.SetNormal( j, mp.norm[cnt  ], mp.norm[cnt+1], mp.norm[cnt+2] );
			gpu_mesh.SetTexCoord( j, mp.uv[cntu  ], mp.uv[cntu+1] );
			
			j++;
		}


		//Set faces
		cnt = (int)mp.indices.size();
		gpu_mesh.SetNumIndices( cnt );

		for ( i=0; i<cnt; i++ )
			gpu_mesh.SetIndex( i, vmap2[mp.indices[i]] );

		gpu_mesh.AssignMaterial( GetMaterial(mp.matId) );

		m_tcount += cnt/3;
		m_vcount += (int)mp.pos.size()/3;
	}

	m_ecount += (int)m_meshes.size();

	m_meshes.clear();
}

std::wstring SGPUExporter::
GetStats()
{
	std::wstring stats;
	wchar_t s[32];

	if (m_isOk)
	{

		stats  = std::wstring( L"Nodes: " ) + _itow( m_ncount, s, 10 ) + L"\r\n";
		stats += std::wstring( L"Materials: " ) + _itow( (int)m_materials.size(), s, 10 ) + L"\r\n";
		stats += std::wstring( L"Surfaces: " ) + _itow( m_ocount, s, 10 ) + L"\r\n";
#ifdef MERGE_FACES
		stats += std::wstring( L"Meshes: " ) + _itow( m_ecount, s, 10 ) + L"\r\n";
#endif
		stats += std::wstring( L"Triangles: " ) + _itow( m_tcount, s, 10 ) + L"\r\n";
		stats += std::wstring( L"Vertices: " ) + _itow( m_vcount, s, 10 ) + L"\r\n";
	}
	else
		stats = L"Export failed!";

	return stats;
}