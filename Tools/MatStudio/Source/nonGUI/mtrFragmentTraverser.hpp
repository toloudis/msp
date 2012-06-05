/*****************************************************************************
**  mtrFragmentTraverser.hpp
**
**      mtrFragmentTraverser applies operation to each fragment
**  in scObject.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MTR_FRAGMENTTRAVERSER_HPP
#error mtrFragmentTraverser.hpp multiply included
#endif
#define MTR_FRAGMENTTRAVERSER_HPP

class g3dFragment;
class maMatrix4x4;
class scObject;


class mtFragmentOp
{
public:
	virtual void Apply(const maMatrix4x4& i_Matrix,	g3dFragment* i_pFragment) = 0;
				  
};

namespace mtrFragmentTraverser 
{

	//========================================================================
	// TraverseFragments()
	//========================================================================
	void TraverseFragments( scObject* i_Object,
						mtFragmentOp& i_FragmentOp,
						const maMatrix4x4& i_Matx);

};

