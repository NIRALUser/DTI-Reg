# Make sure this file is included only once
get_filename_component(CMAKE_CURRENT_LIST_FILENAME ${CMAKE_CURRENT_LIST_FILE} NAME_WE)
if(${CMAKE_CURRENT_LIST_FILENAME}_FILE_INCLUDED)
  return()
endif()
set(${CMAKE_CURRENT_LIST_FILENAME}_FILE_INCLUDED 1)

# Include dependent projects if any
set(extProjName BatchMake) #The find_package known name
set(proj ${extProjName})              #This local name

#if(${USE_SYSTEM_${extProjName}})
#  unset(${extProjName}_DIR CACHE)
#endif()

# Sanity checks
if(DEFINED ${extProjName}_DIR AND NOT EXISTS ${${extProjName}_DIR})
  message(FATAL_ERROR "${extProjName}_DIR variable is defined but corresponds to non-existing directory (${${extProjName}_DIR})")
endif()

# Set dependency list
set(${proj}_DEPENDENCIES ITKv4)

SlicerMacroCheckExternalProjectDependency(${proj})

if(NOT DEFINED ${extProjName}_DIR AND NOT ${USE_SYSTEM_${extProjName}})
  #message(STATUS "${__indent}Adding project ${proj}")
  ExternalProject_Add(${proj}
      GIT_REPOSITORY ${git_protocol}://batchmake.org/BatchMake.git
      GIT_TAG "8addbdb62f0135ba01ffe12ddfc32121b6d66ef5" # 01-30-2013 # "0abb2faca1251f808ab3d0b820cc27b570a994f1" # 08-26-2012 updated for ITKv4 # "43d21fcccd09e5a12497bc1fb924bc6d5718f98c" # used in DTI-Reg 12-21-2012
      SOURCE_DIR BatchMake
      BINARY_DIR BatchMake-build
      CMAKE_GENERATOR ${gen}
      CMAKE_ARGS
        ${COMMON_BUILD_OPTIONS_FOR_EXTERNALPACKAGES}
        -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_CURRENT_BINARY_DIR}/BatchMake-build/bin
        -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_CURRENT_BINARY_DIR}/BatchMake-build/bin
        -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_CURRENT_BINARY_DIR}/BatchMake-build/bin
        -DCMAKE_BUNDLE_OUTPUT_DIRECTORY:PATH=${CMAKE_CURRENT_BINARY_DIR}/BatchMake-build/bin
        -DBUILD_SHARED_LIBS:BOOL=OFF
        -DBUILD_TESTING:BOOL=OFF
        -DUSE_FLTK:BOOL=OFF
        -DDASHBOARD_SUPPORT:BOOL=OFF
        -DGRID_SUPPORT:BOOL=OFF
        -DUSE_SPLASHSCREEN:BOOL=OFF
        -DITK_DIR:PATH=${ITK_DIR}
        ${BatchMakeCURLCmakeArg}
      INSTALL_COMMAND ""
      PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild/BatchMakePatchedZip.c ${CMAKE_CURRENT_BINARY_DIR}/BatchMake/Utilities/Zip/zip.c # No "" # Patch for windows compilation error (declaration of variable after beginning of block - "uLong year")
      DEPENDS  ${${proj}_DEPENDENCIES}
    )
    set(BatchMake_DIR ${CMAKE_CURRENT_BINARY_DIR}/BatchMake-build)
    mark_as_advanced(CLEAR BatchMake_DIR)
    set(BatchMake_ITK_DIR ${ITK_DIR}) # If batchmake recompiled, no include(${BatchMake_USE_FILE}) has been done so BatchMake_ITK_DIR does not exist, and we used ${ITK_DIR} to compile it.
    set(BatchMake_DEPEND BatchMake)

else()
  if(${USE_SYSTEM_${extProjName}})
    find_package(${extProjName} REQUIRED)
    if(NOT ${extProjName}_DIR)
      message(FATAL_ERROR "To use the system ${extProjName}, set ${extProjName}_DIR")
    endif()
    message("USING the system ${extProjName}, set ${extProjName}_DIR=${${extProjName}_DIR}")
  endif()
  # The project is provided using ${extProjName}_DIR, nevertheless since other
  # project may depend on ${extProjName}v4, let's add an 'empty' one
  SlicerMacroEmptyExternalProject(${proj} "${${proj}_DEPENDENCIES}")
endif()

list(APPEND ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS ${extProjName}_DIR:PATH)
