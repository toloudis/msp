/*****************************************************************************
**  mainCommandLineTest.cpp
**
**      mainCommandLine::Parse: MachStudio's command-line flags become
**      graphics options, early settings and an ordered list of actions,
**      with the same meaning mnmApp::ParseCommandLine gave them.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef MAIN_COMMANDLINE_HPP
#include "MainApp/mainCommandLine.hpp"
#endif

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace
{
	typedef mainCommandLine::Action Action;

	Action act(mainCommandLine::ActionKind i_Kind, const std::string& i_Value = "")
	{
		Action action;
		action.m_Kind = i_Kind;
		action.m_Value = i_Value;
		return action;
	}

	void expect_actions(const std::vector<Action>& i_Expected, const mainCommandLine& i_Actual)
	{
		ASSERT_EQ(i_Expected.size(), i_Actual.m_Actions.size());
		for (size_t i = 0; i < i_Expected.size(); ++i)
		{
			EXPECT_EQ(i_Expected[i].m_Kind, i_Actual.m_Actions[i].m_Kind) << "action " << i;
			EXPECT_EQ(i_Expected[i].m_Value, i_Actual.m_Actions[i].m_Value) << "action " << i;
		}
	}
}

TEST(mainCommandLine, Empty)
{
	mainCommandLine line = mainCommandLine::Parse("");
	EXPECT_TRUE(line.m_Actions.empty());
	EXPECT_EQ("", line.m_Gfx.m_ProfileFile);
	EXPECT_FALSE(line.m_bNoSplash);
	EXPECT_FALSE(line.m_bShowWindows);
}

TEST(mainCommandLine, RenderWithCaptureSettingsInOrder)
{
	mainCommandLine line = mainCommandLine::Parse(
		"-capFormat PNG -capOutputPath \"C:\\out dir\" -capWidth 640 -render \"C:\\s.mab\"");
	expect_actions({act(mainCommandLine::e_CapFormat, "PNG"),
					act(mainCommandLine::e_CapOutputPath, "C:\\out dir"),
					act(mainCommandLine::e_CapWidth, "640"),
					act(mainCommandLine::e_Render, "C:\\s.mab")}, line);
}

TEST(mainCommandLine, GfxProfileIsNotAnAction)
{
	mainCommandLine line = mainCommandLine::Parse("-gfxProfile p.csv -render s.mab");
	EXPECT_EQ("p.csv", line.m_Gfx.m_ProfileFile);
	expect_actions({act(mainCommandLine::e_Render, "s.mab")}, line);
}

TEST(mainCommandLine, NoSplashAndShowAreEarlySettings)
{
	mainCommandLine line = mainCommandLine::Parse("-NoSplash -show -new");
	EXPECT_TRUE(line.m_bNoSplash);
	EXPECT_TRUE(line.m_bShowWindows);
	expect_actions({act(mainCommandLine::e_New)}, line);
}

TEST(mainCommandLine, WholeLineIsPathWhenFirstTokenUnknown)
{
	mainCommandLine line = mainCommandLine::Parse("C:\\a b\\scene.mab");
	expect_actions({act(mainCommandLine::e_OpenPath, "C:\\a b\\scene.mab")}, line);
}

TEST(mainCommandLine, QuotedWholeLinePath)
{
	mainCommandLine line = mainCommandLine::Parse("\"C:\\a b\\scene.mab\"");
	expect_actions({act(mainCommandLine::e_OpenPath, "C:\\a b\\scene.mab")}, line);
}

TEST(mainCommandLine, HelpThenPathStillOpensPath)
{
	// Today's quirk, kept: -help doesn't count as parsed data, so the
	// unknown token that follows turns the whole line into a path.
	mainCommandLine line = mainCommandLine::Parse("-help C:\\x.mab");
	expect_actions({act(mainCommandLine::e_Help),
					act(mainCommandLine::e_OpenPath, "-help C:\\x.mab")}, line);
}

TEST(mainCommandLine, UnknownTokenAfterActionIsIgnored)
{
	mainCommandLine line = mainCommandLine::Parse("-render a.mab -bogus");
	expect_actions({act(mainCommandLine::e_Render, "a.mab")}, line);
	ASSERT_EQ(1u, line.m_Warnings.size());
	EXPECT_NE(std::string::npos, line.m_Warnings[0].find("-bogus"));
}

TEST(mainCommandLine, ScriptTakesRestOfLine)
{
	// The value is what the old -script branch handed on: everything after
	// "script", with a quoted script path cut at its closing quote.
	mainCommandLine line = mainCommandLine::Parse("-NoSplash -script C:\\t.py arg1 arg2");
	EXPECT_TRUE(line.m_bNoSplash);
	expect_actions({act(mainCommandLine::e_Script, "C:\\t.py arg1 arg2")}, line);
}

TEST(mainCommandLine, QuotedScriptPathIsCutAtClosingQuote)
{
	mainCommandLine line = mainCommandLine::Parse("-script \"C:\\t.py\" arg1 arg2");
	expect_actions({act(mainCommandLine::e_Script, "\"C:\\t.py\"")}, line);
}

TEST(mainCommandLine, ScriptDoesNotStopParsing)
{
	// As before: the script gets the rest of the line as its arguments,
	// and the flags after it are still parsed.
	mainCommandLine line = mainCommandLine::Parse("-script C:\\t.py -render a.mab");
	expect_actions({act(mainCommandLine::e_Script, "C:\\t.py -render a.mab"),
					act(mainCommandLine::e_Render, "a.mab")}, line);
}

TEST(mainCommandLine, ValueFlagAtEndIsIgnored)
{
	mainCommandLine line = mainCommandLine::Parse("-render");
	EXPECT_TRUE(line.m_Actions.empty());
	ASSERT_EQ(1u, line.m_Warnings.size());
	EXPECT_NE(std::string::npos, line.m_Warnings[0].find("-render"));
}

TEST(mainCommandLine, NoWarningsForAWellFormedLine)
{
	EXPECT_TRUE(mainCommandLine::Parse("-NoSplash -render a.mab").m_Warnings.empty());
}

TEST(mainCommandLine, SlashPrefixWorks)
{
	mainCommandLine line = mainCommandLine::Parse("/render a.mab");
	expect_actions({act(mainCommandLine::e_Render, "a.mab")}, line);
}

TEST(mainCommandLine, LicenseAndLastFileAndVersion)
{
	mainCommandLine line = mainCommandLine::Parse("-license KEY -lastfile -version");
	expect_actions({act(mainCommandLine::e_License, "KEY"),
					act(mainCommandLine::e_LastFile),
					act(mainCommandLine::e_Version)}, line);
}

TEST(mainCommandLine, AllCaptureFlags)
{
	mainCommandLine line = mainCommandLine::Parse(
		"-capCompress c -capCamera Cam1 -capHeight 480 -capOutputFile f -capOutputPathRoot r -batch b.mab -file x.mab");
	expect_actions({act(mainCommandLine::e_CapCompress, "c"),
					act(mainCommandLine::e_CapCamera, "Cam1"),
					act(mainCommandLine::e_CapHeight, "480"),
					act(mainCommandLine::e_CapOutputFile, "f"),
					act(mainCommandLine::e_CapOutputPathRoot, "r"),
					act(mainCommandLine::e_Batch, "b.mab"),
					act(mainCommandLine::e_File, "x.mab")}, line);
}

TEST(mainCommandLine, CaptureFrameWindow)
{
	mainCommandLine line = mainCommandLine::Parse("-render s.mab -capStartFrame 296 /capEndFrame 315");
	expect_actions({act(mainCommandLine::e_Render, "s.mab"),
					act(mainCommandLine::e_CapStartFrame, "296"),
					act(mainCommandLine::e_CapEndFrame, "315")}, line);
	EXPECT_TRUE(line.m_Warnings.empty());
}

TEST(mainCommandLine, HelpListsEveryFlag)
{
	const std::string help = mainCommandLine::HelpText();
	const char* const flags[] =
	{
		"-file", "-lastfile", "-new", "-render", "-batch", "-script",
		"-capFormat", "-capCompress", "-capCamera", "-capWidth", "-capHeight",
		"-capStartFrame", "-capEndFrame",
		"-capOutputFile", "-capOutputPath", "-capOutputPathRoot",
		"-license", "-NoSplash", "-show", "-version", "-help", "-gfxProfile",
		"-gfx", "-gfxValidation",
		"-capSampling", "-capJitter", "-capMotionSamples", "-capPasses", "-capHardwareAA",
	};
	for (const char* flag : flags)
	{
		//	each flag starts a line of the help text
		EXPECT_NE(std::string::npos, help.find(std::string("\n  ") + flag + " "))
			<< flag << " is missing from the help text";
	}
}

TEST(mainCommandLine, GfxVulkan)
{
	mainCommandLine line = mainCommandLine::Parse("-gfx vulkan -new");
	EXPECT_EQ(g3dGfxOptions::e_Vulkan, line.m_Gfx.m_Backend);
	expect_actions({act(mainCommandLine::e_New)}, line);
	EXPECT_TRUE(line.m_Warnings.empty());
}

TEST(mainCommandLine, GfxDefaultsToDx11)
{
	EXPECT_EQ(g3dGfxOptions::e_DX11, mainCommandLine::Parse("-new").m_Gfx.m_Backend);
	EXPECT_FALSE(mainCommandLine::Parse("-new").m_Gfx.m_bValidation);
}

TEST(mainCommandLine, GfxDx11CaseInsensitive)
{
	mainCommandLine line = mainCommandLine::Parse("-gfx DX11");
	EXPECT_EQ(g3dGfxOptions::e_DX11, line.m_Gfx.m_Backend);
	EXPECT_EQ(g3dGfxOptions::e_Vulkan, mainCommandLine::Parse("/gfx Vulkan").m_Gfx.m_Backend);
	EXPECT_TRUE(line.m_Warnings.empty());
}

TEST(mainCommandLine, GfxBadValueWarns)
{
	mainCommandLine line = mainCommandLine::Parse("-gfx metal");
	EXPECT_EQ(g3dGfxOptions::e_DX11, line.m_Gfx.m_Backend);
	ASSERT_EQ(1u, line.m_Warnings.size());
	EXPECT_NE(std::string::npos, line.m_Warnings[0].find("metal"));
}

TEST(mainCommandLine, GfxValidation)
{
	EXPECT_TRUE(mainCommandLine::Parse("-gfxValidation").m_Gfx.m_bValidation);
}

TEST(mainCommandLine, GfxFirstMeansNotAPath)
{
	mainCommandLine line = mainCommandLine::Parse("-gfx vulkan C:\\x.mab");
	EXPECT_TRUE(line.m_Actions.empty());
	ASSERT_EQ(1u, line.m_Warnings.size());
	EXPECT_NE(std::string::npos, line.m_Warnings[0].find("x.mab"));
}

//	Plan 6 Task 10: the capture flags that RenderNode needs to reach the
//	sampling, jitter, motion-sample, pass and hardware-AA settings.

TEST(mainCommandLine, CaptureSamplingFlags)
{
	mainCommandLine line = mainCommandLine::Parse(
		"-render s.mab -capSampling 3 -capJitter gaussian 1.5 -capMotionSamples 4 -capHardwareAA OFF");
	expect_actions({act(mainCommandLine::e_Render, "s.mab"),
					act(mainCommandLine::e_CapSampling, "3"),
					act(mainCommandLine::e_CapJitter, "Gaussian 1.5"),
					act(mainCommandLine::e_CapMotionSamples, "4"),
					act(mainCommandLine::e_CapHardwareAA, "off")}, line);
	EXPECT_TRUE(line.m_Warnings.empty());
}

TEST(mainCommandLine, CaptureJitterOffAndEveryFilter)
{
	mainCommandLine line = mainCommandLine::Parse(
		"-render s.mab -capJitter off -capJitter Box 1 -capJitter mitchell 2 -capJitter TRIANGLE 0.5 "
		"-capJitter sinc 3 -capJitter lanczos 4 -capJitter blackman-harris 2.5 -capJitter catmull-rom 8 "
		"/capHardwareAA on");
	expect_actions({act(mainCommandLine::e_Render, "s.mab"),
					act(mainCommandLine::e_CapJitter, "off"),
					act(mainCommandLine::e_CapJitter, "Box 1"),
					act(mainCommandLine::e_CapJitter, "Mitchell 2"),
					act(mainCommandLine::e_CapJitter, "Triangle 0.5"),
					act(mainCommandLine::e_CapJitter, "Sinc 3"),
					act(mainCommandLine::e_CapJitter, "Lanczos 4"),
					act(mainCommandLine::e_CapJitter, "Blackman-Harris 2.5"),
					act(mainCommandLine::e_CapJitter, "Catmull-Rom 8"),
					act(mainCommandLine::e_CapHardwareAA, "on")}, line);
	EXPECT_TRUE(line.m_Warnings.empty());
}

TEST(mainCommandLine, CaptureJitterSplits)
{
	int filter = -1;
	float width = 0.0f;
	EXPECT_TRUE(mainCommandLine::SplitJitter("Gaussian 1.5", filter, width));
	EXPECT_EQ(1, filter);		// captRenderOutputData::eFilterGaussian
	EXPECT_FLOAT_EQ(1.5f, width);
	EXPECT_TRUE(mainCommandLine::SplitJitter("Catmull-Rom 8", filter, width));
	EXPECT_EQ(7, filter);		// captRenderOutputData::eFilterCatmullRom
	EXPECT_FLOAT_EQ(8.0f, width);
	EXPECT_FALSE(mainCommandLine::SplitJitter("off", filter, width));
}

TEST(mainCommandLine, CapturePassesKeepsTheList)
{
	mainCommandLine line = mainCommandLine::Parse("-render s.mab -capPasses BTY,Z,NOR");
	expect_actions({act(mainCommandLine::e_Render, "s.mab"),
					act(mainCommandLine::e_CapPasses, "BTY,Z,NOR")}, line);
	EXPECT_TRUE(line.m_Warnings.empty());
}

TEST(mainCommandLine, CaptureNewFlagsBeforeRenderKeepTheirOrder)
{
	//	parsed in order; loading the scene in -render then replaces them
	mainCommandLine line = mainCommandLine::Parse("-capSampling 2 -capPasses BTY -render s.mab");
	expect_actions({act(mainCommandLine::e_CapSampling, "2"),
					act(mainCommandLine::e_CapPasses, "BTY"),
					act(mainCommandLine::e_Render, "s.mab")}, line);
}

TEST(mainCommandLine, CaptureNewFlagsMissingValue)
{
	const char* const lines[] =
	{
		"-render s.mab -capSampling",
		"-render s.mab -capJitter",
		"-render s.mab -capJitter gaussian",
		"-render s.mab -capMotionSamples",
		"-render s.mab -capPasses",
		"-render s.mab -capHardwareAA",
	};
	for (const char* text : lines)
	{
		mainCommandLine line = mainCommandLine::Parse(text);
		expect_actions({act(mainCommandLine::e_Render, "s.mab")}, line);
		EXPECT_EQ(1u, line.m_Warnings.size()) << text;
	}
}

TEST(mainCommandLine, CaptureNewFlagsBadValue)
{
	const char* const lines[] =
	{
		"-render s.mab -capSampling 0",
		"-render s.mab -capSampling 9",
		"-render s.mab -capSampling two",
		"-render s.mab -capSampling 2x",
		"-render s.mab -capJitter wobbly 1",
		"-render s.mab -capJitter gaussian 0",
		"-render s.mab -capJitter gaussian -1",
		"-render s.mab -capJitter gaussian 9",
		"-render s.mab -capJitter gaussian wide",
		"-render s.mab -capMotionSamples 0",
		"-render s.mab -capMotionSamples 17",
		"-render s.mab -capHardwareAA maybe",
		"-render s.mab -capPasses BTY,,Z",
		"-render s.mab -capPasses ,",
	};
	for (const char* text : lines)
	{
		mainCommandLine line = mainCommandLine::Parse(text);
		expect_actions({act(mainCommandLine::e_Render, "s.mab")}, line);
		EXPECT_EQ(1u, line.m_Warnings.size()) << text;
	}
}

TEST(mainCommandLine, CaptureNewFlagsInRange)
{
	mainCommandLine line = mainCommandLine::Parse(
		"-render s.mab -capSampling 1 -capSampling 8 -capMotionSamples 1 -capMotionSamples 16");
	expect_actions({act(mainCommandLine::e_Render, "s.mab"),
					act(mainCommandLine::e_CapSampling, "1"),
					act(mainCommandLine::e_CapSampling, "8"),
					act(mainCommandLine::e_CapMotionSamples, "1"),
					act(mainCommandLine::e_CapMotionSamples, "16")}, line);
	EXPECT_TRUE(line.m_Warnings.empty());
}

TEST(mainCommandLine, CapturePassesMatchTheScenesNames)
{
	//	the scene's pass names, by pass type (captRenderOutputDataUtil::GetRenderPassName)
	const std::vector<std::string> names = { "BTY", "DIFF", "SPEC", "AO", "Z", "MyNormals" };
	const std::vector<int> none;
	std::vector<int> passes;
	std::string unknown;
	std::vector<std::string> skipped;
	EXPECT_TRUE(mainCommandLine::MatchPasses("bty,Z,mynormals", names, none, passes, unknown, skipped));
	EXPECT_EQ(std::vector<int>({0, 4, 5}), passes);
	EXPECT_EQ("", unknown);
	EXPECT_TRUE(skipped.empty());

	//	a pass named twice is rendered once
	EXPECT_TRUE(mainCommandLine::MatchPasses("AO,ao", names, none, passes, unknown, skipped));
	EXPECT_EQ(std::vector<int>({3}), passes);

	//	an unknown name rejects the whole list
	EXPECT_FALSE(mainCommandLine::MatchPasses("BTY,NOR", names, none, passes, unknown, skipped));
	EXPECT_TRUE(passes.empty());
	EXPECT_EQ("NOR", unknown);
}

TEST(mainCommandLine, CapturePassesSkipRenderManPasses)
{
	//	pass type 6 stands for a RenderMan pass (COLBL): GPU renders have no
	//	renderer for it, so it is left out and the rest of the list kept
	const std::vector<std::string> names = { "BTY", "DIFF", "SPEC", "AO", "Z", "MyNormals", "COLBL" };
	const std::vector<int> renderman = { 6 };
	std::vector<int> passes;
	std::string unknown;
	std::vector<std::string> skipped;
	EXPECT_TRUE(mainCommandLine::MatchPasses("BTY,colbl,Z", names, renderman, passes, unknown, skipped));
	EXPECT_EQ(std::vector<int>({0, 4}), passes);
	EXPECT_EQ(std::vector<std::string>({"colbl"}), skipped);
	EXPECT_EQ("", unknown);

	//	nothing left: the list is ignored, the skipped pass still reported
	EXPECT_FALSE(mainCommandLine::MatchPasses("COLBL", names, renderman, passes, unknown, skipped));
	EXPECT_TRUE(passes.empty());
	EXPECT_EQ(std::vector<std::string>({"COLBL"}), skipped);
	EXPECT_EQ("", unknown);

	//	an unknown name still rejects the whole list
	EXPECT_FALSE(mainCommandLine::MatchPasses("COLBL,NOR", names, renderman, passes, unknown, skipped));
	EXPECT_EQ("NOR", unknown);
	EXPECT_TRUE(skipped.empty());
}

TEST(mainCommandLine, HelpSaysRenderManPassesAreSkipped)
{
	const std::string help = mainCommandLine::HelpText();
	EXPECT_NE(std::string::npos, help.find("RenderMan passes (COLBL)"));
}
