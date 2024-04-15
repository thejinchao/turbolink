
using UnrealBuildTool;
using System;
using System.IO;
using System.Collections.Generic;
using System.Reflection;

public class TurboLinkGrpc : ModuleRules
{
	private static TurboLinkPlatform TurboLinkPlatformInstance;

	public TurboLinkGrpc(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		TurboLinkPlatformInstance = GetTurboLinkPlatformInstance(Target);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Serialization",
				"Networking",
				"DeveloperSettings"
			}
		);
		AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenSSL");
		AddEngineThirdPartyPrivateStaticDependencies(Target, "zlib");

		PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private/pb"));

		//ThirdParty include
		PrivateIncludePaths.AddRange(
			new string[] {
				Path.Combine(ThirdPartyRoot(), "protobuf/include"),
				Path.Combine(ThirdPartyRoot(), "grpc/include"),
				Path.Combine(ThirdPartyRoot(), "re2/include"),
				Path.Combine(ThirdPartyRoot(), "abseil/include")
			}
		);
		
		//ThirdParty Libraries
		ConfigurePlatform(Target.Platform.ToString(), Target.Configuration);
	}

	private TurboLinkPlatform GetTurboLinkPlatformInstance(ReadOnlyTargetRules Target)
	{
		var TurboLinkPlatformType = System.Type.GetType("TurboLinkPlatform_" + Target.Platform.ToString());
		if (TurboLinkPlatformType == null)
		{
			throw new BuildException("TurboLink does not support platform " + Target.Platform.ToString());
		}

		var PlatformInstance = Activator.CreateInstance(TurboLinkPlatformType) as TurboLinkPlatform;
		if (PlatformInstance == null)
		{
			throw new BuildException("TurboLink could not instantiate platform " + Target.Platform.ToString());
		}

		return PlatformInstance;
	}

	protected string ThirdPartyRoot()
	{
		return Path.GetFullPath(Path.Combine(ModuleDirectory, "../ThirdParty/"));
	}
	
	private List<string> GrpcLibs = new List<string> 
	{
		"grpc", "grpc++", "gpr", "upb", "address_sorting"
	};
	private List<string> ProtobufLibs = new List<string> 
	{
		"protobuf", "utf8_validity", "utf8_range"
	};
	private List<string> AbseilLibs = new List<string>
	{
		"absl_log_internal_check_op", "absl_leak_check", "absl_die_if_null",
		"absl_log_internal_conditions", "absl_log_internal_message",
		"absl_log_internal_nullguard", "absl_examine_stack", "absl_log_internal_format",
		"absl_log_internal_proto", "absl_log_internal_log_sink_set", "absl_log_sink",
		"absl_log_entry", "absl_log_initialize", "absl_log_globals",
		"absl_log_internal_globals", "absl_statusor", "absl_flags", "absl_flags_internal",
		"absl_flags_reflection", "absl_hash", "absl_city", "absl_low_level_hash",
		"absl_raw_hash_set", "absl_hashtablez_sampler", "absl_flags_config",
		"absl_flags_program_name", "absl_flags_private_handle_accessor",
		"absl_flags_commandlineflag", "absl_flags_commandlineflag_internal",
		"absl_status", "absl_cord", "absl_cordz_info", "absl_cord_internal",
		"absl_cordz_functions", "absl_exponential_biased", "absl_cordz_handle",
		"absl_crc_cord_state", "absl_crc32c", "absl_crc_internal", "absl_crc_cpu_detect",
		"absl_strerror", "absl_synchronization", "absl_stacktrace", "absl_symbolize",
		"absl_debugging_internal", "absl_demangle_internal", "absl_graphcycles_internal",
		"absl_malloc_internal", "absl_time", "absl_civil_time", "absl_time_zone",
		"absl_bad_variant_access", "absl_flags_marshalling", "absl_str_format_internal",
		"absl_random_distributions", "absl_random_seed_sequences", "absl_random_internal_pool_urbg",
		"absl_random_internal_randen", "absl_random_internal_randen_hwaes",
		"absl_random_internal_randen_hwaes_impl", "absl_random_internal_randen_slow",
		"absl_random_internal_platform", "absl_random_internal_seed_material",
		"absl_bad_optional_access", "absl_strings", "absl_throw_delegate", "absl_int128",
		"absl_strings_internal", "absl_base", "absl_spinlock_wait", "absl_raw_logging_internal",
		"absl_log_severity", "absl_random_seed_gen_exception"
	};
	private List<string> Re2Libs = new List<string>
	{
		"re2"
	};

	private bool ConfigurePlatform(string Platform, UnrealTargetConfiguration Configuration)
	{
		//turbolink thirdparty libraries root path
		string root = ThirdPartyRoot();
		
		//grpc
		foreach(var lib in GrpcLibs)
		{
			foreach(var arch in TurboLinkPlatformInstance.Architectures())
			{
				string fullPath = root + "grpc/" + "lib/" +
					TurboLinkPlatformInstance.LibrariesPath + arch +
					TurboLinkPlatformInstance.ConfigurationDir(Configuration) +
					TurboLinkPlatformInstance.LibraryPrefixName + lib + TurboLinkPlatformInstance.LibraryPostfixName;
				PublicAdditionalLibraries.Add(fullPath);
			}
		}
		//protobuf
		foreach(var lib in ProtobufLibs)
		{
			foreach (var arch in TurboLinkPlatformInstance.Architectures())
			{
				string libPrefixName = TurboLinkPlatformInstance.LibraryPrefixName;
				if (TurboLinkPlatformInstance is TurboLinkPlatform_Win64 && lib == "protobuf")
				{
					libPrefixName = "lib"; //'libprotobuf.lib'
				}

				string fullPath = root + "protobuf/" + "lib/" +
					TurboLinkPlatformInstance.LibrariesPath + arch +
					TurboLinkPlatformInstance.ConfigurationDir(Configuration) +
					libPrefixName + lib + TurboLinkPlatformInstance.LibraryPostfixName;
				PublicAdditionalLibraries.Add(fullPath);
			}
		}
		//abseil
		foreach (var lib in AbseilLibs)
		{
			foreach (var arch in TurboLinkPlatformInstance.Architectures())
			{
				string fullPath = root + "abseil/" + "lib/" +
					TurboLinkPlatformInstance.LibrariesPath + arch +
					TurboLinkPlatformInstance.ConfigurationDir(Configuration) +
					TurboLinkPlatformInstance.LibraryPrefixName + lib + TurboLinkPlatformInstance.LibraryPostfixName;
				PublicAdditionalLibraries.Add(fullPath);
			}
		}
		//re2
		foreach (var lib in Re2Libs)
		{
			foreach (var arch in TurboLinkPlatformInstance.Architectures())
			{
				string fullPath = root + "re2/" + "lib/" +
					TurboLinkPlatformInstance.LibrariesPath + arch +
					TurboLinkPlatformInstance.ConfigurationDir(Configuration) +
					TurboLinkPlatformInstance.LibraryPrefixName + lib + TurboLinkPlatformInstance.LibraryPostfixName;
				PublicAdditionalLibraries.Add(fullPath);
			}
		}
		return false;
	}
}

public abstract class TurboLinkPlatform
{
	public virtual string ConfigurationDir(UnrealTargetConfiguration Configuration)
	{
		if (Configuration == UnrealTargetConfiguration.Debug || Configuration == UnrealTargetConfiguration.DebugGame)
		{
			return "Debug/";
		}
		else
		{
			return "Release/";
		}
	}
	public abstract string LibrariesPath { get; }
	public abstract List<string> Architectures();
	public abstract string LibraryPrefixName { get; }
	public abstract string LibraryPostfixName { get; }
}

public class TurboLinkPlatform_Win64 : TurboLinkPlatform
{
	public override string ConfigurationDir(UnrealTargetConfiguration Configuration)
	{
		if (Configuration == UnrealTargetConfiguration.Debug || Configuration == UnrealTargetConfiguration.DebugGame)
		{
			return "RelWithDebInfo/";
		}
		else
		{
			return "Release/";
		}
	}
	public override string LibrariesPath { get { return "win64/"; } }
	public override List<string> Architectures() { return new List<string> { "" }; }
	public override string LibraryPrefixName { get { return ""; } }
	public override string LibraryPostfixName { get { return ".lib"; } }
}

public class TurboLinkPlatform_Android : TurboLinkPlatform
{
	public override string LibrariesPath { get { return "android/"; } }
	public override List<string> Architectures() { return new List<string> {
#if UE_5_0_OR_LATER
		"arm64-v8a/", "x86_64/" 
#else
		"armeabi-v7a/", "arm64-v8a/", "x86_64/" 
#endif
	}; }
	public override string LibraryPrefixName { get { return "lib"; } }
	public override string LibraryPostfixName { get { return ".a"; } }
}

public class TurboLinkPlatform_Linux : TurboLinkPlatform
{
	public override string LibrariesPath { get { return "linux/"; } }
	public override List<string> Architectures() { return new List<string> { "" }; }
	public override string LibraryPrefixName { get { return "lib"; } }
	public override string LibraryPostfixName { get { return ".a"; } }
}

public class TurboLinkPlatform_PS5 : TurboLinkPlatform
{
	public override string LibrariesPath { get { return "ps5/"; } }
	public override List<string> Architectures() { return new List<string> { "" }; }
	public override string LibraryPrefixName { get { return "lib"; } }
	public override string LibraryPostfixName { get { return ".a"; } }
}

public class TurboLinkPlatform_Mac : TurboLinkPlatform
{
	public override string ConfigurationDir(UnrealTargetConfiguration Configuration)
	{
		return "";
	}
	public override string LibrariesPath { get { return "mac/"; } }
	public override List<string> Architectures() { return new List<string> { "" }; }
	public override string LibraryPrefixName { get { return "lib"; } }
	public override string LibraryPostfixName { get { return ".a"; } }
}

public class TurboLinkPlatform_IOS : TurboLinkPlatform
{
	public override string ConfigurationDir(UnrealTargetConfiguration Configuration)
	{
		return "";
	}
	public override string LibrariesPath { get { return "ios/"; } }
	public override List<string> Architectures() { return new List<string> { "" }; }
	public override string LibraryPrefixName { get { return "lib"; } }
	public override string LibraryPostfixName { get { return ".a"; } }
}
