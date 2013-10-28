#Find external tools

macro( FindSlicerToolsMacro path name )
  find_program(${path} ${name} )
  if(NOT ${path} )
    message( WARNING "${name} not found. Its path is not set" )
  endif(NOT ${path} )
endmacro( FindSlicerToolsMacro )

FindSlicerToolsMacro( BRAINSFitTOOL BRAINSFit )
FindSlicerToolsMacro( BRAINSDemonWarpTOOL BRAINSDemonWarp )

macro( FindDtiExecutableMacro path name extra)
  find_program( ${path} ${name} )
  if(NOT ${path} )
    if( ITK_USE_REVIEW OR NOT OPT_USE_SYSTEM_ITK )
      message( STATUS "${name} not found. CMake external used to download it and compile it" )
      set( ${extra} ON )
      set( ${path} ${EXECUTABLE_OUTPUT_PATH}/${name} )
    else( ITK_USE_REVIEW OR NOT OPT_USE_SYSTEM_ITK )
      message( WARNING "${name} not found and will not be downloaded and compiled. ITK should be compiled with ITK_USE_REVIEW set to ON" )
    endif( ITK_USE_REVIEW OR NOT OPT_USE_SYSTEM_ITK )
  endif(NOT ${path} )
endmacro( FindDtiExecutableMacro )

set( COMPILE_DTIPROCESS OFF )

FindDtiExecutableMacro( dtiprocessTOOL dtiprocess COMPILE_DTIPROCESS )

#External Projects
include(ExternalProject)
if(CMAKE_EXTRA_GENERATOR)
  set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
else()
  set(gen "${CMAKE_GENERATOR}")
endif()

OPTION(COMPILE_EXTERNAL_dtiprocess "Compile External dtiprocessToolkit" ${COMPILE_DTIPROCESS} )
IF(COMPILE_EXTERNAL_dtiprocess)
  if(NOT Slicer3_FOUND)
    include(CMake/CMakeCommonExternalDefinitions.cmake)
    PACKAGE_NEEDS_VTK_NOGUI( ${CMAKE_GENERATOR} )
  endif(NOT Slicer3_FOUND)
  set(proj dtiprocessTK)
  ExternalProject_Add(${proj}
    SVN_REPOSITORY "https://www.nitrc.org/svn/dtiprocess/trunk"
    SVN_USERNAME slicerbot
    SVN_PASSWORD slicer
    SOURCE_DIR ${proj}
    BINARY_DIR ${proj}-build
    DEPENDS  ${ITK_DEPEND} ${SlicerExecutionModel_DEPEND} ${VTK_DEPEND}
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${LOCAL_CMAKE_BUILD_OPTIONS}
      -DBUILD_TESTING:BOOL=OFF
      -DVTK_DIR:PATH=${VTK_DIR}
      -DGenerateCLP_DIR:PATH=${GenerateCLP_DIR}
      -DModuleDescriptionParser_DIR:PATH=${ModuleDescriptionParser_DIR}
      -DTCLAP_DIR:PATH=${TCLAP_DIR}
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${EXECUTABLE_OUTPUT_PATH}
    INSTALL_COMMAND ""
  )
set( dtiprocessTOOL ${EXECUTABLE_OUTPUT_PATH}/dtiprocess )
ENDIF(COMPILE_EXTERNAL_dtiprocess)


macro( FindToolMacro path name )
  find_program(${path} ${name} )
  if(NOT ${path} )
        message( WARNING "${name} not found. Its path is not set" )
  endif(NOT ${path} )
endmacro( FindToolMacro )

FindToolMacro(ANTSTOOL ANTS)
FindToolMacro(WARPIMAGEMULTITRANSFORMTOOL WarpImageMultiTransform)
FindToolMacro(WARPTENSORIMAGEMULTITRANSFORMTOOL WarpTensorImageMultiTransform)
FindToolMacro(ResampleDTIlogEuclideanTOOL ResampleDTIlogEuclidean)
FindToolMacro(ITKTransformToolsTOOL ITKTransformTools)

######################################################################
##To add DTITK registration##
######################################################################
#FindToolMacro(TVtoolTOOL TVtool)#DTI-TK - scaling tensor unit
#FindToolMacro(DTIConvertTOOL DTIConvert)#DTI-TK - convert from NRRD to NIFTI
#FindToolMacro(dti_affine_reg_TOOL dti_affine_reg)#DTI_TK - Affine registration
#FindToolMacro(dti_diffeomorphic_reg_TOOL dti_diffeomorphic_reg)#DTI_TK - diffeomorphic registration
######################################################################
