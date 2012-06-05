/*****************************************************************************
**	api3dReference.hpp
**
**	Named reference point for attachments, targets on an object
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef API3D_REFERENCE_HPP
#error api3dReference.hpp multiply included
#endif
#define API3D_REFERENCE_HPP

#ifndef MA_MATRIX4X4_HPP
#include "Core/ma/maMatrix4x4.hpp"
#endif
#ifndef ENV_AUDITOR_HPP
#include "Core/env/envAuditor.hpp"
#endif


//============================================================================
//============================================================================
class api3dReference : public envAuditable
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	api3dReference();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~api3dReference();

	//--------------------------------------------------------------------
	// GetMatrix for this named refence
	//--------------------------------------------------------------------
	virtual maMatrix4x4 GetMatrix() const = 0;

	//--------------------------------------------------------------------
	// Set whether to attach to the center of the bounding box at a
	//	given reference point instead of using world transform.
	//--------------------------------------------------------------------
	void SetUseBoundingBox(bool i_bUseBBox) { m_bUseBBox = i_bUseBBox; }
	bool GetUseBoundingBox() const { return m_bUseBBox; }

private:
	bool m_bUseBBox;
};
