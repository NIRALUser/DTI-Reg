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

find_package(ITK REQUIRED PATHS ${ITK_DIR} )
if( ITK_FOUND )
  include(${ITK_USE_FILE})
else( ITK_FOUND )
  message( FATAL_ERROR "ITK not found. Please set ITK_DIR. " )
endif( ITK_FOUND )


find_package(SlicerExecutionModel REQUIRED PATHS ${SlicerExecutionModel_DIR} )
if(SlicerExecutionModel_FOUND)
  include(${SlicerExecutionModel_USE_FILE})
else( SlicerExecutionModel_FOUND)
  message( FATAL_ERROR "SlicerExecutionModel not found. Please set SlicerExecutionModel_DIR. " )
endif( SlicerExecutionModel_FOUND )


find_package(BatchMake REQUIRED PATHS ${BatchMake_DIR} )
if( BatchMake_FOUND )
  include(${BatchMake_USE_FILE})
else( BatchMake_FOUND )
  message( FATAL_ERROR "BatchMake not found. Please set BatchMake_DIR. " )
endif( BatchMake_FOUND )



FILE( GLOB list_files ${CMAKE_CURRENT_SOURCE_DIR}/*.bms )
FILE( COPY ${list_files} DESTINATION ${EXECUTABLE_OUTPUT_PATH} )

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/DTI-Reg_Config.h.in"
      "${CMAKE_CURRENT_BINARY_DIR}/DTI-Reg_Config.h")

#set(DTIReg_SOURCE DTI-Reg.cxx)
#GenerateCLP(DTIReg_SOURCE DTI-Reg.xml)

slicerMacroBuildCLI(
  NAME DTI-Reg
  TARGET_LIBRARIES ModuleDescriptionParser ${ITK_LIBRARIES} BatchMakeLib
  LINK_DIRECTORIES ${ModuleDescriptionParser_BINARY_DIR} ${DTIReg_ADDITIONAL_LINK_DIRS}
  INCLUDE_DIRECTORIES
    ${DTI-Reg_SOURCE_DIR} ${DTI-Reg_BINARY_DIR}
  )

if( DEFINED BUILD_RESAMPLEDTI AND ${BUILD_RESAMPLEDTI} )
  add_subdirectory( ResampleDTI ResampleDTI-build )
endif()

#if(DTIReg_ADDITIONAL_LINK_DIRS)
#link_directories(${DTIReg_ADDITIONAL_LINK_DIRS})
#endif()

#add_executable( DTI-Reg ${DTIReg_SOURCE} )
#target_link_libraries(DTI-Reg BatchMakeLib)

