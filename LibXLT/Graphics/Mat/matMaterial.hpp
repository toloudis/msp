/*****************************************************************************
**	matMaterial.hpp
**
**		The matMaterial represents all intensive visual properties of a
**	surface.  It includes information about the ambient, diffuse, and
**	specular colors, and the texture maps applied to the surface.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef MAT_MATERIAL_HPP
#error matMaterial.hpp multiply included
#endif
#define MAT_MATERIAL_HPP

#ifndef MAT_MATERIALLAYER_HPP
#include "Graphics/Mat/matMaterialLayer.hpp"
#endif
#ifndef EFF_GLOWDATA_HPP
#include "Graphics/eff/effGlowData.hpp"
#endif
#ifndef EFF_OUTLINEDATA_HPP
#include "Graphics/eff/effOutlineData.hpp"
#endif
#ifndef EFF_REFLDATA_HPP
#include "Graphics/eff/effReflData.hpp"
#endif
#ifndef EFF_DISPLACEMENTDATA_HPP
#include "Graphics/eff/effDisplacementData.hpp"
#endif
#ifndef EFF_NORMALSDATA_HPP
#include "Graphics/eff/effNormalsData.hpp"
#endif
#ifndef EFF_RENDERMANOVERRIDEDATA_HPP
#include "Graphics/eff/effRendermanOverrideData.hpp"
#endif
#ifndef EFF_STRANDHAIRDATA_HPP
#include "Graphics/eff/effStrandHairData.hpp"
#endif
#ifndef EFF_TEXTUREFILTERDATA_HPP
#include "Graphics/eff/effTextureFilterData.hpp"
#endif
//============================================================================
//============================================================================
class matMatAnim;


//============================================================================
//============================================================================
class matMaterial
{
	public:
		//--------------------------------------------------------------------
		//	Default constructor
		//--------------------------------------------------------------------
		matMaterial();

		//--------------------------------------------------------------------
		//	convenience constructor that will allow you to set the initial color
		//	values
		//--------------------------------------------------------------------
		matMaterial(const maFloatRGBA& i_Diffuse,
					const maFloatRGBA& i_Ambient,
					const maFloatRGBA& i_Emissive);

		//--------------------------------------------------------------------
		//	convenience constructor that will allow you to set the shader
		//  and its data
		//--------------------------------------------------------------------
		matMaterial(const std::string& i_EffectID,
					effShaderData* i_ShaderData = NULL);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		matMaterial(const fsLocator& i_EffectID);

		//--------------------------------------------------------------------
		//	The copy constructor does copy the material animations, if
		//	any.
		//--------------------------------------------------------------------
		matMaterial(const matMaterial& i_CopyFrom);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~matMaterial();

		//--------------------------------------------------------------------
		//	The operator = does copy the material animations, if any.
		//--------------------------------------------------------------------
		matMaterial& operator = (const matMaterial& i_CopyFrom);

		//--------------------------------------------------------------------
		//	SimpleCopy() copies values from given material, but
		//	does not copy the material animations, if any.
		//--------------------------------------------------------------------
		void SimpleCopy(const matMaterial& i_CopyFrom);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		bool operator == (const matMaterial& i_Material) const;

		//--------------------------------------------------------------------
		// Name of the material
		//--------------------------------------------------------------------
		std::string GetName() const {return m_Name;}
		void SetName(std::string name) {m_Name = name;}

		//--------------------------------------------------------------------
		// Access to material layers
		//--------------------------------------------------------------------
		inline int GetNumMaterialLayers() const;
		const shared_ptr<matMaterialLayer> GetMaterialLayer(int i_Index) const;

		//--------------------------------------------------------------------
		// Add an extra material layer with the given material parameters
		//--------------------------------------------------------------------
		void AddMaterialLayer();
		//bga - can we have a function like this?...
		//void AddMaterialLayer(shared_ptr<effShaderParams> i_Params); 

		//--------------------------------------------------------------------
		// Remove all material layers except the base material layer
		//--------------------------------------------------------------------
		void RemoveExtraMaterialLayers();

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
		//	The paint overlay will allow the material to be painted
		//--------------------------------------------------------------------
		void SetPaintOverlay(matTexture* i_PaintOverlay);
		matTexture* GetPaintOverlay() const;

		//--------------------------------------------------------------------
		//	ShaderEffect controls algorithm for rendering with
		//		multiple passes
		//--------------------------------------------------------------------
		virtual void SetShaderEffect(const std::string& i_EffectID, const effShaderData* i_Data = NULL) const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		std::string GetEffectID() const;

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
		//	GetMatAnimList returns the list of material animations used by
		//	the material.  The user is free to manipulate this list as he
		//	wishes, but as a convenience, the material deletes the items in
		//	the list when it is destroyed.
		//
		//  The non-const version has been deprecated.
		//  Use AddMatAnim() and DestroyAllMatAnims().
		//--------------------------------------------------------------------
		const std::list<matMatAnim*>& GetMatAnimList() const;
		//std::list<matMatAnim*>& GetMatAnimList();

		//--------------------------------------------------------------------
		// ModifyMatAnim will return a modifiable material anim, the given
		// index must be in the mat anim list or this will assert
		//--------------------------------------------------------------------
		matMatAnim* ModifyMatAnim(int i_Index);

		//--------------------------------------------------------------------
		//	AddMatAnim adds a material animation to the list
		//--------------------------------------------------------------------
		void AddMatAnim( matMatAnim* i_MatAnim );

		//--------------------------------------------------------------------
		//	DestroyAllMatAnims deletes all of the material animations
		//--------------------------------------------------------------------
		void DestroyAllMatAnims();

		//--------------------------------------------------------------------
		//	Activate/DeactivateMatAnim - sets/unsets active flag
		//	on material animation with given index.  If StartTime>0
		//	then the start time of the material animation is reset.
		//  Use -1 as Index to affect all animations.
		//--------------------------------------------------------------------
		void ActivateMatAnim( int i_Index, float i_StartTime = 0 );
		void DeactivateMatAnim( int i_Index );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		effShaderData* GetEffectData(int i_Layer = 0) const;

		//--------------------------------------------------------------------
		// Use of this function implies user wants the base material layer
		//--------------------------------------------------------------------
		template<class D> const D* GetTypedData() const 
		{
			return (m_MaterialLayers[0]->GetTypedData<D>());
		}
		template<class D> D* TypedData() 
		{
			return (m_MaterialLayers[0]->TypedData<D>());
		}

		//--------------------------------------------------------------------
		// Shader Parameters - properties specific to the shader technique
		//--------------------------------------------------------------------
		shared_ptr<effShaderParams> GetShaderParams(int i_Layer = 0) const;
		void SetShaderParams(shared_ptr<effShaderParams> i_Params);
		void SetShaderParams(int i_Layer, shared_ptr<effShaderParams> i_Params);

		//--------------------------------------------------------------------
		// Glow
		//--------------------------------------------------------------------
		bool GetHasGlow() const;
		void SetHasGlow(bool i_val);
		effGlowData& GlowData() const {return m_GlowData;}
		const effGlowData& GetGlowData() const {return m_GlowData;}

		//--------------------------------------------------------------------
		// Outline
		//--------------------------------------------------------------------
		bool GetHasOutline() const;
		void SetHasOutline(bool i_val);
		effOutlineData& OutlineData() const {return m_OutlineData;}
		const effOutlineData& GetOutlineData() const {return m_OutlineData;}
		
		//--------------------------------------------------------------------
		// Reflection
		//--------------------------------------------------------------------
		bool GetHasReflection() const;
		void SetHasReflection(bool i_val);
		effReflectionMap& ReflectionData() const {return m_ReflectionData;}
		const effReflectionMap& GetReflectionData() const {return m_ReflectionData;}

		//--------------------------------------------------------------------
		// Displacement
		//--------------------------------------------------------------------
		bool GetHasDisplacement() const;
		void SetHasDisplacement(bool i_val);
		effDisplacementData& DisplacementData() const {return m_DisplacementData;}
		const effDisplacementData& GetDisplacementData() const {return m_DisplacementData;}

		//--------------------------------------------------------------------
		// UVTransform
		//--------------------------------------------------------------------
		effUVTransform& UVTransform(int i_Layer = 0) const;
		const effUVTransform& GetUVTransform(int i_Layer = 0) const;

		//--------------------------------------------------------------------
		// TextureFilter
		//--------------------------------------------------------------------
		effTextureFilterData& TextureFilter(int i_Layer = 0) {return m_TextureFilterData;}
		const effTextureFilterData& GetTextureFilter(int i_Layer = 0) {return m_TextureFilterData;}

		//--------------------------------------------------------------------
		// Normals
		//--------------------------------------------------------------------
		effNormalsData& NormalsData() const {return m_NormalsData;}
		const effNormalsData& GetNormalsData() const {return m_NormalsData;}

		//--------------------------------------------------------------------
		// RendermanOverride
		//--------------------------------------------------------------------
		bool GetHasRendermanOverride() const;
		void SetHasRendermanOverride(bool i_val);
		effRendermanOverrideData& RendermanOverrideData() const {return m_RendermanOverrideData;}
		const effRendermanOverrideData& GetRendermanOverrideData() const {return m_RendermanOverrideData;}

		void SetSolidColor(maVector3d i_Color);
		maVector3d GetSolidColor();

		//--------------------------------------------------------------------
		// Strand Hair
		//--------------------------------------------------------------------
		bool GetHasStrandHair() const;
		void SetHasStrandHair(bool i_val);
		effStrandHairData& StrandHairData() const {return m_StrandHairData;}
		const effStrandHairData& GetStrandHairData() const {return m_StrandHairData;}

		//--------------------------------------------------------------------
		// Keep track if this material is for light shaft
		//--------------------------------------------------------------------
		void SetBelongsToLightShaft(bool i_bBelongsToLightShaft);
		bool GetBelongsToLightShaft() const;

	
	private:
		std::vector< shared_ptr<matMaterialLayer> > m_MaterialLayers;

		matTexture* m_PaintOverlay;

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

		std::list<matMatAnim*> m_MatAnims;
		
		// is there an additional glow effect?
		bool m_bHasGlow;
		mutable effGlowData m_GlowData;
		bool m_bBelongsToLightShaft;

		// is there an additional outline effect?
		bool m_bHasOutline;
		mutable effOutlineData m_OutlineData;

		// is there an additional reflection effect?
		bool m_bHasReflection;
		mutable effReflectionMap m_ReflectionData;

		// is there an additional displacement effect?
		bool m_bHasDisplacement;
		mutable effDisplacementData m_DisplacementData;

		// is there an additional normal map effect?
		mutable effNormalsData m_NormalsData;

		// is there an additional texture filter effect?
		mutable effTextureFilterData m_TextureFilterData;

		// is there an additional StrandHair effect?
		bool m_bHasStrandHair;
		mutable effStrandHairData m_StrandHairData;

		// is there an additional Renderman Override effect?
		bool m_bHasRendermanOverride;
		mutable effRendermanOverrideData m_RendermanOverrideData;

		std::string m_Name;

		maVector3d m_SolidColor;

};

//----------------------------------------------------------------------------
//	some matMaterial implementation
//----------------------------------------------------------------------------

//--------------------------------------------------------------------
// Access to material layers
//--------------------------------------------------------------------
inline int matMaterial::GetNumMaterialLayers() const
{
	return m_MaterialLayers.size();
}

//--------------------------------------------------------------------
//	GetMatAnimList returns the list of material animations used by
//	the material.  The user is free to manipulate this list as he
//	wishes, but as a convenience, the material deletes the items in
//	the list when it is destroyed.
//--------------------------------------------------------------------
//inline std::list<matMatAnim*>& matMaterial::GetMatAnimList()
//{
//	DBG_MESSAGE0( "This function has been deprecated."
//				  "Use the function AddMatAnim() and DestroyAllMatAnims()." );
//	return m_MatAnims;
//}

inline const std::list<matMatAnim*>& matMaterial::GetMatAnimList() const
{
	return m_MatAnims;
}

//--------------------------------------------------------------------
//	GetAdditive()
//--------------------------------------------------------------------
inline bool matMaterial::GetAdditive() const
{
	return m_Flags.m_bAdditive;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline bool matMaterial::GetHasReflection() const
{
	return m_bHasReflection;
}

inline void matMaterial::SetHasReflection(bool i_val)
{
	m_bHasReflection = i_val;
}

inline bool matMaterial::GetHasGlow() const
{
	return m_bHasGlow;
}

inline void matMaterial::SetHasGlow(bool i_val)
{
	m_bHasGlow = i_val;
}

inline bool matMaterial::GetHasOutline() const
{
	return m_bHasOutline;
}

inline void matMaterial::SetHasOutline(bool i_val)
{
	m_bHasOutline = i_val;
}

inline bool matMaterial::GetHasDisplacement() const
{
	return m_bHasDisplacement;
}

inline void matMaterial::SetHasDisplacement(bool i_val)
{
	m_bHasDisplacement = i_val;
}

inline bool matMaterial::GetHasRendermanOverride() const
{
	return m_bHasRendermanOverride;
}

inline void matMaterial::SetHasRendermanOverride(bool i_val)
{
	m_bHasRendermanOverride = i_val;
}

inline bool matMaterial::GetHasStrandHair() const
{
	return m_bHasStrandHair;
}

inline void matMaterial::SetHasStrandHair(bool i_val)
{
	m_bHasStrandHair = i_val;
}
