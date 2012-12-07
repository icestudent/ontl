-- psinfo2 premake file
-- To build use premake4 tool (http://industriousone.com/premake)

-- Configure paths to the dependent projects
newoption {
  trigger = "ntldir",
  description = "Provide directory to the oNTL (e.g. 'C:\\ontl\\branches\\x64\\ntl')",
  value = "path"
}
newoption {
  trigger = "ddkdir",
  description = "Provide directory to the DDK libraries (e.g. C:\\DDK\\6000 or C:\\DDK\\6000\\lib\\wnet)",
  value = "path"
}

newoption {
  trigger = 'arch',
  description = 'Specify the target platform',
  value = 'value',
  allowed = {
    { 'x86', '32-bit mode'},
    { 'x64', '64-bit mode'}
  }
}

-- Solution and project configuration
solution "psinfo"
  configurations { "debug", "release" }
  platforms { "x32", "x64" }
  targetdir "out"
  objdir    "out"

project  "psinfo2"
  language  "C++"
  kind      "ConsoleApp"
  flags { "ExtraWarnings", "NoPCH", "NoEditAndContinue", "NoExceptions", "No64BitChecks", "NoManifest", "StaticRuntime", "Unicode" }
  buildoptions { "/MT", "/GS-", "/Gy-" }
  linkoptions  { "/incremental:no", "/nodefaultlib:libcmt.lib", "/nodefaultlib:libcmtd.lib" }
	links { "ntdll" }
	flags { "WinMain", "Optimize" }

  files {
    -- sources
    "psinfo.cpp"
  }

  if _ACTION and _ACTION ~= 'clean' then
  	if _OPTIONS["ntldir"] then 
	    local ontl = _OPTIONS["ntldir"] 
	    includedirs { ontl }
	    ontl = ontl .. '/rtl/'
	    files {
	    	ontl .. 'crt.cpp',
	    	ontl .. 'iostream.cpp',
	    	ontl .. 'wchar_mask_data.cpp'
	    }
	  else 
	    print("Warning: path to the oNTL doesn't specified! See `premake4 --help` for options")
	  end
  end



function get_libdir(arch)
  -- skip '--help' action
  if (not _ACTION) or _ACTION == 'clean' then return end
  -- 'ddkdir' required
  if not _OPTIONS["ddkdir"] then
    print("Error: path to the DDK required! See `premake4 --help` for options")
    return
  end
  local ddk = _OPTIONS["ddkdir"]
  local libarch = iif(arch == 'x64', 'amd64', 'i386')
  local nt = ddk .. "/" .. libarch
  if not os.isfile(nt .. "/ntdll.lib") then
    local nt2 = ddk .. "/lib/wnet/" .. libarch
    if not os.isfile(nt2 .. "/ntdll.lib") then 
      print("Error: can't find 'ntdll.lib' in provided paths!\n" .. string.format("Looked up in '%s' and '%s'\n", nt, nt2))
      return
    end
    nt = nt2
  end
  return nt
end


configuration "debug"
	defines { "DEBUG" }
  flags { "Symbols" }
  buildoptions { "/Od" }
configuration "release"
	defines { "NDEBUG" }
  flags { "Optimize" }
  buildoptions{ "/Ob2ity", "/GL" }
  linkoptions { "/release", "/LTCG" }
 
configuration "x32"
  libdirs { get_libdir('x32') }
configuration "x64"
  linkoptions { "/machine:x64" }
  libdirs { get_libdir('x64') }


