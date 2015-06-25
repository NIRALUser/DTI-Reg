set(MODULE_NAME ${LOCAL_PROJECT_NAME}) # Do not use 'project()'
set(MODULE_TITLE ${MODULE_NAME})

include(CMakeParseArguments)

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

if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
