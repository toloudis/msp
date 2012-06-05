
/*****************************************************************************
**  InstanceWork.cpp
**  The core algorithm for guessing the transformation
**  from one mesh to another mesh.
**
**  This is written independent of the 3ds Max API
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "InstanceWork.hpp"

#include "eigen2/Eigen/LU" 
USING_PART_OF_NAMESPACE_EIGEN


bool epsilonEqual ( float a, float b )
{
	return ( fabs(a - b) <= 1e-3f );
}

bool epsilonEqual ( double a, double b )
{
	return ( fabs(a - b) <= 1e-3f );
}

//========================================================================
	//	Main function which does the job. 
	//  Each of the two input meshes has the same number of vertices.
	//	meshVerts1 is vector of floats of size 3*n, contains the vertex info
	//  for the first mesh.
	//  meshVerts1 = [ v0.x, v0.y, v0.z, v1.x, v1.y, v1.z ...... vn-1.x, vn-1.y, vn-1.z ]
	//  meshVerts2 contains the vertex info for the second mesh.
	//  o_Xform is the transform that transforms the vrtices of the
	//  second mesh into the vrtices of the first mesh.
	//  This routine retirns the least square error.
	//========================================================================


float InstanceWork::Do(
		int nVert,  
		const std::vector< float > &meshVerts1,
		const std::vector< float > &meshVerts2,
		std::vector< float > &xform )
{
	//Make an n x 4 matrix A
	// If vi is  the ith vertex of the second mesh,
	//A[i,0] = vi.x
	//A[i,1]= vi.y
    //A[i,2] = vi.z
	//A[i,3] = 1.0
	MatrixXd A( nVert, 4 );
	for( int i=0; i < nVert; ++i )
	{
		for( int j=0; j < 4; ++j )
		{
			A(i, j )= meshVerts2[4*i + j];
		}
	}
	//Find A' and A' x A
	MatrixXd At = A.transpose();
	MatrixXd AtA = At * A;

	Matrix<double,4, 4> X;

	for( int j=0; j < 4 ; ++j )
	{
		//Make a n x 1 vector b
		//If vi is the ith vrtex of the second mesh
		// b[i] =  vi.x if j ==0
		// b[i] = vi.y if j ==1
		// b[i] = vi.z if j == 2
		// b[i] = 1.0 if j ==0
		VectorXd b(nVert);
		for( int i=0; i < nVert; ++i )
		{
			b(i) = meshVerts1[4*i + j];
		}
		//Find 4 x 1 vector Atb
		//such that Atb = A' x b
		MatrixXd Atb_temp = At * b;
		VectorXd Atb( 4 );
		for(int i=0; i < 4; ++i )
		{
			Atb(i) = Atb_temp(i,0);
		}
		//Solve for the the transformation column
		//using the LU decomposition method
		VectorXd x;
		AtA.lu().solve( Atb, &x );
		//x, the solution should be a 4 x 1 vector
		if( x.rows() != 4 )
		{
			return std::numeric_limits<float>::max();
		}
		assert( 4 == x.rows());
		//assign the jth column of the solution as x
		X.col( j ) = x;
	}
	//verify the equation A x X and check whether we get back
	//the b - components
	//Also find the L1 error
	MatrixXd res = A * X;
	assert( res.rows() == nVert );
	assert( res.cols() == 4 );
	bool success = true;
	double maxRelError = 0.0;
    for( int i=0; i < nVert  ; ++i )
	{ 
		for( int j=0; j < 4  ; ++j )
		{ 
			double a = res( i, j );
			double b =  meshVerts1[4*i + j ];
			double d = a - b;
			if( !epsilonEqual( res(i, j) , 0.0 ) )
			{
				double relError = fabs( d / res(i, j ) );
				if( relError > maxRelError )
				{
					maxRelError = relError;
				}
			}			
		}
	}
	//finally re-assign the 4x4 solution matrix
	//into the output parameter
	for( int i=0; i < 4; ++i )
		for( int j=0; j < 4; ++j )
		{
			xform[ i * 4 + j ] = static_cast<float>( X(i, j ) );
		}
	return static_cast<float>( maxRelError );
}