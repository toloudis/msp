
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

struct mdlHairInfo;

namespace HairReader
{
	//------------------------------------------------------------------------
	//	Read in a mdlHairInfo from an ascii file using the testing file format
	//------------------------------------------------------------------------
	bool ReadData( std::istream &i_File, mdlHairInfo &o_Info );
}