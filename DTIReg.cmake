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
find_package(ITK 4 NO_MODULE REQUIRED)

include(${ITK_USE_FILE})

#-----------------------------------------------------------------------------
find_package(SlicerExecutionModel NO_MODULE REQUIRED GenerateCLP)
include(${GenerateCLP_USE_FILE})
include(${SlicerExecutionModel_USE_FILE})


#-----------------------------------------------------------------------------
find_package(BatchMake REQUIRED)
include(${BatchMake_USE_FILE})

include(${CMAKE_CURRENT_LIST_DIR}/Common.cmake)
SETIFEMPTY(INSTALL_RUNTIME_DESTINATION bin)

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/DTI-Reg_Config.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/DTI-Reg_Config.h")
if( Slicer_CLIMODULES_BIN_DIR )
  ADD_DEFINITIONS( -DSlicer_CLIMODULES_BIN_DIR="${Slicer_CLIMODULES_BIN_DIR}" )
endif()
set(DTIReg_SOURCE DTI-Reg.cxx DTI-Reg-bms.h)
GenerateCLP(DTIReg_SOURCE DTI-Reg.xml)

add_executable( DTI-Reg ${DTIReg_SOURCE} )
target_link_libraries(DTI-Reg ${ITK_LIBRARIES} ${BatchMake_LIBRARIES})

install(TARGETS DTI-Reg DESTINATION ${INSTALL_RUNTIME_DESTINATION} )

