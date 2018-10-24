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

set(DTI-Reg_SOURCE DTI-Reg-bms.h)

if( ${STATIC_DTI-Reg} )
  set( STATIC "EXECUTABLE_ONLY" )
endif()

if( DTI-Reg_BUILD_SLICER_EXTENSION )
  ADD_DEFINITIONS( -DSlicer_Extension )
endif()

SETIFEMPTY(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
SETIFEMPTY(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
SETIFEMPTY(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)
SETIFEMPTY(INSTALL_RUNTIME_DESTINATION bin)
SETIFEMPTY(INSTALL_LIBRARY_DESTINATION lib)
SETIFEMPTY(INSTALL_ARCHIVE_DESTINATION lib)

SEMMacroBuildCLI(
  NAME ${MODULE_NAME}
  ${STATIC}
  INCLUDE_DIRECTORIES ${Slicer_DIR}#Only defined if DTI-Reg is built as a Slicer extension
  ADDITIONAL_SRCS ${DTI-Reg_SOURCE}
  TARGET_LIBRARIES ${ITK_LIBRARIES} ${BatchMake_LIBRARIES}
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
  INSTALL_RUNTIME_DESTINATION ${INSTALL_RUNTIME_DESTINATION}
  INSTALL_LIBRARY_DESTINATION ${INSTALL_LIBRARY_DESTINATION}
  INSTALL_ARCHIVE_DESTINATION ${INSTALL_ARCHIVE_DESTINATION}
)
export(TARGETS ${MODULE_NAME} FILE ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}-exports.cmake)
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()

if(CONFIGURE_TOOLS_PATHS)
  configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/DTI-Reg_Config_with_config.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/DTI-Reg_Config.h")
else()
  configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/DTI-Reg_Config_no_config.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/DTI-Reg_Config.h" COPYONLY)
endif()

