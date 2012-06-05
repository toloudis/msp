/*****************************************************************************
**  HashSetPoint3
**
**	Auxilliary  class-es  for HashSetPoint3
**	
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/


#ifdef MAXEXP_HASHSETPOINT3AUX_HPP
#error MAXEXP_HASHSETPOINT3AUX_HPP multiply defined!!
#endif
#define MAXEXP_HASHSETPOINT3AUX_HPP


#include <algorithm>
#include <hash_set>
#include <cmath>
#include <cassert>


namespace MaxExp
{
	//========================================================================
	//There are various ways to hash a general 3d point structure.
	//The following hashing algorithm is borrowed from the hash_value function 
	//defined in the xhash  file in std c library. It may or may not suite 3d points
	//
	//StdLibHashAlgorithm::hash_value is the public member function,
	//that computes the hash. It uses a similar algorithm as in the hash_value
	//function in the xahsh file in std c library.
	//It accepts an unsigned character array of 12 bytes ( 3 * sizeof(float)).
	//ToDo [kg:03/26/09] Currently e have no means of imposing a compile-time restriction
	//on this input requirement that the input should be a 12 byte unsigned char array)

	//We have modified the hash_value functin from xhash file,
	//and employed template meta-programming to compute that hash.
	//Since template-meta programming evaluation is done in compile time.
	//the source code should unroll to a more efficient output code stream.
	//========================================================================
	class StdLibHashAlgorithm 
	{
	public:
		StdLibHashAlgorithm(){}
		//main public member function
		size_t hash_value( unsigned char *i_Ch ) const
		{
			return _ComputeHash<12>::Compute( i_Ch );
		}

	private:
		//an embedded struct for template-meta programming evaluation
		//of the hash function
		template<int n >
		struct _ComputeHash
		{
			static size_t Compute ( const unsigned char *i_Ch )
			{
				//x-or c[0] and  some prime number times result of the recursive computation
				//of the rest of the character stream
				return 
					static_cast<size_t>(*i_Ch) ^ //x-or
					( _ComputeHash<n-1>::Compute( i_Ch+1 ) * 16777619U ); 
			}
		};		

	};


	//boundary-case specialization of the template-meta programming
	//evaluation
	template<>
	struct StdLibHashAlgorithm::_ComputeHash<1>
	{
		static size_t Compute ( const unsigned char *i_Ch )
		{
#pragma warning(disable:4307)
			//these values  are extracted from  xhash.h
			//in the std library
			return static_cast<size_t>(*i_Ch) ^ 
				(2166136261U  * 16777619U);
#pragma warning(default:4307)

		}
	};


	//========================================================================
	// Since our point3d hash method should work for all types of 3d point 
	// structs, the component access of a 3d point is generalized through
	// a traits class. Any  user of this hash method, employing a custom 
	// point3d structure can specialize the traits class, as long as the 
	// as long as traits class has X(), Y() and Z() member functions
	//========================================================================

	template < class T >
	struct PointTraits
	{
		static float &X(T &i_P) { return i_P.x;}
		static float &Y(T &i_P) { return i_P.y;}
		static float &Z(T &i_P) { return i_P.z;}

		static const float &X(const T &i_P) { return i_P.x;}
		static const float &Y(const T &i_P) { return i_P.y;}
		static const float &Z(const T &i_P) { return i_P.z;}
		//ToDo[kg[03/26/09] a more efficient
		//accessing method can be considered as in
		//static const unsigned char * Bytes( const T &P ) {... }
		//but can't be generalized due to the case where T doesnt
		//expose the components as a contiguous aray of 12 bytes

	};


	//========================================================================
	// We need an implementation of less< Point3 > that imposes at least  a 
	// partial order on the set of points.
	//========================================================================
	template< class P, class PTraits = PointTraits< P > >
	struct LessPoint3
	{
		bool operator()( const P &i_First, const P &i_Second ) const
		{
#if defined(_DEBUG)
#if SGPU_DEBUG
			Quantizer q
				assert( PTraits::X(i_First) == q( PTraits::X(i_First) ));
			assert( PTraits::Y(i_First) == q(PTraits::Y(i_First)));			
			assert( PTraits::Z(i_First) == q( PTraits::Z(i_First)));

			assert( PTraits::X(i_Second) == q( PTraits::X(i_Second) ));
			assert( PTraits::Y(i_Second) == q(PTraits::Y(i_Second)));			
			assert( PTraits::Z(i_Second) == q( PTraits::Z(i_Second)));
#endif
#endif

			if(  PTraits::X(i_First) == PTraits::X(i_Second) )
				if( PTraits::Y(i_First) ==  PTraits::Y(i_Second) )
					return ( PTraits::Z(i_First) < PTraits::Z(i_Second) );
				else
					return ( PTraits::Y(i_First) < PTraits::Y(i_Second) );
			else
				return ( PTraits::X(i_First) < PTraits::X(i_Second) );
		}
	};



	//========================================================================
	// The HashSetPoint3 employs quantizing of 3d Points.
	// So that points which are very close, are stored as 
	// their quantized values and such points produce the same 
	// hash value
	//========================================================================
	struct Quantizer
	{
		Quantizer():
			m_fEps(1.0e-6f)
			{}
		float operator()( float i_fVal ) const
		{
			float ret;
			double ftemp = floor(i_fVal /(2.0 * m_fEps));
			float c1 = (2.0f * (float)(ftemp * m_fEps)) ;
			float c2 = (2.0f * (float)(( ftemp  + 1.0) *  m_fEps )) ; 	
			assert( i_fVal >= c1 );
			ret  = ( (i_fVal - c1 )  < m_fEps ?  c1  : c2 ) ;
			return ret;
		}
		const float m_fEps;
	};



} //namespace MaxExp