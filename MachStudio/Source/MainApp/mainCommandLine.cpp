/****************************************************************************\
**  mainCommandLine.cpp
**
**      See mainCommandLine.hpp. The flag meanings are the ones
**	mnmApp::ParseCommandLine had before parsing moved here.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef MAIN_COMMANDLINE_HPP
#include "MainApp/mainCommandLine.hpp"
#endif
#include <cctype>

#ifndef APP_COMMANDLINE_HPP
#include "Core/App/appCommandLine.hpp"
#endif

namespace
{
	struct ValueFlag
	{
		const char*					m_pName;
		mainCommandLine::ActionKind	m_Kind;
	};

	const ValueFlag c_ValueFlags[] =
	{
		{ "file",				mainCommandLine::e_File },
		{ "batch",				mainCommandLine::e_Batch },
		{ "render",				mainCommandLine::e_Render },
		{ "capFormat",			mainCommandLine::e_CapFormat },
		{ "capCompress",		mainCommandLine::e_CapCompress },
		{ "capCamera",			mainCommandLine::e_CapCamera },
		{ "capWidth",			mainCommandLine::e_CapWidth },
		{ "capHeight",			mainCommandLine::e_CapHeight },
		{ "capStartFrame",		mainCommandLine::e_CapStartFrame },
		{ "capEndFrame",		mainCommandLine::e_CapEndFrame },
		{ "capOutputFile",		mainCommandLine::e_CapOutputFile },
		{ "capOutputPath",		mainCommandLine::e_CapOutputPath },
		{ "capOutputPathRoot",	mainCommandLine::e_CapOutputPathRoot },
		{ "license",			mainCommandLine::e_License },
	};

	mainCommandLine::Action make_action(mainCommandLine::ActionKind i_Kind, const std::string& i_Value = "")
	{
		mainCommandLine::Action action;
		action.m_Kind = i_Kind;
		action.m_Value = i_Value;
		return action;
	}

	bool is_space(char i_Char)
	{
		return i_Char == ' ' || i_Char == '\t' || i_Char == '\n' || i_Char == '\r';
	}

	//------------------------------------------------------------------------
	//	strip_quotes - removes all leading and trailing double quotes
	//------------------------------------------------------------------------
	std::string strip_quotes(const std::string& i_Text)
	{
		size_t start = i_Text.find_first_not_of('"');
		if (start == std::string::npos)
		{
			return std::string();
		}
		size_t end = i_Text.find_last_not_of('"');
		return i_Text.substr(start, end - start + 1);
	}

	//------------------------------------------------------------------------
	//	script_value - the text the -script flag hands on: everything after
	//	"script", with a quoted script path cut at its closing quote. This
	//	is the computation mnmApp::ParseCommandLine did, except that it
	//	starts at the -script flag itself rather than at the first
	//	"script" anywhere in the line.
	//------------------------------------------------------------------------
	std::string script_value(const std::string& i_Line)
	{
		size_t flag = std::string::npos;
		for (size_t pos = 0; pos + 7 <= i_Line.length(); ++pos)
		{
			if ((i_Line[pos] == '-' || i_Line[pos] == '/')
				&& i_Line.compare(pos + 1, 6, "script") == 0
				&& (pos == 0 || is_space(i_Line[pos - 1]))
				&& (pos + 7 == i_Line.length() || is_space(i_Line[pos + 7])))
			{
				flag = pos;
				break;
			}
		}
		if (flag == std::string::npos)
		{
			return std::string();
		}

		std::string command_line_string = i_Line.substr(flag + 1);

		size_t startpos = command_line_string.find_first_not_of("script");
		if (startpos == std::string::npos)
		{
			return std::string();
		}
		size_t endpos = command_line_string.find_last_not_of("\"");
		size_t endpos2 = command_line_string.find_last_of("\"");
		if (endpos2 != std::string::npos &&
			endpos2 < endpos &&
			endpos2 > startpos)
		{
			endpos = endpos2;
		}
		if (endpos == std::string::npos)
		{
			endpos = command_line_string.length();
		}

		std::string script_command = command_line_string.substr(startpos + 1, endpos - startpos);
		startpos = script_command.find_first_not_of(" ");
		if (startpos != std::string::npos)
		{
			script_command = script_command.substr(startpos);
		}
		return script_command;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mainCommandLine mainCommandLine::Parse(const std::string& i_Line)
{
	mainCommandLine result;
	std::vector<std::string> tokens = appCommandLine::Tokenize(i_Line);

	bool parsed_any_data = false;
	for (size_t i = 0; i < tokens.size(); ++i)
	{
		const std::string& token = tokens[i];

		bool is_value_flag = false;
		for (const ValueFlag& value_flag : c_ValueFlags)
		{
			if (appCommandLine::IsFlag(token, value_flag.m_pName))
			{
				is_value_flag = true;
				parsed_any_data = true;
				if (i + 1 < tokens.size())
				{
					result.m_Actions.push_back(make_action(value_flag.m_Kind, tokens[++i]));
				}
				else
				{
					result.m_Warnings.push_back("Command-line " + token + " has no value; ignored");
				}
				break;
			}
		}
		if (is_value_flag)
		{
			continue;
		}

		if (appCommandLine::IsFlag(token, "gfxProfile"))
		{
			parsed_any_data = true;
			if (i + 1 < tokens.size())
			{
				result.m_Gfx.m_ProfileFile = tokens[++i];
			}
			else
			{
				result.m_Warnings.push_back("Command-line " + token + " has no value; ignored");
			}
		}
		else if (appCommandLine::IsFlag(token, "gfx"))
		{
			parsed_any_data = true;
			if (i + 1 < tokens.size())
			{
				const std::string& value = tokens[++i];
				std::string lower(value);
				for (char& c : lower)
				{
					c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
				}
				if (lower == "dx11")
				{
					result.m_Gfx.m_Backend = g3dGfxOptions::e_DX11;
				}
				else if (lower == "vulkan")
				{
					result.m_Gfx.m_Backend = g3dGfxOptions::e_Vulkan;
				}
				else
				{
					result.m_Warnings.push_back("Command-line " + token + " " + value + ": unknown backend (use dx11 or vulkan); using dx11");
				}
			}
			else
			{
				result.m_Warnings.push_back("Command-line " + token + " has no value; ignored");
			}
		}
		else if (appCommandLine::IsFlag(token, "gfxValidation"))
		{
			parsed_any_data = true;
			result.m_Gfx.m_bValidation = true;
		}
		else if (appCommandLine::IsFlag(token, "script"))
		{
			parsed_any_data = true;
			result.m_Actions.push_back(make_action(e_Script, script_value(i_Line)));
		}
		else if (appCommandLine::IsFlag(token, "lastfile"))
		{
			parsed_any_data = true;
			result.m_Actions.push_back(make_action(e_LastFile));
		}
		else if (appCommandLine::IsFlag(token, "new"))
		{
			parsed_any_data = true;
			result.m_Actions.push_back(make_action(e_New));
		}
		else if (appCommandLine::IsFlag(token, "NoSplash"))
		{
			parsed_any_data = true;
			result.m_bNoSplash = true;
		}
		else if (appCommandLine::IsFlag(token, "help"))
		{
			result.m_Actions.push_back(make_action(e_Help));
		}
		else if (appCommandLine::IsFlag(token, "version"))
		{
			result.m_Actions.push_back(make_action(e_Version));
		}
		else if (appCommandLine::IsFlag(token, "show"))
		{
			result.m_bShowWindows = true;
		}
		else if (!parsed_any_data)
		{
			//	Nothing recognized yet: assume the app was started by
			//	double-clicking a file, and the whole line is its path.
			result.m_Actions.push_back(make_action(e_OpenPath, strip_quotes(i_Line)));
			break;
		}
		else
		{
			result.m_Warnings.push_back("Command-line token ignored: " + token);
		}
	}
	return result;
}

//----------------------------------------------------------------------------
//	Keep in step with the reference at the top of mainCommandLine.hpp.
//----------------------------------------------------------------------------
const char* mainCommandLine::HelpText()
{
	return R"(Usage:
  MachStudio [options]
  MachStudio <scene.mab | character.chx | character.gxb>

Every flag may start with '-' or '/'. A value with spaces must be in double
quotes. Flags run in the order given; the capture settings (-cap*) change
the settings of the open scene, so put them after -render or -file (loading
a scene replaces them with the scene's own).

If the first thing on the line isn't a known flag, the whole line is taken
as a file to open, as when a file is double-clicked: a scene (.mab) is
opened, a character (.chx/.gxb) is added to a new scene in its project.

Scenes
  -file <scene.mab>          open a scene
  -lastfile                  open the most recently used scene
  -new                       start a new, empty scene

Rendering
  -render <scene.mab>        open a scene and render it with its render
                             (capture) settings; MachStudio exits when the
                             render is done. The usual way to use the
                             RenderNode build.
  -batch <batchfile>         render every scene listed in a render batch
                             file, one after another
  -capFormat <format>        output format: PNG, TGA, BMP, JPG, DDS, TIF,
                             HDR, PFM, PPM, DIB, EXR (frames) or AVI
  -capCompress <code>        video compression code for AVI output
  -capCamera <camera>        also render from this camera (by name); repeat
                             to add more cameras
  -capWidth <pixels>         output width
  -capHeight <pixels>        output height
  -capStartFrame <frame>     render only from this frame on (an absolute
                             frame number, as in output file names);
                             narrows the scene's range, whichever way it is
                             set. The frame before it is rendered too, not
                             saved, so motion matches a full render. Unlike
                             the other -cap* flags it isn't a scene setting:
                             it holds for every render in the run, wherever
                             it is on the line
  -capEndFrame <frame>       render only up to this frame (included); as
                             -capStartFrame
  -capOutputFile <tags>      output file name, made of text and tags such as
                             <SceneFilename>, <CameraName>, <RenderLayerName>,
                             <RenderPassName> and <0000> (frame number,
                             zero-padded)
  -capOutputPath <tags>      output folder, absolute or relative to the
                             output root; may use the same tags
  -capOutputPathRoot <dir>   output root folder for relative output folders

Graphics
  -gfx dx11|vulkan           graphics backend; default dx11. If Vulkan can't
                             start, MachStudio says why and exits (it never
                             falls back to DX11)
  -gfxValidation             turn on the Vulkan validation layer (Debug
                             builds always have it on); the error and
                             warning counts are printed at exit
  -gfxProfile <file.csv>     write the CPU and GPU time and draw count of
                             every renderer and pass, per frame, to a CSV
                             file. Only in builds with profiling markers
                             (Debug, or built with MspGfxProfiling=true);
                             ignored otherwise.

Start-up and other
  -NoSplash                  don't show the splash screen
  -show                      show windows in RenderNode (batch) builds,
                             which hide them by default
  -script <file.py> [args]   run a Python script after start-up, with the
                             rest of the line as its arguments (needs a build
                             with Python enabled; current builds don't run it)
  -license <key>             validate a license key
  -version                   print the version
  -help                      print this list
)";
}
