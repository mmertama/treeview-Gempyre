cmake_minimum_required (VERSION 3.14)

set(NAME treeview)
project (${NAME})

find_package (gempyre REQUIRED)
include(gempyre)

add_compile_options(-DNOFS)

if(NOT MSVC AND (NOT APPLE) AND (NOT  UNIX))
add_link_options(-no-pie)
endif()

set(CMAKE_CXX_STANDARD 17)

include_directories(
    include
    )


add_executable(${PROJECT_NAME}
    src/main.cpp
    gui/${NAME}.html
    )

gempyre_add_resources(PROJECT ${PROJECT_NAME} TARGET include/${NAME}_resource.h SOURCES gui/${NAME}.html gui/tree.css gui/favicon.ico)
	
target_link_libraries (${PROJECT_NAME} gempyre::gempyre)


