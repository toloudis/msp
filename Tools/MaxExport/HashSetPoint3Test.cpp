/*****************************************************************************
**  HashSetPoint3Test
**
**	HashSetPoint3Test tests the functionalities of HashSetPoint3
**	
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "HashSetPoint3.hpp"

#include <iostream>
#include <vector>

using namespace std;
using namespace MaxExp;


namespace 
{
	//========================================================================
	//Custom Point3 structure, to illustate that HahshSetPoint3
	//works with any Point3 datastructure

	//========================================================================

	struct MyPoint3
	{
		MyPoint3(float x, float y, float z)
		{ 
			_data[0] = x;
			_data[1] = y;
			_data[2] = z;
			_data[3] = 0.0f;
		}

		MyPoint3()
		{
			memset( _data, 0, sizeof(float)*4 );
		}

		MyPoint3( const MyPoint3 &other )
		{
			memcpy( _data, other._data, sizeof(float)*4);
		}

		bool operator==( const MyPoint3 &other )const
		{
			return _data[0] == other._data[0] &&
				_data[1] == other._data[1] &&
				_data[2] == other._data[2];
		}

		float _data[4];
	};



	//========================================================================
	//Traits struct, writen by the person integrating MPoint3 to HashSetpoint3
	//This class helps to access the components of the custom MyPoint3 struct

	//========================================================================

	struct MyPointTraits
	{
		static float &X(MyPoint3 &i_P) { return i_P._data[0];}
		static float &Y(MyPoint3 &i_P) { return i_P._data[1];}
		static float &Z(MyPoint3 &i_P) { return i_P._data[2];}

		static const float &X(const MyPoint3 &i_P) { return i_P._data[0];}
		static const float &Y(const MyPoint3 &i_P) { return i_P._data[1];}
		static const float &Z(const MyPoint3 &i_P) { return i_P._data[2];}

	};


} //anonymous namespace

namespace TestHashSetPoint3
{

	//========================================================================
	//TestInput: contains an arbitrary 3d point, its quantized representation,
	//	and the expected result, if we were to insert it in a HashSetPOint3,
	//Of course, the exoecteed result is valid, only if a sqeuence
	//of TestInput-s are inserted into the HashSetPoint3 in the given order

	//========================================================================

	struct TestInput
	{
		TestInput( const MyPoint3 &tobe, const MyPoint3 &quantized, bool wasInserted):
	_pointToBeInserted( tobe ),
		_pointQuantized( quantized ),
		_wasInserted( wasInserted)
	{}
	TestInput( float x1, float y1, float z1, float x2, float y2, float z2, bool b):
	_pointToBeInserted(x1, y1, z1),
		_pointQuantized(x2, y2, z2),
		_wasInserted(b)
	{
	}
	MyPoint3 _pointToBeInserted;
	MyPoint3 _pointQuantized;
	bool _wasInserted;
	};

#define INPUTSET_ADD( x1, y1, z1, x2, y2, z2, b )\
	inputSet.push_back( TestInput(x1, y1, z1, x2, y2, z2, b) )

	bool test()
	{
		//input sequence of TestInput-s
		vector< TestInput > inputSet;
		//HashSetPoint3, which is being tested
		typedef HashSetPoint3< MyPoint3, MyPointTraits > TestHashSet;
		TestHashSet testHashSet;

		//add the Testinput-s to the input sequence container
		{
			INPUTSET_ADD( 2.191049e-014f, -1.000000e+000f, 1.200399e-007f, 0.000000e+000f, -1.000000e+000f, 0.000000e+000f, true);
			INPUTSET_ADD( 4.523456e-015f, -1.000000e+000f, 6.987405e-008f, 0.000000e+000f, -1.000000e+000f, 0.000000e+000f, false);
			INPUTSET_ADD( -7.886498e-008f, -1.000000e+000f, 6.987408e-008f, 0.000000e+000f, -1.000000e+000f, 0.000000e+000f, false);
			INPUTSET_ADD( -7.886504e-008f, -1.000000e+000f, 1.668603e-006f, 0.000000e+000f, -1.000000e+000f, 2.000000e-006f, true);
			INPUTSET_ADD( -1.000000e+000f, -2.337193e-007f, 4.117243e-015f, -1.000000e+000f, 0.000000e+000f, 0.000000e+000f, true);
			INPUTSET_ADD( -1.000000e+000f, -2.337193e-007f, 1.633092e-014f, -1.000000e+000f, 0.000000e+000f, 0.000000e+000f, false);
			INPUTSET_ADD( -1.000000e+000f, -2.337194e-007f, -3.614405e-013f, -1.000000e+000f, 0.000000e+000f, 0.000000e+000f, false);
			INPUTSET_ADD( -1.000000e+000f, -2.337194e-007f, -3.614405e-013f, -1.000000e+000f, 0.000000e+000f, 0.000000e+000f, false);
			INPUTSET_ADD( -1.000000e+000f, -2.337193e-007f, -4.346775e-015f, -1.000000e+000f, 0.000000e+000f, 0.000000e+000f, false);
			INPUTSET_ADD( 0.000000e+000f, 1.000000e+000f, 0.000000e+000f, 0.000000e+000f, 1.000000e+000f, 0.000000e+000f, true);
			INPUTSET_ADD( -4.056665e-010f, 1.000000e+000f, 8.753556e-008f, 0.000000e+000f, 1.000000e+000f, 0.000000e+000f, false);
			INPUTSET_ADD( -1.000000e+000f, -2.337194e-007f, 0.000000e+000f, -1.000000e+000f, 0.000000e+000f, 0.000000e+000f, false);
			INPUTSET_ADD( 0.000000e+000f, 0.000000e+000f, -1.000000e+000f, 0.000000e+000f, 0.000000e+000f, -1.000000e+000f, true);
			INPUTSET_ADD( 0.000000e+000f, 1.000000e+000f, -6.987406e-008f, 0.000000e+000f, 1.000000e+000f, 0.000000e+000f, false);
			INPUTSET_ADD( 1.000000e+000f, -1.166722e-007f, 1.204779e-010f, 1.000000e+000f, 0.000000e+000f, 0.000000e+000f, true);
			INPUTSET_ADD( 1.000000e+000f, -1.168597e-007f, 1.168597e-007f, 1.000000e+000f, 0.000000e+000f, 0.000000e+000f, false );
			INPUTSET_ADD( 1.000000e+000f, -1.167345e-007f, 3.887312e-008f, 1.000000e+000f, 0.000000e+000f, 0.000000e+000f, false );
			INPUTSET_ADD( 1.000000e+000f, -1.166720e-007f, -4.264677e-015f, 1.000000e+000f, 0.000000e+000f, 0.000000e+000f, false );
			INPUTSET_ADD( 1.000000e+000f, -1.166722e-007f, 1.204779e-010f, 1.000000e+000f, 0.000000e+000f, 0.000000e+000f, false );
			INPUTSET_ADD( 1.000000e+000f, -1.167345e-007f, 3.887312e-008f, 1.000000e+000f, 0.000000e+000f, 0.000000e+000f, false );
			INPUTSET_ADD( 1.000000e+000f, -1.166720e-007f, -4.264677e-015f, 1.000000e+000f, 0.000000e+000f, 0.000000e+000f, false );
			INPUTSET_ADD( 1.000000e+000f, -1.167345e-007f, 3.887312e-008f, 1.000000e+000f, 0.000000e+000f, 0.000000e+000f, false );
			INPUTSET_ADD( 0.000000e+000f, 0.000000e+000f, 1.000000e+000f, 0.000000e+000f, 0.000000e+000f, 1.000000e+000f, true );
			INPUTSET_ADD( 3.180944e-006f, -1.000000e+000f, 2.313591e-007f, 4.000000e-006f, -1.000000e+000f, 0.000000e+000f, true );
			INPUTSET_ADD( 1.653357e-006f, -1.000000e+000f, 1.959333e-007f, 2.000000e-006f, -1.000000e+000f, 0.000000e+000f, true );
			INPUTSET_ADD( -1.838965e-006f, 1.000000e+000f, 3.739892e-007f, -2.000000e-006f, 1.000000e+000f, 0.000000e+000f, true );
			INPUTSET_ADD( -1.777097e-006f, 1.000000e+000f, -1.056917e-007f, -2.000000e-006f, 1.000000e+000f, 0.000000e+000f, false );
			INPUTSET_ADD( 4.027279e-007f, -1.000000e+000f, 2.049529e-006f, 0.000000e+000f, -1.000000e+000f, 2.000000e-006f, false );
			INPUTSET_ADD( 3.639052e-006f, 1.000000e+000f, -2.383446e-006f, 4.000000e-006f, 1.000000e+000f, -2.000000e-006f, true );
			INPUTSET_ADD( -1.733379e-006f, 1.000000e+000f, -2.537846e-006f, -2.000000e-006f, 1.000000e+000f, -2.000000e-006f, true );
			INPUTSET_ADD( -6.992842e-007f, 1.000000e+000f, -2.016129e-006f, 0.000000e+000f, 1.000000e+000f, -2.000000e-006f, true );
			INPUTSET_ADD( -1.221169e-006f, -1.000000e+000f, 0.000000e+000f, -2.000000e-006f, -1.000000e+000f, 0.000000e+000f, true );
			INPUTSET_ADD( -8.776424e-007f, -1.000000e+000f, -1.269933e-006f, 0.000000e+000f, -1.000000e+000f, -2.000000e-006f, true );
			INPUTSET_ADD( 1.708958e-006f, 1.000000e+000f, -1.749634e-007f, 2.000000e-006f, 1.000000e+000f, 0.000000e+000f, true );
			INPUTSET_ADD( 1.516247e-006f, 1.000000e+000f, 1.742572e-006f, 2.000000e-006f, 1.000000e+000f, 2.000000e-006f, true );
			INPUTSET_ADD( -1.859903e-005f, 1.000000e+000f, -1.084082e-005f, -1.800000e-005f, 1.000000e+000f, -1.000000e-005f, true );
			INPUTSET_ADD( 4.738935e-006f, 1.000000e+000f, -4.988147e-006f, 4.000000e-006f, 1.000000e+000f, -4.000000e-006f, true );
			INPUTSET_ADD( -2.935704e-006f, 1.000000e+000f, -3.024327e-006f, -2.000000e-006f, 1.000000e+000f, -4.000000e-006f, true );
			INPUTSET_ADD( -1.712873e-006f, -1.000000e+000f, -1.130296e-006f, -2.000000e-006f, -1.000000e+000f, -2.000000e-006f, true );
			INPUTSET_ADD( -4.229446e-006f, 1.000000e+000f, 4.574190e-006f, -4.000000e-006f, 1.000000e+000f, 4.000000e-006f, true );
			INPUTSET_ADD( 2.995409e-006f, -1.000000e+000f, -1.529207e-006f, 2.000000e-006f, -1.000000e+000f, -2.000000e-006f, true );			
		}

		//For each test input  in the input sequence,
		//try inserting it in the hashsetpoint3
		//and check whether we get the expected result
		vector< TestInput >::const_iterator cit;
		for( cit = inputSet.begin(); cit != inputSet.end(); ++ cit )
		{
			const TestInput &ti = *cit;
			pair< TestHashSet::iterator, bool> res = testHashSet.insert( ti._pointToBeInserted );		
			const MyPoint3 &pointQuantized = *res.first;
			bool wasInserted = res.second;
			if( !(ti._pointQuantized == pointQuantized) || wasInserted != ti._wasInserted )
			{
				cout << "test failed!";
				return false;
				break;
			}
		}
		return true;
	}
} //namesapace TestHashSetPoint3 


