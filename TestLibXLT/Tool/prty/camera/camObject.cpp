//
//	cam system object
//
#include "camObject.hpp"

#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyColorRGBEditUIInfo.hpp"
#include "Core/prty/prtyKeyComboUIInfo.hpp"
#include "Core/prty/prtyTextBoxUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"


//------------------------------------------------------------------------
//------------------------------------------------------------------------
camObject::camObject()
{
	RegisterProperties();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void camObject::RegisterProperties()
{
	prtyPropertyUIInfo* pPUII;

	pPUII = new prtyVector3dEditUIInfo(&(m_basedata.m_Position), "category2", "position of the object");
	AddProperty( pPUII );

	prtyVector3dEditUpDownUIInfo* pV3EUDUII = new prtyVector3dEditUpDownUIInfo(&(m_basedata.m_Orientation), "category2", "orientation of the object");
	pV3EUDUII->SetIncrement(0.1f, 0.01f, 0.001f);
	AddProperty( pV3EUDUII );

	pPUII = new prtyTextBoxUIInfo(&(m_basedata.m_Description), "category1", "description of the camera");
	AddProperty( pPUII );

	AddProperty( new prtyFloatEditUIInfo(&(m_basedata.m_Angle), "", "") );

	pPUII = new prtyColorRGBEditUIInfo(&(m_basedata.m_LightColor), "category3", "light color");
	AddProperty( pPUII );

	pPUII = new prtyTextBoxUIInfo(&(m_basedata.m_CamName), "category1", "the name of the camera...duh");
	AddProperty( pPUII );

	//pPUII = new prtyKeyComboUIInfo(&(m_basedata.m_HotKey), "category4", "Hot Key");
	//AddProperty( pPUII );
}

