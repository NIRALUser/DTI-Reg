PROJECT (DTI-Reg)

cmake_minimum_required(VERSION 2.8)
CMAKE_POLICY(VERSION 2.8)

IF(CMAKE_COMPILER_2005)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
ENDIF(CMAKE_COMPILER_2005)

IF (${CMAKE_SOURCE_DIR} STREQUAL ${DTI-Reg_SOURCE_DIR})
  SET (LIBRARY_OUTPUT_PATH ${DTI-Reg_BINARY_DIR}/lib CACHE PATH "Single output directory for building all libraries.")
  SET (EXECUTABLE_OUTPUT_PATH ${DTI-Reg_BINARY_DIR}/bin CACHE PATH "Single output directory for building all executables.")
ENDIF (${CMAKE_SOURCE_DIR} STREQUAL ${DTI-Reg_SOURCE_DIR})

FIND_PACKAGE(Slicer3 QUIET NO_DEFAULT_PATH)
IF (Slicer3_FOUND)
  INCLUDE(${Slicer3_USE_FILE})
  slicer3_set_default_install_prefix_for_external_projects()
  if( NOT GenerateCLP_FOUND)
    message( FATAL_ERROR "GenerateCLP not found. Please set GenerateCLP_DIR. " )
  endif(NOT GenerateCLP_FOUND)
  find_package(BatchMake REQUIRED PATH ${BatchMake_DIR} )
  include(${BatchMake_USE_FILE})
  if(NOT BatchMake_FOUND )
    message( FATAL_ERROR "BatchMake not found. Please set BatchMake_DIR. " )
  endif(NOT BatchMake_FOUND )
  if(NOT ITK_FOUND )
    message( FATAL_ERROR "ITK not found. Please set ITK_DIR. " )
  endif(NOT ITK_FOUND )
ELSE (Slicer3_FOUND)
  find_package(ITK REQUIRED PATH ${ITK_DIR} )
  include(${ITK_USE_FILE})
  find_package(BatchMake REQUIRED PATH ${BatchMake_DIR} )
  include(${BatchMake_USE_FILE})
  find_package(GenerateCLP REQUIRED PATH ${GenerateCLP_DIR} )
  if(GenerateCLP_FOUND)
    include(${GenerateCLP_USE_FILE})
  endif(GenerateCLP_FOUND)
ENDIF (Slicer3_FOUND)

FILE( GLOB list_files ${CMAKE_CURRENT_SOURCE_DIR}/*.bms )
FILE( COPY ${list_files} DESTINATION ${EXECUTABLE_OUTPUT_PATH} )

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/DTI-Reg_Config.h.in"
      "${CMAKE_CURRENT_BINARY_DIR}/DTI-Reg_Config.h")

set(DTIReg_SOURCE DTI-Reg.cxx)
GenerateCLP(DTIReg_SOURCE DTI-Reg.xml)

add_executable( DTI-Reg ${DTIReg_SOURCE} )
target_link_libraries(DTI-Reg BatchMakeLib)

