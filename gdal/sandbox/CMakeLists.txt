cmake_minimum_required(VERSION 3.10)

project(test CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SAPI_ROOT "" CACHE PATH "Path to the Sandboxed API source tree")
#   cmake .. -G Ninja -DSAPI_ROOT=$HOME/sapi_root


set(SAPI_ENABLE_EXAMPLES OFF CACHE BOOL "")
set(SAPI_ENABLE_TESTS OFF CACHE BOOL "")
add_subdirectory("${SAPI_ROOT}"
                 "${CMAKE_BINARY_DIR}/sandboxed-api-build"
                 # Omit this to have the full Sandboxed API in IDE
                 EXCLUDE_FROM_ALL)

add_library(libgdal STATIC IMPORTED)
set_property(TARGET libgdal PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/libgdal.a")
                 
target_link_libraries(libgdal INTERFACE
    crypto expat jpeg 
    /usr/local/lib/libproj.so 
    sqlite3 tiff z pthread m rt dl curl)

add_sapi_library(gdal_sapi
    FUNCTIONS GDALOpen
    
    INPUTS "../gcore/gdal.h"
    LIBRARY libgdal
    LIBRARY_NAME gdal
    
    NAMESPACE ""
)

target_include_directories(gdal_sapi INTERFACE
    "${PROJECT_BINARY_DIR}"
)

add_executable(main
    main.cc
)

target_link_libraries(main
    gdal_sapi
    sapi::sapi
)