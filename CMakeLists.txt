cmake_minimum_required(VERSION 3.7)
project(easyrpg-tools VERSION 1.0 LANGUAGES CXX)

option(DISABLE_LCF2XML "Disable lcf2xml converter tool" OFF)
option(DISABLE_LMU2PNG "Disable lmu2png converter tool" OFF)
option(DISABLE_PNG2XYZ "Disable png2xyz converter tool" OFF)
option(DISABLE_XYZ2PNG "Disable xyz2png converter tool" OFF)
option(DISABLE_GENCACHE "Disable gencache tool" OFF)
option(DISABLE_XYZCRUSH "Disable xyzcrush tool" OFF)

set(CMAKE_MODULE_PATH APPEND "${CMAKE_CURRENT_SOURCE_DIR}/Modules")

function(enable_tool tool)
	string(TOUPPER ${tool} tool_upper)
	if(NOT DISABLE_${tool_upper})
		add_subdirectory(${tool})
	endif()
endfunction()

macro(tool_is_enabled tool)
	string(TOUPPER ${tool} tool_upper)
	if(NOT DISABLE_${tool_upper})
		set(TOOL_ENABLED "yes")
	else()
		set(TOOL_ENABLED "no")
	endif()
	mark_as_advanced(TOOL_ENABLED)
	mark_as_advanced(tool_upper)
endmacro()

foreach(tool lcf2xml lmu2png png2xyz xyz2png gencache xyzcrush)
	enable_tool(${tool})
endforeach()

message(STATUS "")
message(STATUS "Summary:")
message(STATUS "========")
message(STATUS "")
message(STATUS "  prefix:   ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
message(STATUS "Converters:")
foreach(tool lcf2xml lmu2png png2xyz xyz2png)
	tool_is_enabled(${tool})
	message(STATUS "  ${tool}:  ${TOOL_ENABLED}")
endforeach()
message(STATUS "")
message(STATUS "Other:")
foreach(tool xyzcrush gencache)
	tool_is_enabled(${tool})
	message(STATUS "  ${tool}: ${TOOL_ENABLED}")
endforeach()
message(STATUS "")
