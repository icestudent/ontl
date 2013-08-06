// TODO: find this path dynamic
var NtlBasePath = "d:/usr/lib/common/ontl/ntl";

function OnFinish(selProj, selObj) {
	try {
		var strProjectPath = wizard.FindSymbol('PROJECT_PATH');
		var strProjectName = wizard.FindSymbol('PROJECT_NAME');

		var WizardVersion = wizard.FindSymbol('WIZARD_VERSION');

		// Create symbols based on the project name
		var strSafeProjectName = CreateSafeName(strProjectName);
		wizard.AddSymbol("SAFE_PROJECT_NAME", strSafeProjectName);
		wizard.AddSymbol("NICE_SAFE_PROJECT_NAME", strSafeProjectName.substr(0, 1).toUpperCase() + strSafeProjectName.substr(1))
		wizard.AddSymbol("UPPERCASE_SAFE_PROJECT_NAME", strSafeProjectName.toUpperCase());

		// Set app type symbols

		// Set view symbols

		// Create project and configurations
		selProj = CreateCustomProject(strProjectName, strProjectPath);
		// AddPlatforms(selProj);
		AddConfigurations(selProj, strProjectName);
		AddFilters(selProj);
		AddRuntimeSources(selProj);

		AddFilesToProjectWithInfFile(selProj, strProjectName);

		selProj.Object.Save();

	}
	catch (e) {
		if (e.description.length != 0)
			SetErrorInfo(e);
		return e.number
	}
}

function CreateCustomProject(strProjectName, strProjectPath) {
	try {
		var strProjTemplatePath = wizard.FindSymbol('PROJECT_TEMPLATE_PATH');
		var strProjTemplate = '';
		strProjTemplate = strProjTemplatePath + '\\default.vcproj';

		var Solution = dte.Solution;
		var strSolutionName = "";
		if (wizard.FindSymbol("CLOSE_SOLUTION")) {
			Solution.Close();
			strSolutionName = wizard.FindSymbol("VS_SOLUTION_NAME");
			if (strSolutionName.length) {
				var strSolutionPath = strProjectPath.substr(0, strProjectPath.length - strProjectName.length);
				Solution.Create(strSolutionPath, strSolutionName);
			}
		}

		var strProjectNameWithExt = '';
		strProjectNameWithExt = strProjectName + '.vcproj';

		var oTarget = wizard.FindSymbol("TARGET");
		var prj;
		if (wizard.FindSymbol("WIZARD_TYPE") == vsWizardAddSubProject)  // vsWizardAddSubProject
		{
			var prjItem = oTarget.AddFromTemplate(strProjTemplate, strProjectNameWithExt);
			prj = prjItem.SubProject;
		}
		else {
			prj = oTarget.AddFromTemplate(strProjTemplate, strProjectPath, strProjectNameWithExt);
		}
		return prj;
	}
	catch (e) {
		throw e;
	}
}

function AddPlatforms(proj){
	try{
		if(wizard.FindSymbol('NTL_PLATFORM_X64'))
			proj.AddPlatform("x64");
	}catch(e){
		throw e;
	}
}
function AddFilters(proj) {
	try {
		// Add the folders to your project
		var strSrcFilter = wizard.FindSymbol('SOURCE_FILTER');
		var group = proj.Object.AddFilter('Source Files');
		group.Filter = strSrcFilter;

		group = group.AddFilter('RTL');
		group.Filter = strSrcFilter;

		strSrcFilter = wizard.FindSymbol('INCLUDE_FILTER');
		group = proj.Object.AddFilter('Header Files');
		group.Filter = strSrcFilter;
	}
	catch (e) {
		throw e;
	}
}

function GetRuntimeFilter(obj){
	try{
		var flts = obj.Filters;
		if(flts == null)
			return null;
		for(i = 1; i <= flts.Count; i++){
			var flt = flts.Item(i);
			//alert(flt.Name);
			if(flt.Name == 'RTL')
				return flt;
			else if(flt.Name == 'Source Files'){
				return GetRuntimeFilter(flt);
			}
		}
		return null;
	}catch(e){
		throw e;
	}
}

function AddRuntimeSources(proj){
	try {
		// Add runtime sources to project
    /*
     *	crt, exc, rtti, ios, fpu
     *  exc: crt, [rtti]
     *  ios: crt, no km
     *  fpu: crt, no km
     */
		var rtl = GetRuntimeFilter(proj.Object);
		if(rtl == null){
			//alert("cannot find a RTL filter :(");
			return;
		}
		var rtlPath = NtlBasePath + "/rtl/";
		var sources = {
			'NTL_RUNTIME_CRT': ['crt.cpp'],
			'NTL_RUNTIME_EXC': ['eh.cpp', 'ehsup.asm'],
			'NTL_RUNTIME_RTT': ['rtti.cpp'],
			'NTL_RUNTIME_IOS': ['iostream.cpp'],
			'NTL_RUNTIME_FLT': ['math.cpp', 'msvcrt.cpp']
		};
		for(sym in sources){
			if(!wizard.FindSymbol(sym))
				continue;
			// add sources
			var files = sources[sym];
			for(i = 0; i < files.length; i++){
				rtl.AddFile(rtlPath + files[i]);
		};
	};
	}
	catch (e) {
		throw e;
	}
}

// Configurations data
var nNumConfigs = 2;

var astrConfigName = new Array();
astrConfigName[0] = "debug";
astrConfigName[1] = "release";

function AddConfigurations(proj, strProjectName) {
	try {
		var nCntr;
		for (nCntr = 0; nCntr < nNumConfigs; nCntr++) {
			// Check if it's Debug configuration
			var bDebug = false;
			if (astrConfigName[nCntr].search("debug") != -1)
				bDebug = true;

			// General settings
			var config = proj.Object.Configurations(astrConfigName[nCntr]);

			if (wizard.FindSymbol("NTL_USE_UNICODE"))
				config.CharacterSet = charSetUnicode;
			else
				config.CharacterSet = charSetNotSet;
			
			if (wizard.FindSymbol("NTL_APPTYPE_DLL"))
				config.ConfigurationType = typeDynamicLibrary;

			//config.IntermediateDirectory = '$(PlatformName)\\$(ConfigurationName)';
			config.OutputDirectory = '$(PlatformName)\\$(ConfigurationName)';

			// Compiler settings
			var CLTool = config.Tools('VCCLCompilerTool');
			CLTool.UsePrecompiledHeader = pchNone;
			CLTool.WarningLevel = warningLevel_3 + !bDebug;
			CLTool.RuntimeLibrary = rtMultiThreaded;
			CLTool.DebugInformationFormat = debugEnabled;
			CLTool.BasicRuntimeChecks = runtimeBasicCheckNone;
			CLTool.Detect64BitPortabilityProblems = false;
			CLTool.BufferSecurityCheck = false;
			if (bDebug) {
				CLTool.MinimalRebuild = true;
				CLTool.Optimization = optimizeDisabled;
			} else {
				CLTool.StringPooling = true;
				CLTool.InlineFunctionExpansion = expandAnySuitable;
				CLTool.Optimization = optimizeFull;
				config.WholeProgramOptimization = WholeProgramOptimizationLinkTimeCodeGen;
			}
			
			if (wizard.FindSymbol("NTL_RUNTIME_EXC"))
				CLTool.ExceptionHandling = cppExceptionHandlingYes;
			if (wizard.FindSymbol("NTL_RUNTIME_FLT"))
				CLTool.EnableEnhancedInstructionSet = enhancedInstructionSetTypeSIMD;	// we have no sse2 library support
			

			var strDefines = GetPlatformDefine(config);
			if (wizard.FindSymbol("NTL_APPTYPE_DRIVER"))
				strDefines += "NTL_SUBSYSTEM_KM;";
			if (bDebug)
				strDefines += "_DEBUG";
			else
				strDefines += "NDEBUG";
			CLTool.PreprocessorDefinitions = strDefines;

			// Linker settings
			var LinkTool = config.Tools('VCLinkerTool');
			LinkTool.TargetMachine = machineX86;
			LinkTool.GenerateDebugInformation = true;
			LinkTool.GenerateManifest = false;

			// debug linker options
			if (bDebug) {
				LinkTool.LinkIncremental = linkIncrementalYes;
			} else {
				LinkTool.SetChecksum = true;
				LinkTool.LinkIncremental = linkIncrementalNo;
			}
			
			// driver
			if (wizard.FindSymbol("NTL_APPTYPE_DRIVER")) {
				LinkTool.driver = driver;
			}

			// subsystem
			if (wizard.FindSymbol("NTL_APPTYPE_CONSOLE")){
				LinkTool.SubSystem = subSystemConsole;
			}else if(wizard.FindSymbol("NTL_APPTYPE_WIN32") || wizard.FindSymbol("NTL_APPTYPE_DLL")){
				LinkTool.SubSystem = subSystemWindows;
			}else{
				LinkTool.SubSystem = subSystemNative;
				LinkTool.SetChecksum = true;
				LinkTool.LinkIncremental = linkIncrementalNo;
			}
			
			// Property sheets
			var WizardVersion = wizard.FindSymbol('WIZARD_VERSION');
			var wizardVersion = parseFloat(WizardVersion);
			var vsprops = wizardVersion >= 10 ? "props" : "vsprops";
			
			var SheetPath = NtlBasePath + "/../samples/vstudio/";
			var sheets = SheetPath + "ntl-props." + vsprops;
			if(WizardVersion == "8.0")
				sheets += ";" + SheetPath + "ntl-x86-8." + vsprops;
			else
				sheets += ";" + SheetPath + "ntl-x86." + vsprops;
			
			if(wizard.FindSymbol("NTL_APPTYPE_DRIVER"))
				sheets += ";" + SheetPath + "ntl-km." + vsprops;
			
			try {
				config.InheritedPropertySheets = sheets;
			}catch(e){
				sheets = "";
			}
			
			

		}
	}
	catch (e) {
		throw e;
	}
}

function GetTargetName(strName, strProjectName) {
	try {
		var strTarget = strName;
		
		return strTarget;
	}
	catch (e) {
		throw e;
	}
}

function SetFileProperties(projfile, strName)
{
	return false;
}