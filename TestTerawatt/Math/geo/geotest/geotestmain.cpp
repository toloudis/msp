
#undef CreateFile
#undef CopyFile
#undef DeleteFile
#undef CreateDirectory
#undef DrawText

#include "dbgPackage.hpp"
#include "envPackage.hpp"
#include "maPackage.hpp"
#include "maPoint3d.hpp"
#include "geoPackage.hpp"
#include "geoPickedPoint.hpp"
#include "geoKDTree.hpp"

#include <vector>
#include <stdlib.h>

namespace
{

inline float FloatRand(float iLo, float iHi)
{
	int num = rand();
	float ret_val = float(num) / float(RAND_MAX);
	ret_val *= iHi - iLo;
	ret_val += iLo;
	return ret_val;
}

void MakeGrid(	float i_Width,
				float i_Height,
				int i_WidthSections,
				int i_HeightSections,
				const maPoint3d& i_Offset,
				std::vector<maPoint3d>& o_Vertices, 
				std::vector<unsigned short>& o_Indices)
{
	o_Vertices.resize( (i_WidthSections+1) * (i_HeightSections+1) );

	float width_delta = i_Width / float(i_WidthSections);
	float height_delta = i_Height / float(i_HeightSections);

	float cur_height = -i_Height * 0.5f;

	int width_num, height_num;
	int num_width_divisions = i_WidthSections+1;
	int num_height_divisions = i_HeightSections+1;
	int cur_vertex_index = 0;

	for( height_num = 0 ; height_num < num_height_divisions ; height_num++ )
	{
		float cur_width = -i_Width * 0.5f;
		for( width_num = 0 ; width_num < num_width_divisions ; width_num++ )
		{
			o_Vertices[cur_vertex_index].Set(cur_width, cur_width, cur_height);
			o_Vertices[cur_vertex_index] += i_Offset;
			cur_width += width_delta;
			cur_vertex_index++;

		}

		cur_height += height_delta;
	}

	o_Indices.resize(i_WidthSections * i_HeightSections * 6);

	int cur_index_index = 0;
	for( height_num = 0 ; height_num < i_HeightSections ; height_num++ )
	{
		for( width_num = 0 ; width_num < i_WidthSections ; width_num++ )
		{
			int base_vertex_num = (height_num * num_width_divisions + width_num);
			o_Indices[cur_index_index+0] = base_vertex_num;
			o_Indices[cur_index_index+1] = base_vertex_num + 1;
			o_Indices[cur_index_index+2] = base_vertex_num + num_width_divisions;
			o_Indices[cur_index_index+3] = base_vertex_num + num_width_divisions;
			o_Indices[cur_index_index+4] = base_vertex_num + 1;
			o_Indices[cur_index_index+5] = base_vertex_num + num_width_divisions + 1;
			cur_index_index += 6;
		}
	}
}

void TestKDTree()
{
	//	first we'll do a real simple test, using a kd tree with just two triangles
	//	arranged to make a square.
	//
	{
		geoKDTree simple_tree;

		maPoint3d points[4];
		points[0].Set(-10, 0, -10);
		points[1].Set(-10, 0, 10);
		points[2].Set(10, 0, 10);
		points[3].Set(10, 0, -10);

		unsigned short indices[6];
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 2;
		indices[4] = 3;
		indices[5] = 0;

		simple_tree.AddTriangles(	points,
									4,
									indices,
									6);

		simple_tree.Create();

		bool intersected;
		geoPickedPoint pick_pt;

		intersected = simple_tree.ComputeRayIntersection(maPoint3d(0, 1, 0), maPoint3d(0, -1, 0), pick_pt);
		DBG_ASSERT0(intersected == true, "Should intersect");

		intersected = simple_tree.ComputeRayIntersection(maPoint3d(-20, 1, 0), maPoint3d(-20, -1, 0), pick_pt);
		DBG_ASSERT0(intersected == false, "Shouldn't intersect");
	}

	//	now we'll arrange a group of points in a regular grid
	//
	{
		const int c_WidthSections = 10;
		const int c_HeightSections = 10;
		const float c_Width = 20;
		const float c_Height = 20;

		std::vector<maPoint3d> vertices( (c_WidthSections+1) * (c_HeightSections+1) );

		float width_delta = c_Width / float(c_WidthSections);
		float height_delta = c_Height / float(c_HeightSections);

		float cur_height = -c_Height * 0.5f;

		int width_num, height_num;
		int num_width_divisions = c_WidthSections+1;
		int num_height_divisions = c_HeightSections+1;
		int cur_vertex_index = 0;

		for( height_num = 0 ; height_num < num_height_divisions ; height_num++ )
		{
			float cur_width = -c_Width * 0.5f;
			for( width_num = 0 ; width_num < num_width_divisions ; width_num++ )
			{
				vertices[cur_vertex_index].Set(cur_width, cur_width, cur_height);
				cur_width += width_delta;
				cur_vertex_index++;

			}

			cur_height += height_delta;
		}

		std::vector<unsigned short> indices(c_WidthSections * c_HeightSections * 6);

		int cur_index_index = 0;
		for( height_num = 0 ; height_num < c_HeightSections ; height_num++ )
		{
			for( width_num = 0 ; width_num < c_WidthSections ; width_num++ )
			{
				int base_vertex_num = (height_num * num_width_divisions + width_num);
				indices[cur_index_index+0] = base_vertex_num;
				indices[cur_index_index+1] = base_vertex_num + 1;
				indices[cur_index_index+2] = base_vertex_num + num_width_divisions;
				indices[cur_index_index+3] = base_vertex_num + num_width_divisions;
				indices[cur_index_index+4] = base_vertex_num + 1;
				indices[cur_index_index+5] = base_vertex_num + num_width_divisions + 1;
				cur_index_index += 6;
			}
		}

		geoKDTree tree;
		tree.SetMinBox(5, 5, 5);
		tree.AddTriangles(&(vertices[0]), vertices.size(), &(indices[0]), indices.size());
		tree.Create();

		bool intersected;

		int i;
		geoPickedPoint pick_pt;

		for( i = 0 ; i < 500 ; i++ )
		{
			//	test a bunch of random rays
			//	all of them should hit the plane
			float range = 9.99f;
			float x = FloatRand(-range, range);
			float z = FloatRand(-range, range);
			intersected = tree.ComputeRayIntersection(maPoint3d(x, -30, z), maPoint3d(x, 30, z), pick_pt);
			DBG_ASSERT0(intersected == true, "Should intersect");
		}

		intersected = tree.ComputeRayIntersection(maPoint3d(-20, 1, 0), maPoint3d(-20, -1, 0), pick_pt);
		DBG_ASSERT0(intersected == false, "Shouldn't intersect");
	}

	//	now, 4 regular grids arranged such that there is a hole
	//	in the middle.
	//
	{
		geoKDTree tree;
		tree.SetMinBox(5, 5, 5);

		std::vector<maPoint3d> vertices;
		std::vector<unsigned short> indices;

		MakeGrid(	20.0f,
					20.0f,
					10,
					10,
					maPoint3d(-15, 0, -15),
					vertices,
					indices);		

		tree.AddTriangles(&(vertices[0]), vertices.size(), &(indices[0]), indices.size());

		MakeGrid(	20.0f,
					20.0f,
					10,
					10,
					maPoint3d(-15, 0, 15),
					vertices,
					indices);		

		tree.AddTriangles(&(vertices[0]), vertices.size(), &(indices[0]), indices.size());

		MakeGrid(	20.0f,
					20.0f,
					10,
					10,
					maPoint3d(15, 0, 15),
					vertices,
					indices);		

		tree.AddTriangles(&(vertices[0]), vertices.size(), &(indices[0]), indices.size());

		MakeGrid(	20.0f,
					20.0f,
					10,
					10,
					maPoint3d(15, 0, -15),
					vertices,
					indices);		

		tree.AddTriangles(&(vertices[0]), vertices.size(), &(indices[0]), indices.size());

		tree.Create();

		bool intersected;

		int i;
		geoPickedPoint pick_pt;

		for( i = 0 ; i < 500 ; i++ )
		{
			//	test a bunch of random rays
			//	none of them should hit the plane
			float range = 4.99f;
			float x = FloatRand(-range, range);
			float z = FloatRand(-range, range);
			intersected = tree.ComputeRayIntersection(maPoint3d(x, -30, z), maPoint3d(x, 30, z), pick_pt);
			DBG_ASSERT0(intersected == false, "Shouldn't intersect");
		}

		for( i = 0 ; i < 500 ; i++ )
		{
			//	test a bunch of random rays
			//	all of them should hit the plane
			float range1 = 5.01f;
			float range2 = 10.0f;
			float x = FloatRand(range1, range2);
			float z = FloatRand(range1, range2);
			intersected = tree.ComputeRayIntersection(maPoint3d(x, -30, z), maPoint3d(x, 30, z), pick_pt);
			DBG_ASSERT0(intersected == true, "Should intersect");
		}

		intersected = tree.ComputeRayIntersection(maPoint3d(-20, 1, 0), maPoint3d(-20, -1, 0), pick_pt);
		DBG_ASSERT0(intersected == false, "Shouldn't intersect");
	}
}

void DoTests()
{
	TestKDTree();
}

}


//====================================================================
//====================================================================
int main()
{
	envPackage::Init();
	dbgPackage::Init();
	maPackage::Init();
	geoPackage::Init();

	DoTests();

	geoPackage::CleanUp();
	maPackage::CleanUp();
	dbgPackage::CleanUp();
	envPackage::CleanUp();
	return 0;
}