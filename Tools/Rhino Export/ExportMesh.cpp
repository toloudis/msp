#include "stdafx.h"
#include "ExportMesh.h"

namespace
{

float ConvertColor(int intColor)
{
	return (float)intColor/255.f;
}

inline ON_wString
Trim( const ON_wString& s0 )
{
	int i;
	for ( i=s0.Length()-1; i>=0; --i )
	{
		if (!iswdigit(s0[i]))
			break;
	}

	return ON_wString( s0.operator const wchar_t*(), i+1 );
}

inline bool
EqNum( const ON_wString& s0, const ON_wString& s1 )
{
	return Trim( s0 ) == Trim( s1 );
}

inline bool
Eq( double v0, double v1 )
{
	const double eps = 1e-20;

	return fabs( v0-v1 )<eps;
}

inline bool
EqT( const ON_Texture& t0, const ON_Texture& t1 )
{
	return (t0.m_type == t1.m_type) && (t0.m_bOn == t1.m_bOn) && (!t0.m_uvw.Compare(t1.m_uvw)) && (t0.m_filename == t1.m_filename);
}

};

bool MatItem::
Compare( CRhinoMaterialTable& table, int n )
{
	//Properties
	if (!EqNum( table[num].m_material_name, table[n].m_material_name ))
		return false;
	if (table[num].m_ambient != table[n].m_ambient)
		return false;
	if (table[num].m_diffuse != table[n].m_diffuse)
		return false;
	if (table[num].m_emission != table[n].m_emission)
		return false;
	if (table[num].m_specular != table[n].m_specular)
		return false;
	if (table[num].m_reflection != table[n].m_reflection)
		return false;
	if (table[num].m_transparent != table[n].m_transparent)
		return false;

	if (!Eq( table[num].m_reflectivity, table[n].m_reflectivity ))
		return false;
	if (!Eq( table[num].m_shine, table[n].m_shine ))
		return false;
	if (!Eq( table[num].m_transparency, table[n].m_transparency ))
		return false;

	//Textures
	if (table[num].m_textures.Count()!=table[n].m_textures.Count())
		return false;

	int i, j, k = table[num].m_textures.Count();
	if (!k)
		return true;

	std::vector<bool> check(n);
	bool b = true;
	
	for ( i=0; b && (i<k); i++ )
	{
		b = false;		
		for ( j=0; j<k; j++ )
		{
			if (check[j])
				continue;

			if ( !table[num].m_textures[i].Compare( table[n].m_textures[j] ) ) //EqT( table[num].m_textures[i], table[n].m_textures[j] ) )
			{
				b = true;
				break;
			}
		}
	}

	return b;
}

int ExportMesh::
CheckMaterial( CRhinoMaterialTable& table, int num )
{
	ON_wString name = Trim( table[num].m_material_name );
	RhinoMatMap::iterator it = m_mreplaces.find( name );

	for ( ; it != m_mreplaces.end() && (it->first == name); ++it )
	{
		if (it->second.Compare( table, num ))
			return it->second.num;
	}

	MatItem item;
	item.num = num;
	m_mreplaces.insert( std::pair<ON_wString, MatItem>(name, item) );
	return num;
}

bool ExportMesh::
AddMesh(CRhinoDoc& doc, const CRhinoObjectMesh& mesh, Log& log)
{
	try
	{
		ON_Mesh * m = mesh.m_mesh;
		if (!m)
			return false;

		if (!m->m_V.Count() || !m->m_F.Count())
			return true; //No vertices in mesh

		ON_wString name, nodeName = mesh.m_mesh_attributes.m_name;
		if (!nodeName.Compare(""))
			name = TEXT("Unnamed");
		else
			name = nodeName;

		std::wstring nm = MakeUniqueName( std::wstring(name), false );
		std::wstring shapeName = std::wstring(nm) + TEXT("_Shape");
		std::wstring nmm = MakeUniqueName( shapeName, false );
		
		if (!nodeName.Compare(""))
			log.WriteLog( L"(no name) node renamed to '%s'\n", nm.c_str() );
		else if (nm.compare(nodeName))
			log.WriteLog( L"'%s' node renamed to '%s'\n", nodeName.operator const wchar_t*(), nm.c_str() );

		if (nmm.compare(shapeName))
			log.WriteLog( L"'%s' shape renamed to '%s'\n", shapeName.c_str(), nmm.c_str() );


		sgpuNode root_node = scene.GetRootNode();

		sgpuNode node = root_node.AddChildNode();
		node.SetNodeName(nm.c_str());
		sgpuMesh gpu_mesh = node.CreateTriangleMesh();
		gpu_mesh.SetMeshName(nmm.c_str());

		sgpuMatrix trans;
		trans.MakeTranslate(0, 0, 0);
		node.SetTransformationMatrix(trans);

		int i, n, cnt, j;

		//------------------------------------
		//Create map for final vertices array (without unused vertics)
		std::vector<bool> s( n = m->m_V.Count(), false );
		std::vector<int> vmap(n), vmap2(n);

		//Create set of used vertices
		for ( i=0, n=m->m_F.Count(); i<n; i++ )
		{
			s[ m->m_F[i].vi[0] ] = true;
			s[ m->m_F[i].vi[1] ] = true;
			s[ m->m_F[i].vi[2] ] = true;
			s[ m->m_F[i].vi[3] ] = true;
		}
		
		for (i=0, cnt = n = (int)m->m_V.Count(); i<cnt; i++)
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

		//------------------------------------
		//Material

		//const CRhinoMaterial& cmat  = mesh.m_brep_object->ObjectMaterial();
		int nmat = mesh.m_mesh_attributes.m_material_index;
		bool createDefMaterial = true;

		ON::object_material_source src = mesh.m_mesh_attributes.MaterialSource();

		if (src != ON::material_from_object)
		{
			int nlayer = mesh.m_mesh_attributes.m_layer_index;
			const CRhinoLayer& layer = doc.m_layer_table[nlayer];
			nmat = layer.m_material_index;
		}

		ON_Xform uvt(1.0f);

		if (nmat>=0)
		{
			nmat = CheckMaterial( doc.m_material_table, nmat );

			MatMap::iterator it = m_materials.find( nmat );
			if (it!=m_materials.end())
			{
				gpu_mesh.AssignMaterial( *it->second );
			}
			else
			{
				ON_Material mat = doc.m_material_table[nmat];//cmat;//
				
				ON_wString name, matName = mat.m_material_name;
				if (!matName.Compare(""))
					name = TEXT("Unnamed");
				else
					name = matName;

				std::wstring nm = MakeUniqueName( std::wstring(name), true );
				
				if (!matName.Compare(""))
					log.WriteLog( L"(no name) material renamed to '%s'\n", nm.c_str() );
				else if (nm.compare(matName))
					log.WriteLog( L"'%s' material renamed to '%s'\n", matName.operator const wchar_t*(), nm.c_str() );
				
				sgpuMaterial gpu_mat = scene.CreateMaterial(nm.c_str());

				gpu_mat.SetDiffuseColor( ConvertColor(mat.m_diffuse.Red()), ConvertColor(mat.m_diffuse.Green()), ConvertColor(mat.m_diffuse.Blue()) );
				gpu_mat.SetOpacity( 1.f - (float)mat.m_transparency );
				gpu_mat.SetSpecularColor( ConvertColor(mat.m_specular.Red()), ConvertColor(mat.m_specular.Green()), ConvertColor(mat.m_specular.Blue()) );
				gpu_mat.SetShininess((float)(100.*mat.m_shine/255.));//???

				for ( i=0, n=mat.m_textures.Count(); i<n; i++ )
					if ( (mat.m_textures[i].m_type == ON_Texture::bitmap_texture) && mat.m_textures[i].m_bOn )
					{
						ON_wString filename = mat.m_textures[i].m_filename;
						if (filename.Compare(""))
						{
							wchar_t drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
							_wsplitpath( filename, drive, dir, fname, ext );

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
							
							gpu_mat.SetDiffuseTexture(filename);

							/*
							if ( mat.m_textures[i].m_uvw.Compare( ON_Xform(1.0f) ) )
							{
								log.WriteLog( L"Texture '%s' in material '%s' has tiling (ignored)\n", filename.operator const wchar_t*(), name.operator const wchar_t*() );
							}
							*/

							uvt = mat.m_textures[i].m_uvw;
						}
						break;
					}

				gpu_mesh.AssignMaterial( gpu_mat );
				m_materials[nmat] = AutoPtr<sgpuMaterial>( new sgpuMaterial( gpu_mat ) );
			}

			createDefMaterial = false;
		}
		
		if (createDefMaterial)
		{
			//Assign default material
			AssignDefMat( gpu_mesh );

			log.WriteLog( L"Default material is assigned to '%s' node\n", nm.c_str() );
		}



		//------------------------------------
		//Set real mesh data
		gpu_mesh.SetNumVertices( n = cnt );

		for ( i=0, j=0; i<n; i++ )
		{
			cnt = vmap[i];

			gpu_mesh.SetPosition( j, m->m_V[cnt].x, m->m_V[cnt].y, m->m_V[cnt].z );

			if ( n == m->m_T.Count() )
			{
				ON_2dPoint p = uvt * ON_2dPoint( m->m_T[cnt] );
				gpu_mesh.SetTexCoord( j, (float)p.x, - (float)p.y );

				//gpu_mesh.SetTexCoord( j, m->m_T[cnt].x, 1.f - m->m_T[cnt].y );
			}

			if ( n == m->m_N.Count() )
				gpu_mesh.SetNormal( j, m->m_N[cnt].x, m->m_N[cnt].y, m->m_N[cnt].z );

			j++;
		}

		

		

		//Assign positions, UVs, normals
		/*
		gpu_mesh.SetNumVertices( n = m->m_V.Count() );

		for ( i=0; i<n; i++ )
			gpu_mesh.SetPosition( i, m->m_V[i].x, m->m_V[i].y, m->m_V[i].z );

		if ( n == m->m_T.Count() )
			for ( i=0; i<n; i++ )
				gpu_mesh.SetTexCoord( i, m->m_T[i].x, m->m_T[i].y );

		if ( n == m->m_N.Count() )
			for ( i=0; i<n; i++ )
				gpu_mesh.SetNormal( i, m->m_N[i].x, m->m_N[i].y, m->m_N[i].z );
		*/


		//Compute real face count (face can be triangle or quad)
		for ( i=0, cnt = 0, n = m->m_F.Count(); i<n; i++ ) // - 7041
			cnt += ( m->m_F[i].vi[2] == m->m_F[i].vi[3] )? 3 : 6;

		//Set faces
		gpu_mesh.SetNumIndices( cnt );

		for ( i=0, j=0; i<n; i++ )
		{
			gpu_mesh.SetIndex( j++, vmap2[m->m_F[i].vi[0]] );
			gpu_mesh.SetIndex( j++, vmap2[m->m_F[i].vi[1]] );
			gpu_mesh.SetIndex( j++, vmap2[m->m_F[i].vi[2]] );

			if ( m->m_F[i].vi[2] != m->m_F[i].vi[3] )
			{
				gpu_mesh.SetIndex( j++, vmap2[m->m_F[i].vi[0]] );
				gpu_mesh.SetIndex( j++, vmap2[m->m_F[i].vi[2]] );
				gpu_mesh.SetIndex( j++, vmap2[m->m_F[i].vi[3]] );
			}
		}
		
		
		/*
		for ( i=0, j=0; i<n; i++ )
		{
			gpu_mesh.SetIndex( j++, m->m_F[i].vi[0] );
			gpu_mesh.SetIndex( j++, m->m_F[i].vi[1] );
			gpu_mesh.SetIndex( j++, m->m_F[i].vi[2] );

			if ( m->m_F[i].vi[2] != m->m_F[i].vi[3] )
			{
				gpu_mesh.SetIndex( j++, m->m_F[i].vi[0] );
				gpu_mesh.SetIndex( j++, m->m_F[i].vi[2] );
				gpu_mesh.SetIndex( j++, m->m_F[i].vi[3] );
			}
		}
		*/

		
		
		
		
	}
	catch (...)
	{
		return false;
	}

	return true;
}

ExportMesh::
ExportMesh():scene(), m_def_mat(0)
{
}

ExportMesh::
~ExportMesh()
{
	delete m_def_mat;
}

void ExportMesh::
AssignDefMat( sgpuMesh& mesh )
{
	if (!m_def_mat)
		m_def_mat = new DefMat( scene.CreateMaterial(MakeUniqueName( L"DefaultMaterial", true ).c_str()) );

	mesh.AssignMaterial( m_def_mat->m_def_mat );
}

bool ExportMesh::
StartExport(const wchar_t* filename)
{
	m_filename = filename;
	sgpuNode root_node = scene.GetRootNode();
	root_node.SetNodeName(MakeUniqueName(L"RootNode", false).c_str());
	sgpuMatrix matrix;
	matrix.MakeRotate( (float)(-3.14159265*0.5), 1.f, 0.f, 0.f );
	//sgpuMatrix matrix1;
	//matrix1.MakeRotate( (float)(3.14159265), 0.f, 1.f, 0.f );
	//matrix = matrix*matrix1;

	root_node.SetTransformationMatrix(matrix);

	return true;
}

bool ExportMesh::
FinishExport()
{
	try
	{
		return scene.WriteScene( m_filename, TEXT("Rhino export plug-in. v 1.0") );
	}
	catch (...)
	{
		return false;
	}
}

std::wstring ExportMesh::
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
		name = nm + _i64tow( ++n, buff, 10 );

	list.insert( name );
	return name;
}