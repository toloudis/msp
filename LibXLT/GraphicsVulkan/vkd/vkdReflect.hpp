/*****************************************************************************
**  vkdReflect.hpp
**
**      vkdReflect reads what one SPIR-V shader (compiled by DXC with
**	GraphicsVulkan's options, see Fx/private/EmbedShadersVk.ps1) uses, and
**	describes it the way D3D11 shader reflection does, so code ported from
**	GraphicsDX11 sees the same constant buffers, variables, types and
**	register slots:
**
**	- Register slots: DXC's -fvk-[btsu]-shift put b registers at bindings
**	  0.., t at 100.., s at 200.., u at 300..; the slot is the binding minus
**	  the shift.
**	- Types use D3D_SHADER_VARIABLE_CLASS / _TYPE numbering. DXC turns an
**	  HLSL RxC matrix into a SPIR-V matrix of R columns of C components and
**	  decorates an HLSL column_major matrix RowMajor, so both are swapped
**	  back here.
**	- Sizes are D3D's: a constant buffer is rounded up to 16 bytes; an array
**	  variable is (n-1) * stride + the size of one element.
**	- A bool in a constant buffer is a uint in SPIR-V; callers that know
**	  the declared type (the effect manifest) correct it.
**
**	Pure CPU code: needs no device.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDREFLECT_HPP
#error vkdReflect.hpp multiply included
#endif
#define GVK_VKDREFLECT_HPP

#include <volk.h>

#include <cstdint>
#include <string>
#include <vector>

namespace gvk
{
	//	D3D_SHADER_VARIABLE_CLASS and D3D_SHADER_VARIABLE_TYPE values
	enum fxVarClass { fx_SVC_SCALAR = 0, fx_SVC_VECTOR, fx_SVC_MATRIX_ROWS, fx_SVC_MATRIX_COLUMNS, fx_SVC_OBJECT, fx_SVC_STRUCT };
	enum fxVarType { fx_SVT_VOID = 0, fx_SVT_BOOL = 1, fx_SVT_INT = 2, fx_SVT_FLOAT = 3, fx_SVT_UINT = 19 };

	//	The shape of a constant, as D3D11 reflection reports it
	struct vkdShaderType
	{
		struct Member;
		std::string	m_Name;			// e.g. "float4", or the struct's name
		int			m_Class = 0;	// fxVarClass
		int			m_Type = 0;		// fxVarType
		uint32_t	m_Rows = 0;
		uint32_t	m_Columns = 0;
		uint32_t	m_Elements = 0;	// 0 if not an array
		std::vector<Member> m_Members;

		//	Bytes one element takes in a constant buffer, before padding to
		//	the next 16-byte register; array elements are 16-byte aligned.
		uint32_t GetElementSize() const;
	};
	struct vkdShaderType::Member
	{
		std::string		m_Name;
		uint32_t		m_Offset = 0;	// from the start of the struct
		vkdShaderType	m_Type;
	};

	struct vkdShaderVariable		// one variable of a constant buffer
	{
		std::string		m_Name;
		uint32_t		m_Offset = 0;
		uint32_t		m_Size = 0;
		bool			m_bColumnMajor = false;
		vkdShaderType	m_Type;
	};

	struct vkdShaderBuffer			// a constant buffer (b register)
	{
		std::string		m_Name;		// the cbuffer's name in the HLSL
		uint32_t		m_Slot = 0;
		uint32_t		m_Size = 0;	// rounded up to 16
		std::vector<vkdShaderVariable> m_Variables;
	};

	struct vkdShaderResource		// a t, s or u register
	{
		enum Kind { e_Texture, e_Sampler, e_Uav };
		Kind		m_Kind = e_Texture;
		std::string	m_Name;
		uint32_t	m_Slot = 0;
		uint32_t	m_Count = 1;	// > 1 for an array of resources
		int			m_Dim = -1;		// SpvDim of an image (1D, 2D, 3D, Cube, Buffer), -1 for buffers and samplers
		bool		m_bArrayed = false;
		bool		m_bMultisampled = false;
		bool		m_bBuffer = false;	// structured/byte-address/typed buffer
	};

	struct vkdShaderInput			// a vertex shader input with a location
	{
		std::string	m_Semantic;		// e.g. "TEXCOORD" (from DXC's in.var.TEXCOORD1)
		uint32_t	m_Index = 0;	// e.g. 1
		uint32_t	m_Location = 0;
		VkFormat	m_Format = VK_FORMAT_UNDEFINED;
	};

	struct vkdShaderReflection
	{
		VkShaderStageFlagBits			m_Stage = VK_SHADER_STAGE_VERTEX_BIT;
		std::vector<vkdShaderBuffer>	m_ConstantBuffers;
		std::vector<vkdShaderResource>	m_Resources;
		std::vector<vkdShaderInput>		m_Inputs;			// vertex shaders only
		uint32_t						m_ColorOutputs = 0;	// pixel shaders: highest SV_Target + 1
		std::vector<uint32_t>			m_Capabilities;		// SpvCapability values
	};

	//	false, with o_Error, if i_pSpirv isn't a SPIR-V module this can read
	//	(or uses a binding outside the DXC shift ranges).
	bool vkdReflect(const void* i_pSpirv, size_t i_Size, vkdShaderReflection& o_Reflection, std::string& o_Error);
}
