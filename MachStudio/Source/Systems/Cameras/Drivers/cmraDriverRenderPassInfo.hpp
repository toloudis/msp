/********************************************************************************************\
**  cmraDriverRenderPassInfo.hpp
**
**	Data structure for parsing RenderPass drivers
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#ifdef CMRA_DRIVERRENDERPASSINFO_HPP
#error cmraDriverRenderPassInfo.hpp multiply included
#endif
#define CMRA_DRIVERRENDERPASSINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef PRTY_TEXTUREFILENAME_HPP
#include "Core/prty/prtyTextureFileName.hpp"
#endif

//============================================================================
//============================================================================

//============================================================================
//============================================================================
class cmraDriverRenderPassInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverRenderPassInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmraDriverRenderPassInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	prtyTextureFileName		m_FirstTextureFileName;
	bool					m_bEnable;
	int						m_NumberOfFrames;
	int						m_BlendOp;
	float					m_BlendIntensity;
	int						m_RenderPass;
};

