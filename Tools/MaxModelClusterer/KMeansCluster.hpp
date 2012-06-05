
/*****************************************************************************
**  KMeansCluster.hpp
** 
**  Core algorithm doesnt depend on max api
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include <vector>
#include <hash_set>
#include <limits>
#include <list>

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#undef max
#undef min

template< typename T >
float SqDistance( const T &first, const T &sec )
{
	float fx = first[0] - sec[0];
	float fy = first[1] - sec[1];
	float fz = first[2] - sec[2];
	return fx * fx + fy * fy + fz * fz;
}


//T = type of input point eg: Point3 from Max api
//I = iterator for the points
//O = container type for holding indices of the nodes in a cluster, eg: hash_set<int>
template< typename T, typename I , typename O >
class KMeansCluster
{
public:
	//k = number of clusters desired
	//begin = begin iterator for input points
	//end = end iterator for input points
	//pMax = bounding box maximum of the input points
	//pMin = bounding box minimum of the inpit points
	KMeansCluster(int k,  I begin, I end , const T &pMax, const T &pMin ):
	m_k(k),
	m_NodesBegin( begin ),
	m_NodesEnd( end ),
	m_pMax( pMax ),
	m_pMin( pMin )
	{}


	~KMeansCluster(){}

	//Core algorithm
	bool Do(  std::vector< shared_ptr<  O >  > &output );

protected:
    //For a given input data point,
	//find the nearest centroid
	//Iter = iterator for centroid
	//T = basic point type
	template< typename Iter >
	Iter FindNearestCentroid( const T & curDataPoint,  Iter beginCentroid, Iter endCentroid );
	I m_NodesBegin;
	I m_NodesEnd;
	const T &m_pMax;
	const T &m_pMin;
	const int m_k;
};

template< typename T, typename I , typename O >
template< typename Iter >
Iter KMeansCluster<T, I, O>::FindNearestCentroid( const T & curDataPoint,  Iter beginCentroid, Iter endCentroid )
{
	float minSqDistSoFar = std::numeric_limits<float>::max();
	Iter nearestCentroidIter = endCentroid;
	Iter cit;
	//find the nearest centroid
	for( cit = beginCentroid; cit != endCentroid ; ++cit )
	{
		float sqDist = SqDistance( curDataPoint, *cit );
		if ( sqDist < minSqDistSoFar )
		{
			minSqDistSoFar = sqDist;
			nearestCentroidIter =  cit;
		}
	}
	return nearestCentroidIter;
}


//main algorithm
	/*
Create k news random points to serve as the cluster centroids.

total_error = some huge number
new_total_error = 0

while( (total_error - new_total_error) is not small )
{
   For each of the n input points, find the nearest cluster centroid,
   and assign the input point to that cluster. Please note the distance of the
   point from its cluster centroid.

   new_total_error = the total of the square of the distances.

   For each cluster, find the centroid of the all the points assigned to the cluster.
   and that new centroid will be the cluster centroid for the next iteration.
}
*/

template< typename T, typename I , typename O >
bool KMeansCluster< T, I, O >::Do(  std::vector< shared_ptr< O >  > &output )
{ 
	const float offset = 1.0e-5f;
	const T bboxDiff = m_pMax - m_pMin;
	srand ( static_cast< unsigned int > ( time(NULL) ) );
	std::vector< T > centroids( m_k );
	std::vector< int > numDataPointsAssociatedWithCentroids(m_k, 0);
	std::vector< float > sumSqDistanceOfAssociatedPoints( m_k, 0 );
	std::vector< T > centroidsForNextIter ( m_k, T(0,0,0) );
	//find k random points within the bounding box limit
	for( int i=0; i < m_k; ++i )
	{
		float fx = static_cast<float> ( rand() ) / RAND_MAX;
		float fy = static_cast<float> ( rand() ) / RAND_MAX;
		float fz = static_cast<float> ( rand() ) / RAND_MAX;
		T centroidK =  T(bboxDiff[0] * fx, bboxDiff[1] * fy, bboxDiff[2] * fz );
		centroidK += m_pMin;
		centroids [ i ] = centroidK;
	}

	float prevTotalSqDistanceFromCentroids = std::numeric_limits<float>::max();
	float curTotalSqDistanceFromCentroids  = 0.0f;
	float diffPrevCur = prevTotalSqDistanceFromCentroids - curTotalSqDistanceFromCentroids;
	do
	{
		I pit;
		int i=0;
		curTotalSqDistanceFromCentroids =0;
		centroidsForNextIter.assign( m_k, T(0,0,0) );
		numDataPointsAssociatedWithCentroids.assign( m_k, 0);	
		for( pit = m_NodesBegin; pit != m_NodesEnd; ++pit, ++i )
		{		
			const T  &curPoint = *pit;
			typedef std::vector< T >::const_iterator TCentroidIter;
			TCentroidIter nearestCentroidIter = FindNearestCentroid < TCentroidIter > ( curPoint, centroids.begin(), centroids.end() );		
			TCentroidIter centroidsBegin = centroids.begin();
			float dist = SqDistance( *nearestCentroidIter, curPoint );
			if( nearestCentroidIter != centroids.end() )
			{	
				int iNearestCentroid = std::distance(  centroidsBegin, nearestCentroidIter );		
				++numDataPointsAssociatedWithCentroids[ iNearestCentroid ];
				sumSqDistanceOfAssociatedPoints[ iNearestCentroid ] += dist;
				curTotalSqDistanceFromCentroids += dist;
				centroidsForNextIter[ iNearestCentroid ] += curPoint;
			}
		}
		
		diffPrevCur = prevTotalSqDistanceFromCentroids - curTotalSqDistanceFromCentroids;
		prevTotalSqDistanceFromCentroids = curTotalSqDistanceFromCentroids;
		std::vector< T >::iterator vit;
		std::vector< int  >::const_iterator hit;
		std::vector< T >::iterator vit2;
		for( vit = centroidsForNextIter.begin(),
			hit = numDataPointsAssociatedWithCentroids.begin(),
			vit2 = centroids.begin(); 
			vit != centroidsForNextIter.end(); 
		    ++vit,++hit , ++vit2)
		{
			assert( hit != numDataPointsAssociatedWithCentroids.end() );
			assert( vit2 != centroids.end() );
			int numDataPointsAssociatedWithThisCentroid = *hit;
			if( numDataPointsAssociatedWithThisCentroid > 0)
			{
				T t= * vit;
				t = t / static_cast< float > ( numDataPointsAssociatedWithThisCentroid );
				T &newCentroid = *vit2;
				newCentroid = t;
			}
		}
	} while ( diffPrevCur > offset );
	I pit;
	
	std::vector< T  >::const_iterator cit;
	map< int, O* > outputLookupMap;
	int i = 0;
	for( cit = centroids.begin(); cit != centroids.end(); ++cit, ++i )
	{
		outputLookupMap[ i ] = output[i].get();
	}

	i=0;
	for( pit = m_NodesBegin; pit != m_NodesEnd; ++pit, ++i )
	{	
		const T  &curPoint = *pit;
		typedef std::vector< T >::const_iterator TCentroidIter;
		TCentroidIter nearestCentroidIter = FindNearestCentroid < TCentroidIter > ( curPoint, centroids.begin(), centroids.end() );		
		if( nearestCentroidIter != centroids.end() )
		{	
			int iNearestCentroidIter =  nearestCentroidIter - centroids.begin();
			shared_ptr< O > pOutput = output[ iNearestCentroidIter ];
			int ip = static_cast< int > (  std::distance( m_NodesBegin, pit  ) );
			pOutput->insert( ip );
		}
	}
	return true;
}