// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class UnrealAISandbox : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../Extensions/")); }
    }

    private string FluxIncludesPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../../")); }
    }

    public UnrealAISandbox(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        LoadLibraries(Target);
    }

    public bool LoadLibraries(ReadOnlyTargetRules Target)
    {
        bool isLibrarySupported = false;

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            isLibrarySupported = true;

            string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
            string LibrariesPath = Path.Combine(ThirdPartyPath, "Libraries");

            /*
            test your path with:
            using System; // Console.WriteLine("");
            Console.WriteLine("... LibrariesPath -> " + LibrariesPath);
            */

            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, PlatformString, "fluxNEAT.Common.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, PlatformString, "fluxNEAT.Training.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, PlatformString, "mlpack.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, PlatformString, "libboost_serialization-vc141-mt-x64-1_71.lib"));
        }
        
        if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            isLibrarySupported = true;

            string PlatformString = "Mac";
            string LibrariesPath = Path.Combine(ThirdPartyPath, "Libraries");

            /*
            test your path with:
            using System; // Console.WriteLine("");
            Console.WriteLine("... LibrariesPath -> " + LibrariesPath);
            */

            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, PlatformString, "libfluxNEAT.Common.a"));
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, PlatformString, "libfluxNEAT.Training.a"));
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, PlatformString, "libmlpack.a"));
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, PlatformString, "libboost_serialization.a"));
        }

        if (isLibrarySupported)
        {
            // Include path
            PublicIncludePaths.Add(Path.Combine(FluxIncludesPath, "Common", "include"));
            PublicIncludePaths.Add(Path.Combine(FluxIncludesPath, "Training", "include"));
        }

        Definitions.Add(string.Format("FLUX_NEAT_SUPPORTED={0}", isLibrarySupported ? 1 : 0));

        return isLibrarySupported;
    }
}
