 
# Copyright 2020 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.10)

project(gdal CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(gdal STATIC
    gcore/gdal.h
)

# Change sapi root 
set(SAPI_ROOT "/usr/local/google/home/inach/work/sandboxed-api" CACHE PATH "Path to the Sandboxed API source tree")

set(SAPI_ENABLE_EXAMPLES OFF CACHE BOOL "")
set(SAPI_ENABLE_TESTS OFF CACHE BOOL "")

add_subdirectory("${SAPI_ROOT}"
            "${CMAKE_BINARY_DIR}/sandboxed-api-build"
            EXCLUDE_FROM_ALL)

# Add all the functions 
add_sapi_library(gdal_sapi

    FUNCTIONS GDALGetDataTypeSize
              GDALGetDataTypeSizeBits
              GDALGetDataTypeSizeBytes
              GDALDataTypeIsComplex
              GDALDataTypeIsInteger
              GDALDataTypeIsFloating
              GDALDataTypeIsSigned
              GDALGetDataTypeName
              GDALGetDataTypeByName
              GDALDataTypeUnion
              GDALDataTypeUnionWithValue
              GDALFindDataType
              GDALFindDataTypeForValue
              GDALAdjustValueToDataType
              GDALGetNonComplexDataType
              GDALDataTypeIsConversionLossy
              GDALGetAsyncStatusTypeName
              GDALGetAsyncStatusTypeByName
              GDALGetColorInterpretationName
              GDALGetColorInterpretationByName
              GDALGetPaletteInterpretationName
              GDALAllRegister
              GDALCreate
              GDALIdentifyDriver
              GDALIdentifyDriverEx
              GDALOpen
              GDALOpenShared
              GDALOpenEx
              GDALDumpOpenDatasets
              GDALGetDriverByName

    INPUTS gcore/gdal.h
    LIBRARY gdal
    LIBRARY_NAME gdal

    NAMESPACE ""
)  

target_include_directories(gdal_sapi INTERFACE
    "${PROJECT_BINARY_DIR}"
)




