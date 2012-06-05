
#include "Support/dyn/dynControlData.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
dynControlData::dynControlData()
: m_Name("Name"), 
	m_Node("Node"), 
	m_RotateX("Rotate X", 0), 
	m_RotateY("Rotate Y", 0), 
	m_RotateZ("Rotate Z", 0), 
	m_Translation("Translate", maVector3d(0,0,0)), 
	m_Scale("Scale", maVector3d(1,1,1))
{

}
