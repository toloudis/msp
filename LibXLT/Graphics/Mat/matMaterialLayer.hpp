/*****************************************************************************
**  matMaterialLayer.hpp
**
**      The matMaterialLayer represents one layer of a multi-layered 
**	material. This corresponds to a single effects shader type and
**	so controls for how to blend this layer with the other layers.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef MAT_MATERIALLAYER_HPP
#error matMaterialLayer.hpp multiply included
#endif
#define MAT_MATERIALLAYER_HPP


#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/eff/effShaderParams.hpp"
#endif


class matMaterialLayer
{
	public:
		//--------------------------------------------------------------------
		//	Default constructor
		//--------------------------------------------------------------------
		matMaterialLayer();

		//--------------------------------------------------------------------
		//	convenience constructor that will allow you to set the initial color
		//	values
		//--------------------------------------------------------------------
		matMaterialLayer(const maFloatRGBA& i_Diffuse,
					const maFloatRGBA& i_Ambient,
					const maFloatRGBA& i_Emissive);

		//--------------------------------------------------------------------
		//	convenience constructor that will allow you to set the shader
		//  and its data
		//--------------------------------------------------------------------
		matMaterialLayer(const std::string& i_EffectID,
					effShaderData* i_ShaderData = NULL);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		matMaterialLayer(const fsLocator& i_EffectID);

		//--------------------------------------------------------------------
		//	The copy constructor does copy the material animations, if
		//	any.
		//--------------------------------------------------------------------
		matMaterialLayer(const matMaterialLayer& i_CopyFrom);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~matMaterialLayer();

		//--------------------------------------------------------------------
		//	The operator = does copy the material animations, if any.
		//--------------------------------------------------------------------
		matMaterialLayer& operator = (const matMaterialLayer& i_CopyFrom);

		//--------------------------------------------------------------------
		//	SimpleCopy() copies values from given material, but
		//	does not copy the material animations, if any.
		//--------------------------------------------------------------------
		void SimpleCopy(const matMaterialLayer& i_CopyFrom);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		bool operator == (const matMaterialLayer& i_Material) const;

		//--------------------------------------------------------------------
		//	GetHasSpecular returns true if the material should be rendered
		//	with a specular highlight.
		//--------------------------------------------------------------------
		bool GetHasSpecular() const;

		//--------------------------------------------------------------------
		//	SetHasSpecular allows the user to specify if the material should
		//	be rendered with a specular highlight.
		//--------------------------------------------------------------------
		void SetHasSpecular(bool i_Specular);

		//--------------------------------------------------------------------
		//	ShaderEffect controls algorithm for rendering with
		//		multiple passes
		//--------------------------------------------------------------------
		virtual void SetShaderEffect(const std::string& i_EffectID, const effShaderData* i_Data = NULL) const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline std::string GetEffectID() const;

		//--------------------------------------------------------------------
		//	RemoveTextures removes all textures
		//--------------------------------------------------------------------
		void RemoveTextures();

		//--------------------------------------------------------------------
		//	GetHasTransparency returns true if the material
		//	might be translucent or transparent (this is important for
		//	sorting things).
		//--------------------------------------------------------------------
		bool GetHasTransparency() const;

		//--------------------------------------------------------------------
		//	ForceTransparency forces the material to be considered
		//	transparent.  If it is false then it is tested for transparency
		//	using the normal methods.
		//--------------------------------------------------------------------
		void ForceTransparency(bool i_bTransparent);

		//----------------------------------------------------------------------------
		//	SetAdditive turns on additive mode rendering.
		//----------------------------------------------------------------------------
		void SetAdditive( bool i_bAdditive );
		inline bool GetAdditive() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		effShaderData* GetEffectData() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		template<class D> const D* GetTypedData() const {return dynamic_cast<const D*>(this->m_pShaderData);}
		template<class D> D* TypedData() {return dynamic_cast<D*>(this->m_pShaderData);}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		shared_ptr<effShaderParams> GetShaderParams() const;
		void SetShaderParams(shared_ptr<effShaderParams> i_Params);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		effUVTransform& UVTransform() const {return m_UVTransform;}
		const effUVTransform& GetUVTransform() const {return m_UVTransform;}

	protected:
		mutable std::string m_EffectID;
		mutable effShaderData* m_pShaderData;
		
		shared_ptr<effShaderParams> m_pShaderParams;
	
	private:
		struct
		{
			bool m_bTransparency		: 1;
			bool m_bHasSpecular			: 1;
			bool m_bForceTransparent	: 1;
			bool m_bAdditive			: 1;
		} m_Flags;

		//----------------------------------------------------------------------------
		//	test_transparency inspects all of the texture and color information
		//	and decides if the material could be transparent.
		//----------------------------------------------------------------------------
		void test_transparency();

		mutable effUVTransform m_UVTransform;
};

//----------------------------------------------------------------------------
//	some matMaterialLayer implementation
//----------------------------------------------------------------------------

//--------------------------------------------------------------------
//	GetHasTransparency returns true if the material
//	might be translucent or transparent (this is important for
//	sorting things).
//--------------------------------------------------------------------
inline bool matMaterialLayer::GetHasTransparency() const
{
	if (m_Flags.m_bTransparency)
		return true;
    if (m_pShaderParams)
	{
		if (m_pShaderParams->HasTransparency())
		{
			return true;
		}
	}
    if (m_pShaderData)
	{
		if (m_pShaderData->HasTransparency())
		{
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------------
//	GetHasSpecular returns true if the material should be rendered
//	with a specular highlight.
//--------------------------------------------------------------------
inline bool matMaterialLayer::GetHasSpecular() const
{
	return m_Flags.m_bHasSpecular;
}

//--------------------------------------------------------------------
//	GetAdditive()
//--------------------------------------------------------------------
inline bool matMaterialLayer::GetAdditive() const
{
	return m_Flags.m_bAdditive;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline std::string matMaterialLayer::GetEffectID() const
{
	return m_EffectID;
}
