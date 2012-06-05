/****************************************************************************\
**  sgpuMaterialImpl.hpp
**
**      sgpuMaterialImpl.hpp defines private implementation for sgpuMaterial.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifndef SGPU_MATERIALIMPL_HPP
#define SGPU_MATERIALIMPL_HPP

#ifndef MDL_MATINFO_HPP
#include "Graphics/mdl/mdlMatInfo.hpp"
#endif 
#ifndef EFF_PHONGDATA_HPP
#include "Graphics/Eff/effPhongData.hpp"
#endif 
#include <string>

//============================================================================
//============================================================================
struct sgpuMaterialImpl
{
	shared_ptr<mdlMatInfo> m_MatInfo;
	shared_ptr<effPhongData> m_PhongData;
	std::string	m_Name;
	bool operator == (const sgpuMaterialImpl &other) const
	{
		return m_MatInfo == other.m_MatInfo && m_PhongData == other.m_PhongData && m_Name == other.m_Name;
	}
};

#endif // #ifndef SGPU_MATERIALIMPL_HPP
