/*****************************************************************************
**  optxRayQuery.cpp
**
**      See optxRayQuery.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_OPTXRAYQUERY_HPP
#include "GraphicsOptiX/optx/optxRayQuery.hpp"
#endif
#ifndef GOX_OPTXCHECK_HPP
#include "GraphicsOptiX/optx/optxCheck.hpp"
#endif
#ifndef GOX_OPTXCONTEXT_HPP
#include "GraphicsOptiX/optx/optxContext.hpp"
#endif
#ifndef GOX_OPTXEMBEDDEDPROGRAMS_HPP
#include "GraphicsOptiX/optx/private/optxEmbeddedPrograms.hpp"
#endif
#ifndef GOX_CULOADER_HPP
#include "GraphicsOptiX/cu/cuLoader.hpp"
#endif

#include <optix_stack_size.h>

#include <cstring>

namespace gox
{
	namespace
	{
		//	An SBT record with no data: just the header OptiX fills
		struct alignas(OPTIX_SBT_RECORD_ALIGNMENT) EmptyRecord
		{
			char m_Header[OPTIX_SBT_RECORD_HEADER_SIZE];
		};

		std::string with_log(const std::string& i_What, OptixResult i_Result, const char* i_Log, size_t i_LogSize)
		{
			std::string text = i_What + " (" + optxResultName(i_Result) + ")";
			if (i_LogSize > 1 && i_Log[0])
			{
				text += ": ";
				text += i_Log;
			}
			return text;
		}
	}

	optxRayQuery::optxRayQuery()
		: m_Module(NULL)
		, m_RaygenGroup(NULL)
		, m_MissGroup(NULL)
		, m_HitGroup(NULL)
		, m_Pipeline(NULL)
		, m_Sbt()
	{
	}

	optxRayQuery::~optxRayQuery()
	{
		Destroy();
	}

	bool optxRayQuery::Create(std::string& o_Error)
	{
		Destroy();
		o_Error.clear();
		OptixDeviceContext context = optxContext::GetOptix();

		const optxEmbeddedProgram* program = optxEmbeddedPrograms::Find("optxRayQuery");
		if (!program)
		{
			o_Error = "the optxRayQuery program isn't embedded";
			return false;
		}

		OptixModuleCompileOptions module_options = {};
		module_options.maxRegisterCount = OPTIX_COMPILE_DEFAULT_MAX_REGISTER_COUNT;
		module_options.optLevel = OPTIX_COMPILE_OPTIMIZATION_DEFAULT;
		module_options.debugLevel = OPTIX_COMPILE_DEBUG_LEVEL_MINIMAL;	// nvcc -lineinfo

		OptixPipelineCompileOptions pipeline_options = {};
		pipeline_options.usesMotionBlur = 0;
		pipeline_options.traversableGraphFlags = OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_ANY;
		pipeline_options.numPayloadValues = 4;
		pipeline_options.numAttributeValues = 2;
		pipeline_options.exceptionFlags = OPTIX_EXCEPTION_FLAG_NONE;
		pipeline_options.pipelineLaunchParamsVariableName = "params";
		pipeline_options.usesPrimitiveTypeFlags = OPTIX_PRIMITIVE_TYPE_FLAGS_TRIANGLE;

		char log[2048];
		size_t log_size = sizeof(log);
		log[0] = 0;
		OptixResult result = optixModuleCreate(context, &module_options, &pipeline_options,
											   reinterpret_cast<const char*>(program->m_pIR), program->m_Size,
											   log, &log_size, &m_Module);
		if (result != OPTIX_SUCCESS)
		{
			o_Error = with_log("optxRayQuery: module creation failed", result, log, log_size);
			Destroy();
			return false;
		}

		OptixProgramGroupDesc descs[3] = {};
		descs[0].kind = OPTIX_PROGRAM_GROUP_KIND_RAYGEN;
		descs[0].raygen.module = m_Module;
		descs[0].raygen.entryFunctionName = "__raygen__rayQuery";
		descs[1].kind = OPTIX_PROGRAM_GROUP_KIND_MISS;
		descs[1].miss.module = m_Module;
		descs[1].miss.entryFunctionName = "__miss__rayQuery";
		descs[2].kind = OPTIX_PROGRAM_GROUP_KIND_HITGROUP;
		descs[2].hitgroup.moduleCH = m_Module;
		descs[2].hitgroup.entryFunctionNameCH = "__closesthit__rayQuery";
		OptixProgramGroupOptions group_options = {};
		OptixProgramGroup groups[3] = {};
		log_size = sizeof(log);
		log[0] = 0;
		result = optixProgramGroupCreate(context, descs, 3, &group_options, log, &log_size, groups);
		if (result != OPTIX_SUCCESS)
		{
			o_Error = with_log("optxRayQuery: program group creation failed", result, log, log_size);
			Destroy();
			return false;
		}
		m_RaygenGroup = groups[0];
		m_MissGroup = groups[1];
		m_HitGroup = groups[2];

		OptixPipelineLinkOptions link_options = {};
		link_options.maxTraceDepth = 1;
		log_size = sizeof(log);
		log[0] = 0;
		result = optixPipelineCreate(context, &pipeline_options, &link_options, groups, 3, log, &log_size, &m_Pipeline);
		if (result != OPTIX_SUCCESS)
		{
			o_Error = with_log("optxRayQuery: pipeline creation failed", result, log, log_size);
			Destroy();
			return false;
		}

		//	Stack sizes for one level of tracing, no callables; IAS over GAS
		OptixStackSizes sizes = {};
		for (OptixProgramGroup group : groups)
		{
			if (OPTIX_CHECK(optixUtilAccumulateStackSizes(group, &sizes, m_Pipeline)))
			{
				o_Error = "optxRayQuery: stack sizes unavailable";
				Destroy();
				return false;
			}
		}
		unsigned int dc_from_traversal = 0, dc_from_state = 0, continuation = 0;
		if (OPTIX_CHECK(optixUtilComputeStackSizes(&sizes, 1, 0, 0, &dc_from_traversal, &dc_from_state, &continuation)) ||
			OPTIX_CHECK(optixPipelineSetStackSize(m_Pipeline, dc_from_traversal, dc_from_state, continuation, 2)))
		{
			o_Error = "optxRayQuery: stack sizes couldn't be set";
			Destroy();
			return false;
		}

		EmptyRecord records[3];
		if (OPTIX_CHECK(optixSbtRecordPackHeader(m_RaygenGroup, &records[0])) ||
			OPTIX_CHECK(optixSbtRecordPackHeader(m_MissGroup, &records[1])) ||
			OPTIX_CHECK(optixSbtRecordPackHeader(m_HitGroup, &records[2])) ||
			!m_Records.Upload(records, sizeof(records)))
		{
			o_Error = "optxRayQuery: shader binding table couldn't be made";
			Destroy();
			return false;
		}
		m_Sbt = OptixShaderBindingTable();
		m_Sbt.raygenRecord = m_Records.Get();
		m_Sbt.missRecordBase = m_Records.Get() + sizeof(EmptyRecord);
		m_Sbt.missRecordStrideInBytes = sizeof(EmptyRecord);
		m_Sbt.missRecordCount = 1;
		m_Sbt.hitgroupRecordBase = m_Records.Get() + 2 * sizeof(EmptyRecord);
		m_Sbt.hitgroupRecordStrideInBytes = sizeof(EmptyRecord);
		m_Sbt.hitgroupRecordCount = 1;
		return true;
	}

	void optxRayQuery::Destroy()
	{
		if (m_Pipeline)
		{
			OPTIX_CHECK(optixPipelineDestroy(m_Pipeline));
		}
		for (OptixProgramGroup group : { m_RaygenGroup, m_MissGroup, m_HitGroup })
		{
			if (group)
			{
				OPTIX_CHECK(optixProgramGroupDestroy(group));
			}
		}
		if (m_Module)
		{
			OPTIX_CHECK(optixModuleDestroy(m_Module));
		}
		m_Pipeline = NULL;
		m_RaygenGroup = m_MissGroup = m_HitGroup = NULL;
		m_Module = NULL;
		m_Sbt = OptixShaderBindingTable();
		m_Records.Free();
		m_Rays.Free();
		m_Hits.Free();
		m_Params.Free();
	}

	bool optxRayQuery::Trace(OptixTraversableHandle i_Handle,
							 const std::vector<optxRayQueryRay>& i_Rays,
							 std::vector<optxRayQueryHit>& o_Hits)
	{
		o_Hits.assign(i_Rays.size(), optxRayQueryHit());
		if (i_Rays.empty())
		{
			return true;
		}
		if (!m_Pipeline)
		{
			return false;
		}
		const size_t ray_bytes = i_Rays.size() * sizeof(optxRayQueryRay);
		const size_t hit_bytes = i_Rays.size() * sizeof(optxRayQueryHit);
		if (!m_Rays.Upload(i_Rays.data(), ray_bytes) || !m_Hits.Reserve(hit_bytes))
		{
			return false;
		}

		optxRayQueryParams params = {};
		params.m_Handle = i_Handle;
		params.m_pRays = reinterpret_cast<const optxRayQueryRay*>(m_Rays.Get());
		params.m_pHits = reinterpret_cast<optxRayQueryHit*>(m_Hits.Get());
		params.m_Count = static_cast<unsigned int>(i_Rays.size());
		if (!m_Params.Upload(&params, sizeof(params)))
		{
			return false;
		}

		if (OPTIX_CHECK(optixLaunch(m_Pipeline, optxContext::GetStream(), m_Params.Get(), sizeof(params), &m_Sbt,
									params.m_Count, 1, 1)))
		{
			return false;
		}
		return m_Hits.Download(o_Hits.data(), hit_bytes);
	}
}
