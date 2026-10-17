/****************************************************************************\
**  mainCommandLine.hpp
**
**      mainCommandLine is MachStudio's command line, parsed but not acted
**	on. It is parsed first thing at start-up, before the main window and
**	graphics exist: graphics options and the early settings are used
**	while starting up, and the actions run afterwards, in command-line
**	order (mnmApp::RunCommandLine).
**
**	Toolkit-neutral; depends only on Core and the g3dGfxOptions header,
**	so UnitTests/MainAppTests can compile it.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef MAIN_COMMANDLINE_HPP
#error mainCommandLine.hpp multiply included
#endif
#define MAIN_COMMANDLINE_HPP

#ifndef G3D_GFXOPTIONS_HPP
#include "Graphics/G3d/g3dGfxOptions.hpp"
#endif

#include <string>
#include <vector>

//============================================================================
//============================================================================
struct mainCommandLine
{
	enum ActionKind
	{
		e_File,				// -file <scene>
		e_LastFile,			// -lastfile
		e_Help,				// -help
		e_Batch,			// -batch <scene>
		e_Render,			// -render <scene>
		e_Script,			// -script <script.py> [args]
		e_CapFormat,		// -capFormat <format>
		e_CapCompress,		// -capCompress <code>
		e_CapCamera,		// -capCamera <camera>
		e_CapWidth,			// -capWidth <pixels>
		e_CapHeight,		// -capHeight <pixels>
		e_CapOutputFile,	// -capOutputFile <file>
		e_CapOutputPath,	// -capOutputPath <dir>
		e_CapOutputPathRoot,// -capOutputPathRoot <dir>
		e_New,				// -new
		e_License,			// -license <key>
		e_Version,			// -version
		e_OpenPath			// the whole line is a file to open (double-click)
	};

	struct Action
	{
		ActionKind	m_Kind;
		std::string	m_Value;
	};

	g3dGfxOptions		m_Gfx;
	bool				m_bNoSplash = false;		// -NoSplash
	bool				m_bShowWindows = false;		// -show
	std::vector<Action>	m_Actions;					// in command-line order
	std::vector<std::string> m_Warnings;			// ignored tokens etc., logged once Core is up

	//------------------------------------------------------------------------
	//	Parse - flags take '-' or '/'. A flag's value is the next token (a
	//	double-quoted string is one token). -script takes the rest of the
	//	line as its value (as mnmApp always did). If an unknown token comes
	//	before any flag that counts as data (all but -help, -show and
	//	-version), the whole line is a file to open and parsing stops.
	//------------------------------------------------------------------------
	static mainCommandLine Parse(const std::string& i_Line);
};
