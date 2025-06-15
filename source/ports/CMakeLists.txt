# Check if ports are enabled
if(NOT OPTION_BUILD_PORTS)
	return()
endif()

#
# Project configuration options
#

# Project options
option(OPTION_BUILD_PORTS_GO	"Build Go port."				OFF)
option(OPTION_BUILD_PORTS_JS	"Build JavaScript port."		OFF)
option(OPTION_BUILD_PORTS_NODE	"Build NodeJS port."			ON)
option(OPTION_BUILD_PORTS_PY	"Build Python port."			ON)

#
# Port languages
#

add_subdirectory(go_port)
add_subdirectory(js_port)
add_subdirectory(node_port)
add_subdirectory(py_port)
