/*****************************************************************************
**  xtraPropertyData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Support/xtra/xtraPropertyData.hpp"


//--------------------------------------------------------------------
// Assigns output list with clones of properties in input list.
//--------------------------------------------------------------------
void xtraPropertyData::CloneProperties(const std::vector<shared_ptr<xtraPropertyData>> &i_Properties,
									   std::vector<shared_ptr<xtraPropertyData>> &o_Properties)
{
	const int num_properties = i_Properties.size();
	o_Properties.resize(num_properties);
	for (int i=0; i<num_properties; i++)
	{
		o_Properties[i].reset(i_Properties[i]->Clone());
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
xtraPropertyData::xtraPropertyData(xtraPropertyType i_Type)
: m_Type(i_Type),
  m_Name("Property Name", ""),
  m_Category("Category", ""),
  m_Description("Description", "")
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
xtraBooleanPropertyData::xtraBooleanPropertyData()
: xtraPropertyData(e_Boolean)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
xtraPropertyData* xtraBooleanPropertyData::Clone() const
{
	return new xtraBooleanPropertyData(*this);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
xtraColorPropertyData::xtraColorPropertyData()
: xtraPropertyData(e_Color),
  m_bShowAlpha("Show Alpha", true)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
xtraPropertyData* xtraColorPropertyData::Clone() const
{
	return new xtraColorPropertyData(*this);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
xtraFloatPropertyData::xtraFloatPropertyData()
: xtraPropertyData(e_Float),
  m_Minimum("Minimum", 0),
  m_Maximum("Maximum", 100),
  m_DecimalPlaces("Decimal Places", 0)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
xtraPropertyData* xtraFloatPropertyData::Clone() const
{
	return new xtraFloatPropertyData(*this);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
xtraOrientationPropertyData::xtraOrientationPropertyData()
: xtraPropertyData(e_Orientation)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
xtraPropertyData* xtraOrientationPropertyData::Clone() const
{
	return new xtraOrientationPropertyData(*this);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
xtraPositionPropertyData::xtraPositionPropertyData()
: xtraPropertyData(e_Position)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
xtraPropertyData* xtraPositionPropertyData::Clone() const
{
	return new xtraPositionPropertyData(*this);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
xtraStringPropertyData::xtraStringPropertyData()
: xtraPropertyData(e_String),
  m_bMultiline("Multiline", false)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
xtraPropertyData* xtraStringPropertyData::Clone() const
{
	return new xtraStringPropertyData(*this);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
xtraTexturePropertyData::xtraTexturePropertyData()
: xtraPropertyData(e_Texture)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
xtraPropertyData* xtraTexturePropertyData::Clone() const
{
	return new xtraTexturePropertyData(*this);
}
