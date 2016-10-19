-- premake5.lua
--[[
Usage example: 
	for windows: premake5.exe --os=windows vs2015
	fot linux:   premake5.exe --os=linux gmake
]]

grpc_root = "../third_party/grpc"

workspace "gRPC_cb"
	configurations { "Debug", "Release" }
	language "C++"
	flags {
		"C++11",
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
		"../src/grpc_cb/**.*",
	}

project "greeter_cb_client"
	kind "ConsoleApp"
	files {
		"../example/helloworld/**.*",
	}
	removefiles {
		"../example/helloworld/greeter_cb_server.cc",
	}
	links {
		"grpc_cb",
	}
project "greeter_cb_server"
	kind "ConsoleApp"
	files {
		"../example/helloworld/**.*",
	}
	removefiles {
		"../example/helloworld/greeter_cb_client.cc",
	}
	links {
		"grpc_cb",
	}
project "route_guide_cb_client"
	kind "ConsoleApp"
	files {
		"../example/route_guide/**.*",
	}
	removefiles {
		"../example/route_guide/route_guide_cb_server.cc",
	}
	links {
		"grpc_cb",
	}
project "route_guide_cb_server"
	kind "ConsoleApp"
	files {
		"../example/route_guide/**.*",
	}
	removefiles {
		"../example/route_guide/route_guide_cb_client.cc",
	}
	links {
		"grpc_cb",
	}

project "grpc_cpp_cb_plugin"
	kind "ConsoleApp"
	files {
		"../src/compiler/**.*",
	}
