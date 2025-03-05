set_xmakever("2.9.8")
set_project("WinMMWRP")

set_allowedplats("windows")
set_allowedmodes("debug", "release")
set_allowedarchs("x86", "x86_64", "x64", "arm64")
	
add_rules("mode.release", "mode.debug")
set_languages("clatest", "cxxlatest")
set_runtimes("stdc++_static")

target("WinMMWRP")
    set_basename("winmm")
	set_kind("shared")

	if is_mode("debug") then
		add_defines("DEBUG")
		add_defines("_DEBUG")
		set_symbols("debug")
		set_optimize("none")
	else	
		add_defines("NDEBUG")
		set_symbols("none")
		set_optimize("fastest")
		set_strip("all")
	end
	
	add_defines("OMNIMIDI_EXPORTS")

	add_ldflags("-j")

	add_includedirs("inc")
	add_files("src/*.cpp")
    add_files("*.def")

	if is_plat("windows") then	
		set_toolchains("mingw")

		-- Remove lib prefix
		set_prefixname("")

		add_defines("_WIN32", "WINXPMODE", "_WIN32_WINNT=0x6000")
		add_shflags("-static-libgcc", { force = true })

		if is_arch("x86") then 
			add_shflags("x86exports.def", { force = true })
		elseif is_arch("x64") then 
			add_shflags("amd64exports.def", { force = true })
		end

		add_syslinks("uuid", "shlwapi", "shell32", "user32", "ole32", "-l:libwinpthread.a")
	else
		remove_files("*.cpp")
	end
target_end()