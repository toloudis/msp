/****************************************************************************\
**	itTestMain.cpp
**
**		Main file for running the tests.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Core/dbg/dbgMsg.hpp"
#include "Core/dbg/dbgPackage.hpp"
#include "Core/env/envPackage.hpp"
#include "Core/Gf/gfPackage.hpp"
//#include "Core/it/itLocales.hpp"
//#include "Core/it/itLocaleUtil.hpp"
#include "Core/it/itPackage.hpp"
#include "Core/it/itString.hpp"
#include "Core/it/itStringUtil.hpp"

#include <iomanip>
#include <iostream>
#include <fstream>


//============================================================================
//	Test functions
//============================================================================
namespace
{

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestBuilding()
{
	DBG_LOG("\tTestBuilding - Start");

	itString Str1("Base");
	itString Str2("Suffix");
	itString Str3("png");
	itString Str4("jpg");
	itString comp;

	DBG_LOG("\t\tString 1 = " << Str1);
	DBG_LOG("\t\tString 2 = " << Str2);
	DBG_LOG("\t\tString 3 = " << Str3);
	DBG_LOG("\t\tString 4 = " << Str4);

	comp = Str1 + Str2;
	DBG_LOG("\t\tAdd - str1 + str2");
	DBG_LOG("\t\t\tresult   = " << comp);

	comp = Str1;
	comp += Str2;
	comp.ReplaceExtension(Str3);
	DBG_LOG("\t\t+= and replace extension");
	DBG_LOG("\t\t\tresult   = " << comp);
	comp.ReplaceExtension(Str4);
	DBG_LOG("\t\t\tresult   = " << comp);

	comp = Str1;
	comp += Str2;
	comp += itString(".bmp");
	itString Str5;
	DBG_LOG("\t\tfile     = " << comp);
	DBG_LOG("\t\tbase");
	comp.GetBase(Str5);
	DBG_LOG("\t\t\tresult   = " << Str5);
	DBG_LOG("\t\textension");
	comp.GetExtension(Str5);
	DBG_LOG("\t\t\tresult   = " << Str5);
	DBG_LOG("\t\tlength");
	DBG_LOG("\t\t\tresult   = " << comp.GetLength());
	DBG_LOG("\t\tSubstring - uff");
	DBG_LOG("\t\t\tresult   = " << (comp.HasSubString(itString("uff"))?"true":"false"));
	DBG_LOG("\t\tSubstring - zzz");
	DBG_LOG("\t\t\tresult   = " << (comp.HasSubString(itString("zzz"))?"true":"false"));

	DBG_LOG("\tTestBuilding - End");
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestExtracting()
{
	DBG_LOG("\tTestExtracting - Start");

	itString filename("SceneFile015.mab");
	itString base, ext;
	int number;

	itStringUtil::Breakup_Filename( filename, base, ext, number );

	DBG_LOG("\t\tfile   = " << filename);
	DBG_LOG("\t\tbase   = " << base);
	DBG_LOG("\t\text    = " << ext);
	DBG_LOG("\t\tnumber = " << number);
	DBG_LOG(" ");

	filename = itString("Scene.File.bmp");
	itStringUtil::Breakup_Filename( filename, base, ext, number );

	DBG_LOG("\t\tfile   = " << filename);
	DBG_LOG("\t\tbase   = " << base);
	DBG_LOG("\t\text    = " << ext);
	DBG_LOG("\t\tnumber = " << number);

	DBG_LOG("\tTestExtracting - End");
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestSearching()
{
	DBG_LOG("\TestSearching - Start");

	itString mainString("abcdefghijklmnopqrstuvwxyz");
	itString substr;
	DBG_LOG("\t\tmain string = " << mainString);

	substr = "def";
	DBG_LOG("\t\t\tsubstring " ,<< substr);
	DBG_LOG("\t\t\tcontains    = " << (mainString.HasSubString(substr)?"true":"false"));
	DBG_LOG("\t\t\tstarts with = " << (mainString.StartsWith(substr)?"true":"false"));
	DBG_LOG("\t\t\tend with    = " << (mainString.EndsWith(substr)?"true":"false"));

	substr = "abc";
	DBG_LOG("\t\t\tsubstring " << substr);
	DBG_LOG("\t\t\tcontains    = " << (mainString.HasSubString(substr)?"true":"false"));
	DBG_LOG("\t\t\tstarts with = " << (mainString.StartsWith(substr)?"true":"false"));
	DBG_LOG("\t\t\tend with    = " << (mainString.EndsWith(substr)?"true":"false"));

	substr = "wxyz";
	DBG_LOG("\t\t\tsubstring " << substr);
	DBG_LOG("\t\t\tcontains    = " << (mainString.HasSubString(substr)?"true":"false"));
	DBG_LOG("\t\t\tstarts with = " << (mainString.StartsWith(substr)?"true":"false"));
	DBG_LOG("\t\t\tend with    = " << (mainString.EndsWith(substr)?"true":"false"));

	substr = "abcdefghijklmnopqrstuvwxyzasdf";
	DBG_LOG("\t\t\tsubstring " << substr);
	DBG_LOG("\t\t\tcontains    = " << (mainString.HasSubString(substr)?"true":"false"));
	DBG_LOG("\t\t\tstarts with = " << (mainString.StartsWith(substr)?"true":"false"));
	DBG_LOG("\t\t\tend with    = " << (mainString.EndsWith(substr)?"true":"false"));

	DBG_LOG("\TestSearching - End");
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void TestParsingRaw()
{
	DBG_LOG("\tTestParsing - Start");

	itString tags("<scene>_<camera>hello<layer><cameraz>");
	DBG_LOG("\t\tTag line = " << tags);

	enum
	{
		e_nontag = 0,
		e_tag = 1
	};

	int state = e_nontag;
	int cindex = 0;
	itString::CharType ch;
	itString accum;

	for (int i=0; i < tags.GetLength(); ++i)
	{
		ch = tags[i];
		//DBG_LOG("\t\t" << std::setw(3) << i << " " << std::setw(3) << ch << " " << std::setw(3) << (char)ch);

		if (ch == '<')
		{
			if (accum.GetLength() > 0)
				if (state == e_tag) DBG_LOG("\t\ttag = " << accum); else DBG_LOG("\t\tnon-tag = " << accum);
			state = e_tag;
			accum.Clear();
		}
		else if (ch == '>')
		{
			state = e_nontag;
			if (accum.GetLength() > 0)
				DBG_LOG("\t\ttag = " << accum);
			accum.Clear();
		}
		else
		{
			accum += ch;
		}
	}

	DBG_LOG("\tTestParsing - End");
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void DoTests()
{
	try
	{
		TestBuilding();
		TestExtracting();
		TestSearching();
		TestParsingRaw();
	}
	catch (...)
	{
		DBG_ERROR("Unknown Error");
	}
}

}

//============================================================================
//============================================================================
void main()
{
	DBG_LOG("Init - gf");
	gfPackage::Init();
	DBG_LOG("Init - env");
	envPackage::Init();
	DBG_LOG("Init - dbg");
	dbgPackage::Init();

	dbgMsg::addStream(std::wstring(L"TestLog"), new std::ofstream(L"testLog.log"));

	DBG_LOG("Init - it");
	itPackage::Init();

	DBG_LOG("Starting it tests");

	DoTests();

	DBG_LOG("Finished it tests");

	DBG_LOG("CleanUp - it");
	itPackage::CleanUp();
	DBG_LOG("CleanUp - dbg");
	dbgPackage::CleanUp();
	DBG_LOG("CleanUp - env");
	envPackage::CleanUp();
	DBG_LOG("CleanUp - gf");
	gfPackage::CleanUp();

	DBG_LOG("Program finished.");
}
