/*****************************************************************************
**  lyerObject.hpp
**
**      A lyerObject is base class for objects that can be grouped
**	into layers and have active and draw style states set as a group.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef LYER_OBJECT_HPP
#error lyerObject.hpp multiply included
#endif
#define LYER_OBJECT_HPP


//============================================================================
//============================================================================
class lyerObject 
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		lyerObject();

		//--------------------------------------------------------------------
		//	LayerVisible represents if objects in the layer are visible
		//--------------------------------------------------------------------
		virtual void SetLayerVisible(bool i_bVisible);
		bool GetLayerVisible() const;

		//--------------------------------------------------------------------
		//	LayerPickable represents if objects in the layer can be picked.
		//--------------------------------------------------------------------
		virtual void SetLayerPickable(bool i_bPickable);
		bool GetLayerPickable() const;

		//--------------------------------------------------------------------
		//	LayerWireframe represents if the objects are rendered
		//	in a wireframe style.
		//--------------------------------------------------------------------
		virtual void SetLayerWireframe(bool i_bWireframe);
		bool GetLayerWireframe() const;

		//--------------------------------------------------------------------
		//	LayerLowRes represents if the objects are rendered
		//	using a low resolution model.
		//--------------------------------------------------------------------
		virtual void SetLayerLowRes(bool i_bLowRes);
		bool GetLayerLowRes() const;

private:
	
		struct
		{
			bool	m_bLayerVisible : 1;
			bool	m_bLayerPickable : 1;
			bool	m_bLayerWireframe : 1;
			bool	m_bLayerLowRes : 1;
		} m_Flags;
};
