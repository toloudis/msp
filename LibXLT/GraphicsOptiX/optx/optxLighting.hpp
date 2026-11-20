/*****************************************************************************
**  optxLighting.hpp
**
**      optxLighting launches the lighting programs (Illumination Only and
**	Shadow Mask): the host side of optx/private/Programs/optxLighting.cu
**	(launch parameters and what each launch does in optxLightingParams.h).
**	One pipeline holds both ray generation programs; Accumulate and
**	Resolve pick one through the shader binding table's ray generation
**	record. Laid out as optxAO.
**
**	Launches are queued on optxContext's stream and not waited for.
**	Create builds the module, program groups, pipeline and shader binding
**	table; optxContext must exist and be current.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXLIGHTING_HPP
#error optxLighting.hpp multiply included
#endif
#define GOX_OPTXLIGHTING_HPP

#ifndef GOX_OPTXSDK_HPP
#include "GraphicsOptiX/optx/optxSdk.hpp"
#endif
#ifndef GOX_CUBUFFER_HPP
#include "GraphicsOptiX/cu/cuBuffer.hpp"
#endif
#ifndef GOX_OPTXAOPARAMS_H
#include "GraphicsOptiX/optx/private/Programs/optxAOParams.h"
#endif
#ifndef GOX_OPTXLIGHTINGPARAMS_H
#include "GraphicsOptiX/optx/private/Programs/optxLightingParams.h"
#endif

#include <string>

namespace gox
{
	class optxLighting
	{
		public:
			optxLighting();
			~optxLighting();
			optxLighting(const optxLighting&) = delete;
			optxLighting& operator=(const optxLighting&) = delete;

			//--------------------------------------------------------------------
			//	Create - returns false (o_Error says why, with OptiX's log) if
			//	the programs can't be loaded
			//--------------------------------------------------------------------
			bool Create(std::string& o_Error);
			void Destroy();
			bool IsCreated() const { return m_Pipeline != NULL; }

			//--------------------------------------------------------------------
			//	Accumulate - __raygen__lighting over m_Width x m_Height (resolving
			//	too with m_Resolve).
			//	i_nHitRecords: the hit group records the traversable needs
			//	(g3dSceneBVHOptiX::GetSbtRecordCount)
			//--------------------------------------------------------------------
			bool Accumulate(const optxLightingParams& i_Params, unsigned i_nHitRecords);

			//--------------------------------------------------------------------
			//	Resolve - __raygen__resolve over m_Width x m_Height
			//--------------------------------------------------------------------
			bool Resolve(const optxLightingParams& i_Params);

		private:
			bool make_records(unsigned i_nHitRecords);
			bool launch(const optxLightingParams& i_Params, CUdeviceptr i_Raygen, cuBuffer& io_ParamBuffer);

			OptixModule					m_Module;
			OptixProgramGroup			m_Groups[5];	// raygen lighting, raygen resolve, miss camera, miss occlusion, hit
			OptixPipeline				m_Pipeline;
			cuBuffer					m_Records;		// the two raygen, two miss, then the hit group records
			unsigned					m_nHitRecords;
			OptixShaderBindingTable		m_Sbt;
			cuBuffer					m_AccumulateParams;
			cuBuffer					m_ResolveParams;
	};
}
