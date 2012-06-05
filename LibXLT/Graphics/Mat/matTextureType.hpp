/*****************************************************************************
**  matTextureType.hpp
**
**      matTextureType defines the types of textures that are allowable based on usage.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef MAT_TEXTURETYPE_HPP
#error matTextureType.hpp multiply included
#endif
#define MAT_TEXTURETYPE_HPP

enum TEXTURE_TYPE
{
	TEXTURE_TYPE_UNKNOWN,
	TEXTURE_TYPE_1D,
	TEXTURE_TYPE_2D,
	TEXTURE_TYPE_CUBE,
	TEXTURE_TYPE_3D,
};

typedef unsigned int BIND_TYPE;

enum BIND_TYPES
{
	BIND_VERTEX_BUFFER = 0x1L,
	BIND_INDEX_BUFFER = 0x2L,
	BIND_CONSTANT_BUFFER = 0x4L,
	BIND_SHADER_RESOURCE = 0x8L,
	BIND_STREAM_OUTPUT = 0x10L,
	BIND_RENDER_TARGET = 0x20L,
	BIND_DEPTH_STENCIL = 0x40L,
	BIND_UNORDERED_ACCESS = 0x80L,
	BIND_MAX_INT = 0xffffffff
};
