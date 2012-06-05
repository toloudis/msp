/****************************************************************************\
**  g2dScanlineOpenEXRUtil.hpp
**
**  OpenEXR file format support
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_EXRDEFINES_HPP
#error g2dOpenEXRDefines.hpp multiply included
#endif
#define CPTR_EXRDEFINES_HPP


//============================================================================
//============================================================================
#define R_OFFSET_RGBAxxf	0
#define G_OFFSET_RGBAxxf	1
#define B_OFFSET_RGBAxxf	2
#define A_OFFSET_RGBAxxf	3

#define R_OFFSET_E_COLOR	0
#define G_OFFSET_E_COLOR	1
#define B_OFFSET_E_COLOR	2
#define A_OFFSET_E_COLOR	3

#define TILE_SIZE_X			50
#define TILE_SIZE_Y			50

#define BYTES_PER_PIX_2		2
#define BYTES_PER_PIX_4		4
#define BYTES_PER_PIX_8		8
#define BYTES_PER_PIX_16	16


//============================================================================
//============================================================================
typedef struct 
{
	float r;
	float g;
	float b;
	float a;
} floatRgba;
