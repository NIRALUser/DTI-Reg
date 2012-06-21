#Find external tools

#External Projects
include(ExternalProject)
if(CMAKE_EXTRA_GENERATOR)
  set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
else()
  set(gen "${CMAKE_GENERATOR}")
endif()


set( Slicer3_Manual_bin "" CACHE PATH "Path to Slicer3 (binary directory)" )

macro( FindSlicerToolsMacro path name )
  find_program(${path} ${name} )
  if(NOT ${path} )
    IF(Slicer3_Manual_bin)
      set( ${path} ${Slicer3_Manual_bin}/lib/Slicer3/Plugins/${name} )
    ELSE(Slicer3_Manual_bin)
      IF(Slicer3_FOUND)
        set( ${path} ${Slicer3_HOME}/${Slicer3_INSTALL_PLUGINS_BIN_DIR}/${name} )
      else(Slicer3_FOUND)
        message( WARNING "${name} not found." )
      endIF (Slicer3_FOUND)
    ENDIF(Slicer3_Manual_bin)
  endif(NOT ${path} )
endmacro( FindSlicerToolsMacro )

OPTION(COMPILE_EXTERNAL_ResampleDTI "Compile External ResampleDTI" OFF )
if( NOT ${COMPILE_EXTERNAL_ResampleDTI} )
  FindSlicerToolsMacro( ResampleDTITOOL ResampleDTI )
endif()

if(COMPILE_EXTERNAL_ResampleDTI)
  set(proj ResampleDTI)
  ExternalProject_Add(${proj}
    SVN_REPOSITORY "http://svn.slicer.org/Slicer4/trunk/Modules/CLI/ResampleDTIVolume"
#    SVN_REPOSITORY "http://svn.slicer.org/Slicer3/tags/Slicer-3-6-RC3/Applications/CLI/DiffusionApplications/ResampleDTI"
    SVN_REVISION -r 20422
    SOURCE_DIR ${proj}
    BINARY_DIR ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )
  set( dtiprocessTOOL ${EXECUTABLE_OUTPUT_PATH}/dtiprocess CACHE STRING "" FORCE)
  mark_as_advanced( dtiprocessTOOL )
endif()


macro( FindDtiExecutableMacro path name extra)
  find_program( ${path} ${name} )
  if(NOT ${path} )
    if( ITK_USE_REVIEW OR NOT OPT_USE_SYSTEM_ITK )
      message( STATUS "${name} not found. CMake external used to download it and compile it" )
      set( ${extra} ON CACHE BOOL "" FORCE)
      set( ${path} ${EXECUTABLE_OUTPUT_PATH}/${name} )
    else( ITK_USE_REVIEW OR NOT OPT_USE_SYSTEM_ITK )
      message( WARNING "${name} not found and will not be downloaded and compiled. ITK should be compiled with ITK_USE_REVIEW set to ON" )
    endif( ITK_USE_REVIEW OR NOT OPT_USE_SYSTEM_ITK )
  endif(NOT ${path} )
endmacro( FindDtiExecutableMacro )

#if(NOT DEFINED COMPILE_DTIPROCESS )
#  set( COMPILE_DTIPROCESS OFF )
#endif()
OPTION(COMPILE_EXTERNAL_dtiprocess "Compile External dtiprocessToolkit" OFF )
if( NOT ${COMPILE_EXTERNAL_dtiprocess} )
  FindDtiExecutableMacro( dtiprocessTOOL dtiprocess COMPILE_EXTERNAL_dtiprocess )
endif()

OPTION(COMPILE_EXTERNAL_BRAINS "Compile External ANTS_Toolkit" OFF )
if( NOT ${COMPILE_EXTERNAL_BRAINS} )
  FindSlicerToolsMacro( BRAINSFitTOOL BRAINSFit )
  FindSlicerToolsMacro( BRAINSDemonWarpTOOL BRAINSDemonWarp )
endif()



IF(COMPILE_EXTERNAL_dtiprocess OR COMPILE_EXTERNAL_BRAINS )
  include(CMake/CMakeCommonExternalDefinitions.cmake)
  PACKAGE_NEEDS_VTK_NOGUI( ${CMAKE_GENERATOR} )
endif()

if(COMPILE_EXTERNAL_BRAINS)
  include(CMake/CMakeCommonExternalDefinitions.cmake)
  set(proj brains)
  if(NOT DEFINED git_protocol)
    set(git_protocol "git")
  endif()
  ExternalProject_Add(${proj}
    GIT_REPOSITORY https://github.com/BRAINSia/BRAINSStandAlone.git
    GIT_TAG "12b6d41a74ec30465a07df9c361237f2b77c2955"
    SOURCE_DIR ${proj}
    BINARY_DIR ${proj}-build
    DEPENDS  ${ITK_DEPEND} ${SlicerExecutionModel_DEPEND} ${VTK_DEPEND}
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${LOCAL_CMAKE_BUILD_OPTIONS}
      -DBUILD_TESTING:BOOL=OFF
      -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${EXECUTABLE_OUTPUT_PATH}
      -DSuperBuild_BRAINSTools_USE_GIT:BOOL=${USE_GIT_PROTOCOL}
      -DITK_VERSION_MAJOR:STRING=3
      -DITK_DIR:PATH=${ITK_DIR}
      -DGenerateCLP_DIR:PATH=${GenerateCLP_DIR}
      -DModuleDescriptionParser_DIR:PATH=${ModuleDescriptionParser_DIR}
      -DBUILD_TESTING:BOOL=OFF
      -DUSE_AutoWorkup:BOOL=OFF
      -DUSE_BRAINSContinuousClass:BOOL=OFF
      -DUSE_BRAINSDemonWarp:BOOL=ON
      -DUSE_BRAINSFit:BOOL=ON
      -DUSE_BRAINSFitEZ:BOOL=OFF
      -DUSE_BRAINSROIAuto:BOOL=OFF
      -DUSE_BRAINSResample:BOOL=OFF
      -DUSE_BRAINSSurfaceTools:BOOL=OFF
      -DUSE_DebugImageViewer:BOOL=OFF
      -DUSE_GTRACT:BOOL=OFF
      -DUSE_SYSTEM_ITK=ON
      -DUSE_SYSTEM_SlicerExecutionMode=ON
      -DUSE_SYSTEM_VTK=ON
    INSTALL_COMMAND ""
  )
  set( BRAINSFitTOOL ${EXECUTABLE_OUTPUT_PATH}/BRAINSFit CACHE STRING "" FORCE)
  set( BRAINSDemonWarpTOOL ${EXECUTABLE_OUTPUT_PATH}/BRAINSDemonWarp CACHE STRING "" FORCE)
  mark_as_advanced( BRAINSFitTOOL BRAINSDemonWarpTOOL )
endif()



IF(COMPILE_EXTERNAL_dtiprocess)
  set(proj dtiprocessTK)
  ExternalProject_Add(${proj}
    SVN_REPOSITORY "https://www.nitrc.org/svn/dtiprocess/trunk"
    SVN_USERNAME slicerbot
    SVN_PASSWORD slicer
    SVN_REVISION -r 82
    SOURCE_DIR ${proj}
    BINARY_DIR ${proj}-build
    DEPENDS  ${ITK_DEPEND} ${SlicerExecutionModel_DEPEND} ${VTK_DEPEND}
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${LOCAL_CMAKE_BUILD_OPTIONS}
      -DBUILD_TESTING:BOOL=OFF
      -DITK_DIR:PATH=${ITK_DIR}
      -DVTK_DIR:PATH=${VTK_DIR}
      -DGenerateCLP_DIR:PATH=${GenerateCLP_DIR}
      -DModuleDescriptionParser_DIR:PATH=${ModuleDescriptionParser_DIR}
      -DTCLAP_DIR:PATH=${TCLAP_DIR}
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${EXECUTABLE_OUTPUT_PATH}
    INSTALL_COMMAND ""
  )
  set( dtiprocessTOOL ${EXECUTABLE_OUTPUT_PATH}/dtiprocess CACHE STRING "" FORCE)
  mark_as_advanced( dtiprocessTOOL )
ENDIF(COMPILE_EXTERNAL_dtiprocess)


macro( FindToolMacro path name )
  find_program(${path} ${name} )
  if(NOT ${path} )
        message( WARNING "${name} not found. Its path is not set" )
  endif(NOT ${path} )
endmacro( FindToolMacro )
OPTION(COMPILE_EXTERNAL_ANTS "Compile External ANTS_Toolkit" OFF )
if( NOT ${COMPILE_EXTERNAL_ANTS} )
  FindToolMacro(ANTSTOOL ANTS)
  FindToolMacro(WARPIMAGEMULTITRANSFORMTOOL WarpImageMultiTransform)
  FindToolMacro(WARPTENSORIMAGEMULTITRANSFORMTOOL WarpTensorImageMultiTransform)
endif()

IF(COMPILE_EXTERNAL_ANTS)
  include(CMake/CMakeCommonExternalDefinitions.cmake)
  set(proj ants)
  ExternalProject_Add(${proj}
    SVN_REPOSITORY "https://advants.svn.sourceforge.net/svnroot/advants/trunk"
    SVN_REVISION -r 1395
    SOURCE_DIR ${proj}
    BINARY_DIR ${proj}-build
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${LOCAL_CMAKE_BUILD_OPTIONS}
      -DBUILD_TESTING:BOOL=OFF
      -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${EXECUTABLE_OUTPUT_PATH}
      -DSuperBuild_ANTS_USE_GIT_PROTOC:BOOL=${USE_GIT_PROTOCOL}
    INSTALL_COMMAND ""
  )
  set( ANTSTOOL ${EXECUTABLE_OUTPUT_PATH}/ANTS CACHE STRING "" FORCE)
  set( WARPIMAGEMULTITRANSFORMTOOL ${EXECUTABLE_OUTPUT_PATH}/WarpImageMultiTransform CACHE STRING "" FORCE)
  set( WARPTENSORIMAGEMULTITRANSFORMTOOL ${EXECUTABLE_OUTPUT_PATH}/WarpTensorImageMultiTransform CACHE STRING "" FORCE)
  mark_as_advanced( ANTSTOOL WARPIMAGEMULTITRANSFORMTOOL WARPTENSORIMAGEMULTITRANSFORMTOOL )
ENDIF()


