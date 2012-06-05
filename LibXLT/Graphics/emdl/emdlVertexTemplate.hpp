/*****************************************************************************
**  emdlVertexTemplate.hpp
**
**      A emdlVertexTemplate contains the geometry and animation information 
**	needed to generate smdlVertexObjects.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef EMDL_VERTEXTEMPLATE_HPP
#error emdlVertexTemplate.hpp multiply included
#endif
#define EMDL_VERTEXTEMPLATE_HPP

#ifndef ENT_MODELTEMPLATE_HPP
#include "Graphics/ent/entModelTemplate.hpp"
#endif
#ifndef MDL_FRAGINFO_HPP
#include "Graphics/mdl/mdlFragInfo.hpp"
#endif


//============================================================================
//============================================================================
class emdlVertexTemplate : public entModelTemplate
{
	public:
		//--------------------------------------------------------------------
		//	Constructor
		//--------------------------------------------------------------------
		emdlVertexTemplate();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~emdlVertexTemplate();

		//--------------------------------------------------------------------
		//	These functions are only used for single-skin models - they
		//	return information needed for the skin.
		//--------------------------------------------------------------------
		inline std::vector<mdlFragInfo>& FragInfos();
		inline const std::vector<mdlFragInfo>& GetFragInfos() const;

	private:
		// Single skin stuff
		std::vector<mdlFragInfo> m_FragInfos;
};


//--------------------------------------------------------------------
//	GetFragInfos - used for single-skin models only
//--------------------------------------------------------------------
inline std::vector<mdlFragInfo>& emdlVertexTemplate::FragInfos()
{
	return m_FragInfos;
}
inline const std::vector<mdlFragInfo>& emdlVertexTemplate::GetFragInfos() const
{
	return m_FragInfos;
}
