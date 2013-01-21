/*****************************************************************************
**	prjltRangeIcon.cpp
**
**	3D Object that display range of falloff
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/PrjLt/Object/prjltRangeIcon.hpp"

#include "MainApp/mnmApp.hpp"

#include "Core/ma/maConstants.hpp"
#include "Graphics/eff/effSolidData.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/gpx/gpxSceneObject.hpp"
#include "Tool/icn/icnIconLayer.hpp"
#include "Tool/icn/icnIconSet.hpp"


namespace
{
	//--------------------------------------------------------------------
	//	create hemisphere for the falloff icon
	//--------------------------------------------------------------------
	g3dFragment*  create_hemisphere(	float i_Radius,
										int i_LatDiv,
										int i_LongDiv,
										matMaterial *i_pMaterial )
	{
		DBG_ASSERT(i_Radius > 0, "Sphere radius must be greater than 0");
		DBG_ASSERT(i_LatDiv > 1, "Sphere must have at least two lateral divisions");
		DBG_ASSERT(i_LongDiv > 2, "Sphere must have at least three longitudinal divisions");

		typedef std::vector<maPoint3d> Row;

		//	make the latitude rows
		//
		const float lat_angle_delta = maConstants::c_fPI / float(i_LatDiv);
		const float long_angle_delta = maConstants::c_fPI / float(i_LongDiv);
		const int num_rows = i_LatDiv - 1;
		const int num_columns = i_LongDiv + 1;
		int row_num;

		std::vector<maPoint3d> row_list;
		std::vector<maPoint3d> normals;
		std::vector<maPoint2d> texture_coords;
		int vertex_row_base = 0;

		for( row_num = 0 ; row_num < num_rows ; row_num++ )
		{
			float theta = lat_angle_delta * float(row_num + 1);
			float sin_theta = sin(theta);
			float cos_theta = cos(theta);

			int column_num;

			for( column_num = 0 ; column_num < num_columns ; column_num++ )
			{
				float phi = long_angle_delta * float(column_num);
				float sin_phi = sin(phi);
				float cos_phi = cos(phi);
				float x = cos_phi * sin_theta;
				float z = sin_phi * sin_theta;
				float y = cos_theta;
				maPoint3d cur(x, y, z);
				row_list.push_back(cur * i_Radius);
				normals.push_back(-cur);
				texture_coords.push_back(maPoint2d(theta / maConstants::c_fPI, phi / maConstants::c_fPI_Times_2));
			}
		}

		//	The top point's vertex number is (num_rows * num_columns)
		//	The bottom point's vertex number is (num_rows * num_columns) + 1
		int top_index = num_rows * num_columns;
		int bot_index = top_index + 1;
		DBG_ASSERT(top_index == row_list.size(), "Miscount");
		row_list.push_back(maPoint3d(0, i_Radius, 0));
		row_list.push_back(maPoint3d(0, -i_Radius, 0));
		normals.push_back(maPoint3d(0, 1.0f, 0));
		normals.push_back(maPoint3d(0, -1.0f, 0));
		texture_coords.push_back(maPoint2d(0, 0));
		texture_coords.push_back(maPoint2d(1, 0));

		std::vector<unsigned short> indices;

		int num_strips = num_rows - 1;

		// make indices for the center strips (if necessary)
		for( row_num = 0 ; row_num < num_strips ; ++row_num )
		{
			int column_num;

			int row_index_base = row_num * i_LongDiv * 2 * 3;
			int cur_index_base = row_index_base;
			for( column_num = 0 ; column_num < i_LongDiv ; column_num++ )
			{
				int next = column_num + 1;
				int v0 = row_num * num_columns + column_num;
				int v1 = (row_num+1) * num_columns + column_num;
				int v2 = (row_num+1) * num_columns + next;
				int v3 = row_num * num_columns + next;
				indices.push_back(v2);
				indices.push_back(v3);
				indices.push_back(v0);
				indices.push_back(v0);
				indices.push_back(v1);
				indices.push_back(v2);
			}
		}

		// make the top cap
		int column_num;

		for( column_num = 0 ; column_num < i_LongDiv ; column_num++ )
		{
			indices.push_back( column_num );
			indices.push_back( column_num + 1 );
			indices.push_back( top_index );
		}

		// make the bottom cap
		int bot_row_base = (num_rows-1) * num_columns;
		for( column_num = 0 ; column_num < i_LongDiv ; column_num++ )
		{
			indices.push_back( bot_row_base + column_num + 1 );
			indices.push_back( bot_row_base + column_num );
			indices.push_back( bot_index );
		}

		g3dFragment* ret_val = g3dFragmentCreate::CreateFragment(	&(row_list[0]),
														&(normals[0]),
														&(texture_coords[0]),
														row_list.size(),
														&(indices[0]),
														indices.size(),
														i_pMaterial );
		return ret_val;
	}

}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
prjltRangeIcon::prjltRangeIcon()
{
	// Material
	matMaterial *mat = new matMaterial("Solid.fx");

	effSolidData* pData = dynamic_cast<effSolidData*>(mat->GetEffectData());
	if (pData == NULL) {
		DBG_WARNING("prjltRangeIcon not using effSolid");
	}
	else {
	//	pData->m_ColorDiffuse = maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f);
	//	pData->m_ColorAmbient = maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f);
		pData->m_Color = maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f);
	//	pData->m_ColorSpecular = maFloatRGBA(0.0f, 0.0f, 0.0f, 1.0f);
	//	pData->m_Transparency = 1;
	}


	// Create fragment
	m_pFragment = create_hemisphere(1.0f, 6, 6, mat);

	// Set up fragment in object
	api3dObjectSimple *pObjectBase = new api3dObjectSimple(m_pFragment, mat);
	m_pObject = icnIconLayer::CreateIconSet(pObjectBase); // clones one icon per viewer
	m_pObject->SetGPUPickable(false);
	m_pObject->SetWireframe(true);
	//api3dScene::AddObject(m_pObject, mnmApp::GetIconsLayerIndex());

	// create thread-safe proxy for the icon
	m_pObjectProxy = new gpxSceneObject(*m_pObject);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prjltRangeIcon::~prjltRangeIcon()
{
	delete m_pObjectProxy;

	//api3dScene::RemoveObject(m_pObject, mnmApp::GetIconsLayerIndex());
	delete m_pObject;

	// fragments owned by simple objects, we don't have to delete it
}

//--------------------------------------------------------------------
// Update vertices of morphable fragment match view
//--------------------------------------------------------------------
void  prjltRangeIcon::Update(const maPoint3d &i_Pos, 
							const maVector3d &i_Dir, 
							float i_Range,
							float i_Percent)
{
	//DBG_LOG3("Update pos: %f %f %f", i_Pos.m_X, i_Pos.m_Y, i_Pos.m_Z);

	// Orient to light's direction
	maRotation rot;
	rot.SetValue(maVector3d(0,0,1), i_Dir);
	m_pObjectProxy->SetOrientation( rot );

	m_pObjectProxy->SetPosition(i_Pos);
	m_pObjectProxy->SetUniformScale(i_Range);

	m_pObjectProxy->SetColor(maFloatRGBA(i_Percent, i_Percent, i_Percent, 1.0f));
}

//----------------------------------------------------------------------------
//	Renderable sets whether the object is visible
//----------------------------------------------------------------------------
void prjltRangeIcon::SetRenderable(bool i_Renderable)
{
	m_pObjectProxy->SetRenderable(i_Renderable);
}
bool prjltRangeIcon::GetRenderable() const
{
	return m_pObjectProxy->GetRenderable();
}
