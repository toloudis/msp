/*****************************************************************************
**  mtrlFurData.hpp
**
**      mtrlFurData is the data for a shader effect.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_FURDATA_HPP
#error mtrlFurData.hpp multiply included
#endif
#define MTRL_FURDATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif
#ifndef PRTY_DIRECTORY_HPP
#include "Core/prty/prtyDirectory.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_VECTOR3D_HPP
#include "Core/prty/prtyVector3d.hpp"
#endif


//============================================================================
//============================================================================
class mtrlFurData //: public mtrShaderData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlFurData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlFurData(const mtrlFurData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~mtrlFurData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlFurData& operator=(const mtrlFurData& i_Data);

	prtyDirectory m_TextureFolder;
	prtyFloat m_NumShells;
	prtyFloat m_LengthScale;
	prtyVector3d m_SpreadScale;
	prtyFloat m_ShellFader;
	prtyBoolean m_bShowFins;
	prtyFloat m_FinFader;
	prtyBoolean m_bColorSourcing;
	prtyBoolean m_bFurThinning;
	prtyBoolean m_bAnisotropic;

};

