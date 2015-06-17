set(MODULE_NAME ${LOCAL_PROJECT_NAME}) # Do not use 'project()'
set(MODULE_TITLE ${MODULE_NAME})

#-----------------------------------------------------------------------------
# Update CMake module path
#------------------------------------------------------------------------------
set(CMAKE_MODULE_PATH
  ${${PROJECT_NAME}_SOURCE_DIR}/CMake
  ${${PROJECT_NAME}_BINARY_DIR}/CMake
  ${CMAKE_MODULE_PATH}
  )

include(CMakeParseArguments)
include(SlicerExtensionsConfigureMacros)

#-----------------------------------------------------------------------------
find_package(SlicerExecutionModel)
include(${SlicerExecutionModel_USE_FILE})
#-----------------------------------------------------------------------------
find_package(BatchMake REQUIRED)
include(${BatchMake_USE_FILE})
#-----------------------------------------------------------------------------
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
#-----------------------------------------------------------------------------
include(${CMAKE_CURRENT_LIST_DIR}/Common.cmake)

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/DTI-Reg_Config.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/DTI-Reg_Config.h")

set(DTI-Reg_SOURCE DTI-Reg-bms.h)

if( ${STATIC_DTI-Reg} )
  set( STATIC "EXECUTABLE_ONLY" )
endif()

if( DTI-Reg_BUILD_SLICER_EXTENSION )
  ADD_DEFINITIONS( -DSlicer_Extension )
endif()

SEMMacroBuildCLI(
  NAME ${MODULE_NAME}
  ${STATIC}
  INCLUDE_DIRECTORIES ${Slicer_DIR}#Only defined if DTI-Reg is built as a Slicer extension
  ADDITIONAL_SRCS ${DTI-Reg_SOURCE}
  TARGET_LIBRARIES ${ITK_LIBRARIES} ${BatchMake_LIBRARIES}
)

#-----------------------------------------------------------------------------
if( DTI-Reg_BUILD_SLICER_EXTENSION )
  unsetForSlicer( NAMES SlicerExecutionModel_DIR DCMTK_DIR ITK_DIR CMAKE_C_COMPILER CMAKE_CXX_COMPILER CMAKE_CXX_FLAGS CMAKE_C_FLAGS ITK_LIBRARIES zlib_DIR ZLIB_ROOT ZLIB_LIBRARY ZLIB_INCLUDE_DIR)
  # Create fake imported target to avoid importing Slicer target: See SlicerConfig.cmake:line 820
  add_library(SlicerBaseLogic SHARED IMPORTED)
  find_package(Slicer REQUIRED)
  include(${Slicer_USE_FILE})
  resetForSlicer( NAMES ITK_DIR SlicerExecutionModel_DIR CMAKE_C_COMPILER CMAKE_CXX_COMPILER CMAKE_CXX_FLAGS CMAKE_C_FLAGS ITK_LIBRARIES zlib_DIR ZLIB_ROOT ZLIB_LIBRARY ZLIB_INCLUDE_DIR)
endif()


#-----------------------------------------------------------------------------
if( DTI-Reg_BUILD_SLICER_EXTENSION )
  # You cannot pass lists as cmake arguments. We transform the string back into a list
  string(REPLACE "\"" "" EXTENSION_NO_CLI_PATHS ${EXTENSION_NO_CLI_PATHS})
  string(REPLACE " " ";" EXTENSION_NO_CLI_PATHS ${EXTENSION_NO_CLI_PATHS})
  set(NOCLI_INSTALL_DIR ${SlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION}/../ExternalBin)
  foreach( VAR ${EXTENSION_NO_CLI_PATHS})
    install( PROGRAMS ${VAR} DESTINATION ${NOCLI_INSTALL_DIR} )
  endforeach()
  set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${CMAKE_BINARY_DIR};${EXTENSION_NAME};ALL;/")
  include(${Slicer_EXTENSION_CPACK})
endif()

