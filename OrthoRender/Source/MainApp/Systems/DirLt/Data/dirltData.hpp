/********************************************************************************************\
**  dirltData.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#ifdef DIRLT_DATA_HPP
#error dirltData.hpp multiply included
#endif
#define DIRLT_DATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "prtyBoolean.hpp"
#endif
#ifndef PRTY_COLOR_HPP
#include "prtyColor.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "prtyFileName.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "prtyFloat.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "prtyName.hpp"
#endif
#ifndef PRTY_POINT3D_HPP
#include "prtyPoint3d.hpp"
#endif
#ifndef PRTY_VECTOR3D_HPP
#include "prtyVector3d.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================


//============================================================================
//============================================================================
class dirltData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	dirltData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	dirltData(const dirltData& i_Data);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~dirltData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const dirltData& i_Item);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	dirltData& operator=(const dirltData& i_Data);

public:
	prtyBoolean		m_bEditorVisible;
	prtyName		m_Name;
	prtyFileName	m_Filename;
	prtyPoint3d		m_Position;
	prtyBoolean		m_Enabled;
	prtyBoolean		m_ShadowSource;
	prtyBoolean		m_bDiffuseEnabled;
	prtyBoolean		m_bSpecularEnabled;
	prtyColor		m_Color;
	prtyVector3d	m_Direction;
};
