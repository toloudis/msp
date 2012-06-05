/****************************************************************************\
**	mdlMaterialMerge.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/mdlMaterialMerge.hpp"

#include "Core/ma/maConstants.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"

#include <set>


//----------------------------------------------------------------------------
//	Retreive the merged FragInfo corresponding to the input material
//	from m_MtlFragMap. If no such frag exist in the map, create a new one in
//	the map. Otherwise if the frag retreived from the map is too big,
//	transfer it to the saturated frags list and create an empty frag in
//	the map.
//----------------------------------------------------------------------------
shared_ptr< mdlFragInfo > & mdlMaterialMerge::GetMergedFragInfoCorrespondingToMtl( TMtlFragMapKey &i_Key )
{
	shared_ptr< mdlFragInfo > &mergedResult =  m_MtlFragMap[ i_Key ];
	shared_ptr< mdlMatInfo > & mtlInKey = i_Key.m_MatInfo;
	if( mergedResult == NULL )
	{
		mergedResult.reset(new mdlFragInfo );
		mergedResult->m_Materials.push_back( mtlInKey );
		mergedResult->m_Name = m_PrefixName + "_" + mtlInKey->m_Info.GetMaterialName();
	} else if ( mergedResult->m_Indices.size() > 3 * m_cNumMaxPrimitivesInMergedFrag )
	{
		//if the merged mdlFragInfo corresponding to this material has grown beyond a limit
		//transfer it to the m_SaturatedMergedFragments. 
		//Assign a new fragment in m_MtlFragMap for this material
		m_SaturatedMergedFragments.push_back( mergedResult );
		m_MtlFragMap[ i_Key ] = shared_ptr< mdlFragInfo > ( new mdlFragInfo );
		mergedResult = m_MtlFragMap[ i_Key ];
		mergedResult->m_Materials.push_back( mtlInKey );
		mergedResult->m_Name = m_PrefixName + "_" + mtlInKey->m_Info.GetMaterialName();
	}
	return mergedResult;
}

//------------------------------------------------------------------------
//	merges each part of the input mdlFragInfo-s to a target mdlFragInfo
//	determined by the material used by the part.
//------------------------------------------------------------------------
void mdlMaterialMerge::DoMerge( shared_ptr< mdlFragInfo >  &i_ToBeMergedFrag , 
							   const maMatrix4x4 *i_pTransformToBeAppliedBeforeMerging )
{		
	std::string &tobeMergedMeshName = i_ToBeMergedFrag->m_Name;
	std::vector< shared_ptr< mdlMatInfo > > & tobeMergedFragMtls =  i_ToBeMergedFrag->m_Materials;
	std::vector< shared_ptr< mdlMatInfo > >::iterator mit;
	int nMtlChangeBeginIdx =0;
	int nIdxToMtlChangeIdx=-1;
	int nMtlChangeEndIdx = 0;

	bool bToBeMergedFragHasBasisVectors = i_ToBeMergedFrag->HasValidBasisVectors();
	//check the consistency of the ,m_MaterialChanges with m_Materials
	DBG_ASSERT( ( (i_ToBeMergedFrag->m_MaterialChanges.size() + 1) == i_ToBeMergedFrag->m_Materials.size() ), "number of material changes not consistent with materials for mesh " << tobeMergedMeshName.c_str() );
	if ((i_ToBeMergedFrag->m_MaterialChanges.size() + 1) != i_ToBeMergedFrag->m_Materials.size())
		return;
	if( i_ToBeMergedFrag->m_MaterialChanges.size() > 0)
	{
		for( mit = tobeMergedFragMtls.begin(); mit != tobeMergedFragMtls.end(); ++ mit , ++nIdxToMtlChangeIdx )
		{
			//for each material in this mdlFragInfo
			shared_ptr< mdlMatInfo > &mtl = *mit;
			//get the first triangle face cirresponding to this material
			if( nIdxToMtlChangeIdx >= 0 && nIdxToMtlChangeIdx <  i_ToBeMergedFrag->m_MaterialChanges.size() )
			{
				nMtlChangeBeginIdx = i_ToBeMergedFrag->m_MaterialChanges[ nIdxToMtlChangeIdx ];
			}
			//get the next triangle face coming after the last triangle face corresponding to the material
			if( ( nIdxToMtlChangeIdx + 1 ) < i_ToBeMergedFrag->m_MaterialChanges.size() )
			{
				nMtlChangeEndIdx = i_ToBeMergedFrag->m_MaterialChanges[ nIdxToMtlChangeIdx + 1 ];
			} else
			{					
				nMtlChangeEndIdx = i_ToBeMergedFrag->m_Indices.size() / 3;
			}
			DBG_ASSERT( (nMtlChangeBeginIdx < nMtlChangeEndIdx), "inconsistency in material change indices: " << tobeMergedMeshName.c_str() );
			//get the merged mdlFragInfo corresponding to this material
			//if it doesnt exist make one
			TMtlFragMapKey key;
			key.m_HasBasisVector = bToBeMergedFragHasBasisVectors;
			key.m_MatInfo = mtl;
			shared_ptr< mdlFragInfo > &mergedResult = GetMergedFragInfoCorrespondingToMtl(  key );
			//merge the partial goemtry corresponding to the material in the input frag
			//to the merged result

			mergedResult->AddGeometry( *i_ToBeMergedFrag, nMtlChangeBeginIdx, nMtlChangeEndIdx , i_pTransformToBeAppliedBeforeMerging);	
		}
	} else
	{
		shared_ptr< mdlMatInfo > &mtl = i_ToBeMergedFrag->m_Materials[0];
		TMtlFragMapKey key;
		key.m_HasBasisVector = bToBeMergedFragHasBasisVectors;
		key.m_MatInfo = mtl;
		shared_ptr< mdlFragInfo > &mergedResult = GetMergedFragInfoCorrespondingToMtl(  key );
		mergedResult->AddGeometry( *i_ToBeMergedFrag,  i_pTransformToBeAppliedBeforeMerging);
	}
	++m_NumMeshesSubjectedToMergeSoFar;
}

//------------------------------------------------------------------------
//	Transfer all the freshly created mdlFragInfos to the output result sequence
//------------------------------------------------------------------------
void mdlMaterialMerge::DepleteResult( std::vector< shared_ptr< mdlFragInfo > > &io_Result )
{
	std::vector< shared_ptr< mdlFragInfo > >::iterator sit;
	for( sit = m_SaturatedMergedFragments.begin(); sit !=  m_SaturatedMergedFragments.end(); ++sit )
	{
		//mdlFragUtil::CreateBasisVectors( **sit );		
		io_Result.push_back( *sit );
	}
	TMtlFragMap::iterator mit;
	for( mit= m_MtlFragMap.begin(); mit != m_MtlFragMap.end(); ++mit )
	{
		shared_ptr< mdlFragInfo > &frag = mit->second;
		//mdlFragUtil::CreateBasisVectors( *frag );
		io_Result.push_back( mit->second );
	}
	m_MtlFragMap.clear();
}

