cmake_minimum_required(VERSION 3.16)

project(example_imgui_printer_proj C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/project/intermediates)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/project/intermediates)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/$<0:>) 

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/deps")

# 
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui_software_renderer/src")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/mini_printer")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/imgui_printer")

#
file(GLOB imgui_src   
	"${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui/*.cpp" 
	"${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui/*.h"
)
file(GLOB imguisr_src   
	"${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui_software_renderer/src/imgui_sw.cpp" 
	"${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui_software_renderer/src/imgui_sw.hpp" 
)
file(GLOB mini_printer_src   
	"${CMAKE_CURRENT_SOURCE_DIR}/mini_printer/*.cpp" 
	"${CMAKE_CURRENT_SOURCE_DIR}/mini_printer/*.h"
)
file(GLOB imgui_printer_src   
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui_printer/*.cpp" 
	"${CMAKE_CURRENT_SOURCE_DIR}/imgui_printer/*.h"
)

#  
file(GLOB example_src   
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h" 
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" 
)
add_executable(example_imgui_printer ${example_src} ${imgui_src} ${imguisr_src} ${mini_printer_src} ${imgui_printer_src})

source_group("sources"  FILES  ${example_src})
source_group("sources/imgui"  FILES ${imgui_src})
source_group("sources/imgui_software_renderer"  FILES ${imguisr_src})
source_group("sources/mini_printer"  FILES ${mini_printer_src})
source_group("sources/imgui_printer"  FILES ${imgui_printer_src})

set_target_properties(example_imgui_printer PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
