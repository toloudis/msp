//
//		prop system object
//
#include "propObject.hpp"

#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"


//------------------------------------------------------------------------
//------------------------------------------------------------------------
propObject::propObject()
{
	RegisterProperties();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void propObject::RegisterProperties()
{
	prtyPropertyUIInfo* pPUII;

	pPUII = new prtyCheckBoxUIInfo(&(m_basedata.m_bEditorVisible), "category1", "Is this object visible in the editor");
	AddProperty( pPUII );
	pPUII = new prtyFloatEditUIInfo(&(m_basedata.m_FOV), "category3", "Is this object visible in the editor");
	AddProperty( pPUII );
	pPUII = new prtyFloatEditUIInfo(&(m_basedata.m_Tilt), "category3", "degrees tilt");
	AddProperty( pPUII );
	pPUII = new prtyRangedFloatUIInfo(&(m_basedata.m_Near), "category3", "Near clip");
	AddProperty( pPUII );
	pPUII = new prtyRangedFloatUIInfo(&(m_basedata.m_Far), "category3", "Far clip");
	AddProperty( pPUII );
	pPUII = new prtyVector3dEditUIInfo(&(m_basedata.m_Position), "category2", "position of the object");
	AddProperty( pPUII );
	pPUII = new prtyNumericUpDownUIInfo(&(m_basedata.m_ID), "category1", "object's ID");
	AddProperty( pPUII );
	pPUII = new prtyFloatEditUIInfo(&(m_basedata.m_Count), "category1", "object count");
	AddProperty( pPUII );
	//AddProperty( &(m_basedata.m_WorldMatrix), "category", "world matrix for the prop" );
	//AddProperty( pPUII );
	pPUII = new prtyTextBoxUIInfo(&(m_basedata.m_Description), "category1", "obj description");
	AddProperty( pPUII );
}

