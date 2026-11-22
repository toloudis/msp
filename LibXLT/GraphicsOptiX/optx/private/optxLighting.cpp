/*****************************************************************************
**  optxLighting.cpp
**
**      See optxLighting.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_OPTXLIGHTING_HPP
#include "GraphicsOptiX/optx/optxLighting.hpp"
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
			e_RaygenLighting,
			e_RaygenResolve,
			e_RaygenEnvCells,
			e_RaygenEnvRows,
			e_RaygenEnvMarginal,
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

	optxLighting::optxLighting()
		: m_Module(NULL)
		, m_Groups()
		, m_Pipeline(NULL)
		, m_nHitRecords(0)
		, m_Sbt()
	{
	}

	optxLighting::~optxLighting()
	{
		Destroy();
	}

	bool optxLighting::Create(std::string& o_Error)
	{
		Destroy();
		o_Error.clear();
		OptixDeviceContext context = optxContext::GetOptix();

		const optxEmbeddedProgram* program = optxEmbeddedPrograms::Find("optxLighting");
		if (!program)
		{
			o_Error = "the optxLighting program isn't embedded";
			return false;
		}

		OptixModuleCompileOptions module_options = {};
		module_options.maxRegisterCount = OPTIX_COMPILE_DEFAULT_MAX_REGISTER_COUNT;
		module_options.optLevel = OPTIX_COMPILE_OPTIMIZATION_DEFAULT;
		module_options.debugLevel = OPTIX_COMPILE_DEBUG_LEVEL_MINIMAL;	// nvcc -lineinfo

		OptixPipelineCompileOptions pipeline_options = {};
		pipeline_options.usesMotionBlur = 0;
		pipeline_options.traversableGraphFlags = OPTIX_TRAVERSABLE_GRAPH_FLAG_ALLOW_SINGLE_LEVEL_INSTANCING;
		pipeline_options.numPayloadValues = 19;
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
			o_Error = with_log("optxLighting: module creation failed", result, log, log_size);
			Destroy();
			return false;
		}

		OptixProgramGroupDesc descs[e_GroupCount] = {};
		descs[e_RaygenLighting].kind = OPTIX_PROGRAM_GROUP_KIND_RAYGEN;
		descs[e_RaygenLighting].raygen.module = m_Module;
		descs[e_RaygenLighting].raygen.entryFunctionName = "__raygen__lighting";
		descs[e_RaygenResolve].kind = OPTIX_PROGRAM_GROUP_KIND_RAYGEN;
		descs[e_RaygenResolve].raygen.module = m_Module;
		descs[e_RaygenResolve].raygen.entryFunctionName = "__raygen__resolve";
		const char* env_programs[3] = { "__raygen__env_cells", "__raygen__env_rows", "__raygen__env_marginal" };
		for (int i = 0; i < 3; ++i)
		{
			descs[e_RaygenEnvCells + i].kind = OPTIX_PROGRAM_GROUP_KIND_RAYGEN;
			descs[e_RaygenEnvCells + i].raygen.module = m_Module;
			descs[e_RaygenEnvCells + i].raygen.entryFunctionName = env_programs[i];
		}
		descs[e_MissCamera].kind = OPTIX_PROGRAM_GROUP_KIND_MISS;
		descs[e_MissCamera].miss.module = m_Module;
		descs[e_MissCamera].miss.entryFunctionName = "__miss__camera";
		descs[e_MissOcclusion].kind = OPTIX_PROGRAM_GROUP_KIND_MISS;
		descs[e_MissOcclusion].miss.module = m_Module;
		descs[e_MissOcclusion].miss.entryFunctionName = "__miss__occlusion";
		descs[e_Hit].kind = OPTIX_PROGRAM_GROUP_KIND_HITGROUP;
		descs[e_Hit].hitgroup.moduleCH = m_Module;
		descs[e_Hit].hitgroup.entryFunctionNameCH = "__closesthit__camera";
		descs[e_Hit].hitgroup.moduleAH = m_Module;
		descs[e_Hit].hitgroup.entryFunctionNameAH = "__anyhit__opacity";
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
			o_Error = with_log("optxLighting: program group creation failed", result, log, log_size);
			Destroy();
			return false;
		}

		//	Every ray (camera, bounce, shadow) is traced from the ray
		//	generation program, not from a hit program: depth 1
		OptixPipelineLinkOptions link_options = {};
		link_options.maxTraceDepth = 1;
		log_size = sizeof(log);
		log[0] = 0;
		result = optixPipelineCreate(context, &pipeline_options, &link_options, m_Groups, e_GroupCount, log, &log_size,
									 &m_Pipeline);
		if (result != OPTIX_SUCCESS)
		{
			m_Pipeline = NULL;
			o_Error = with_log("optxLighting: pipeline creation failed", result, log, log_size);
			Destroy();
			return false;
		}

		//	Stack sizes for one level of tracing, no callables; IAS over GAS
		OptixStackSizes sizes = {};
		for (OptixProgramGroup group : m_Groups)
		{
			if (OPTIX_CHECK(optixUtilAccumulateStackSizes(group, &sizes, m_Pipeline)))
			{
				o_Error = "optxLighting: stack sizes unavailable";
				Destroy();
				return false;
			}
		}
		unsigned int dc_from_traversal = 0, dc_from_state = 0, continuation = 0;
		if (OPTIX_CHECK(optixUtilComputeStackSizes(&sizes, 1, 0, 0, &dc_from_traversal, &dc_from_state, &continuation)) ||
			OPTIX_CHECK(optixPipelineSetStackSize(m_Pipeline, dc_from_traversal, dc_from_state, continuation, 2)))
		{
			o_Error = "optxLighting: stack sizes couldn't be set";
			Destroy();
			return false;
		}

		if (!make_records(1))
		{
			o_Error = "optxLighting: shader binding table couldn't be made";
			Destroy();
			return false;
		}
		return true;
	}

	//	Five raygen, two miss, then i_nHitRecords identical hit group records
	//	(one per build input of the biggest GAS)
	bool optxLighting::make_records(unsigned i_nHitRecords)
	{
		std::vector<EmptyRecord> records(e_Hit + i_nHitRecords);
		for (int g = 0; g < e_Hit; ++g)
		{
			if (OPTIX_CHECK(optixSbtRecordPackHeader(m_Groups[g], &records[g])))
			{
				return false;
			}
		}
		for (unsigned i = 0; i < i_nHitRecords; ++i)
		{
			if (OPTIX_CHECK(optixSbtRecordPackHeader(m_Groups[e_Hit], &records[e_Hit + i])))
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
		m_Sbt.missRecordBase = m_Records.Get() + e_MissCamera * sizeof(EmptyRecord);
		m_Sbt.missRecordStrideInBytes = sizeof(EmptyRecord);
		m_Sbt.missRecordCount = 2;
		m_Sbt.hitgroupRecordBase = m_Records.Get() + e_Hit * sizeof(EmptyRecord);
		m_Sbt.hitgroupRecordStrideInBytes = sizeof(EmptyRecord);
		m_Sbt.hitgroupRecordCount = i_nHitRecords;
		return true;
	}

	void optxLighting::Destroy()
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
		m_EnvironmentParams.Free();
	}

	bool optxLighting::launch(const optxLightingParams& i_Params, CUdeviceptr i_Raygen, cuBuffer& io_ParamBuffer)
	{
		//	Stream-ordered: a launch queued before reads the old contents
		if (!m_Pipeline || !io_ParamBuffer.Upload(&i_Params, sizeof(i_Params)))
		{
			return false;
		}
		return launch_uploaded(i_Raygen, io_ParamBuffer, i_Params.m_Width, i_Params.m_Height);
	}

	bool optxLighting::launch_uploaded(CUdeviceptr i_Raygen, const cuBuffer& i_ParamBuffer, unsigned i_Width, unsigned i_Height)
	{
		if (!m_Pipeline || i_Width == 0 || i_Height == 0)
		{
			return false;
		}
		OptixShaderBindingTable sbt = m_Sbt;
		sbt.raygenRecord = i_Raygen;
		return !OPTIX_CHECK(optixLaunch(m_Pipeline, optxContext::GetStream(), i_ParamBuffer.Get(), sizeof(optxLightingParams),
										&sbt, i_Width, i_Height, 1));
	}

	bool optxLighting::Accumulate(const optxLightingParams& i_Params, unsigned i_nHitRecords)
	{
		if (i_nHitRecords > m_nHitRecords && !make_records(i_nHitRecords))
		{
			return false;
		}
		return launch(i_Params, m_Records.Get() + e_RaygenLighting * sizeof(EmptyRecord), m_AccumulateParams);
	}

	bool optxLighting::Resolve(const optxLightingParams& i_Params)
	{
		return launch(i_Params, m_Records.Get() + e_RaygenResolve * sizeof(EmptyRecord), m_ResolveParams);
	}

	bool optxLighting::BuildEnvironmentTable(const optxLightingParams& i_Params)
	{
		const optxEnvironmentLight& env = i_Params.m_Environment;
		if (!m_Pipeline || !env.m_Table || env.m_TableWidth == 0 || env.m_TableHeight == 0 ||
			!m_EnvironmentParams.Upload(&i_Params, sizeof(i_Params)))
		{
			return false;
		}
		const CUdeviceptr records = m_Records.Get();
		return launch_uploaded(records + e_RaygenEnvCells * sizeof(EmptyRecord), m_EnvironmentParams, env.m_TableWidth,
							   env.m_TableHeight) &&
			   launch_uploaded(records + e_RaygenEnvRows * sizeof(EmptyRecord), m_EnvironmentParams, env.m_TableHeight, 1) &&
			   launch_uploaded(records + e_RaygenEnvMarginal * sizeof(EmptyRecord), m_EnvironmentParams, 1, 1);
	}
}
