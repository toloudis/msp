/********************************************************************************************\
**  sbrdListData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#ifdef SBRD_LISTDATA_HPP
#error sbrdListData.hpp multiply included
#endif
#define SBRD_LISTDATA_HPP

#ifndef SBRD_SCRIPTDATA_HPP
#include "Systems/Storyboards/Data/sbrdScriptData.hpp"
#endif

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif
#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
#endif
#ifndef PRTY_ROTATION_HPP
#include "Core/prty/prtyRotation.hpp"
#endif


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
class sbrdListData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	sbrdListData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	sbrdListData(const itString& i_Filename);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Clear();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Insert(int i_Index, const itString& i_Filename);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Add(const itString& i_Filename);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Remove(int i_Index);

	//------------------------------------------------------------------------
	//	data
	//------------------------------------------------------------------------
	std::vector<prtyFileName>	m_Filenames;
	std::vector<sbrdScriptData> m_Items;
};

