/*****************************************************************************
**  optxAO.cpp
**
**      See optxAO.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_OPTXAO_HPP
#include "GraphicsOptiX/optx/optxAO.hpp"
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

#include <optix_stack_size.h>

#include <vector>

namespace gox
{
	namespace
	{
		//	An SBT record with no data: just the header OptiX fills
		struct alignas(OPTIX_SBT_RECORD_ALIGNMENT) EmptyRecord
		{
			char m_Header[OPTIX_SBT_RECORD_HEADER_SIZE];
		};

		enum Group
		{
			e_RaygenAO,
			e_RaygenResolve,
			e_MissCamera,
			e_MissOcclusion,
			e_Hit,
			e_GroupCount
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

	optxAO::optxAO()
		: m_Module(NULL)
		, m_Groups()
		, m_Pipeline(NULL)
		, m_nHitRecords(0)
		, m_Sbt()
	{
	}

	optxAO::~optxAO()
	{
		Destroy();
	}

	bool optxAO::Create(std::string& o_Error)
	{
		Destroy();
		o_Error.clear();
		OptixDeviceContext context = optxContext::GetOptix();

		const optxEmbeddedProgram* program = optxEmbeddedPrograms::Find("optxAO");
		if (!program)
		{
			o_Error = "the optxAO program isn't embedded";
			return false;
		}

		OptixModuleCompileOptions module_options = {};
		module_options.maxRegisterCount = OPTIX_COMPILE_DEFAULT_MAX_REGISTER_COUNT;
		module_options.optLevel = OPTIX_COMPILE_OPTIMIZATION_DEFAULT;
		module_options.debugLevel = OPTIX_COMPILE_DEBUG_LEVEL_MINIMAL;	// nvcc -lineinfo

		OptixPipelineCompileOptions pipeline_options = {};
		pipeline_options.usesMotionBlur = 0;
		pipeline_options.traversableGraphFlags = OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_LEVEL_INSTANCING;
		pipeline_options.numPayloadValues = 5;
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
			o_Error = with_log("optxAO: module creation failed", result, log, log_size);
			Destroy();
			return false;
		}

		OptixProgramGroupDesc descs[e_GroupCount] = {};
		descs[e_RaygenAO].kind = OPTIX_PROGRAM_GROUP_KIND_RAYGEN;
		descs[e_RaygenAO].raygen.module = m_Module;
		descs[e_RaygenAO].raygen.entryFunctionName = "__raygen__ao";
		descs[e_RaygenResolve].kind = OPTIX_PROGRAM_GROUP_KIND_RAYGEN;
		descs[e_RaygenResolve].raygen.module = m_Module;
		descs[e_RaygenResolve].raygen.entryFunctionName = "__raygen__resolve";
		descs[e_MissCamera].kind = OPTIX_PROGRAM_GROUP_KIND_MISS;
		descs[e_MissCamera].miss.module = m_Module;
		descs[e_MissCamera].miss.entryFunctionName = "__miss__camera";
		descs[e_MissOcclusion].kind = OPTIX_PROGRAM_GROUP_KIND_MISS;
		descs[e_MissOcclusion].miss.module = m_Module;
		descs[e_MissOcclusion].miss.entryFunctionName = "__miss__occlusion";
		descs[e_Hit].kind = OPTIX_PROGRAM_GROUP_KIND_HITGROUP;
		descs[e_Hit].hitgroup.moduleCH = m_Module;
		descs[e_Hit].hitgroup.entryFunctionNameCH = "__closesthit__camera";
		OptixProgramGroupOptions group_options = {};
		log_size = sizeof(log);
		log[0] = 0;
		result = optixProgramGroupCreate(context, descs, e_GroupCount, &group_options, log, &log_size, m_Groups);
		if (result != OPTIX_SUCCESS)
		{
			for (OptixProgramGroup& group : m_Groups)
			{
				group = NULL;
			}
			o_Error = with_log("optxAO: program group creation failed", result, log, log_size);
			Destroy();
			return false;
		}

		//	The camera ray traces occlusion rays from the ray generation
		//	program, not from a hit program: depth 1
		OptixPipelineLinkOptions link_options = {};
		link_options.maxTraceDepth = 1;
		log_size = sizeof(log);
		log[0] = 0;
		result = optixPipelineCreate(context, &pipeline_options, &link_options, m_Groups, e_GroupCount, log, &log_size,
									 &m_Pipeline);
		if (result != OPTIX_SUCCESS)
		{
			m_Pipeline = NULL;
			o_Error = with_log("optxAO: pipeline creation failed", result, log, log_size);
			Destroy();
			return false;
		}

		//	Stack sizes for one level of tracing, no callables; IAS over GAS
		OptixStackSizes sizes = {};
		for (OptixProgramGroup group : m_Groups)
		{
			if (OPTIX_CHECK(optixUtilAccumulateStackSizes(group, &sizes, m_Pipeline)))
			{
				o_Error = "optxAO: stack sizes unavailable";
				Destroy();
				return false;
			}
		}
		unsigned int dc_from_traversal = 0, dc_from_state = 0, continuation = 0;
		if (OPTIX_CHECK(optixUtilComputeStackSizes(&sizes, 1, 0, 0, &dc_from_traversal, &dc_from_state, &continuation)) ||
			OPTIX_CHECK(optixPipelineSetStackSize(m_Pipeline, dc_from_traversal, dc_from_state, continuation, 2)))
		{
			o_Error = "optxAO: stack sizes couldn't be set";
			Destroy();
			return false;
		}

		if (!make_records(1))
		{
			o_Error = "optxAO: shader binding table couldn't be made";
			Destroy();
			return false;
		}
		return true;
	}

	//	Two raygen, two miss, then i_nHitRecords identical hit group records
	//	(one per build input of the biggest GAS)
	bool optxAO::make_records(unsigned i_nHitRecords)
	{
		std::vector<EmptyRecord> records(4 + i_nHitRecords);
		for (int g = 0; g < 4; ++g)
		{
			if (OPTIX_CHECK(optixSbtRecordPackHeader(m_Groups[g], &records[g])))
			{
				return false;
			}
		}
		for (unsigned i = 0; i < i_nHitRecords; ++i)
		{
			if (OPTIX_CHECK(optixSbtRecordPackHeader(m_Groups[e_Hit], &records[4 + i])))
			{
				return false;
			}
		}
		//	Queued launches may still read the table: the upload is ordered
		//	after them on the stream, and freeing a grown allocation waits
		if (!m_Records.Upload(records.data(), records.size() * sizeof(EmptyRecord)))
		{
			return false;
		}
		m_nHitRecords = i_nHitRecords;
		m_Sbt = OptixShaderBindingTable();
		m_Sbt.raygenRecord = m_Records.Get();
		m_Sbt.missRecordBase = m_Records.Get() + 2 * sizeof(EmptyRecord);
		m_Sbt.missRecordStrideInBytes = sizeof(EmptyRecord);
		m_Sbt.missRecordCount = 2;
		m_Sbt.hitgroupRecordBase = m_Records.Get() + 4 * sizeof(EmptyRecord);
		m_Sbt.hitgroupRecordStrideInBytes = sizeof(EmptyRecord);
		m_Sbt.hitgroupRecordCount = i_nHitRecords;
		return true;
	}

	void optxAO::Destroy()
	{
		if (m_Pipeline)
		{
			OPTIX_CHECK(optixPipelineDestroy(m_Pipeline));
		}
		for (OptixProgramGroup& group : m_Groups)
		{
			if (group)
			{
				OPTIX_CHECK(optixProgramGroupDestroy(group));
			}
			group = NULL;
		}
		if (m_Module)
		{
			OPTIX_CHECK(optixModuleDestroy(m_Module));
		}
		m_Pipeline = NULL;
		m_Module = NULL;
		m_Sbt = OptixShaderBindingTable();
		m_nHitRecords = 0;
		m_Records.Free();
		m_AccumulateParams.Free();
		m_ResolveParams.Free();
	}

	bool optxAO::launch(const optxAOParams& i_Params, CUdeviceptr i_Raygen, cuBuffer& io_ParamBuffer)
	{
		if (!m_Pipeline || i_Params.m_Width == 0 || i_Params.m_Height == 0)
		{
			return false;
		}
		//	Stream-ordered: a launch queued before reads the old contents
		if (!io_ParamBuffer.Upload(&i_Params, sizeof(i_Params)))
		{
			return false;
		}
		OptixShaderBindingTable sbt = m_Sbt;
		sbt.raygenRecord = i_Raygen;
		return !OPTIX_CHECK(optixLaunch(m_Pipeline, optxContext::GetStream(), io_ParamBuffer.Get(), sizeof(i_Params), &sbt,
										i_Params.m_Width, i_Params.m_Height, 1));
	}

	bool optxAO::Accumulate(const optxAOParams& i_Params, unsigned i_nHitRecords)
	{
		if (i_nHitRecords > m_nHitRecords && !make_records(i_nHitRecords))
		{
			return false;
		}
		return launch(i_Params, m_Records.Get(), m_AccumulateParams);
	}

	bool optxAO::Resolve(const optxAOParams& i_Params)
	{
		return launch(i_Params, m_Records.Get() + sizeof(EmptyRecord), m_ResolveParams);
	}
}
