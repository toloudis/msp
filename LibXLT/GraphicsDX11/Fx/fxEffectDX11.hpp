/*****************************************************************************
**  fxEffectDX11.hpp
**
**      fxEffectDX11 is the D3D11 backend for a plain-HLSL effect described by
**      an fxEffectDesc. It replaces the D3DX11 Effects runtime, and is laid
**      out the way D3D12 and Vulkan want things:
**
**      - Every pass becomes an immutable pipeline when the effect is created:
**        one shader object per stage plus the binding layout of each stage
**        (which constant buffers, textures and samplers it reads, at which
**        register), taken from shader reflection. Nothing is looked up by
**        name while drawing. A pass maps onto a pipeline state object and
**        its layout onto a root signature / descriptor set layout.
**      - Parameters are resolved to handles once (FindConstant/FindResource)
**        and written through those handles into CPU-side constant buffer
**        blocks and a resource table. Apply() uploads the blocks that
**        changed and binds what the pass's layout asks for.
**      - Every constant buffer must have one layout across all of the
**        effect's entry points (declare it as an explicit cbuffer), so a
**        block can be shared by every pass that reads it.
**      - Samplers are immutable objects created from the manifest (static
**        samplers in D3D12 terms).
\****************************************************************************/

#ifdef FX_EFFECTDX11_HPP
#error fxEffectDX11.hpp multiply included
#endif
#define FX_EFFECTDX11_HPP

#ifndef FX_EFFECTDESC_HPP
#include "GraphicsDX11/Fx/fxEffectDesc.hpp"
#endif

#include <d3d11.h>
#include <wrl/client.h>

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

//============================================================================
// Compiled shader bytecode for one entry point and profile.
//============================================================================
struct fxBytecode
{
	const void* m_pData = nullptr;
	size_t m_Size = 0;
};

// Returns the bytecode for (entry, profile), or false if there is none.
typedef std::function<bool(const std::string& i_Entry, const std::string& i_Profile,
						   fxBytecode& o_Bytecode)> fxBytecodeSource;

//============================================================================
//============================================================================
class fxEffectDX11
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

	// The shape of a constant, from shader reflection: D3D_SHADER_VARIABLE_CLASS
	// and D3D_SHADER_VARIABLE_TYPE values, and the members of a struct with
	// their byte offsets from the start of the struct.
	struct Type
	{
		struct Member;
		std::string m_Name;			// e.g. "float4", or the struct's name
		int m_Class = 0;
		int m_Type = 0;
		uint32_t m_Rows = 0;
		uint32_t m_Columns = 0;
		uint32_t m_Elements = 0;	// 0 if not an array
		std::vector<Member> m_Members;

		// Bytes one element takes in a constant buffer, before padding to
		// the next 16-byte register; array elements are 16-byte aligned.
		uint32_t GetElementSize() const;
	};
	struct Type::Member
	{
		std::string m_Name;
		uint32_t m_Offset = 0;
		Type m_Type;
	};

	// Handle to a texture/buffer (SRV) slot in the effect's resource table.
	struct Resource
	{
		int m_Index = -1;
		bool IsValid() const { return m_Index >= 0; }
	};

	// Builds every pass's pipeline. Returns nullptr and sets o_Error on failure.
	static std::unique_ptr<fxEffectDX11> Create(ID3D11Device* i_pDevice,
		const fxEffectDesc& i_Desc, const fxBytecodeSource& i_Bytecode,
		std::string& o_Error);

	~fxEffectDX11();

	const fxEffectDesc& GetDesc() const { return m_Desc; }

	// Techniques and passes
	int GetTechniqueCount() const { return (int)m_Desc.m_Techniques.size(); }
	int FindTechnique(const std::string& i_Name) const;	// -1 if missing
	int GetPassCount(int i_Technique) const;

	// Parameter handles; invalid if no entry point uses the name.
	Constant FindConstant(const std::string& i_Name) const;
	Resource FindResource(const std::string& i_Name) const;
	// The reflected type of a constant; nullptr if no entry point uses it.
	const Type* FindConstantType(const std::string& i_Name) const;
	// Names of every constant and resource some entry point uses.
	std::vector<std::string> GetConstantNames() const;
	std::vector<std::string> GetResourceNames() const;

	// Writes go to CPU-side storage; Apply() uploads and binds them.
	void SetConstant(const Constant& i_Constant, const void* i_pData, uint32_t i_Bytes);
	template <class T> void SetConstant(const Constant& i_Constant, const T& i_Value)
	{
		SetConstant(i_Constant, &i_Value, (uint32_t)sizeof(T));
	}
	// A vector: writes as many floats as the variable has components, as
	// Effects' SetFloatVector did.
	void SetFloatVector(const Constant& i_Constant, const float* i_pVector)
	{
		SetConstant(i_Constant, i_pVector, i_Constant.m_Columns * (uint32_t)sizeof(float));
	}
	// An array of vectors given packed (a float2[] as 2 floats per element), as
	// Effects' SetFloatVectorArray took it. Constant buffers store every array
	// element on its own 16-byte register, so the elements are spread out.
	void SetVectorArray(const Constant& i_Constant, const float* i_pPacked,
		uint32_t i_FirstElement, uint32_t i_Count);
	// A 4x4 matrix given row-major, as Effects' SetMatrix took it; transposed
	// on the way in when the shader stores it column-major.
	void SetMatrix(const Constant& i_Constant, const float* i_pRowMajor4x4);
	void SetResource(const Resource& i_Resource, ID3D11ShaderResourceView* i_pView);

	// Read back what was written (or the default), for the material UI.
	void GetConstant(const Constant& i_Constant, void* o_pData, uint32_t i_Bytes) const;
	ID3D11ShaderResourceView* GetResource(const Resource& i_Resource) const;

	// Whole constant buffers, by the cbuffer's name in the .hlsl. A material
	// instance keeps its own copy of the buffers that hold material
	// parameters and swaps them in before drawing; in D3D12/Vulkan that
	// copy becomes the instance's own constant buffer view.
	int FindConstantBuffer(const std::string& i_Name) const;		// -1 if missing
	uint32_t GetConstantBufferSize(int i_Buffer) const;
	const void* GetConstantBufferData(int i_Buffer) const;
	const void* GetConstantBufferDefaults(int i_Buffer) const;	// as created
	void SetConstantBufferData(int i_Buffer, const void* i_pData, uint32_t i_Bytes);

	// Binds the pass's shaders on every graphics stage (unused stages get
	// none), then the constant buffers, textures and samplers it reads.
	void Apply(int i_Technique, int i_Pass, ID3D11DeviceContext* i_pContext);

	// Vertex shader bytecode of a pass, for creating input layouts.
	fxBytecode GetVertexShaderBytecode(int i_Technique, int i_Pass) const;

private:
	fxEffectDX11() {}

	struct Binding
	{
		uint32_t m_Slot;	// register in the shader
		int m_Index;		// into m_ConstantBuffers, m_Resources or m_Samplers
	};

	struct Shader
	{
		Microsoft::WRL::ComPtr<ID3D11DeviceChild> m_pShader;
		fxBytecode m_Bytecode;
		std::vector<Binding> m_ConstantBuffers;
		std::vector<Binding> m_Resources;
		std::vector<Binding> m_Samplers;
	};

	// One pass: a shader (or none) per graphics stage. Immutable after Create.
	struct Pipeline
	{
		const Shader* m_Stages[fx_NumGraphicsStages] = {};
		std::vector<int> m_ConstantBuffers;	// every buffer any stage reads
	};

	struct ConstantBuffer
	{
		std::string m_Name;
		uint32_t m_Size = 0;
		std::vector<uint8_t> m_Data;
		std::vector<uint8_t> m_Defaults;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pBuffer;
		bool m_bDirty = true;
	};

	struct Variable
	{
		Constant m_Constant;
		Type m_Type;
	};

	bool build(ID3D11Device* i_pDevice, const fxBytecodeSource& i_Bytecode, std::string& o_Error);
	const Shader* get_shader(ID3D11Device* i_pDevice, fxStage i_Stage, const fxStageDesc& i_Desc,
		const fxBytecodeSource& i_Bytecode, std::string& o_Error);
	bool reflect(ID3D11Device* i_pDevice, Shader& io_Shader, std::string& o_Error);
	int get_sampler(ID3D11Device* i_pDevice, const std::string& i_Name, std::string& o_Error);
	void apply_defaults();

	fxEffectDesc m_Desc;
	std::vector<std::vector<Pipeline>> m_Pipelines;		// [technique][pass]
	std::map<std::string, std::unique_ptr<Shader>> m_Shaders;	// by entry|profile
	std::vector<ConstantBuffer> m_ConstantBuffers;
	std::map<std::string, Variable> m_Variables;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_Resources;
	std::map<std::string, int> m_ResourceNames;
	std::vector<Microsoft::WRL::ComPtr<ID3D11SamplerState>> m_Samplers;
	std::map<std::string, int> m_SamplerNames;
};
