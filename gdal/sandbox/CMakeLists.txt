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
    FUNCTIONS GDALOpen GDALAllRegister GDALGetDatasetDriver GDALCreate GDALGetDriverByName
              GDALGetRasterBand GDALSetProjection GDALSetGeoTransform  
              GDALSetRasterNoDataValue GDALRasterIO GDALClose
        GDALGetDriverShortName GDALGetDriverLongName GDALGetRasterXSize 
        GDALGetRasterYSize GDALGetRasterCount GDALGetProjectionRef
        GDALOpenEx GDALInfo
    
    INPUTS "../gcore/gdal.h" "../apps/gdalinfo_bin.cpp" "../apps/gdalinfo_lib.cpp"
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

add_library(data_retriever STATIC
  get_raster_data.h get_raster_data.cc
)

target_link_libraries(data_retriever
  libgdal
)

add_executable(raster_to_gtiff
    raster_to_gtiff.cc
)

target_link_libraries(raster_to_gtiff
    data_retriever
    gdal_sapi
    sapi::sapi
)
