/*****************************************************************************
**  mcpDirectController.hpp
**
**      Implements a connection from motion capture data to scene node
**	hierarchies.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef MCP_DIRECTCONTROLLER_HPP
#error mcpDirectController.hpp multiply included
#endif
#define MCP_DIRECTCONTROLLER_HPP

#include <vector>

//============================================================================
//============================================================================
class g3dSceneNode;
class mcpHTRSegmentData;


//============================================================================
//	mcpDirectController
//============================================================================
class mcpDirectController
{
	public:
		//--------------------------------------------------------------------
		//	There is a hierarchy defined by the HTR SegmentData. This
		//	constructor tries to match up the hierarchy with the 
		//	scene graph hierarchy rooted at i_pRootNode.
		//--------------------------------------------------------------------
		mcpDirectController(const std::vector<mcpHTRSegmentData> &i_Segments,
							g3dSceneNode *i_pRootNode);

		//--------------------------------------------------------------------
		// Set transforms of scene nodes to match the transforms in the
		//	segment data.
		//--------------------------------------------------------------------
		bool Update(const std::vector<mcpHTRSegmentData> &i_Segments);
		
		//--------------------------------------------------------------------
		// Check to make see how many segments have an connected node
		//--------------------------------------------------------------------
		int GetNumGoodConnections() const;

	private:
		std::vector<g3dSceneNode*>	m_Connections;
};

