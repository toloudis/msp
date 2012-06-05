/*****************************************************************************
**	prjltIconObject.cpp
**
**	BAse class for 3D Object that holds an icon for a spot light
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/PrjLt/Object/prjltIconObject.hpp"

#include "Core/ma/maConstants.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/mat/matMaterial.hpp"

//============================================================================
//============================================================================
namespace
{
	
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
prjltIconObject::prjltIconObject()
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prjltIconObject::~prjltIconObject()
{

}

//----------------------------------------------------------------------------
// Create a cone shape with lines in order to represent cone angles
//----------------------------------------------------------------------------
g3dFragment* prjltIconObject::CreateLineCone(float i_BaseRadius, float i_Height, int i_Divisions,
											matMaterial *i_pMaterial)
{
	std::vector<maPoint3d> normals;
	std::vector<maPoint3d> vertex_list;

	float   dj =  maConstants::c_fPI_Times_2/i_Divisions;

	float side_len = sqrtf(i_Height* i_Height + i_BaseRadius * i_BaseRadius);

	int num_columns = i_Divisions;
	int column_num;
	// vertices for the bottom facing down
	for( column_num = 0 ; column_num < num_columns ; column_num++ )
	{
		float x = (float) cos(column_num * dj);
		float y = -i_Height;
		float z = (float) sin(column_num * dj);
		maPoint3d cur(x, y, z);
		vertex_list.push_back(cur * i_BaseRadius);
		normals.push_back(maPoint3d(0,-1,0));
	}

	// and for the top
	maPoint3d top_pos(0,0,0);
	maPoint3d top_normal(0,1,0);
	normals.push_back(top_normal);
	vertex_list.push_back(top_pos);

	int top_index = i_Divisions;

	std::vector<unsigned short> indices;

	// make the bottom circle
	for( column_num = 0 ; column_num < num_columns ; column_num++ )
	{
		int v1 = (column_num+1) % num_columns;
		int v2 = column_num;
		indices.push_back( v1 );
		indices.push_back( v2 );
	}

	// make the lines from tip to circle, but just 3 or 4 lines
	int skip = num_columns / 3;
	for( column_num = 0 ; column_num < num_columns ; column_num+=skip )
	{
		indices.push_back( top_index );
		indices.push_back( column_num );
	}

	g3dFragment* ret_val = g3dFragmentCreate::CreateLineList(	&vertex_list[0],
													&normals[0],
													vertex_list.size(),
													&indices[0],
													indices.size(),
													i_pMaterial );

	return ret_val;
}

//----------------------------------------------------------------------------
// Create a cone shape in order to represent cone angles
//----------------------------------------------------------------------------
g3dFragment* prjltIconObject::CreateCone(float i_BaseRadius, float i_Height, int i_Divisions,
											matMaterial *i_pMaterial)
{
	std::vector<maPoint3d> normals;
	std::vector<maPoint3d> vertex_list;

	float   dj =  maConstants::c_fPI_Times_2/i_Divisions;

	float side_len = sqrtf(i_Height* i_Height + i_BaseRadius * i_BaseRadius);

	//	bottom facing outwards
	int num_columns = i_Divisions;
	int column_num;
	for( column_num = 0 ; column_num < num_columns ; column_num++ )
	{
		float px = cos((float(column_num)) * dj);
		float py = -i_Height;
		float pz = sin((float(column_num)) * dj);
		float nx = cos((float(column_num)) * dj) * (i_Height / side_len);
		float ny = i_BaseRadius / side_len;
		float nz = sin((float(column_num)) * dj) * (i_Height / side_len);
		maPoint3d pos(px, py, pz);
		pos *= i_BaseRadius;
		maPoint3d normal(nx, ny, nz);
		normal.Normalize();
		normals.push_back(normal);
		vertex_list.push_back(pos);
	}

	// once again for the bottom facing down
	for( column_num = 0 ; column_num < num_columns ; column_num++ )
	{
		float x = (float) cos(column_num * dj);
		float y = -i_Height;
		float z = (float) sin(column_num * dj);
		maPoint3d cur(x, y, z);
		vertex_list.push_back(cur * i_BaseRadius);
		normals.push_back(maPoint3d(0,-1,0));
	}

	// and for the top
	for( column_num = 0 ; column_num < num_columns ; column_num++ )
	{
		float px = 0.0f;
		float py = 0.0f;
		float pz = 0.0f;
		float nx = cos((float(column_num) + 0.5f) * dj) * (i_Height / side_len);
		float ny = i_BaseRadius / side_len;
		float nz = sin((float(column_num) + 0.5f) * dj) * (i_Height / side_len);
		maPoint3d pos(px, py, pz);
		maPoint3d normal(nx, ny, nz);
		normals.push_back(normal);
		vertex_list.push_back(pos);
	}

	//	bottom point
	vertex_list.push_back(maPoint3d(0, -i_Height, 0));
	normals.push_back(maPoint3d(0, -1.0f, 0));

	int bot_index = i_Divisions * 3;

	std::vector<unsigned short> indices;

	// make the top cap
	for( column_num = 0 ; column_num < num_columns ; column_num++ )
	{
		int top_index = 2 * num_columns + column_num;
		int v0 = top_index;
		int v1 = (column_num+1) % num_columns;
		int v2 = column_num;
		indices.push_back( v0 );
		indices.push_back( v1 );
		indices.push_back( v2 );
	}

	// make the bottom disc
	for( column_num = num_columns ; column_num < (num_columns * 2) ; column_num++ )
	{
		indices.push_back( bot_index );
		indices.push_back( column_num );

		int final_index = column_num + 1;
		if( final_index > (num_columns * 2) - 1 )
			final_index = num_columns;

		indices.push_back( final_index );
	}

	g3dFragment* ret_val = g3dFragmentCreate::CreateFragment(	&vertex_list[0],
													&normals[0],
													vertex_list.size(),
													&indices[0],
													indices.size(),
													i_pMaterial );

	return ret_val;
}