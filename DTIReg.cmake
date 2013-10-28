
macro(INSTALL_EXECUTABLE)
  set(options )
  set( oneValueArgs OUTPUT_DIR )
  set(multiValueArgs LIST_EXEC )
  CMAKE_PARSE_ARGUMENTS(LOCAL
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN}
    )
  foreach( tool ${LOCAL_LIST_EXEC})
    install(PROGRAMS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${tool} DESTINATION ${LOCAL_OUTPUT_DIR} )
  endforeach()
endmacro()


include(${CMAKE_CURRENT_LIST_DIR}/Common.cmake)

set(MODULE_NAME ${EXTENSION_NAME}) # Do not use 'project()'
set(MODULE_TITLE ${MODULE_NAME})


#-----------------------------------------------------------------------------
# Update CMake module path
#------------------------------------------------------------------------------
set(CMAKE_MODULE_PATH
  ${${PROJECT_NAME}_SOURCE_DIR}/CMake
  ${${PROJECT_NAME}_BINARY_DIR}/CMake
  ${CMAKE_MODULE_PATH}
  )

#-----------------------------------------------------------------------------
set(expected_ITK_VERSION_MAJOR ${ITK_VERSION_MAJOR})
find_package(ITK NO_MODULE REQUIRED)
if(${ITK_VERSION_MAJOR} VERSION_LESS ${expected_ITK_VERSION_MAJOR})
  # Note: Since ITKv3 doesn't include a ITKConfigVersion.cmake file, let's check the version
  #       explicitly instead of passing the version as an argument to find_package() command.
  message(FATAL_ERROR "Could not find a configuration file for package \"ITK\" that is compatible "
                      "with requested version \"${expected_ITK_VERSION_MAJOR}\".\n"
                      "The following configuration files were considered but not accepted:\n"
                      "  ${ITK_CONFIG}, version: ${ITK_VERSION_MAJOR}.${ITK_VERSION_MINOR}.${ITK_VERSION_PATCH}\n")
endif()

include(${ITK_USE_FILE})

#-----------------------------------------------------------------------------
find_package(BatchMake )
include(${BatchMake_USE_FILE})

#-----------------------------------------------------------------------------
find_package(SlicerExecutionModel NO_MODULE REQUIRED GenerateCLP)
include(${GenerateCLP_USE_FILE})
include(${SlicerExecutionModel_USE_FILE})


configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/DTI-Reg_Config.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/DTI-Reg_Config.h")

set(DTIReg_SOURCE DTI-Reg.cxx DTI-Reg-bms.h)
GenerateCLP(DTIReg_SOURCE DTI-Reg.xml)

add_executable( DTI-Reg ${DTIReg_SOURCE} )
target_link_libraries(DTI-Reg ${ITK_LIBRARIES} BatchMakeLib)

