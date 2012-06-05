
#include "Max.h"
#include "utilapi.h"
#include "MaxInstanceMaker.hpp"


//========================================================================
//	Function publishing mechanism for this plugin
//	
//========================================================================

#define INSTANCEMAKER_FP_INTERFACE_ID Interface_ID(0x58ef357b, 0x45dc6c56)

class InstanceMakerFP : public FPStaticInterface {
	public:
		DECLARE_DESCRIPTOR(InstanceMakerFP);

		enum OpID {
			kCanDo,
			kGetXForm,
			kGetError
			};
		
		BEGIN_FUNCTION_MAP
			FN_2(kCanDo,  TYPE_BOOL, CanExpressAsInstances, TYPE_INODE,  TYPE_INODE)
			FN_2(kGetXForm,   TYPE_MATRIX3_BV, GetTransform, TYPE_INODE, TYPE_INODE)
			FN_2(kGetError,   TYPE_FLOAT, GetError, TYPE_INODE, TYPE_INODE)
		END_FUNCTION_MAP

		
		bool  CanExpressAsInstances( INode *i_pNode1, INode *i_pNode2 );
		Matrix3 GetTransform( INode *i_pNode1, INode *i_pNode2 );
		float GetError( INode *i_pNode1, INode *i_pNode2 );

	};

static InstanceMakerFP theInstanceMakerFP(
	INSTANCEMAKER_FP_INTERFACE_ID, _T("SgpuInstanceMaker"), -1, 
		0, FP_CORE,
	// The first operation, boneCreate:
	InstanceMakerFP::kCanDo, _T("CanExpressAsInstances"), -1, TYPE_BOOL, 0, 2,
		_T("node1"), -1, TYPE_INODE,
		_T("node2"), -1, TYPE_INODE,
	InstanceMakerFP::kGetXForm, _T("GetTransform"), -1, TYPE_MATRIX3_BV, 0, 2,
		_T("node1"), -1, TYPE_INODE,
		_T("node2"), -1, TYPE_INODE,
	InstanceMakerFP::kGetError, _T("GetError"), -1, TYPE_FLOAT, 0, 2,
		_T("node1"), -1, TYPE_INODE,
		_T("node2"), -1, TYPE_INODE,
	end);

bool InstanceMakerFP::CanExpressAsInstances( INode *i_pNode1, INode *i_pNode2 )
{
	return SgpuInstanceMaker::theSgpuInstanceMaker.CanExpressAsInstances( i_pNode1, i_pNode2 );
}

Matrix3 InstanceMakerFP::GetTransform( INode *i_pNode1, INode *i_pNode2 )
{
	return SgpuInstanceMaker::theSgpuInstanceMaker.GetTransform( i_pNode1, i_pNode2 );
}

float InstanceMakerFP::GetError( INode *i_pNode1, INode *i_pNode2 )
{
	return SgpuInstanceMaker::theSgpuInstanceMaker.GetError( i_pNode1, i_pNode2 );
}
