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
**	Command line reference (what `-help` prints; HelpText() below):
**
**	  MachStudio [options]
**	  MachStudio <scene.mab | scene.gltf | scene.glb | character.chx | character.gxb>
**
**	Every flag may start with '-' or '/'. A value with spaces must be in
**	double quotes. Flags run in the order given; the capture settings
**	(-cap*) change the settings of the open scene, so put them after
**	-render or -file (loading a scene replaces them with the scene's own).
**
**	If the first thing on the line isn't a known flag, the whole line is
**	taken as a file to open, as when a file is double-clicked: a scene
**	(.mab) is opened, a glTF (.gltf/.glb) is opened as a new, untitled
**	scene, a character (.chx/.gxb) is added to a new scene in its project.
**
**	Scenes
**	  -file <scene>              open a scene (.mab), or a glTF (.gltf/.glb)
**	                             as a new, untitled scene
**	  -lastfile                  open the most recently used scene
**	  -new                       start a new, empty scene
**
**	Rendering
**	  -render <scene.mab>        open a scene and render it with its render
**	                             (capture) settings; MachStudio exits when
**	                             the render is done. The usual way to use
**	                             the RenderNode build. Not a glTF: open it
**	                             and save it as a .mab first.
**	  -batch <batchfile>         render every scene listed in a render batch
**	                             file, one after another
**	  -capFormat <format>        output format: PNG, TGA, BMP, JPG, DDS, TIF,
**	                             HDR, PFM, PPM, DIB, EXR (frames) or AVI
**	  -capCompress <code>        video compression code for AVI output
**	  -capCamera <camera>        also render from this camera (by name);
**	                             repeat to add more cameras
**	  -capWidth <pixels>         output width
**	  -capHeight <pixels>        output height
**	  -capStartFrame <frame>     render only from this frame on (an absolute
**	                             frame number, as in output file names);
**	                             narrows the scene's range, whichever way it
**	                             is set. The frame before it is rendered too,
**	                             not saved, so motion matches a full render.
**	                             Unlike the other -cap* flags it isn't a
**	                             scene setting: it holds for every render in
**	                             the run, wherever it is on the line
**	  -capEndFrame <frame>       render only up to this frame (included); as
**	                             -capStartFrame
**	  -capOutputFile <tags>      output file name, made of text and tags such
**	                             as <SceneFilename>, <CameraName>,
**	                             <RenderLayerName>, <RenderPassName> and
**	                             <0000> (frame number, zero-padded)
**	  -capOutputPath <tags>      output folder, absolute or relative to the
**	                             output root; may use the same tags
**	  -capOutputPathRoot <dir>   output root folder for relative output
**	                             folders
**	  -capSampling <n>           supersampling: n x n samples per pixel, 1 to
**	                             8 (1 = none); how they are taken is
**	                             -capJitter's
**	  -capJitter off             take the samples on a regular grid (the
**	                             frame is rendered n times the size and
**	                             scaled down)
**	  -capJitter <filter> <width>
**	                             take jittered samples, accumulated with this
**	                             filter (Box, Gaussian, Mitchell, Triangle,
**	                             Sinc, Lanczos, Blackman-Harris or
**	                             Catmull-Rom) and width in pixels (above 0,
**	                             at most 8)
**	  -capMotionSamples <n>      motion samples per frame, 1 to 16 (kept with
**	                             the scene; renders ignore it for now, motion
**	                             blur is turned off in the renderer)
**	  -capPasses <pass,...>      render these passes, by the scene's pass
**	                             names (by default BTY, DIFF, SPEC, AO, GI, Z,
**	                             SHDW, ILLUM, NOR, MAT, WIRE, MTL, REFL, VEL,
**	                             BLOOM, STAR, DOF, PREV, EMI, SPECENV,
**	                             SPECLIT, DIFFENV, DIFFLIT, GLOW), in every
**	                             render layer; the others are turned off.
**	                             RenderMan passes (COLBL) are skipped with a
**	                             warning: GPU renders can't make them
**	  -capHardwareAA on|off      the renderer's own antialiasing (HDR AA)
**
**	Graphics
**	  -gfx dx11|vulkan           graphics backend; default vulkan. If Vulkan
**	                             can't start, MachStudio says why and exits
**	                             (it never falls back to DX11)
**	  -gfxValidation             turn on the Vulkan validation layer (Debug
**	                             builds always have it on); the error and
**	                             warning counts are printed at exit
**	  -gfxProfile <file.csv>     write the CPU and GPU time and draw count of
**	                             every renderer and pass, per frame, to a CSV
**	                             file. Only in builds with profiling markers
**	                             (Debug, or built with MspGfxProfiling=true);
**	                             ignored otherwise.
**
**	Start-up and other
**	  -NoSplash                  don't show the splash screen
**	  -show                      show windows in RenderNode (batch) builds,
**	                             which hide them by default
**	  -script <file.py> [args]   run a Python script after start-up, with
**	                             the rest of the line as its arguments
**	                             (needs a build with Python enabled; current
**	                             builds don't run it)
**	  -license <key>             validate a license key
**	  -version                   print the version
**	  -help                      print this list
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
	//	The actions, run after start-up in command-line order. See the
	//	reference above for what each flag does.
	enum ActionKind
	{
		e_File,				// -file <scene.mab>: open a scene
		e_LastFile,			// -lastfile: open the most recently used scene
		e_Help,				// -help: print the command line reference
		e_Batch,			// -batch <batchfile>: render every scene in a render batch file
		e_Render,			// -render <scene.mab>: open a scene, render it, then exit
		e_Script,			// -script <file.py> [args]: run a Python script (Python builds only)
		e_CapFormat,		// -capFormat <format>: output format (PNG, EXR, AVI, ...)
		e_CapCompress,		// -capCompress <code>: AVI video compression code
		e_CapCamera,		// -capCamera <camera>: also render from this camera
		e_CapWidth,			// -capWidth <pixels>: output width
		e_CapHeight,		// -capHeight <pixels>: output height
		e_CapStartFrame,	// -capStartFrame <frame>: render only from this frame on
		e_CapEndFrame,		// -capEndFrame <frame>: render only up to this frame
		e_CapOutputFile,	// -capOutputFile <tags>: output file name (text and tags)
		e_CapOutputPath,	// -capOutputPath <tags>: output folder
		e_CapOutputPathRoot,// -capOutputPathRoot <dir>: root for relative output folders
		e_CapSampling,		// -capSampling <n>: n x n samples per pixel ("1" to "8")
		e_CapJitter,		// -capJitter off | <filter> <width>: "off", or "<Filter> <width>" with the filter's own spelling (SplitJitter)
		e_CapMotionSamples,	// -capMotionSamples <n>: motion samples per frame ("1" to "16")
		e_CapPasses,		// -capPasses <pass,...>: the passes to render, by name (MatchPasses)
		e_CapHardwareAA,	// -capHardwareAA on|off: "on" or "off"
		e_New,				// -new: start a new, empty scene
		e_License,			// -license <key>: validate a license key
		e_Version,			// -version: print the version
		e_OpenPath			// (no flag) the whole line is a file to open, as on double-click
	};

	struct Action
	{
		ActionKind	m_Kind;
		std::string	m_Value;
	};

	g3dGfxOptions		m_Gfx;							// -gfx, -gfxValidation, -gfxProfile
	bool				m_bNoSplash = false;			// -NoSplash: no splash screen
	bool				m_bShowWindows = false;			// -show: show windows in RenderNode builds
	std::vector<Action>	m_Actions;						// in command-line order
	std::vector<std::string> m_Warnings;				// ignored tokens etc., logged once Core is up

	//------------------------------------------------------------------------
	//	Parse - flags take '-' or '/'. A flag's value is the next token (a
	//	double-quoted string is one token). -script takes the rest of the
	//	line as its value (as mnmApp always did). If an unknown token comes
	//	before any flag that counts as data (all but -help, -show and
	//	-version), the whole line is a file to open and parsing stops.
	//------------------------------------------------------------------------
	static mainCommandLine Parse(const std::string& i_Line);

	//------------------------------------------------------------------------
	//	HelpText - the command line reference that -help prints (the same
	//	as the reference at the top of this file). Lines end with '\n'.
	//------------------------------------------------------------------------
	static const char* HelpText();

	//------------------------------------------------------------------------
	//	SplitJitter - an e_CapJitter value: false for "off", else true with
	//	the filter (captRenderOutputData::eFilterFunc's order: Box,
	//	Gaussian, Mitchell, Triangle, Sinc, Lanczos, Blackman-Harris,
	//	Catmull-Rom) and the width.
	//------------------------------------------------------------------------
	static bool SplitJitter(const std::string& i_Value, int& o_Filter, float& o_Width);

	//------------------------------------------------------------------------
	//	MatchPasses - an e_CapPasses value against the scene's pass names
	//	(i_PassNames[t] is pass type t's name, from
	//	captRenderOutputDataUtil::GetRenderPassName): o_Passes gets the
	//	pass types named, each once, in pass-type order. Names match
	//	without regard to case. Names of the pass types in i_Refused
	//	(RenderMan passes) are left out and listed in o_Skipped. An unknown
	//	name fails the whole list (false, o_Passes empty, the name in
	//	o_Unknown), and so does a list with nothing left (false, o_Unknown
	//	empty).
	//------------------------------------------------------------------------
	static bool MatchPasses(const std::string& i_List, const std::vector<std::string>& i_PassNames,
							const std::vector<int>& i_Refused, std::vector<int>& o_Passes,
							std::string& o_Unknown, std::vector<std::string>& o_Skipped);
};
