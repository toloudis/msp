/*****************************************************************************
**	mnmReferencePosChannel.hpp
**
**	Reference implementation that attaches to the value of a position channel
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#ifdef MNM_REFERENCEPOSCHANNEL_HPP
#error mnmReferencePosChannel.hpp multiply included
#endif
#define MNM_REFERENCEPOSCHANNEL_HPP


#ifndef API3D_REFERENCE_HPP
#include "Tool/api3d/api3dReference.hpp"
#endif

class tmlnChannelPosition;

class mnmReferencePosChannel : public api3dReference
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	mnmReferencePosChannel(const tmlnChannelPosition &i_Channel);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~mnmReferencePosChannel();

	//--------------------------------------------------------------------
	// GetMatrix for this named refence
	//--------------------------------------------------------------------
	virtual maMatrix4x4 GetMatrix() const;

private:
	const tmlnChannelPosition &m_Channel;
};
