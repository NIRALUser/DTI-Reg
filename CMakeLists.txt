PROJECT (DTI-Reg_Package)

cmake_minimum_required(VERSION 2.8)
CMAKE_POLICY(VERSION 2.8)
#External Projects

include(ExternalProject)
if(CMAKE_EXTRA_GENERATOR)
  set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
else()
  set(gen "${CMAKE_GENERATOR}")
endif()


set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/CMake" )

include(CMakeCommonExternalDefinitions)

IF(CMAKE_COMPILER_2005)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
ENDIF(CMAKE_COMPILER_2005)

IF (${CMAKE_SOURCE_DIR} STREQUAL ${DTI-Reg_Package_SOURCE_DIR})
  SET (LIBRARY_OUTPUT_PATH ${DTI-Reg_Package_BINARY_DIR}/lib CACHE PATH "Single output directory for building all libraries.")
  SET (EXECUTABLE_OUTPUT_PATH ${DTI-Reg_Package_BINARY_DIR}/bin CACHE PATH "Single output directory for building all executables.")
ENDIF (${CMAKE_SOURCE_DIR} STREQUAL ${DTI-Reg_Package_SOURCE_DIR})

if( NOT EXECUTABLE_OUTPUT_PATH )
  SET (EXECUTABLE_OUTPUT_PATH ${DTI-Reg_Package_BINARY_DIR}/bin CACHE PATH "Single output directory for building all executables.")
  message( WARNING "EXECUTABLE_OUTPUT_PATH was not set. Set to ${DTI-Reg_Package_BINARY_DIR}/bin" )
endif( NOT EXECUTABLE_OUTPUT_PATH )


option(USE_GIT_PROTOCOL "If behind a firewall turn this off to use http instead." ON)
set(git_protocol "git")
if(NOT USE_GIT_PROTOCOL)
  set(git_protocol "http")
else(NOT USE_GIT_PROTOCOL)
  set(git_protocol "git")
endif()

OPTION(BUILD_SHARED_LIBS "Compiles everything with static libraries" ON)
OPTION(COMPILE_PACKAGE "Compiles all the external projects and tools" ON)
if( ${COMPILE_PACKAGE} )
  set( OPT_USE_SYSTEM_ITK OFF CACHE STRING "" FORCE)
  PACKAGE_NEEDS_ITK( ${CMAKE_GENERATOR} )
  set( OPT_USE_SYSTEM_SlicerExecutionModel OFF CACHE STRING "" FORCE)
  PACKAGE_NEEDS_SlicerExecutionModel( ${CMAKE_GENERATOR} )
  set( OPT_USE_SYSTEM_BatchMake OFF CACHE STRING "" FORCE)
  PACKAGE_NEEDS_BatchMake( ${CMAKE_GENERATOR} )
  set( BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE )
  set( COMPILE_EXTERNAL_dtiprocess ON CACHE BOOL "" FORCE )
  set( COMPILE_EXTERNAL_ANTS  ON CACHE BOOL "" FORCE )
  set( COMPILE_EXTERNAL_BRAINS  ON CACHE BOOL "" FORCE )
  set( COMPILE_EXTERNAL_ResampleDTI  ON CACHE BOOL "" FORCE )
  set( was_package TRUE CACHE INTERNAL "" FORCE )
  mark_as_advanced(Slicer_DIR Slicer3_Manual_bin Slicer3_DIR OPT_USE_SYSTEM_SlicerExecutionModel OPT_USE_SYSTEM_ITK OPT_USE_SYSTEM_BatchMake COMPILE_EXTERNAL_dtiprocess COMPILE_EXTERNAL_ANTS COMPILE_EXTERNAL_BRAINS COMPILE_EXTERNAL_ResampleDTI)
else()
  set( ITK_DIR_TEMP ${ITK_DIR} )
  set( VTK_DIR_TEMP ${VTK_DIR} )
  set( SlicerExecutionModel_DIR_TEMP ${SlicerExecutionModel_DIR} )
  unset( ITK_DIR CACHE )
  unset( VTK_DIR CACHE )
  unset( SlicerExecutionModel_DIR CACHE )
  FIND_PACKAGE(Slicer QUIET NO_DEFAULT_PATH)
  IF (Slicer_FOUND)
#    set( Slicer_SKIP_USE_FILE_INCLUDE_CHECK TRUE )
    INCLUDE(${Slicer_USE_FILE})
  ELSE ()
    FIND_PACKAGE(Slicer3 QUIET NO_DEFAULT_PATH)
    IF (Slicer3_FOUND)

      INCLUDE(${Slicer3_USE_FILE})
      slicer3_set_default_install_prefix_for_external_projects()
    ENDIF ()
  ENDIF ()
  if( Slicer_FOUND OR Slicer3_FOUND )
    if( ${was_package} )
      message( WARNING "hello")
      set( OPT_USE_SYSTEM_ITK ON CACHE BOOL "" FORCE )
      set( OPT_USE_SYSTEM_VTK ON CACHE BOOL "" FORCE )
      set( COMPILE_EXTERNAL_ResampleDTI  OFF CACHE BOOL "" FORCE )
    endif()
  else()
    set( ITK_DIR ${ITK_DIR_TEMP} )
    set( VTK_DIR ${VTK_DIR_TEMP} )
    set( SlicerExecutionModel_DIR ${SlicerExecutionModel_DIR_TEMP} )
  endif()
  if( ${was_package} )
    message( WARNING "hello")
    set( was_package FALSE CACHE INTERNAL "" FORCE )
  endif()
  find_package(ITK REQUIRED PATHS ${ITK_DIR} )
  if( ITK_FOUND )
    include(${ITK_USE_FILE})
  else()
    PACKAGE_NEEDS_ITK( ${CMAKE_GENERATOR} )
  endif()
  find_package(BatchMake REQUIRED PATHS ${BatchMake_DIR} )
  if( BatchMake_FOUND )
    include(${BatchMake_USE_FILE})
  else()
    PACKAGE_NEEDS_BatchMake( ${CMAKE_GENERATOR} )
  endif()
  find_package(SlicerExecutionModel QUIET PATHS ${SlicerExecutionModel_DIR} )
  if( SlicerExecutionModel_FOUND )
    include(${SlicerExecutionModel_USE_FILE})
  else()
    PACKAGE_NEEDS_SlicerExecutionModel( ${CMAKE_GENERATOR} )
  endif()
  mark_as_advanced( CLEAR Slicer_DIR Slicer3_Manual_bin Slicer3_DIR OPT_USE_SYSTEM_SlicerExecutionModel OPT_USE_SYSTEM_ITK OPT_USE_SYSTEM_BatchMake COMPILE_EXTERNAL_dtiprocess COMPILE_EXTERNAL_ANTS COMPILE_EXTERNAL_BRAINS COMPILE_EXTERNAL_ResampleDTI)

endif()


include( FindExternalTools)


mark_as_advanced( BUILD_SHARED_LIBS )

if( ${OPT_USE_SYSTEM_ITK} AND NOT ${ITK_USE_REVIEW} )
#  set( OPT_USE_SYSTEM_ITK OFF )
  message( SEND_ERROR "ITK was compiled with ITK_USE_REVIEW set to off. Please recompile it setting this option to ON or set OPT_USE_SYSTEM_ITK to OFF" )
endif( )

set(proj DTI-Reg)
ExternalProject_Add(${proj}
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${proj}
  BINARY_DIR ${proj}-build
  DEPENDS ${ITK_DEPEND} ${SlicerExecutionModel_DEPEND} ${BatchMake_DEPEND}
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${LOCAL_CMAKE_BUILD_OPTIONS}
    -DITK_DIR:PATH=${ITK_DIR}
    -DBatchMake_DIR:PATH=${BatchMake_DIR}
    -DSlicerExecutionModel_DIR:PATH=${SlicerExecutionModel_DIR}
    -DModuleDescriptionParser_DIR:PATH=${ModuleDescriptionParser_DIR}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DEXECUTABLE_OUTPUT_PATH:PATH=${EXECUTABLE_OUTPUT_PATH}
    -DLIBRARY_OUTPUT_PATH:PATH=${LIBRARY_OUTPUT_PATH}
    -DBUILD_RESAMPLEDTI:BOOL=${COMPILE_EXTERNAL_ResampleDTI}
  INSTALL_COMMAND ""
)
FORCE_BUILD_CHECK(${proj})
