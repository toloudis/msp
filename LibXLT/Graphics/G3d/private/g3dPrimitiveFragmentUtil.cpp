/*****************************************************************************
**	g3dPrimitiveFragmentUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"

#include "Core/ma/maConstants.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/mat/matMaterial.hpp"


//============================================================================
//============================================================================
namespace
{
	matMaterial l_DefaultMaterial;
	const float l_DefaultSphereRadius = 1.0f;
}


//--------------------------------------------------------------------
//	CreateCube makes a fragment which is a cube with sides of the
//	given length.
//--------------------------------------------------------------------
g3dFragment* g3dPrimitiveFragmentUtil::CreateCube(float i_Side, bool i_bMorphable)
{
	float hs = i_Side * 0.5f;

	//	A cube has 8 vertices
	//	for our purposes,
	//	front -> +Z
	//	top -> +Y
	//	right -> +X
	enum
	{
		LeftBotFront = 0,	//	0 - - +
		RightBotFront,		//	1 + - +
		RightBotBack,		//	2 + - -
		LeftBotBack,		//	3 - - -
		LeftTopFront,		//	4 - + +
		RightTopFront,		//	5 + + +
		RightTopBack,		//	6 + + -
		LeftTopBack			//	7 - + -
	};

	maPoint3d template_vertices[8];
	template_vertices[0].Set(-hs, -hs, +hs);
	template_vertices[1].Set(+hs, -hs, +hs);
	template_vertices[2].Set(+hs, -hs, -hs);
	template_vertices[3].Set(-hs, -hs, -hs);
	template_vertices[4].Set(-hs, +hs, +hs);
	template_vertices[5].Set(+hs, +hs, +hs);
	template_vertices[6].Set(+hs, +hs, -hs);
	template_vertices[7].Set(-hs, +hs, -hs);

	maPoint3d cube_vertices[24];
	maPoint3d cube_normals[24];
	maPoint3d normal;
	int base = 0;

	// define each face of the cube
	//

	// front
	cube_vertices[base+0] = template_vertices[LeftBotFront];
	cube_vertices[base+1] = template_vertices[RightBotFront];
	cube_vertices[base+2] = template_vertices[RightTopFront];
	cube_vertices[base+3] = template_vertices[LeftTopFront];
	normal.Set(0, 0, 1.0f);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;
	base += 4;

	// back
	cube_vertices[base+0] = template_vertices[LeftTopBack];
	cube_vertices[base+1] = template_vertices[RightTopBack];
	cube_vertices[base+2] = template_vertices[RightBotBack];
	cube_vertices[base+3] = template_vertices[LeftBotBack];
	normal.Set(0, 0, -1.0f);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;
	base += 4;

	// left
	cube_vertices[base+0] = template_vertices[LeftBotFront];
	cube_vertices[base+1] = template_vertices[LeftTopFront];
	cube_vertices[base+2] = template_vertices[LeftTopBack];
	cube_vertices[base+3] = template_vertices[LeftBotBack];
	normal.Set(-1.0f, 0, 0);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;
	base += 4;

	// right
	cube_vertices[base+0] = template_vertices[RightBotFront];
	cube_vertices[base+1] = template_vertices[RightBotBack];
	cube_vertices[base+2] = template_vertices[RightTopBack];
	cube_vertices[base+3] = template_vertices[RightTopFront];
	normal.Set(1.0f, 0, 0);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;
	base += 4;

	// top
	cube_vertices[base+0] = template_vertices[LeftTopFront];
	cube_vertices[base+1] = template_vertices[RightTopFront];
	cube_vertices[base+2] = template_vertices[RightTopBack];
	cube_vertices[base+3] = template_vertices[LeftTopBack];
	normal.Set(0, 1.0f, 0);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;
	base += 4;

	// bottom
	cube_vertices[base+0] = template_vertices[LeftBotFront];
	cube_vertices[base+1] = template_vertices[LeftBotBack];
	cube_vertices[base+2] = template_vertices[RightBotBack];
	cube_vertices[base+3] = template_vertices[RightBotFront];
	normal.Set(0, -1.0f, 0);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;

	unsigned short indices[36];

	//--------------------------------------------------------------------
	//	IMPORTANT MICROSOFT SUCKS WARNING!!!!!!!!!!!!!!!!!!!
	//	READ THIS BEFORE MODIFYING THIS CODE!!!!!!!!!!!!!!!!
	//--------------------------------------------------------------------
	//	The loop below used to be written with the "temporary" variables
	//	cur_index_base and cur_vertex_base.  However, when global
	//	optimizations are turned on, this code fails.  I'm not sure why
	//	because I can't make a debug build with global optimiziations.
	//	If you take out the helper variables and do it the stupid
	//	way below it works fine.  Probably some of Microsoft's loop
	//	unrolling code is failing somehow.  If you fix the code
	//	below to be more efficient it will break in release build.
	//	This code doesn't have an important speed impact anyway.
	//	Just beware of MSVCPP optimizations!
	//
	int face;
	for( face = 0 ; face < 6 ; face++ )
	{
		//int cur_index_base = face * 6;
		//int cur_vertex_base = face * 4;
		indices[face*6+5] = indices[face*6] = face*4;
		indices[face*6+1] = face*4 + 1;
		indices[face*6+3] = indices[face*6+2] = face*4 + 2;
		indices[face*6+4] = face*4 + 3;
	}

	g3dFragment* ret_val = g3dFragmentCreate::CreateFragment(	cube_vertices,
													cube_normals,
													24,
													indices,
													36,
													&l_DefaultMaterial,
													i_bMorphable);
	return ret_val;
}

//--------------------------------------------------------------------
//	CreateTexturedCube makes a cube which has texture coordinates
//	assigned.
//--------------------------------------------------------------------
g3dFragment* g3dPrimitiveFragmentUtil::CreateTexturedCube(float i_Side)
{
	float hs = i_Side * 0.5f;

	//	A cube has 8 vertices
	//	for our purposes,
	//	front -> +Z
	//	top -> +Y
	//	right -> +X
	enum
	{
		LeftBotFront = 0,	//	0 - - +
		RightBotFront,		//	1 + - +
		RightBotBack,		//	2 + - -
		LeftBotBack,		//	3 - - -
		LeftTopFront,		//	4 - + +
		RightTopFront,		//	5 + + +
		RightTopBack,		//	6 + + -
		LeftTopBack			//	7 - + -
	};

	maPoint3d template_vertices[8];
	template_vertices[0].Set(-hs, -hs, +hs);
	template_vertices[1].Set(+hs, -hs, +hs);
	template_vertices[2].Set(+hs, -hs, -hs);
	template_vertices[3].Set(-hs, -hs, -hs);
	template_vertices[4].Set(-hs, +hs, +hs);
	template_vertices[5].Set(+hs, +hs, +hs);
	template_vertices[6].Set(+hs, +hs, -hs);
	template_vertices[7].Set(-hs, +hs, -hs);

	maPoint3d cube_vertices[24];
	maPoint2d texture_vertices[24];
	maPoint3d cube_normals[24];
	maPoint3d normal;
	int base = 0;

	// define each face of the cube
	//

	// front
	cube_vertices[base+0] = template_vertices[LeftBotFront];
	cube_vertices[base+1] = template_vertices[RightBotFront];
	cube_vertices[base+2] = template_vertices[RightTopFront];
	cube_vertices[base+3] = template_vertices[LeftTopFront];
	texture_vertices[base+0].Set(0, 0);
	texture_vertices[base+1].Set(1, 0);
	texture_vertices[base+2].Set(1, 1);
	texture_vertices[base+3].Set(0, 1);
	normal.Set(0, 0, 1.0f);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;
	base += 4;

	// back
	cube_vertices[base+0] = template_vertices[LeftTopBack];
	cube_vertices[base+1] = template_vertices[RightTopBack];
	cube_vertices[base+2] = template_vertices[RightBotBack];
	cube_vertices[base+3] = template_vertices[LeftBotBack];
	texture_vertices[base+0].Set(0, 0);
	texture_vertices[base+1].Set(1, 0);
	texture_vertices[base+2].Set(1, 1);
	texture_vertices[base+3].Set(0, 1);
	normal.Set(0, 0, -1.0f);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;
	base += 4;

	// left
	cube_vertices[base+0] = template_vertices[LeftBotFront];
	cube_vertices[base+1] = template_vertices[LeftTopFront];
	cube_vertices[base+2] = template_vertices[LeftTopBack];
	cube_vertices[base+3] = template_vertices[LeftBotBack];
	texture_vertices[base+0].Set(0, 0);
	texture_vertices[base+1].Set(1, 0);
	texture_vertices[base+2].Set(1, 1);
	texture_vertices[base+3].Set(0, 1);
	normal.Set(-1.0f, 0, 0);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;
	base += 4;

	// right
	cube_vertices[base+0] = template_vertices[RightBotFront];
	cube_vertices[base+1] = template_vertices[RightBotBack];
	cube_vertices[base+2] = template_vertices[RightTopBack];
	cube_vertices[base+3] = template_vertices[RightTopFront];
	texture_vertices[base+0].Set(0, 0);
	texture_vertices[base+1].Set(1, 0);
	texture_vertices[base+2].Set(1, 1);
	texture_vertices[base+3].Set(0, 1);
	normal.Set(1.0f, 0, 0);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;
	base += 4;

	// top
	cube_vertices[base+0] = template_vertices[LeftTopFront];
	cube_vertices[base+1] = template_vertices[RightTopFront];
	cube_vertices[base+2] = template_vertices[RightTopBack];
	cube_vertices[base+3] = template_vertices[LeftTopBack];
	texture_vertices[base+0].Set(0, 0);
	texture_vertices[base+1].Set(1, 0);
	texture_vertices[base+2].Set(1, 1);
	texture_vertices[base+3].Set(0, 1);
	normal.Set(0, 1.0f, 0);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;
	base += 4;

	// bottom
	cube_vertices[base+0] = template_vertices[LeftBotFront];
	cube_vertices[base+1] = template_vertices[LeftBotBack];
	cube_vertices[base+2] = template_vertices[RightBotBack];
	cube_vertices[base+3] = template_vertices[RightBotFront];
	texture_vertices[base+0].Set(0, 0);
	texture_vertices[base+1].Set(1, 0);
	texture_vertices[base+2].Set(1, 1);
	texture_vertices[base+3].Set(0, 1);
	normal.Set(0, -1.0f, 0);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;

	unsigned short indices[36];


	//--------------------------------------------------------------------
	//	IMPORTANT MICROSOFT SUCKS WARNING!!!!!!!!!!!!!!!!!!!
	//	READ THIS BEFORE MODIFYING THIS CODE!!!!!!!!!!!!!!!!
	//--------------------------------------------------------------------
	//	The loop below used to be written with the "temporary" variables
	//	cur_index_base and cur_vertex_base.  However, when global
	//	optimizations are turned on, this code fails.  I'm not sure why
	//	because I can't make a debug build with global optimiziations.
	//	If you take out the helper variables and do it the stupid
	//	way below it works fine.  Probably some of Microsoft's loop
	//	unrolling code is failing somehow.  If you fix the code
	//	below to be more efficient it will break in release build.
	//	This code doesn't have an important speed impact anyway.
	//	Just beware of MSVCPP optimizations!
	//
	int face;
	for( face = 0 ; face < 6 ; face++ )
	{
		//int cur_index_base = face * 6;
		//int cur_vertex_base = face * 4;
		indices[face*6+5] = indices[face*6] = face*4;
		indices[face*6+1] = face*4 + 1;
		indices[face*6+3] = indices[face*6+2] = face*4 + 2;
		indices[face*6+4] = face*4 + 3;
	}

	g3dFragment* ret_val = g3dFragmentCreate::CreateFragment(	cube_vertices,
													cube_normals,
													texture_vertices,
													24,
													indices,
													36,
													&l_DefaultMaterial);
	return ret_val;
}

//--------------------------------------------------------------------
//	CreateLineCube makes a cube drawn with lines for the edges.
//--------------------------------------------------------------------
g3dFragment* g3dPrimitiveFragmentUtil::CreateLineCube(float i_Side)
{
	float hs = i_Side * 0.5f;

	//	A cube has 8 vertices
	//	for our purposes,
	//	front -> +Z
	//	top -> +Y
	//	right -> +X
	enum
	{
		LeftBotFront = 0,	//	0 - - +
		RightBotFront,		//	1 + - +
		RightBotBack,		//	2 + - -
		LeftBotBack,		//	3 - - -
		LeftTopFront,		//	4 - + +
		RightTopFront,		//	5 + + +
		RightTopBack,		//	6 + + -
		LeftTopBack			//	7 - + -
	};

	maPoint3d  cube_vertices[8];
	cube_vertices[0].Set(-hs, -hs, +hs);
	cube_vertices[1].Set(+hs, -hs, +hs);
	cube_vertices[2].Set(+hs, -hs, -hs);
	cube_vertices[3].Set(-hs, -hs, -hs);
	cube_vertices[4].Set(-hs, +hs, +hs);
	cube_vertices[5].Set(+hs, +hs, +hs);
	cube_vertices[6].Set(+hs, +hs, -hs);
	cube_vertices[7].Set(-hs, +hs, -hs);

	maPoint3d cube_normals[8];
	const float len = 1.0f / sqrtf(3.0f);
	cube_normals[0].Set(-len, -len, +len);
	cube_normals[1].Set(+len, -len, +len);
	cube_normals[2].Set(+len, -len, -len);
	cube_normals[3].Set(-len, -len, -len);
	cube_normals[4].Set(-len, +len, +len);
	cube_normals[5].Set(+len, +len, +len);
	cube_normals[6].Set(+len, +len, -len);
	cube_normals[7].Set(-len, +len, -len);

	unsigned short indices[24];

	// bottom face
	indices[0] = 0;
	indices[1] = 1;

	indices[2] = 1;
	indices[3] = 2;

	indices[4] = 2;
	indices[5] = 3;

	indices[6] = 3;
	indices[7] = 0;

	// side faces
	indices[8] = 0;
	indices[9] = 4;

	indices[10] = 1;
	indices[11] = 5;

	indices[12] = 2;
	indices[13] = 6;

	indices[14] = 3;
	indices[15] = 7;

	// top face
	indices[16] = 7;
	indices[17] = 6;

	indices[18] = 6;
	indices[19] = 5;

	indices[20] = 5;
	indices[21] = 4;

	indices[22] = 4;
	indices[23] = 7;

	g3dFragment* ret_val = g3dFragmentCreate::CreateLineList(	cube_vertices,
														cube_normals,
														8,
														indices,
														24,
														&l_DefaultMaterial );

	return ret_val;
}

//--------------------------------------------------------------------
//	CreateBlock makes a fragment which is a block with sides of the
//	given lengths.
//--------------------------------------------------------------------
g3dFragment* g3dPrimitiveFragmentUtil::CreateBlock(	float i_XSide,
													float i_YSide,
													float i_ZSide)
{
	float hxs = i_XSide * 0.5f;
	float hys = i_YSide * 0.5f;
	float hzs = i_ZSide * 0.5f;

	//	A cube has 8 vertices
	//	for our purposes,
	//	front -> +Z
	//	top -> +Y
	//	right -> +X
	enum
	{
		LeftBotFront = 0,	//	0 - - +
		RightBotFront,		//	1 + - +
		RightBotBack,		//	2 + - -
		LeftBotBack,		//	3 - - -
		LeftTopFront,		//	4 - + +
		RightTopFront,		//	5 + + +
		RightTopBack,		//	6 + + -
		LeftTopBack			//	7 - + -
	};

	maPoint3d template_vertices[8];
	template_vertices[0].Set(-hxs, -hys, +hzs);
	template_vertices[1].Set(+hxs, -hys, +hzs);
	template_vertices[2].Set(+hxs, -hys, -hzs);
	template_vertices[3].Set(-hxs, -hys, -hzs);
	template_vertices[4].Set(-hxs, +hys, +hzs);
	template_vertices[5].Set(+hxs, +hys, +hzs);
	template_vertices[6].Set(+hxs, +hys, -hzs);
	template_vertices[7].Set(-hxs, +hys, -hzs);

	maPoint3d cube_vertices[24];
	maPoint3d cube_normals[24];
	maPoint3d normal;
	int base = 0;

	// define each face of the cube
	//

	// front
	cube_vertices[base+0] = template_vertices[LeftBotFront];
	cube_vertices[base+1] = template_vertices[RightBotFront];
	cube_vertices[base+2] = template_vertices[RightTopFront];
	cube_vertices[base+3] = template_vertices[LeftTopFront];
	normal.Set(0, 0, 1.0f);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;
	base += 4;

	// back
	cube_vertices[base+0] = template_vertices[LeftTopBack];
	cube_vertices[base+1] = template_vertices[RightTopBack];
	cube_vertices[base+2] = template_vertices[RightBotBack];
	cube_vertices[base+3] = template_vertices[LeftBotBack];
	normal.Set(0, 0, -1.0f);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;
	base += 4;

	// left
	cube_vertices[base+0] = template_vertices[LeftBotFront];
	cube_vertices[base+1] = template_vertices[LeftTopFront];
	cube_vertices[base+2] = template_vertices[LeftTopBack];
	cube_vertices[base+3] = template_vertices[LeftBotBack];
	normal.Set(-1.0f, 0, 0);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;
	base += 4;

	// right
	cube_vertices[base+0] = template_vertices[RightBotFront];
	cube_vertices[base+1] = template_vertices[RightBotBack];
	cube_vertices[base+2] = template_vertices[RightTopBack];
	cube_vertices[base+3] = template_vertices[RightTopFront];
	normal.Set(1.0f, 0, 0);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;
	base += 4;

	// top
	cube_vertices[base+0] = template_vertices[LeftTopFront];
	cube_vertices[base+1] = template_vertices[RightTopFront];
	cube_vertices[base+2] = template_vertices[RightTopBack];
	cube_vertices[base+3] = template_vertices[LeftTopBack];
	normal.Set(0, 1.0f, 0);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;
	base += 4;

	// bottom
	cube_vertices[base+0] = template_vertices[LeftBotFront];
	cube_vertices[base+1] = template_vertices[LeftBotBack];
	cube_vertices[base+2] = template_vertices[RightBotBack];
	cube_vertices[base+3] = template_vertices[RightBotFront];
	normal.Set(0, -1.0f, 0);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;

	unsigned short indices[36];

	//--------------------------------------------------------------------
	//	IMPORTANT MICROSOFT SUCKS WARNING!!!!!!!!!!!!!!!!!!!
	//	READ THIS BEFORE MODIFYING THIS CODE!!!!!!!!!!!!!!!!
	//--------------------------------------------------------------------
	//	The loop below used to be written with the "temporary" variables
	//	cur_index_base and cur_vertex_base.  However, when global
	//	optimizations are turned on, this code fails.  I'm not sure why
	//	because I can't make a debug build with global optimiziations.
	//	If you take out the helper variables and do it the stupid
	//	way below it works fine.  Probably some of Microsoft's loop
	//	unrolling code is failing somehow.  If you fix the code
	//	below to be more efficient it will break in release build.
	//	This code doesn't have an important speed impact anyway.
	//	Just beware of MSVCPP optimizations!
	//
	int face;
	for( face = 0 ; face < 6 ; face++ )
	{
		//int cur_index_base = face * 6;
		//int cur_vertex_base = face * 4;
		indices[face*6+5] = indices[face*6] = face*4;
		indices[face*6+1] = face*4 + 1;
		indices[face*6+3] = indices[face*6+2] = face*4 + 2;
		indices[face*6+4] = face*4 + 3;
	}

	g3dFragment* ret_val = g3dFragmentCreate::CreateFragment(	cube_vertices,
													cube_normals,
													24,
													indices,
													36,
													&l_DefaultMaterial );
	return ret_val;
}


//--------------------------------------------------------------------
//	CreateTexturedCube makes a block which has texture coordinates
//	assigned.
//--------------------------------------------------------------------
g3dFragment*  g3dPrimitiveFragmentUtil::CreateTexturedBlock(	float i_XSide,
																float i_YSide,
																float i_ZSide)
{
	float hxs = i_XSide * 0.5f;
	float hys = i_YSide * 0.5f;
	float hzs = i_ZSide * 0.5f;

	//	A cube has 8 vertices
	//	for our purposes,
	//	front -> +Z
	//	top -> +Y
	//	right -> +X
	enum
	{
		LeftBotFront = 0,	//	0 - - +
		RightBotFront,		//	1 + - +
		RightBotBack,		//	2 + - -
		LeftBotBack,		//	3 - - -
		LeftTopFront,		//	4 - + +
		RightTopFront,		//	5 + + +
		RightTopBack,		//	6 + + -
		LeftTopBack			//	7 - + -
	};

	maPoint3d template_vertices[8];
	template_vertices[0].Set(-hxs, -hys, +hzs);
	template_vertices[1].Set(+hxs, -hys, +hzs);
	template_vertices[2].Set(+hxs, -hys, -hzs);
	template_vertices[3].Set(-hxs, -hys, -hzs);
	template_vertices[4].Set(-hxs, +hys, +hzs);
	template_vertices[5].Set(+hxs, +hys, +hzs);
	template_vertices[6].Set(+hxs, +hys, -hzs);
	template_vertices[7].Set(-hxs, +hys, -hzs);

	maPoint3d cube_vertices[24];
	maPoint2d texture_vertices[24];
	maPoint3d cube_normals[24];
	maPoint3d normal;
	int base = 0;

	// define each face of the cube
	//

	// front
	cube_vertices[base+0] = template_vertices[LeftBotFront];
	cube_vertices[base+1] = template_vertices[RightBotFront];
	cube_vertices[base+2] = template_vertices[RightTopFront];
	cube_vertices[base+3] = template_vertices[LeftTopFront];
	texture_vertices[base+3].Set(0, 0);
	texture_vertices[base+2].Set(1, 0);
	texture_vertices[base+1].Set(1, 1);
	texture_vertices[base+0].Set(0, 1);
	normal.Set(0, 0, 1.0f);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;
	base += 4;

	// back
	cube_vertices[base+0] = template_vertices[LeftTopBack];
	cube_vertices[base+1] = template_vertices[RightTopBack];
	cube_vertices[base+2] = template_vertices[RightBotBack];
	cube_vertices[base+3] = template_vertices[LeftBotBack];
	texture_vertices[base+1].Set(0, 0);
	texture_vertices[base+0].Set(1, 0);
	texture_vertices[base+3].Set(1, 1);
	texture_vertices[base+2].Set(0, 1);
	normal.Set(0, 0, -1.0f);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;
	base += 4;

	// left
	cube_vertices[base+0] = template_vertices[LeftBotFront];
	cube_vertices[base+1] = template_vertices[LeftTopFront];
	cube_vertices[base+2] = template_vertices[LeftTopBack];
	cube_vertices[base+3] = template_vertices[LeftBotBack];
	texture_vertices[base+3].Set(0, 0);
	texture_vertices[base+2].Set(1, 0);
	texture_vertices[base+1].Set(1, 1);
	texture_vertices[base+0].Set(0, 1);
	normal.Set(-1.0f, 0, 0);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;
	base += 4;

	// right
	cube_vertices[base+0] = template_vertices[RightBotFront];
	cube_vertices[base+1] = template_vertices[RightBotBack];
	cube_vertices[base+2] = template_vertices[RightTopBack];
	cube_vertices[base+3] = template_vertices[RightTopFront];
	texture_vertices[base+3].Set(0, 0);
	texture_vertices[base+2].Set(1, 0);
	texture_vertices[base+1].Set(1, 1);
	texture_vertices[base+0].Set(0, 1);
	normal.Set(1.0f, 0, 0);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;
	base += 4;

	// top
	cube_vertices[base+0] = template_vertices[LeftTopFront];
	cube_vertices[base+1] = template_vertices[RightTopFront];
	cube_vertices[base+2] = template_vertices[RightTopBack];
	cube_vertices[base+3] = template_vertices[LeftTopBack];
	texture_vertices[base+3].Set(0, 0);
	texture_vertices[base+2].Set(1, 0);
	texture_vertices[base+1].Set(1, 1);
	texture_vertices[base+0].Set(0, 1);
	normal.Set(0, 1.0f, 0);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;
	base += 4;

	// bottom
	cube_vertices[base+0] = template_vertices[LeftBotFront];
	cube_vertices[base+1] = template_vertices[LeftBotBack];
	cube_vertices[base+2] = template_vertices[RightBotBack];
	cube_vertices[base+3] = template_vertices[RightBotFront];
	texture_vertices[base+3].Set(0, 1);
	texture_vertices[base+2].Set(0, 0);
	texture_vertices[base+1].Set(1, 0);
	texture_vertices[base+0].Set(1, 1);
	normal.Set(0, -1.0f, 0);
	cube_normals[base+0] = cube_normals[base+1] = cube_normals[base+2] = cube_normals[base+3] = normal;

	unsigned short indices[36];

	//--------------------------------------------------------------------
	//	IMPORTANT MICROSOFT SUCKS WARNING!!!!!!!!!!!!!!!!!!!
	//	READ THIS BEFORE MODIFYING THIS CODE!!!!!!!!!!!!!!!!
	//--------------------------------------------------------------------
	//	The loop below used to be written with the "temporary" variables
	//	cur_index_base and cur_vertex_base.  However, when global
	//	optimizations are turned on, this code fails.  I'm not sure why
	//	because I can't make a debug build with global optimiziations.
	//	If you take out the helper variables and do it the stupid
	//	way below it works fine.  Probably some of Microsoft's loop
	//	unrolling code is failing somehow.  If you fix the code
	//	below to be more efficient it will break in release build.
	//	This code doesn't have an important speed impact anyway.
	//	Just beware of MSVCPP optimizations!
	//
	int face;
	for( face = 0 ; face < 6 ; face++ )
	{
		//int cur_index_base = face * 6;
		//int cur_vertex_base = face * 4;
		indices[face*6+5] = indices[face*6] = face*4;
		indices[face*6+1] = face*4 + 1;
		indices[face*6+3] = indices[face*6+2] = face*4 + 2;
		indices[face*6+4] = face*4 + 3;
	}

	g3dFragment* ret_val = g3dFragmentCreate::CreateFragment(	cube_vertices,
													cube_normals,
													texture_vertices,
													24,
													indices,
													36,
													&l_DefaultMaterial );
	return ret_val;
}

//--------------------------------------------------------------------
//	CreateLineBlock makes a fragment which is a block with sides of the
//	given lengths, drawn with lines.
//--------------------------------------------------------------------
g3dFragment*  g3dPrimitiveFragmentUtil::CreateLineBlock(	float i_XSide,
															float i_YSide,
															float i_ZSide,
															bool i_bMorphable /*= false*/)
{
	float hxs = i_XSide * 0.5f;
	float hys = i_YSide * 0.5f;
	float hzs = i_ZSide * 0.5f;

	//	A cube has 8 vertices
	//	for our purposes,
	//	front -> +Z
	//	top -> +Y
	//	right -> +X
	enum
	{
		LeftBotFront = 0,	//	0 - - +
		RightBotFront,		//	1 + - +
		RightBotBack,		//	2 + - -
		LeftBotBack,		//	3 - - -
		LeftTopFront,		//	4 - + +
		RightTopFront,		//	5 + + +
		RightTopBack,		//	6 + + -
		LeftTopBack			//	7 - + -
	};

	maPoint3d cube_vertices[8];
	maPoint3d cube_normals[8];
	unsigned short indices[24];

	cube_vertices[0].Set(-hxs, -hys, +hzs);
	cube_vertices[1].Set(+hxs, -hys, +hzs);
	cube_vertices[2].Set(+hxs, -hys, -hzs);
	cube_vertices[3].Set(-hxs, -hys, -hzs);
	cube_vertices[4].Set(-hxs, +hys, +hzs);
	cube_vertices[5].Set(+hxs, +hys, +hzs);
	cube_vertices[6].Set(+hxs, +hys, -hzs);
	cube_vertices[7].Set(-hxs, +hys, -hzs);

	const float len = 1.0f / sqrtf(3.0f);
	cube_normals[0].Set(-len, -len, +len);
	cube_normals[1].Set(+len, -len, +len);
	cube_normals[2].Set(+len, -len, -len);
	cube_normals[3].Set(-len, -len, -len);
	cube_normals[4].Set(-len, +len, +len);
	cube_normals[5].Set(+len, +len, +len);
	cube_normals[6].Set(+len, +len, -len);
	cube_normals[7].Set(-len, +len, -len);

	// bottom face
	indices[0] = 0;
	indices[1] = 1;

	indices[2] = 1;
	indices[3] = 2;

	indices[4] = 2;
	indices[5] = 3;

	indices[6] = 3;
	indices[7] = 0;

	// side faces
	indices[8] = 0;
	indices[9] = 4;

	indices[10] = 1;
	indices[11] = 5;

	indices[12] = 2;
	indices[13] = 6;

	indices[14] = 3;
	indices[15] = 7;

	// top face
	indices[16] = 7;
	indices[17] = 6;

	indices[18] = 6;
	indices[19] = 5;

	indices[20] = 5;
	indices[21] = 4;

	indices[22] = 4;
	indices[23] = 7;

	g3dFragment* ret_val = g3dFragmentCreate::CreateLineList(	cube_vertices,
																cube_normals,
																8,
																indices,
																24,
																&l_DefaultMaterial,
																i_bMorphable);
	return ret_val;
}

//--------------------------------------------------------------------
//	CreateSphere makes a sphere-like object which has the given radius,
//	latitude divisions, and longitude divisions.
//--------------------------------------------------------------------
g3dFragment*  g3dPrimitiveFragmentUtil::CreateSphere(	float i_Radius,
														int i_LatDiv,
														int i_LongDiv)
{
	DBG_ASSERT(i_LongDiv > 2, "Sphere must have at least three longitudinal divisions");
	DBG_ASSERT(i_Radius > 0, "Sphere radius must be greater than 0");
	if (i_Radius <= 0)
		i_Radius = l_DefaultSphereRadius;
	DBG_ASSERT(i_LatDiv > 1, "Sphere must have at least two lateral divisions");
	if (i_LatDiv <= 1)
		i_LatDiv = 2;
	DBG_ASSERT(i_LongDiv > 2, "Sphere must have at least three longitudinal divisions");
	if (i_LongDiv <= 2)
		i_LongDiv = 3;

	typedef std::vector<maPoint3d> Row;

	//	make the latitude rows
	//
	const float lat_angle_delta = maConstants::c_fPI / float(i_LatDiv);
	const float long_angle_delta = maConstants::c_fPI_Times_2 / float(i_LongDiv);
	const int num_rows = i_LatDiv - 1;
	int row_num;

	std::vector<maPoint3d> row_list;
	std::vector<maPoint3d> normals;
	int vertex_row_base = 0;

	for( row_num = 0 ; row_num < num_rows ; row_num++ )
	{
		float theta = lat_angle_delta * float(row_num + 1);
		float sin_theta = sin(theta);
		float cos_theta = cos(theta);

		int num_columns = i_LongDiv;
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
			normals.push_back(cur);
		}
	}

	//	The top point's vertex number is (num_rows * i_LongDiv)
	//	The bottom point's vertex number is (num_rows * i_LongDiv) + 1
	int top_index = num_rows * i_LongDiv;
	int bot_index = top_index + 1;
	DBG_ASSERT(top_index == row_list.size(), "Miscount");
	row_list.push_back(maPoint3d(0, i_Radius, 0));
	row_list.push_back(maPoint3d(0, -i_Radius, 0));
	normals.push_back(maPoint3d(0, 1.0f, 0));
	normals.push_back(maPoint3d(0, -1.0f, 0));

	std::vector<unsigned short> indices;

	int num_strips = num_rows - 1;

	// make indices for the center strips (if necessary)
	for( row_num = 0 ; row_num < num_strips ; ++row_num )
	{
		int num_columns = i_LongDiv;
		int column_num;

		int row_index_base = row_num * i_LongDiv * 2 * 3;
		int cur_index_base = row_index_base;
		for( column_num = 0 ; column_num < num_columns ; column_num++ )
		{
			int next = (column_num+1) % num_columns;
			int v0 = row_num * num_columns + column_num;
			int v1 = (row_num+1) * num_columns + column_num;
			int v2 = (row_num+1) * num_columns + next;
			int v3 = row_num * num_columns + next;
			indices.push_back(v0);
			indices.push_back(v3);
			indices.push_back(v2);
			indices.push_back(v2);
			indices.push_back(v1);
			indices.push_back(v0);
		}
	}

	// make the top cap
	int num_columns = i_LongDiv;
	int column_num;

	for( column_num = 0 ; column_num < num_columns ; column_num++ )
	{
		indices.push_back( top_index );
		indices.push_back( (column_num+1) % num_columns);
		indices.push_back( column_num );
	}

	// make the bottom cap
	int bot_row_base = (num_rows-1) * i_LongDiv;
	for( column_num = 0 ; column_num < num_columns ; column_num++ )
	{
		indices.push_back( bot_index );
		indices.push_back( bot_row_base + column_num );
		indices.push_back( bot_row_base + ((column_num+1) % num_columns) );
	}

	g3dFragment* ret_val = g3dFragmentCreate::CreateFragment(	&(row_list[0]),
													&(normals[0]),
													num_rows * i_LongDiv + 2,
													&(indices[0]),
													num_rows * i_LongDiv * 6,
													&l_DefaultMaterial );
	return ret_val;
}

//--------------------------------------------------------------------
//	CreateTexturedSphere makes a sphere-like object which has texture
//	coordinates.
//--------------------------------------------------------------------
g3dFragment*  g3dPrimitiveFragmentUtil::CreateTexturedSphere(	float i_Radius,
																int i_LatDiv,
																int i_LongDiv)
{
	DBG_ASSERT(i_Radius > 0, "Sphere radius must be greater than 0");
	if (i_Radius <= 0)
		i_Radius = l_DefaultSphereRadius;
	DBG_ASSERT(i_LatDiv > 1, "Sphere must have at least two lateral divisions");
	if (i_LatDiv <= 1)
		i_LatDiv = 2;
	DBG_ASSERT(i_LongDiv > 2, "Sphere must have at least three longitudinal divisions");
	if (i_LongDiv <= 2)
		i_LongDiv = 3;

	typedef std::vector<maPoint3d> Row;

	//	make the latitude rows
	//
	const float lat_angle_delta = maConstants::c_fPI / float(i_LatDiv);
	const float long_angle_delta = maConstants::c_fPI_Times_2 / float(i_LongDiv);
	const int num_rows = i_LatDiv - 1;
	const int num_columns = i_LongDiv;
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
			normals.push_back(cur);
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
		for( column_num = 0 ; column_num < num_columns ; column_num++ )
		{
			int next = (column_num + 1) % num_columns;
			int v0 = row_num * num_columns + column_num;
			int v1 = (row_num+1) * num_columns + column_num;
			int v2 = (row_num+1) * num_columns + next;
			int v3 = row_num * num_columns + next;
			indices.push_back(v0);
			indices.push_back(v3);
			indices.push_back(v2);
			indices.push_back(v2);
			indices.push_back(v1);
			indices.push_back(v0);
		}
	}

	// make the top cap
	int column_num;

	for( column_num = 0 ; column_num < num_columns ; column_num++ )
	{
		indices.push_back( top_index );
		indices.push_back( (column_num + 1) % num_columns);
		indices.push_back( column_num );
	}

	// make the bottom cap
	int bot_row_base = (num_rows-1) * num_columns;
	for( column_num = 0 ; column_num < num_columns ; column_num++ )
	{
		indices.push_back( bot_index );
		indices.push_back( bot_row_base + column_num );
		indices.push_back( bot_row_base + ((column_num+1) % num_columns) );
	}

	g3dFragment* ret_val = g3dFragmentCreate::CreateFragment(	&(row_list[0]),
													&(normals[0]),
													&(texture_coords[0]),
													row_list.size(),
													&(indices[0]),
													indices.size(),
													&l_DefaultMaterial );
	return ret_val;
}

//--------------------------------------------------------------------
//	CreateReverseTexturedSphere makes a texture sphere-like object
//	which triangles and normals facing inward.
//--------------------------------------------------------------------
g3dFragment*  g3dPrimitiveFragmentUtil::CreateReverseTexturedSphere(	float i_Radius,
																		int i_LatDiv,
																		int i_LongDiv)
{
	DBG_ASSERT(i_Radius > 0, "Sphere radius must be greater than 0");
	if (i_Radius <= 0)
		i_Radius = l_DefaultSphereRadius;
	DBG_ASSERT(i_LatDiv > 1, "Sphere must have at least two lateral divisions");
	if (i_LatDiv <= 1)
		i_LatDiv = 2;
	DBG_ASSERT(i_LongDiv > 2, "Sphere must have at least three longitudinal divisions");
	if (i_LongDiv <= 2)
		i_LatDiv = 3;

	typedef std::vector<maPoint3d> Row;

	//	make the latitude rows
	//
	const float lat_angle_delta = maConstants::c_fPI / float(i_LatDiv);
	const float long_angle_delta = maConstants::c_fPI_Times_2 / float(i_LongDiv);
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
													&l_DefaultMaterial );
	return ret_val;
}


//--------------------------------------------------------------------
//	CreateCone makes a cone-like object which has the given base radius,
//	height, and divisions.
//--------------------------------------------------------------------
g3dFragment*  g3dPrimitiveFragmentUtil::CreateCone(	float i_BaseRadius,
													float i_Height,
													int i_Divisions)
{
	DBG_ASSERT(i_BaseRadius > 0, "Cone radius must be greater than 0");
	if (i_BaseRadius <= 0)
		i_BaseRadius = l_DefaultSphereRadius;
	DBG_ASSERT(i_Height > 0, "Cone height must be greater than 0");
	if (i_Height <= 0)
		i_Height = l_DefaultSphereRadius;
	DBG_ASSERT(i_Divisions > 2, "Cone must have at least three divisions");
	if (i_Divisions <= 2)
		i_Divisions = 3;

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
		float py = 0.0f;
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
		float y = 0.0f;
		float z = (float) sin(column_num * dj);
		maPoint3d cur(x, y, z);
		vertex_list.push_back(cur * i_BaseRadius);
		normals.push_back(maPoint3d(0,-1,0));
	}

	// and for the top
	for( column_num = 0 ; column_num < num_columns ; column_num++ )
	{
		float px = 0.0f;
		float py = i_Height;
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
	vertex_list.push_back(maPoint3d(0, 0, 0));
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
													&l_DefaultMaterial );

	return ret_val;
}

//--------------------------------------------------------------------
//	CreateCylinder makes a cylinder-like object which has the given radius,
//	height, and divisions.
//--------------------------------------------------------------------
g3dFragment*  g3dPrimitiveFragmentUtil::CreateCylinder( float i_Radius, float i_BottomRadius, float i_Height, int i_Divisions )
{
	DBG_ASSERT(i_Radius >= 0, "Cylinder radius must be greater than 0");
	DBG_ASSERT(i_BottomRadius >= 0, "Cylinder radius must be greater than 0");
	DBG_ASSERT((i_BottomRadius >= 0 && i_Radius > 0) || (i_Radius >= 0 && i_BottomRadius > 0),
		"Cylinder must have at least one radius greater than 0");
	if ( !((i_BottomRadius >= 0 && i_Radius > 0) || (i_Radius >= 0 && i_BottomRadius > 0)))
	{
		i_Radius = l_DefaultSphereRadius;
	}
	DBG_ASSERT(i_Height > 0, "Cylinder height must be greater than 0");
	if (i_Height <= 0)
		i_Height = l_DefaultSphereRadius;
	DBG_ASSERT(i_Divisions > 2, "Cylinder must have at least three divisions");
	if (i_Divisions <= 2)
		i_Divisions = 3;

	std::vector<maPoint3d> normals;
	std::vector<maPoint3d> vertex_list;
	std::vector<maPoint2d> texture_coords;

	float   dj =  maConstants::c_fPI_Times_2/i_Divisions;

	typedef std::vector<maPoint3d> Row;

	float dr = i_BottomRadius - i_Radius;
	float side_len = sqrtf(i_Height* i_Height + dr * dr);

	//	bottom facing downwards
	int num_columns = i_Divisions;
	int column_num;
	for( column_num = 0 ; column_num < num_columns ; column_num++ )
	{
		float px = cos((float(column_num)) * dj);
		float py = 0.0f;
		float pz = sin((float(column_num)) * dj);
		maPoint3d pos(px, py, pz);
		vertex_list.push_back(pos * i_BottomRadius);
		maPoint3d normal(0, -1, 0);
		normals.push_back(normal);

		texture_coords.push_back(maPoint2d((float)column_num/num_columns, 0.0f));
	}

	// top facing upwards
	for( column_num = 0 ; column_num < num_columns ; column_num++ )
	{
		float px = cos((float(column_num)) * dj);
		float py = 0.0f;
		float pz = sin((float(column_num)) * dj);
		maPoint3d pos(px, py, pz);
		pos *= i_Radius;
		pos.SetY( i_Height );
		vertex_list.push_back(pos);
		maPoint3d normal(0, 1, 0);
		normals.push_back(normal);

		texture_coords.push_back(maPoint2d((float)column_num/num_columns, 1.0f));

	}

	// sides from the bottom
	for( column_num = 0 ; column_num < num_columns ; column_num++ )
	{
		float x = cos((float(column_num)) * dj);
		float y = 0.0f;
		float z = sin((float(column_num)) * dj);
		maPoint3d pos(x, y, z);
		pos *= i_Radius;
		pos.SetY(i_Height);



		float nx = cos((float(column_num)) * dj) * (i_Height / side_len);
		float ny = dr / side_len;
		float nz = sin((float(column_num)) * dj) * (i_Height / side_len);



		maPoint3d normal(nx, ny, nz);
		//maPoint3d normal(x, 0, z);
		normal.Normalize();
		normals.push_back(normal);
		vertex_list.push_back(pos);

		texture_coords.push_back(maPoint2d((float)column_num/num_columns, 1.0f));
	}

	// sides from the top
	for( column_num = 0 ; column_num < num_columns ; column_num++ )
	{
		float x = cos((float(column_num)) * dj);
		float y = 0;
		float z = sin((float(column_num)) * dj);
		maPoint3d pos(x, y, z);
		pos *= i_BottomRadius;

		float nx = cos((float(column_num)) * dj) * (i_Height / side_len);
		float ny = dr / side_len;
		float nz = sin((float(column_num)) * dj) * (i_Height / side_len);



		maPoint3d normal(nx, ny, nz);
		//maPoint3d normal(x, 0, z);
		normal.Normalize();
		normals.push_back(normal);
		vertex_list.push_back(pos);

		texture_coords.push_back(maPoint2d((float)column_num/num_columns, 0.0f));
	}


	std::vector<unsigned short> indices;

	// make the bottom disc
	int bot_index = 0;
	for( column_num = 1 ; column_num < num_columns-1; ++column_num )
	{
		indices.push_back( bot_index );
		indices.push_back( column_num );
		indices.push_back( column_num+1 );
	}
	// finish it
	indices.push_back( bot_index );		// zeroeth
	indices.push_back( num_columns-1 );	// last
	indices.push_back( num_columns-2 );	// next to last

	// make the top disc
	int top_index = num_columns;
	for( column_num = num_columns+1; column_num < (2*num_columns)-1; ++column_num )
	{
		indices.push_back( top_index );
		indices.push_back( column_num+1 );
		indices.push_back( column_num );
	}
	// finish it
	indices.push_back( top_index );
	indices.push_back( (2*num_columns)-1 );
	indices.push_back( (2*num_columns)-2 );

	// make the sides in rectangles
	for (column_num = 2*num_columns; column_num < (3*num_columns)-1; ++column_num )
	{
		// sides are in two triangle sections to make the rectangle
		indices.push_back( column_num ); // bottom vertex
		indices.push_back( column_num + 1); // next bottom
		indices.push_back( column_num + num_columns  ); // top counter-part
		indices.push_back( column_num + 1); // next bottom
		indices.push_back( column_num + num_columns + 1 ); // next top
		indices.push_back( column_num + num_columns ); // the rectangle is complete
	}
	// finish it
	indices.push_back( 3*num_columns-1); // nth bottom
	indices.push_back( 2*num_columns );  // first bottom
	indices.push_back( 4*num_columns-1); // nth top
	indices.push_back( 2*num_columns );  // first bottom
	indices.push_back( 3*num_columns );	 // first top
	indices.push_back( 4*num_columns-1); // nth top


	g3dFragment* ret_val = g3dFragmentCreate::CreateFragment(	&vertex_list[0],
													&normals[0],
													&texture_coords[0],
													vertex_list.size(),
													&indices[0],
													indices.size(),
													&l_DefaultMaterial );

	return ret_val;
}

//--------------------------------------------------------------------
//	CreateRectangle creates a subdivided rectangle.  The rectangle
//	faces towards positive Z and is centered at the origin.
//--------------------------------------------------------------------
g3dFragment*  g3dPrimitiveFragmentUtil::CreateRectangle(	float i_Width,
															float i_Height,
															int i_WidthSections,
															int i_HeightSections,
															bool i_bMorphable)
{
	DBG_ASSERT(i_WidthSections > 0, "i_WidthSections must be at least 1");
	if (i_WidthSections <= 0)
		i_WidthSections = 1;
	DBG_ASSERT(i_HeightSections > 0, "i_HeightSections must be at least 1");
	if (i_HeightSections <= 0)
		i_HeightSections = 1;

	std::vector<maPoint3d> vertices( (i_WidthSections+1) * (i_HeightSections+1) );
	std::vector<maPoint3d> normals(vertices.size());

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
			vertices[cur_vertex_index].Set(cur_width, cur_height, 0);
			normals[cur_vertex_index].Set(0, 0, 1);
			cur_width += width_delta;
			cur_vertex_index++;

		}

		cur_height += height_delta;
	}

	std::vector<unsigned short> indices(i_WidthSections * i_HeightSections * 6);

	int cur_index_index = 0;
	for( height_num = 0 ; height_num < i_HeightSections ; height_num++ )
	{
		for( width_num = 0 ; width_num < i_WidthSections ; width_num++ )
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

	g3dFragment* ret_val = g3dFragmentCreate::CreateFragment(	&(vertices[0]),
													&(normals[0]),
													vertices.size(),
													&(indices[0]),
													indices.size(),
													&l_DefaultMaterial,
													i_bMorphable);

	return ret_val;
}

//--------------------------------------------------------------------
//	CreateTexturedRectangle creates a subdivided rectangle with
//	texture coordinates.  The rectangle faces towards
//	positive Z and is centered at the origin.
//--------------------------------------------------------------------
g3dFragment*  g3dPrimitiveFragmentUtil::CreateTexturedRectangle(	float i_Width,
																	float i_Height,
																	int i_WidthSections,
																	int i_HeightSections,
																	//int i_NumTextures,
																	bool i_bMorphable)
{
	DBG_ASSERT(i_WidthSections > 0, "i_WidthSections must be at least 1");
	if (i_WidthSections <= 0)
		i_WidthSections = 1;
	DBG_ASSERT(i_HeightSections > 0, "i_HeightSections must be at least 1");
	if (i_HeightSections <= 0)
		i_HeightSections = 1;

	std::vector<maPoint3d> vertices( (i_WidthSections+1) * (i_HeightSections+1) );
	std::vector<maPoint3d> normals(vertices.size());
	std::vector<maPoint2d> texture_coords(vertices.size());

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
			vertices[cur_vertex_index].Set(cur_width, cur_height, 0);
			normals[cur_vertex_index].Set(0, 0, 1);
			texture_coords[cur_vertex_index].Set(	float(width_num) / float(i_WidthSections),
													1.0f - float(height_num) / float(i_HeightSections));
			cur_width += width_delta;
			cur_vertex_index++;

		}

		cur_height += height_delta;
	}

	std::vector<unsigned short> indices(i_WidthSections * i_HeightSections * 6);

	int cur_index_index = 0;
	for( height_num = 0 ; height_num < i_HeightSections ; height_num++ )
	{
		for( width_num = 0 ; width_num < i_WidthSections ; width_num++ )
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

	g3dFragment* ret_val;
	//if( i_NumTextures == 1 )
	{
		ret_val = g3dFragmentCreate::CreateFragment(	&(vertices[0]),
											&(normals[0]),
											&(texture_coords[0]),
											vertices.size(),
											&(indices[0]),
											indices.size(),
											&l_DefaultMaterial,
											i_bMorphable);
	}
/*	else
	{
		ret_val = g3dFragmentCreate::CreateFragment(	&(vertices[0]),
											&(normals[0]),
											&(texture_coords[0]),
											&(texture_coords[0]),
											vertices.size(),
											&(indices[0]),
											indices.size(),
											&l_DefaultMaterial,
											i_bMorphable);
	}*/

	return ret_val;
}

//--------------------------------------------------------------------
//	CreateCircle makes a circle line object around the y-axis with the given
//	radius.  The arc is drawn between the start and end radians.
//--------------------------------------------------------------------
g3dFragment*  g3dPrimitiveFragmentUtil::CreateCircle(	float i_Radius,
														int i_Divisions)
{
	DBG_ASSERT(i_Radius > 0, "Circle radius must be greater than 0");
	if (i_Radius <= 0)
		i_Radius = l_DefaultSphereRadius;
	DBG_ASSERT(i_Divisions > 2, "Circle must have at least three divisions");
	if (i_Divisions <= 2)
		i_Divisions = 3;

	std::vector<maPoint3d> normals(i_Divisions);
	std::vector<maPoint3d> vertex_list(i_Divisions);

	float   dj =  maConstants::c_fPI_Times_2/i_Divisions;

	int division_num;
	for( division_num = 0 ; division_num < i_Divisions ; division_num++ )
	{
		float x = (float) cos(division_num * dj);
		float y = 0.0f;
		float z = (float) sin(division_num * dj);
		maPoint3d cur(x, y, z);
		vertex_list[division_num] = (cur * i_Radius);
		normals[division_num] = (cur);
	}

	std::vector<unsigned short> indices(i_Divisions * 2);

	for( division_num = 0 ; division_num < i_Divisions * 2 ; division_num += 2 )
	{
		indices[division_num] = division_num/2 ;
		indices[division_num+1] = (division_num/2 + 1 );
	}
	// fix the last point
	indices[(i_Divisions * 2)-1] = 0;

	g3dFragment* ret_val = g3dFragmentCreate::CreateLineList(	&vertex_list[0],
														&normals[0],
														i_Divisions,
														&indices[0],
														i_Divisions*2,
														&l_DefaultMaterial );

	return ret_val;
}

//--------------------------------------------------------------------
//	CreateIsoscelesTriangle makes an isosceles triangle (a triangle
//  with two equal sides).
//--------------------------------------------------------------------
g3dFragment*  g3dPrimitiveFragmentUtil::CreateIsoscelesTriangle(float i_fBase, float i_fHeight)
{
	// Check for valid dimensions.
	DBG_ASSERT(i_fBase > 0,   "i_fBase must be greater than 0.");
	if (i_fBase <= 0)
		i_fBase = 1.0f;
	DBG_ASSERT(i_fHeight > 0, "i_fHeight must be greater than 0.");
	if (i_fHeight <= 0)
		i_fHeight = 1.0f;

	// Allocate memory for the vertex list and normals.
	maPoint3d vertices[3];
	maPoint3d normals[3];

	// Store the calculations needed for the triangle.
	float fHalfHeight(i_fHeight/2);
	float fHalfBase(i_fBase/2);

	// Create the vertices for the triangle.
	vertices[0].Set(0.0f, fHalfHeight, 0.0f);
	vertices[1].Set(-fHalfBase, -fHalfHeight, 0.0f);
	vertices[2].Set(fHalfBase, -fHalfHeight, 0.0f);

	// Create normals for the triangle.
	normals[0].Set(0.0f, 0.0f, 1.0f);
	normals[1].Set(0.0f, 0.0f, 1.0f);
	normals[2].Set(0.0f, 0.0f, 1.0f);

	// Create indices for the triangle.
	unsigned short indices[3];
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	g3dFragment* ret_val = g3dFragmentCreate::CreateFragment(	vertices,
													normals,
													3,
													indices,
													3,
													&l_DefaultMaterial );
	return ret_val;
}

//--------------------------------------------------------------------
//	CreateEquilateralTriangle makes an equilateral triangle (a
//  triangle with three equal sides).
//--------------------------------------------------------------------
g3dFragment*  g3dPrimitiveFragmentUtil::CreateEquilateralTriangle(float i_fSide)
{
	// sqrt(3)/2
	double SquareRootOfThreeDivTwo(0.86602540378443864676372317075294);

	return CreateIsoscelesTriangle(i_fSide, i_fSide * SquareRootOfThreeDivTwo);
}

//--------------------------------------------------------------------
//	CreateLine makes a line from point 1 to point 2
//--------------------------------------------------------------------
g3dFragment*  g3dPrimitiveFragmentUtil::CreateLine( const maPoint3d& i_Point1, const maPoint3d& i_Point2 )
{
	// Vertices
	maPoint3d vertices[2];
	vertices[0] = i_Point1;
	vertices[1] = i_Point2;

	// Normals
	maPoint3d normals[2];
	normals[0].Set( 0.0f, 0.0f, 1.0f );
	normals[1].Set( 0.0f, 0.0f, 1.0f );

	// Indices
	unsigned short indices[2];
	indices[0] = 0;
	indices[1] = 1;

	g3dFragment* ret_val = g3dFragmentCreate::CreateLineList(	vertices,
														normals,
														2,
														indices,
														2,
														&l_DefaultMaterial );

	return ret_val;
}

//--------------------------------------------------------------------
//	CreateSpline makes a line list fragment from the spline
//--------------------------------------------------------------------
//g3dFragment*  g3dPrimitiveFragmentUtil::CreateSpline( const geoSpline& i_Spline )
//{
//	// Vertices
//	std::vector< maPoint3d > vertices;
//	std::vector< maVector3d > normals;
//
//	int count = i_Spline.GetCurveCount();
//	int i = 0;
//	for ( i = 0; i < count; ++i )
//	{
//		const geoSplineCurvePoint& curve_point = i_Spline.GetCurvePoint(i);
//		vertices.push_back(curve_point.m_Position);
//
//		normals.push_back( maVector3d( 0.0f, 1.0f, 0.0f ) );
//	}
//
//	// Indices
//	std::vector<unsigned short> indices;
//	for ( i = 0; i < (count - 1); ++i )
//	{
//		indices.push_back(i);
//		indices.push_back(i+1);
//	}
//
//	//	create the material - pick some color
//	//matMaterial default_material;
//	//default_material.SetDiffuse( 0.3f, 0.3f, 1.0f, 1.0f );
//	//default_material.SetAmbient( 0.3f, 0.3f, 1.0f, 1.0f );
//	//default_material.SetEmissive( 0.3f, 0.3f, 1.0f, 1.0f );
//
//	//	set up the fragment
//	g3dFragment* ret_val = g3dFragmentCreate::CreateLineList(	&vertices[0],
//													&normals[0],
//													vertices.size(),
//													&indices[0],
//													indices.size(),
//													&l_DefaultMaterial );
//
//	return ret_val;
//}

//--------------------------------------------------------------------
//	DefaultMaterial returns the default material which is used by the
//	fragments created by the g3dPrimitiveFragmentUtil.  It can be
//	modified by the client.
//--------------------------------------------------------------------
matMaterial& g3dPrimitiveFragmentUtil::DefaultMaterial()
{
	return l_DefaultMaterial;
}

