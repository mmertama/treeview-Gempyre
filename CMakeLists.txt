cmake_minimum_required (VERSION 3.8)

set(NAME treeview)
project (${NAME})

set(GEMPYRE_PATH $ENV{GEMPYRE_DIR})
find_package (gempyre REQUIRED PATHS ${GEMPYRE_PATH})
include(${GEMPYRE_PATH}/scripts/addResource.cmake_script)

add_compile_options(-DNOFS)

if(NOT MSVC AND (NOT APPLE) AND (NOT  UNIX))
add_link_options(-no-pie)
endif()

set(CMAKE_CXX_STANDARD 17)

include_directories(
     ${GEMPYRE_PATH}/gempyrelib/include
    include
    )


add_executable(${PROJECT_NAME}
    src/main.cpp
    gui/${NAME}.html
    )

addResource(PROJECT ${PROJECT_NAME} TARGET include/${NAME}_resource.h SOURCES gui/${NAME}.html gui/tree.css gui/favicon.ico)
	
target_link_libraries (${PROJECT_NAME} gempyre)


