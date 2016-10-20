-- premake5.lua
--[[
Usage examples: 
	for windows: premake5.exe --os=windows vs2015
	fot linux:   premake5.exe --os=linux gmake
]]

grpc_root = "../third_party/grpc"

workspace "grpc_cb"
	configurations { "Debug", "Release" }
	language "C++"
	flags {
		"C++11",
		"StaticRuntime",
	}
	includedirs {
		"../include",
		grpc_root .. "/include"
	}
	
	filter "configurations:Debug"
		flags { "Symbols" }
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
	filter {}

project "grpc_cb"
	kind "StaticLib"
	files {
		"../include/grpc_cb/**.h",
		"../src/cpp_cb/**",
	}

project "greeter_cb_client"
	kind "ConsoleApp"
	files {
		"../examples/cpp_cb/helloworld/**",
	}
	removefiles {
		"../examples/cpp_cb/helloworld/greeter_cb_server.cc",
	}
	links {
		"grpc_cb",
	}
project "greeter_cb_server"
	kind "ConsoleApp"
	files {
		"../examples/cpp_cb/helloworld/**",
	}
	removefiles {
		"../examples/cpp_cb/helloworld/greeter_cb_client.cc",
	}
	links {
		"grpc_cb",
	}
project "route_guide_cb_client"
	kind "ConsoleApp"
	files {
		"../examples/cpp_cb/route_guide/**",
	}
	removefiles {
		"../examples/cpp_cb/route_guide/route_guide_cb_server.cc",
	}
	links {
		"grpc_cb",
	}
project "route_guide_cb_server"
	kind "ConsoleApp"
	files {
		"../examples/cpp_cb/route_guide/**",
	}
	removefiles {
		"../examples/cpp_cb/route_guide/route_guide_cb_client.cc",
	}
	links {
		"grpc_cb",
	}

project "grpc_cpp_cb_plugin"
	kind "ConsoleApp"
	files {
		"../src/compiler/**",
	}
	includedirs {
		"..",
		grpc_root,
		grpc_root .. "/third_party/protobuf/src",
	}
	libdirs {
		grpc_root .. "/vsprojects/%{cfg.buildcfg}",
		grpc_root .. "/third_party/protobuf/cmake/%{cfg.buildcfg}",
	}
	links {
		"grpc_plugin_support",
	}
	filter "configurations:Debug"
		links {
			"libprotocd",
			"libprotobufd",
		}
	filter "configurations:Release"
		links {
			"libprotoc",
			"libprotobuf",
		}
	filter {}
