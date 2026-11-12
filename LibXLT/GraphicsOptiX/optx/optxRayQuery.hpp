/*****************************************************************************
**  optxRayQuery.hpp
**
**      optxRayQuery traces batches of rays against a traversable (an IAS
**	or a GAS) and gives back what each hit: the host side of the
**	optxRayQuery device program (optx/private/Programs/optxRayQuery.cu,
**	launch parameters in optxRayQueryParams.h). For picking and for the
**	BVH tests: it is not fast (each Trace uploads, launches and waits).
**
**	Create builds the module, program groups, pipeline and shader binding
**	table; optxContext must exist and be current.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXRAYQUERY_HPP
#error optxRayQuery.hpp multiply included
#endif
#define GOX_OPTXRAYQUERY_HPP

#ifndef GOX_OPTXSDK_HPP
#include "GraphicsOptiX/optx/optxSdk.hpp"
#endif
#ifndef GOX_CUBUFFER_HPP
#include "GraphicsOptiX/cu/cuBuffer.hpp"
#endif
#ifndef GOX_OPTXRAYQUERYPARAMS_H
#include "GraphicsOptiX/optx/private/Programs/optxRayQueryParams.h"
#endif

#include <string>
#include <vector>

namespace gox
{
	class optxRayQuery
	{
		public:
			optxRayQuery();
			~optxRayQuery();
			optxRayQuery(const optxRayQuery&) = delete;
			optxRayQuery& operator=(const optxRayQuery&) = delete;

			//--------------------------------------------------------------------
			//	Create - returns false (o_Error says why, with OptiX's log) if
			//	the program can't be loaded
			//--------------------------------------------------------------------
			bool Create(std::string& o_Error);
			void Destroy();

			//--------------------------------------------------------------------
			//	Trace - one hit per ray, in order; waits for the result.
			//	i_nHitRecords: the hit group records the traversable needs (the
			//	most build inputs of any GAS in it: g3dSceneBVHOptiX::
			//	GetSbtRecordCount)
			//--------------------------------------------------------------------
			bool Trace(OptixTraversableHandle i_Handle,
					   const std::vector<optxRayQueryRay>& i_Rays,
					   std::vector<optxRayQueryHit>& o_Hits,
					   unsigned i_nHitRecords = 1);

		private:
			bool make_records(unsigned i_nHitRecords);

			OptixModule					m_Module;
			OptixProgramGroup			m_RaygenGroup;
			OptixProgramGroup			m_MissGroup;
			OptixProgramGroup			m_HitGroup;
			OptixPipeline				m_Pipeline;
			cuBuffer					m_Records;		// raygen, miss, hit group SBT records
			unsigned					m_nHitRecords;
			OptixShaderBindingTable		m_Sbt;
			cuBuffer					m_Rays;
			cuBuffer					m_Hits;
			cuBuffer					m_Params;
	};
}
