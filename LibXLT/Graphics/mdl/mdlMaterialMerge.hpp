/****************************************************************************\
**	mdlMaterialMerge.hpp
**
**		mdlMaterialMerge supplies functions for manipulating between
**	our different fragment data structures.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MDL_MATERIALMERGE_HPP
#error mdlMaterialMerge.hpp multiply included
#endif
#define MDL_MATERIALMERGE_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#include <map>
#include <vector>


//============================================================================
//============================================================================
class maMatrix4x4;
class matMaterial;
class mdlFragInfo;
struct mdlMatInfo;
struct mdlSplitFragInfo;


//============================================================================
//============================================================================
const int cDefaultNumMaxPrimitivesInMergedFrag = 30000;


//============================================================================
//	A utility class for merging mdlFragInfo instances based on materials
//============================================================================
class mdlMaterialMerge
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	mdlMaterialMerge(const std::string & i_Prefix, 
					 int cNumMaxPrimitivesInMergedFrag=cDefaultNumMaxPrimitivesInMergedFrag )
	:	m_PrefixName( i_Prefix ),
		m_cNumMaxPrimitivesInMergedFrag( cNumMaxPrimitivesInMergedFrag ),
		m_NumMeshesSubjectedToMergeSoFar(0)
	{};

	//----------------------------------------------------------------------------
	//	merges each part of the input mdlFragInfo-s to a target mdlFragInfo
	//	determined by the material used by the part.
	//	i_FragToBeMerged = the mdlFragInfo that need be merged acording to materials
	//	i_pTransformToBeAppliedBeforeMerging = The transformation that need be applied 
	//											to the argument mdlFragInfo before merging
	//											This can be NULL.
	//----------------------------------------------------------------------------
	void DoMerge( shared_ptr< mdlFragInfo > &i_FragsToBeMerged , const maMatrix4x4  *i_pTransformsToBeAppliedBeforeMerging );

	//----------------------------------------------------------------------------
	//	Transfer all the freshly created and merged mdlFragInfos to the output result sequence
	//	Also, calculate the basis vectors for each of the merged mdlFragInfo-s.
	//	Also clear the m_MtlFragMap
	//----------------------------------------------------------------------------
	void DepleteResult( std::vector< shared_ptr< mdlFragInfo > > &io_Result );

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	int GetNumMaxPrimitivesInMergedMesh() const
	{
		return 	m_cNumMaxPrimitivesInMergedFrag;	
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	int GetNumMeshesSubjectedToMergeSoFar() const
	{
		return m_NumMeshesSubjectedToMergeSoFar;
	}

private:
	
	//----------------------------------------------------------------------------
	//	The lower level merge routines expect basis vectors to be already calculated
	//	for the incoming input frags. These basis vectors are combined with the already
	//	calculated basis vectors for the resultant meshes. However, some input meshes 
	//	do not have basis vectors, because they dont have u-v co-ordinates to calculate
	//	them. Such meshes should be merged separately. So instead of strictly merging by
	//	materials,we merge by the pair (material, has_basis_vectors).
	//----------------------------------------------------------------------------
	struct TMtlFragMapKey
	{
		struct Less
		{
			bool operator()( const TMtlFragMapKey &i_First, const TMtlFragMapKey &i_Second ) const
			{
				if( i_First.m_MatInfo == i_Second.m_MatInfo )
				{
					return ( static_cast<int> ( i_First.m_HasBasisVector ) < static_cast< int > ( i_Second.m_HasBasisVector ) );
				} else if( i_First.m_MatInfo.get() > i_Second.m_MatInfo.get() )
				{
					return false;
				}
				else
				{
					return  true;
				}
			}
		};
		shared_ptr< mdlMatInfo > m_MatInfo;
		bool					m_HasBasisVector;
	};

	//----------------------------------------------------------------------------
	//	Map  where we accumulate all the merged result
	//	There is one merged mdlFragInfo for each material.
	//----------------------------------------------------------------------------
	typedef std::map< TMtlFragMapKey, shared_ptr< mdlFragInfo >, TMtlFragMapKey::Less > TMtlFragMap;
	TMtlFragMap m_MtlFragMap;

	//	The prefix name is prefix-ed to all merged fragments created.
	//	The name of a merged fragment will be (prefixName + materialName)
	std::string m_PrefixName;

	//	When a merged fragmnent grows beyond a limit ( number of primitives  > m_cNumMaxPrimitivesInMergedFrag )
	//	it is transferred from m_MtlFragMap to m_SaturatedMergedFragments
	//	Note that all the merged fragments corresponding to a single material
	//	will have the same name
	std::vector< shared_ptr< mdlFragInfo > > m_SaturatedMergedFragments;

	//	Maximum number of prmitives allowed in a merged fragment.
	//	To be supplied and initizialised by the constructor
	 const int m_cNumMaxPrimitivesInMergedFrag;

	//	Number of input meshes so far merged.
	//	This is an output variable.
	int m_NumMeshesSubjectedToMergeSoFar;

private:
	//----------------------------------------------------------------------------
	//	Retreive the merged FragInfo corresponding to the input material
	//	from m_MtlFragMap. If no such frag exist in the map, create a new one in
	//	the map. Otherwise if the frag retreived from the map is too big,
	//	transfer it to the saturated frags list and create an empty frag in
	//	the map.
	//	The lower level merge routines expect basis vectors to be already calculated
	//	for the incoming input frags. These basis vectors are combined with the already
	//	calculated basis vectors for the resultant meshes. However, some input meshes 
	//	do not have basis vectors, because they dont have u-v co-ordinates to calculate
	//	them. Such meshes should be merged separately. So instead of strictly merging by
	//	materials,we merge by the pair (material, has_basis_vectors).
	//----------------------------------------------------------------------------
	shared_ptr< mdlFragInfo > & GetMergedFragInfoCorrespondingToMtl(  TMtlFragMapKey &i_Key ); 
};