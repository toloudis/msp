/****************************************************************************\
**  sgpuBaseSceneImpl.hpp
**
**      sgpuBaseSceneImpl.hpp defines class for a scene that can be exported
**	to a StudioGPU static geometry file (.gxb)
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_BASESCENEIMPL_HPP
#define SGPU_BASESCENEIMPL_HPP
#include "sgpuExportLib.hpp"
#include "sgpuString.hpp"
#include "sgpuFileWriterLifeTimeKeeper.hpp"
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

struct sgpuBaseSceneImpl
{
	sgpuBaseSceneImpl()
	{}

	~sgpuBaseSceneImpl()
	{}
	
	void RegisterWriter ( shared_ptr< sgpuFileWriterLifeTimeKeeper > &i_Writer )
	{
		m_Writer = i_Writer;
	}

	void UnRegisterWriter()
	{
		m_Writer.reset( );
	}
	
	chBinWriter &GetChBinWriter()
	{
		DBG_ASSERT( ( m_Writer  && m_Writer->m_pWriter ), "Writer should be registered" );
		return *m_Writer->m_pWriter;
	}

	gfFileBin &GetBinFile()
	{
		DBG_ASSERT( ( m_Writer  && m_Writer->m_pFile ), "Writer should be registered" );
		return *m_Writer->m_pFile;
	}
	bool HasRegisteredWriter()const
	{
		return m_Writer != NULL && m_Writer->m_pWriter != NULL && m_Writer->m_pFile != NULL;
	}
	shared_ptr< sgpuFileWriterLifeTimeKeeper> m_Writer;

};
//============================================================================
//============================================================================

#endif // #ifndef SGPU_BASESCENE_HPP