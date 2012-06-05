/*****************************************************************************
**	sbrdDriverConnectChannelTexture.hpp
**
**		Derived ConnectChannel driver class from template for Texture
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef SBRD_DRIVERCONNECTCHANNELTEXTURE_HPP
#error sbrdDriverConnectChannelTexture.hpp multiply included
#endif
#define SBRD_DRIVERCONNECTCHANNELTEXTURE_HPP

#ifndef SBRD_DRIVERCONNECTCHANNELTEMPLATE_HPP
#include "sbrdDriverConnectChannelTemplate.hpp"
#endif
#ifndef SBRD_CHANNELTEXTURE_HPP
#include "sbrdChannelTexture.hpp"
#endif


//============================================================================
//============================================================================
class sbrdDriverConnectChannelTexture : 
	public sbrdDriverConnectChannelTemplate<sbrdChannelTexture>
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	sbrdDriverConnectChannelTexture(sbrdChannelTexture &i_Channel, chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(float i_Time);
};

