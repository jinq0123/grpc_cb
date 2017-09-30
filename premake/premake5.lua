-- premake5.lua
--[[
Usage examples: 
	for windows: premake5.exe --os=windows vs2015
	fot linux:   premake5.exe --os=linux gmake
]]

workspace "grpc_cb"
	location (_ACTION)  -- subdir vs2015 (or gmake, ...)
	configurations { "Release", "Debug" }
	platforms { "x64", "x32" }
	language "C++"
	flags {
		"C++11",
		-- "StaticRuntime",
	}

	require("conanpremake_multi")  -- for third-party libs

	includedirs {
		"../include",
	}

	filter "configurations:Debug"
		flags { "Symbols" }
		links {
			"libprotobufd",
		}
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
		links {
			"libprotobuf",
		}
	filter "system:windows"
		defines {
			"_WIN32_WINNT=0x0600"  -- i.e. Windows 7 target
		}
		links {
			"ws2_32",
		}
	filter {}

project "grpc_cpp_cb_plugin"
	kind "ConsoleApp"
	files {
		"../src/compiler/**",
	}
	links {
		"grpc_plugin_support",
	}

	filter "configurations:Debug"
		links {
			"libprotocd",
		}
	filter "configurations:Release"
		links {
			"libprotoc",
		}
	filter {}

project "grpc_cb"
	kind "StaticLib"
	includedirs {
		"../src/cpp_cb",
	}
	files {
		"../include/grpc_cb/**.h",
		"../src/cpp_cb/**",
	}

group "examples"

	project "greeter_cb_client"
		kind "ConsoleApp"
		files {
			"../examples/cpp_cb/helloworld/**",
		}
		removefiles {
			"../examples/cpp_cb/helloworld/greeter_cb_server.cc",
		}
		links { "grpc_cb" }

	project "greeter_cb_server"
		kind "ConsoleApp"
		files {
			"../examples/cpp_cb/helloworld/**",
		}
		removefiles {
			"../examples/cpp_cb/helloworld/greeter_cb_client.cc",
		}
		links { "grpc_cb" }

	project "route_guide_cb_client"
		kind "ConsoleApp"
		files {
			"../examples/cpp_cb/route_guide/**",
		}
		removefiles {
			"../examples/cpp_cb/route_guide/route_guide_cb_server.cc",
		}
		links { "grpc_cb" }

	project "route_guide_cb_server"
		kind "ConsoleApp"
		files {
			"../examples/cpp_cb/route_guide/**",
		}
		removefiles {
			"../examples/cpp_cb/route_guide/route_guide_cb_client.cc",
		}
		links { "grpc_cb" }

group ""  -- End of group "examples"
