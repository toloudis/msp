/*****************************************************************************
**  geoKDTreeTest.cpp
**
**      Ray intersection against geoKDTree: a single quad, a regular grid,
**      and four grids around a square hole.
**
**      Ported from TestTerawatt/Math/geo/geotest/geotestmain.cpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GEO_KDTREE_HPP
#include "Core/geo/geoKDTree.hpp"
#endif
#ifndef GEO_PICKEDPOINT_HPP
#include "Core/geo/geoPickedPoint.hpp"
#endif

#include <gtest/gtest.h>

#include <random>
#include <vector>

namespace
{

//	AddGrid adds an i_Width x i_Height grid of triangles, centered on
//	i_Offset, to o_Vertices and o_Indices. As in the original test the
//	grid lies on the diagonal plane y = x (before the offset).
//
void AddGrid(	float i_Width,
				float i_Height,
				int i_WidthSections,
				int i_HeightSections,
				const maPoint3d& i_Offset,
				std::vector<maPoint3d>& o_Vertices,
				std::vector<envType::UInt32>& o_Indices)
{
	const envType::UInt32 base = envType::UInt32(o_Vertices.size());
	const int num_width_divisions = i_WidthSections + 1;
	const float width_delta = i_Width / float(i_WidthSections);
	const float height_delta = i_Height / float(i_HeightSections);

	for( int h = 0 ; h <= i_HeightSections ; h++ )
	{
		float z = -i_Height * 0.5f + h * height_delta;
		for( int w = 0 ; w <= i_WidthSections ; w++ )
		{
			float x = -i_Width * 0.5f + w * width_delta;
			o_Vertices.push_back(maPoint3d(x, x, z) + i_Offset);
		}
	}

	for( int h = 0 ; h < i_HeightSections ; h++ )
	{
		for( int w = 0 ; w < i_WidthSections ; w++ )
		{
			envType::UInt32 v = base + h * num_width_divisions + w;
			o_Indices.push_back(v);
			o_Indices.push_back(v + 1);
			o_Indices.push_back(v + num_width_divisions);
			o_Indices.push_back(v + num_width_divisions);
			o_Indices.push_back(v + 1);
			o_Indices.push_back(v + num_width_divisions + 1);
		}
	}
}

bool HitsVertically(const geoKDTree& i_Tree, float i_X, float i_Z)
{
	geoPickedPoint pick_pt;
	return i_Tree.ComputeRayIntersection(maPoint3d(i_X, -30, i_Z), maPoint3d(i_X, 30, i_Z), pick_pt);
}

}

TEST(geoKDTree, SingleQuad)
{
	maPoint3d points[4] = { maPoint3d(-10, 0, -10), maPoint3d(-10, 0, 10), maPoint3d(10, 0, 10), maPoint3d(10, 0, -10) };
	envType::UInt32 indices[6] = { 0, 1, 2, 2, 3, 0 };

	geoKDTree tree;
	tree.AddTriangles(points, 4, indices, 6);
	tree.Create();

	geoPickedPoint pick_pt;
	EXPECT_TRUE(tree.ComputeRayIntersection(maPoint3d(0, 1, 0), maPoint3d(0, -1, 0), pick_pt));
	EXPECT_FALSE(tree.ComputeRayIntersection(maPoint3d(-20, 1, 0), maPoint3d(-20, -1, 0), pick_pt));
}

TEST(geoKDTree, RegularGrid)
{
	std::vector<maPoint3d> vertices;
	std::vector<envType::UInt32> indices;
	AddGrid(20, 20, 10, 10, maPoint3d(0, 0, 0), vertices, indices);

	geoKDTree tree;
	tree.SetMinBox(5, 5, 5);
	tree.AddTriangles(vertices.data(), int(vertices.size()), indices.data(), int(indices.size()));
	tree.Create();

	// every vertical ray over the grid hits it
	std::mt19937 rng(1);
	std::uniform_real_distribution<float> over_grid(-9.99f, 9.99f);
	for( int i = 0 ; i < 500 ; i++ )
	{
		float x = over_grid(rng);
		float z = over_grid(rng);
		EXPECT_TRUE(HitsVertically(tree, x, z)) << "x=" << x << " z=" << z;
	}

	geoPickedPoint pick_pt;
	EXPECT_FALSE(tree.ComputeRayIntersection(maPoint3d(-20, 1, 0), maPoint3d(-20, -1, 0), pick_pt));
}

TEST(geoKDTree, GridsAroundAHole)
{
	std::vector<maPoint3d> vertices;
	std::vector<envType::UInt32> indices;
	AddGrid(20, 20, 10, 10, maPoint3d(-15, 0, -15), vertices, indices);
	AddGrid(20, 20, 10, 10, maPoint3d(-15, 0,  15), vertices, indices);
	AddGrid(20, 20, 10, 10, maPoint3d( 15, 0,  15), vertices, indices);
	AddGrid(20, 20, 10, 10, maPoint3d( 15, 0, -15), vertices, indices);

	geoKDTree tree;
	tree.SetMinBox(5, 5, 5);
	tree.AddTriangles(vertices.data(), int(vertices.size()), indices.data(), int(indices.size()));
	tree.Create();

	std::mt19937 rng(2);

	// the grids leave a 10x10 hole in the middle
	std::uniform_real_distribution<float> in_hole(-4.99f, 4.99f);
	for( int i = 0 ; i < 500 ; i++ )
	{
		float x = in_hole(rng);
		float z = in_hole(rng);
		EXPECT_FALSE(HitsVertically(tree, x, z)) << "x=" << x << " z=" << z;
	}

	// and cover [5, 25] on each axis around it
	std::uniform_real_distribution<float> on_grid(5.01f, 10.0f);
	for( int i = 0 ; i < 500 ; i++ )
	{
		float x = on_grid(rng);
		float z = on_grid(rng);
		EXPECT_TRUE(HitsVertically(tree, x, z)) << "x=" << x << " z=" << z;
	}

	geoPickedPoint pick_pt;
	EXPECT_FALSE(tree.ComputeRayIntersection(maPoint3d(-20, 1, 0), maPoint3d(-20, -1, 0), pick_pt));
}
