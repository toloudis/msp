/*****************************************************************************
**  lyerLayerInterest.hpp
**
**	Callback for when Layer data changes
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef LYER_LAYERINTEREST_HPP
#error lyerLayerInterest.hpp multiply included
#endif
#define LYER_LAYERINTEREST_HPP


//============================================================================
//============================================================================
class lyerLayerInterest
{
	public:
		//--------------------------------------------------------------------
		//	ObjectAdded - an object has been added or removed from the manager
		//--------------------------------------------------------------------
		virtual void ObjectAdded() = 0;

		//--------------------------------------------------------------------
		//	ObjectRenamed - an object has been renamed
		//--------------------------------------------------------------------
		virtual void ObjectRenamed() = 0;

		//--------------------------------------------------------------------
		//	DataChanged - objects have been added/removed from layers
		//--------------------------------------------------------------------
		virtual void DataChanged() = 0;
};
