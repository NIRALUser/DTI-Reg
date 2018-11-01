set(MODULE_NAME ${LOCAL_PROJECT_NAME}) # Do not use 'project()'
set(MODULE_TITLE ${MODULE_NAME})

include(CMakeParseArguments)

#-----------------------------------------------------------------------------
find_package(SlicerExecutionModel)
include(${SlicerExecutionModel_USE_FILE})
#-----------------------------------------------------------------------------
set(BatchMake_SOURCE_DIR "" CACHE PATH "BatchMake source directory")
set(BatchMake_DIR "" CACHE PATH "BatchMake build directory")
set(BatchMake_LIBRARIES 
  BatchMakeLib
  BatchMakeUtilities
  bmModuleDescriptionParser)
set(BatchMake_INCLUDE_DIR
    ${BatchMake_SOURCE_DIR}/Code
    ${BatchMake_SOURCE_DIR}/Utilities
    ${BatchMake_SOURCE_DIR}/Utilities/bmModuleDescriptionParser
    ${BatchMake_DIR}/Utilities/bmModuleDescriptionParser
    ${BatchMake_SOURCE_DIR}/Utilities/Zip
    )
link_directories(${BatchMake_DIR}/bin)
include_directories(${BatchMake_INCLUDE_DIR})
#-----------------------------------------------------------------------------
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
#-----------------------------------------------------------------------------

set(DTI-Reg_SOURCE DTI-Reg-bms.h)

if( ${STATIC_DTI-Reg} )
  set( STATIC "EXECUTABLE_ONLY" )
endif()

if( DTI-Reg_BUILD_SLICER_EXTENSION )
  ADD_DEFINITIONS( -DSlicer_Extension )
  
  find_package(Slicer REQUIRED)
  include(${Slicer_USE_FILE})

  SET(INSTALL_RUNTIME_DESTINATION ${Slicer_INSTALL_CLIMODULES_BIN_DIR})
  SET(INSTALL_LIBRARY_DESTINATION ${Slicer_INSTALL_CLIMODULES_LIB_DIR})
  SET(INSTALL_ARCHIVE_DESTINATION ${Slicer_INSTALL_CLIMODULES_LIB_DIR})
  set(INSTALL_RUNTIME_DESTINATION_EXTERNAL ${INSTALL_RUNTIME_DESTINATION}/../ExternalBin)

  set(BUILD_TESTING OFF)
endif()

SETIFEMPTY(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
SETIFEMPTY(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
SETIFEMPTY(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)
SETIFEMPTY(INSTALL_RUNTIME_DESTINATION bin)
SETIFEMPTY(INSTALL_LIBRARY_DESTINATION lib)
SETIFEMPTY(INSTALL_ARCHIVE_DESTINATION lib)
SETIFEMPTY(INSTALL_RUNTIME_DESTINATION_EXTERNAL bin)

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


if(ITKTransformTools_DIR)

  find_program(ITKTransformTools_LOCATION 
    ITKTransformTools
    HINTS ${ITKTransformTools_DIR}/bin)

  if(ITKTransformTools_LOCATION)
    install(PROGRAMS ${ITKTransformTools_LOCATION}
      DESTINATION ${INSTALL_RUNTIME_DESTINATION_EXTERNAL}
      COMPONENT RUNTIME)
  endif()
  
endif()

if(ANTs_DIR)
  set(ants_tools
    ANTS
    WarpImageMultiTransform)

  foreach(ants_bin ${ants_tools})

    find_program(${ants_bin}_LOCATION 
      ${ants_bin}
      HINTS ${ANTs_DIR}/bin)
    if(${ants_bin}_LOCATION)
      install(PROGRAMS ${${ants_bin}_LOCATION}
        DESTINATION ${INSTALL_RUNTIME_DESTINATION_EXTERNAL}
        COMPONENT RUNTIME)
    endif()

  endforeach()
endif()

if( DTI-Reg_BUILD_SLICER_EXTENSION )
  set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${CMAKE_BINARY_DIR};${EXTENSION_NAME};ALL;/")
  include(${Slicer_EXTENSION_CPACK})
endif()
