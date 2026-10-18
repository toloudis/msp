/*****************************************************************************
**  fxEffectVk.hpp
**
**      fxEffectVk is the Vulkan backend for a plain-HLSL effect described by
**      an fxEffectDesc: GraphicsDX11's fxEffectDX11, with SPIR-V shaders
**      (compiled by DXC, see Fx/private/EmbedShadersVk.ps1) and SPIRV-Reflect
**      (through vkdReflect) in place of D3D11 shaders and D3DReflect. Its
**      parameter names, indices, constant buffer layouts, defaults and
**      register slots are identical to fxEffectDX11's for the same effect
**      (fxEffectVkReflectionTest checks every embedded effect).
**
**      - Every pass is a set of stages; the pipeline for it is looked up
**        when it draws (vkdContext, vkdPipelineCache).
**      - Parameters are resolved to handles once (FindConstant/FindResource)
**        and written through those handles into CPU-side constant buffer
**        blocks and a resource table; Apply() uploads the blocks a pass
**        reads into the per-frame ring buffer and binds them, with the
**        textures and samplers, at their DX11 register slots.
**      - Every constant buffer must have one layout across all of the
**        effect's entry points (as in fxEffectDX11).
**      - Samplers are immutable, created from the manifest.
**
**      Stages DXC can't compile yet (the hair hull shader, see
**      fxEmbeddedShadersVk::k_Deferred) have no bytecode: their passes are
**      "deferred", draw nothing and say so once (GVK_NOT_YET).
\****************************************************************************/
#ifdef GVK_FXEFFECTVK_HPP
#error fxEffectVk.hpp multiply included
#endif
#define GVK_FXEFFECTVK_HPP

#ifndef GVK_FXEFFECTDESC_HPP
#include "GraphicsVulkan/Fx/fxEffectDesc.hpp"
#endif
#ifndef GVK_VKDREFLECT_HPP
#include "GraphicsVulkan/vkd/vkdReflect.hpp"
#endif
#ifndef GVK_VKDSHADER_HPP
#include "GraphicsVulkan/vkd/vkdShader.hpp"
#endif

#include <volk.h>

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace gvk
{
	class vkdDevice;
	class vkdImage;

	//	A manifest sampler state (D3D names) as Vulkan sampler state. false,
	//	with o_Error, for a name or border colour Vulkan can't express.
	bool fxSamplerToVulkan(const fxSamplerDesc& i_Desc, VkSamplerCreateInfo& o_Info, std::string& o_Error);

	//========================================================================
	// Compiled SPIR-V for one entry point and (manifest) profile.
	//========================================================================
	struct fxBytecode
	{
		const void* m_pData = nullptr;
		size_t m_Size = 0;
	};

	// Returns the bytecode for (entry, profile), or false if there is none.
	typedef std::function<bool(const std::string& i_Entry, const std::string& i_Profile,
							   fxBytecode& o_Bytecode)> fxBytecodeSource;

	//========================================================================
	//========================================================================
	class fxEffectVk
	{
	public:
		// Handle to a variable inside one of the effect's constant buffers.
		struct Constant
		{
			int m_Buffer = -1;
			uint32_t m_Offset = 0;
			uint32_t m_Size = 0;
			bool m_bColumnMajor = false;	// a matrix stored column-major (the HLSL default)
			uint16_t m_Columns = 0;			// components per vector
			uint16_t m_Elements = 0;		// array length; 0 if not an array
			bool IsValid() const { return m_Buffer >= 0; }
		};

		// The shape of a constant, as D3D11 reflection describes it
		// (fxVarClass / fxVarType values).
		typedef vkdShaderType Type;

		// Handle to a texture/buffer (t register) slot in the resource table.
		struct Resource
		{
			int m_Index = -1;
			bool IsValid() const { return m_Index >= 0; }
		};

		// Handle to an unordered-access (u register) slot.
		struct UnorderedAccess
		{
			int m_Index = -1;
			bool IsValid() const { return m_Index >= 0; }
		};

		// One register a stage reads, as D3D11 reflection reports it.
		struct StageBinding
		{
			enum Kind { e_ConstantBuffer, e_Texture, e_Sampler, e_Uav };
			Kind m_Kind;
			std::string m_Name;
			uint32_t m_Slot;
		};

		// Reads every stage and, with a device, creates the shader modules
		// and samplers. i_pDevice == NULL: reflection only (tests and tools).
		// i_BoolNames: names the HLSL declares bool; SPIR-V stores a bool
		// in a constant buffer as a uint, and those are reported as bool,
		// as D3D reflection does (the manifest's declared types are used too).
		// Returns nullptr and sets o_Error on failure.
		static std::unique_ptr<fxEffectVk> Create(vkdDevice* i_pDevice, const fxEffectDesc& i_Desc,
												  const fxBytecodeSource& i_Bytecode, std::string& o_Error,
												  const std::vector<std::string>& i_BoolNames = std::vector<std::string>());

		~fxEffectVk();

		const fxEffectDesc& GetDesc() const { return m_Desc; }

		// Techniques and passes
		int GetTechniqueCount() const { return (int)m_Desc.m_Techniques.size(); }
		int FindTechnique(const std::string& i_Name) const;	// -1 if missing
		int GetPassCount(int i_Technique) const;
		// The pass uses a stage DXC couldn't compile (see the header comment)
		bool IsPassDeferred(int i_Technique, int i_Pass) const;
		// What one stage of a pass reads, by register (empty if the stage is unused)
		std::vector<StageBinding> GetStageBindings(int i_Technique, int i_Pass, fxStage i_Stage) const;
		// The reflection of one stage of a pass; nullptr if the stage is unused or deferred
		const vkdShaderReflection* GetStageReflection(int i_Technique, int i_Pass, fxStage i_Stage) const;

		// Parameter handles; invalid if no entry point uses the name.
		Constant FindConstant(const std::string& i_Name) const;
		// windows.h #defines FindResource as FindResourceW (or A): the method
		// keeps its name whatever was included, and the forwarders let calls
		// from code that has windows.h in effect resolve to it
#pragma push_macro("FindResource")
#undef FindResource
		Resource FindResource(const std::string& i_Name) const;
		Resource FindResourceW(const std::string& i_Name) const { return FindResource(i_Name); }
		Resource FindResourceA(const std::string& i_Name) const { return FindResource(i_Name); }
#pragma pop_macro("FindResource")
		UnorderedAccess FindUnorderedAccess(const std::string& i_Name) const;
		// The reflected type of a constant; nullptr if no entry point uses it.
		const Type* FindConstantType(const std::string& i_Name) const;
		// Names of every constant and resource some entry point uses.
		std::vector<std::string> GetConstantNames() const;
		std::vector<std::string> GetResourceNames() const;
		std::vector<std::string> GetUnorderedAccessNames() const;
		std::vector<std::string> GetSamplerNames() const;

		// Writes go to CPU-side storage; Apply() uploads and binds them.
		void SetConstant(const Constant& i_Constant, const void* i_pData, uint32_t i_Bytes);
		template <class T> void SetConstant(const Constant& i_Constant, const T& i_Value)
		{
			SetConstant(i_Constant, &i_Value, (uint32_t)sizeof(T));
		}
		// A vector: as many floats as the variable has components.
		void SetFloatVector(const Constant& i_Constant, const float* i_pVector)
		{
			SetConstant(i_Constant, i_pVector, i_Constant.m_Columns * (uint32_t)sizeof(float));
		}
		// An array of vectors given packed (a float2[] as 2 floats per element);
		// constant buffers store each element on its own 16-byte register.
		void SetVectorArray(const Constant& i_Constant, const float* i_pPacked,
			uint32_t i_FirstElement, uint32_t i_Count);
		// A 4x4 matrix given row-major; transposed on the way in when the
		// shader stores it column-major.
		void SetMatrix(const Constant& i_Constant, const float* i_pRowMajor4x4);
		void SetResource(const Resource& i_Resource, vkdImage* i_pImage);
		void SetUnorderedAccess(const UnorderedAccess& i_UAV, vkdImage* i_pImage);

		// Read back what was written (or the default), for the material UI.
		void GetConstant(const Constant& i_Constant, void* o_pData, uint32_t i_Bytes) const;
		vkdImage* GetResource(const Resource& i_Resource) const;

		// Whole constant buffers, by the cbuffer's name in the .hlsl (see
		// fxMaterialInstance).
		int FindConstantBuffer(const std::string& i_Name) const;		// -1 if missing
		std::vector<std::string> GetConstantBufferNames() const;		// by buffer index
		uint32_t GetConstantBufferSize(int i_Buffer) const;
		const void* GetConstantBufferData(int i_Buffer) const;
		const void* GetConstantBufferDefaults(int i_Buffer) const;	// as created
		void SetConstantBufferData(int i_Buffer, const void* i_pData, uint32_t i_Bytes);

		// Sets the pass's shaders on vkdGlobal::Context(), and binds the
		// constant buffers (uploaded to the ring when changed, or when the
		// last upload was for an earlier submission), textures, samplers and
		// UAVs every stage of the pass reads, at their registers. A NULL
		// texture binds a transparent black one, as D3D reads a NULL view.
		// A deferred pass binds no shaders, so what is drawn next draws
		// nothing (logged once).
		void Apply(int i_Technique, int i_Pass);

	private:
		fxEffectVk() {}

		struct Binding
		{
			uint32_t m_Slot;	// register in the shader
			int m_Index;		// into m_ConstantBuffers, m_Resources, m_Samplers or m_UnorderedAccess
		};

		struct Shader
		{
			fxStage m_Stage = fx_VS;
			fxBytecode m_Bytecode;
			std::unique_ptr<vkdShader> m_pShader;	// a module only with a device
			std::vector<Binding> m_ConstantBuffers;
			std::vector<Binding> m_Resources;
			std::vector<Binding> m_Samplers;
			std::vector<Binding> m_UnorderedAccess;
		};

		// One pass: a shader (or none) per graphics stage. Immutable after Create.
		struct Pipeline
		{
			const Shader* m_Stages[fx_NumGraphicsStages] = {};
			std::vector<int> m_ConstantBuffers;	// every buffer any stage reads
			bool m_bDeferred = false;
			bool m_bUavs = false;				// some stage writes a UAV
			mutable bool m_bReported = false;	// deferral / slot clash logged
		};

		struct ConstantBuffer
		{
			std::string m_Name;
			uint32_t m_Size = 0;
			std::vector<uint8_t> m_Data;
			std::vector<uint8_t> m_Defaults;
			bool m_bDirty = true;
			uint64_t m_UploadedFor = 0;		// vkdDevice::PendingValue() of the last upload
			VkDeviceAddress m_Gpu = 0;		// where it was uploaded
		};

		struct Variable
		{
			Constant m_Constant;
			Type m_Type;
		};

		bool build(vkdDevice* i_pDevice, const fxBytecodeSource& i_Bytecode, std::string& o_Error);
		const Shader* get_shader(vkdDevice* i_pDevice, fxStage i_Stage, const fxStageDesc& i_Desc,
			const fxBytecodeSource& i_Bytecode, bool& o_bDeferred, std::string& o_Error);
		bool reflect(Shader& io_Shader, std::string& o_Error);
		int get_sampler(const std::string& i_Name);
		bool check_samplers(std::string& o_Error) const;
		uint32_t sampler_index(int i_Sampler);
		void apply_defaults();

		fxEffectDesc m_Desc;
		std::vector<std::string> m_BoolNames;
		std::vector<std::vector<Pipeline>> m_Pipelines;		// [technique][pass]
		std::map<std::string, std::unique_ptr<Shader>> m_Shaders;	// by entry|profile
		std::vector<ConstantBuffer> m_ConstantBuffers;
		std::map<std::string, Variable> m_Variables;
		std::vector<vkdImage*> m_Resources;
		std::map<std::string, int> m_ResourceNames;
		std::vector<vkdImage*> m_UnorderedAccess;
		std::map<std::string, int> m_UnorderedAccessNames;
		std::vector<std::string> m_SamplerList;					// by index
		std::vector<uint32_t> m_SamplerHeap;					// vkdBindings sampler index, by index (created on first Apply)
		std::string m_Name;										// for logs and debug names
		std::map<std::string, int> m_SamplerNames;
	};
}
