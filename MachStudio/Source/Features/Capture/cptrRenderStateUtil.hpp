/*****************************************************************************
**	cptrRenderStateUtil.hpp
**
**		Utilities for maintaining the last render state position in a file.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDERSTATEUTIL_HPP
#error cptrRenderStateUtil.hpp multiply included
#endif
#define CPTR_RENDERSTATEUTIL_HPP


//============================================================================
//============================================================================
namespace cptrRenderStateUtil
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//float LoadRenderPosition();
	void SaveRenderPosition();
	void DeleteRenderPosition();

}	// end of namespace
