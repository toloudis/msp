#include "Area18/test/testCameraScenePrty.hpp"

#include "Core/prty/prtyColorRGBEditUIInfo.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"

testCameraScenePrty::testCameraScenePrty(void)
:	m_Color("Color", maFloatRGBA(1,0,0,1))
{
	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;

	// Model animation properties
	pPUII  = new prtyColorRGBEditUIInfo(&m_Color, "Color", "Color");
	AddProperty( pPUII );	
	
}

testCameraScenePrty::~testCameraScenePrty(void)
{
}
