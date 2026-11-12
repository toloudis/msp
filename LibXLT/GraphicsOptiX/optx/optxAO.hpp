/*****************************************************************************
**  optxAO.hpp
**
**      optxAO launches the progressive ambient occlusion programs: the
**	host side of optx/private/Programs/optxAO.cu (launch parameters and
**	what each launch does in optxAOParams.h). One pipeline holds both
**	ray generation programs; Accumulate and Resolve pick one through the
**	shader binding table's ray generation record.
**
**	Launches are queued on optxContext's stream and not waited for.
**	Create builds the module, program groups, pipeline and shader binding
**	table; optxContext must exist and be current.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXAO_HPP
#error optxAO.hpp multiply included
#endif
#define GOX_OPTXAO_HPP

#ifndef GOX_OPTXSDK_HPP
#include "GraphicsOptiX/optx/optxSdk.hpp"
#endif
#ifndef GOX_CUBUFFER_HPP
#include "GraphicsOptiX/cu/cuBuffer.hpp"
#endif
#ifndef GOX_OPTXAOPARAMS_H
#include "GraphicsOptiX/optx/private/Programs/optxAOParams.h"
#endif

#include <string>

namespace gox
{
	class optxAO
	{
		public:
			optxAO();
			~optxAO();
			optxAO(const optxAO&) = delete;
			optxAO& operator=(const optxAO&) = delete;

			//--------------------------------------------------------------------
			//	Create - returns false (o_Error says why, with OptiX's log) if
			//	the programs can't be loaded
			//--------------------------------------------------------------------
			bool Create(std::string& o_Error);
			void Destroy();
			bool IsCreated() const { return m_Pipeline != NULL; }

			//--------------------------------------------------------------------
			//	Accumulate - __raygen__ao over m_Width x m_Height.
			//	i_nHitRecords: the hit group records the traversable needs
			//	(g3dSceneBVHOptiX::GetSbtRecordCount)
			//--------------------------------------------------------------------
			bool Accumulate(const optxAOParams& i_Params, unsigned i_nHitRecords);

			//--------------------------------------------------------------------
			//	Resolve - __raygen__resolve over m_Width x m_Height
			//--------------------------------------------------------------------
			bool Resolve(const optxAOParams& i_Params);

		private:
			bool make_records(unsigned i_nHitRecords);
			bool launch(const optxAOParams& i_Params, CUdeviceptr i_Raygen, cuBuffer& io_ParamBuffer);

			OptixModule					m_Module;
			OptixProgramGroup			m_Groups[5];	// raygen ao, raygen resolve, miss camera, miss occlusion, hit
			OptixPipeline				m_Pipeline;
			cuBuffer					m_Records;		// the two raygen, two miss, then the hit group records
			unsigned					m_nHitRecords;
			OptixShaderBindingTable		m_Sbt;
			cuBuffer					m_AccumulateParams;
			cuBuffer					m_ResolveParams;
	};
}
