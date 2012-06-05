// (c) 2001 Microsoft Corporation

function ConfigureDebug(config)
{
	config.IntermediateDirectory=".\\..\\bin\\$(ConfigurationName)\\obj\\$(ProjectName)"
	config.OutputDirectory=".\\..\\bin\\$(ConfigurationName)\\lib"
	config.CharacterSet = charSetMBCS;
		
	var CLTool = config.Tools("VCCLCompilerTool");
	CLTool.WarningLevel = warningLevel_3;
	CLTool.MinimalRebuild = false;
	CLTool.DebugInformationFormat = debugEnabled;
	CLTool.Optimization = optimizeDisabled;
	CLTool.BasicRuntimeChecks = runtimeBasicCheckNone;
	CLTool.PreprocessorDefinitions = "ENV_MACHINE=1;ENV_OS=15;ENV_COMPILER=30;ENV_BUILD=50;" + GetPlatformDefine(config) + "_DEBUG";
	CLTool.RuntimeLibrary = rtMultiThreadedDebug;
	CLTool.RuntimeTypeInfo = true;
	CLTool.UsePrecompiledHeader = pchNone;
		
	//var LinkTool = config.Tools("VCLinkerTool");
	//LinkTool.GenerateDebugInformation = true;
	//LinkTool.LinkIncremental = linkIncrementalYes;
	//LinkTool.AssemblyDebug = linkAssemblyDebugFull;
}

function ConfigureRelease(config)
{
	config.IntermediateDirectory=".\\..\\bin\\$(ConfigurationName)\\obj\\$(ProjectName)"
	config.OutputDirectory=".\\..\\bin\\$(ConfigurationName)\\lib"
	config.ManagedExtensions = false;
	config.CharacterSet = charSetMBCS;

	var CLTool = config.Tools("VCCLCompilerTool");
	CLTool.WarningLevel = warningLevel_3;
	CLTool.MinimalRebuild = false;
	CLTool.PreprocessorDefinitions = "ENV_MACHINE=1;ENV_OS=15;ENV_COMPILER=30;ENV_TEXT=42;ENV_BUILD=51;" + GetPlatformDefine(config) + "NDEBUG";
	CLTool.RuntimeLibrary = rtMultiThreaded;
	CLTool.RuntimeTypeInfo = true;
	CLTool.DebugInformationFormat = debugEnabled;
	CLTool.UsePrecompiledHeader = pchNone;
				
	//LinkTool = config.Tools("VCLinkerTool");
	//LinkTool.GenerateDebugInformation = true;
	//LinkTool.LinkIncremental = linkIncrementalNo;
		
}

function AddTerawattConfigs(proj)
{
	try
	{
		// Debug Non-Managed
		var config = proj.Object.Configurations("Debug");
		ConfigureDebug(config);
		config.ManagedExtensions = false;

		// Release Non-Managed
		config = proj.Object.Configurations("Release");
		ConfigureRelease(config);
		config.ManagedExtensions = false;
		
		// Debug Managed
		proj.Object.AddConfiguration("Debug Managed");
		config = proj.Object.Configurations("Debug Managed");
		ConfigureDebug(config);
		config.ManagedExtensions = true;

		// Release Managed
		proj.Object.AddConfiguration("Release Managed");
		config = proj.Object.Configurations("Release Managed");
		ConfigureRelease(config);
		config.ManagedExtensions = true;
		
		proj.Object.keyword = "TerawattLibProj";

	}
	catch(e)
	{
		throw e;
	}
}

function AddTerawattConfigsForLIB(proj, strProjectName)
{
	try
	{
		AddTerawattConfigs(proj);
		proj.Object.RootNamespace = CreateSafeName(strProjectName);
		AddTerawattConfigForLIB(proj.Object.Configurations.Item("Debug"), strProjectName);
		AddTerawattConfigForLIB(proj.Object.Configurations.Item("Release"), strProjectName);
		AddTerawattConfigForLIB(proj.Object.Configurations.Item("Debug Managed"), strProjectName);
		AddTerawattConfigForLIB(proj.Object.Configurations.Item("Release Managed"), strProjectName);
	}
	catch(e)
	{
		throw e;
	}
}

function AddTerawattConfigForLIB(config, strProjectName)
{
	try
	{
		config.ConfigurationType = typeStaticLibrary;
		//var LinkTool = config.Tools("VCLibrarianTool");
		//LinkTool.SuppressStartupBanner = "TRUE";
		//LinkTool.OutputFile = "$(OutDir)\\$(ProjectName)" + ".lib";			

	}
	catch(e)
	{
		throw e;
	}

}

function CreateTerawattProject(strProjectName, strProjectPath)
{
	try
	{
		var strSafeProjectName = CreateSafeName(strProjectName);
		wizard.AddSymbol("SAFE_PROJECT_NAME", strSafeProjectName);
		wizard.AddSymbol("SAFE_NAMESPACE_NAME", CreateCPPName(strSafeProjectName));

		//CopyNCBFileToSolutionDirectory(strProjectName, strProjectPath);
		var oProject = CreateProject(strProjectName, strProjectPath);
		return oProject;
	}
	catch(e)
	{   
		throw e;
	}
}


function OnFinish(selProj, selObj)
{
    try
    {
        var strProjectPath = wizard.FindSymbol("PROJECT_PATH");
        var strProjectName = wizard.FindSymbol("PROJECT_NAME");

		if (!CanUseDrive(strProjectPath))
			return VS_E_WIZARDBACKBUTTONPRESS;
			
	// Terawatt
        var proj = CreateTerawattProject(strProjectName, strProjectPath);
        AddTerawattConfigsForLIB(proj, strProjectName);
	//	AddReferencesForApp(proj);
        //AddFilesToNewProjectWithInfFile(proj, strProjectName);
        
        // Managed DLL
	 //       var proj = CreateManagedProject(strProjectName, strProjectPath);
	//        AddManagedConfigsForDLL(proj, strProjectName);
	//		AddReferencesForApp(proj);
	//        AddFilesToNewProjectWithInfFile(proj, strProjectName);
	        
        proj.Object.Save();
    }
	catch(e)
	{
		if (e.description.length != 0)
			SetErrorInfo(e);
		return e.number
	}
}

function SetFileProperties(projfile, strName)
{
	return false;
}

function GetTargetName(strName, strProjectName, strResPath, strHelpPath)
{
	try
	{
		var strTarget = strName;

		if (strName == "readme.txt")
			strTarget = "ReadMe.txt";

		if (strName.substr(0, 4) == "root")
		{
			strTarget = strProjectName + strName.substr(4);
		}
		return strTarget; 
	}
	catch(e)
	{
		throw e;
	}
}


