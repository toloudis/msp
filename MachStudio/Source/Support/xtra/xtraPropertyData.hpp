/********************************************************************************************\
**  xtraPropertyData.hpp
**
**	Data class for information about a custom property object.
**
**  StudioGPU
**  Copyright(C) 2009 - All Rights Reserved
\********************************************************************************************/

#ifdef XTRA_PROPERTYDATA_HPP
#error xtraPropertyData.hpp multiply included
#endif
#define XTRA_PROPERTYDATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif 
#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif 
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif 
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif 
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif 
#ifndef PRTY_ROTATION_HPP
#include "Core/prty/prtyRotation.hpp"
#endif 
#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
#endif 
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif 

//------------------------------------------------------------------------
//------------------------------------------------------------------------
enum xtraPropertyType
{
	// The file format is based on these numbers, don't change them - only add to the enumeration.
	e_Unknown = 0,
	e_Boolean = 1,
	e_Float = 2,
	e_Color = 3,
	e_String = 4,
	e_Position = 5,
	e_Orientation = 6,
	e_Texture = 7
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
class xtraPropertyData
{
public:
	xtraPropertyData(xtraPropertyType i_Type);
	virtual xtraPropertyData* Clone() const = 0;
	virtual ~xtraPropertyData() {}

	xtraPropertyType m_Type;

	prtyText m_Name;
	prtyText m_Category;
	prtyText m_Description;

	//--------------------------------------------------------------------
	// Assigns output list with clones of properties in input list.
	//--------------------------------------------------------------------
	static void CloneProperties(const std::vector<shared_ptr<xtraPropertyData>> &i_Properties,
								std::vector<shared_ptr<xtraPropertyData>> &o_Properties);
};


//------------------------------------------------------------------------
//------------------------------------------------------------------------
class xtraBooleanPropertyData : public xtraPropertyData
{
public:
	xtraBooleanPropertyData();
	virtual xtraPropertyData* Clone() const;

	prtyBoolean m_Value;
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
class xtraColorPropertyData : public xtraPropertyData
{
public:
	xtraColorPropertyData();
	virtual xtraPropertyData* Clone() const;

	prtyColor m_Value;
	prtyBoolean m_bShowAlpha;
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
class xtraFloatPropertyData : public xtraPropertyData
{
public:
	xtraFloatPropertyData();
	virtual xtraPropertyData* Clone() const;

	prtyFloat m_Value;
	prtyFloat m_Minimum;
	prtyFloat m_Maximum;
	prtyInt32 m_DecimalPlaces;
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
class xtraOrientationPropertyData : public xtraPropertyData
{
public:
	xtraOrientationPropertyData();
	virtual xtraPropertyData* Clone() const;

	prtyRotation m_Value;
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
class xtraPositionPropertyData : public xtraPropertyData
{
public:
	xtraPositionPropertyData();
	virtual xtraPropertyData* Clone() const;

	prtyPoint3d m_Value;
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
class xtraStringPropertyData : public xtraPropertyData
{
public:
	xtraStringPropertyData();
	virtual xtraPropertyData* Clone() const;

	prtyText m_Value;
	prtyBoolean m_bMultiline;
};

//------------------------------------------------------------------------
//------------------------------------------------------------------------
class xtraTexturePropertyData : public xtraPropertyData
{
public:
	xtraTexturePropertyData();
	virtual xtraPropertyData* Clone() const;

	prtyFilePath m_Value;
};
