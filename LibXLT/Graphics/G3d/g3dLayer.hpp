/****************************************************************************\
**	g3dLayer.hpp
**
**		A g3dLayer is a definition of a rendering layer used by the
**	g3dRenderer.  It contains rendering info for the layer and
**	a root g3dSceneNode that contains the scene graph for the layer.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#pragma once
#ifdef G3D_LAYER_HPP
#error g3dLayer.hpp already included
#endif
#define G3D_LAYER_HPP


//============================================================================
//	Forward References
//============================================================================
class g3dSceneNode;


//============================================================================
//============================================================================
class g3dLayer
{
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		enum SortMethod
		{
			e_ZBuffer,		//	use Z buffer
			e_ZSort			//	sort and draw back to front using Z translation value
		};

		enum ModelSpace
		{
			e_Screen,		//	models will be drawn in screen space
			e_Camera,		//	models will be drawn in camera space
			e_World			//	models will be drawn in world space
		};

		enum BlendMethod
		{
			e_Multiplicative,
			e_Additive
		};

		//----------------------------------------------------------------------------
		// The root node is not owned by the layer, just pointed to
		//----------------------------------------------------------------------------
		g3dLayer();
		g3dLayer(g3dSceneNode* i_Node);
		g3dLayer(g3dSceneNode* i_Node, SortMethod i_SortMethod, ModelSpace i_ModelSpace,
					 BlendMethod i_BlendMethod = e_Multiplicative,
					 bool i_bFogEnabled = true, bool i_bShadows = false,
					 bool i_bPreLit = false, bool i_bClearDepth = false);

		//----------------------------------------------------------------------------
		// The root node is not owned by the layer, just pointed to
		//----------------------------------------------------------------------------
		inline g3dSceneNode* GetRootNode() const;
		inline void SetRootNode(g3dSceneNode* i_Node);

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		inline SortMethod GetSortMethod() const;
		inline void SetSortMethod(SortMethod i_Method);

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		inline ModelSpace GetModelSpace() const;
		inline void SetModelSpace(ModelSpace i_Space);

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		inline bool GetFogEnabled() const;
		inline void SetFogEnabled(bool i_bFogEnabled);

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		inline bool GetPreLit() const;
		inline void SetPreLit(bool i_bPreLit);

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		inline bool GetShadowRender() const;
		inline void SetShadowRender(bool i_bShadows);

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		inline BlendMethod GetBlendMethod() const;
		inline void SetBlendMethod(BlendMethod i_Method);

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		inline bool GetClearDepth() const;
		inline void SetClearDepth(bool i_bClearDepth);

	private:

		g3dSceneNode*	m_pRootNode;

		SortMethod		m_SortMethod;
		ModelSpace		m_ModelSpace;
		BlendMethod		m_BlendMethod;
		bool			m_bFogEnabled;
		bool			m_bPreLit;
		bool			m_bShadows;
		bool			m_bClearDepth;
};


//----------------------------------------------------------------------------
// The root node is not owned by the layer, just pointed to
//----------------------------------------------------------------------------
inline g3dSceneNode* g3dLayer::GetRootNode() const
{
	return m_pRootNode;
}
inline void g3dLayer::SetRootNode(g3dSceneNode* i_Node)
{
	m_pRootNode = i_Node;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline g3dLayer::SortMethod g3dLayer::GetSortMethod() const
{
	return m_SortMethod;
}

inline void g3dLayer::SetSortMethod(SortMethod i_Method)
{
	m_SortMethod = i_Method;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline g3dLayer::ModelSpace g3dLayer::GetModelSpace() const
{
	return m_ModelSpace;
}

inline void g3dLayer::SetModelSpace(ModelSpace i_Space)
{
	m_ModelSpace = i_Space;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline g3dLayer::BlendMethod g3dLayer::GetBlendMethod() const
{
	return m_BlendMethod;
}

inline void g3dLayer::SetBlendMethod(BlendMethod i_Method)
{
	m_BlendMethod = i_Method;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline bool g3dLayer::GetFogEnabled() const
{
	return m_bFogEnabled;
}

inline void g3dLayer::SetFogEnabled(bool i_bFogEnabled)
{
	m_bFogEnabled = i_bFogEnabled;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline bool g3dLayer::GetPreLit() const
{
	return m_bPreLit;
}

inline void g3dLayer::SetPreLit(bool i_bPreLit)
{
	m_bPreLit = i_bPreLit;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline bool g3dLayer::GetShadowRender() const
{
	return m_bShadows;
}
inline void g3dLayer::SetShadowRender(bool i_bShadows)
{
	m_bShadows = i_bShadows;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inline bool g3dLayer::GetClearDepth() const
{
	return m_bClearDepth;
}
inline void g3dLayer::SetClearDepth(bool i_bClearDepth)
{
	m_bClearDepth = i_bClearDepth;
}
