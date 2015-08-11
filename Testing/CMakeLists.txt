# Add standard remote object stores to user's
# configuration.
list(APPEND ExternalData_URL_TEMPLATES
"http://slicer.kitware.com/midas3/api/rest?method=midas.bitstream.download&checksum=%(hash)"
)

set(CLP DTI-Reg)

include(CMakeEchoAllTargetProperties)

# Build and install tests (only if in inner-build)
# If in outer-build directory, only allow to run the tests
SETIFEMPTY( INSTALL_RUNTIME_DESTINATION bin )
if( NOT ${LOCAL_PROJECT_NAME}_SUPERBUILD )
  add_executable(${CLP}Test ${CLP}Test.cxx)
  add_dependencies(${CLP}Test ${CLP})
  target_link_libraries(${CLP}Test ${CLP}Lib ${ITK_LIBRARIES} ${BatchMake_LIBRARIES})
  set_target_properties(${CLP}Test PROPERTIES LABELS ${CLP})
  install(TARGETS ${CLP}Test
          DESTINATION ${INSTALL_RUNTIME_DESTINATION} )
  set(${CLP}TestPATH $<TARGET_FILE:${CLP}Test>)
else()
  set(${CLP}TestPATH ${DTI-Reg_INSTALL_DIRECTORY}/${INSTALL_RUNTIME_DESTINATION}/DTI-RegTest)
endif()

# Tools paths
set( LIST_TOOLS_TESTS
    dtiprocess
    ANTS
    ResampleDTIlogEuclidean
    ITKTransformTools
    BRAINSFit
    BRAINSDemonWarp
    )

foreach( VAR ${LIST_TOOLS_TESTS} )
  if( NOT DTI-Reg_BUILD_SLICER_EXTENSION AND NOT ${VAR}TOOL})
    find_program( ${Tool}TOOL ${VAR} )
  endif()
  if( ${VAR}TOOL )
    set(${VAR}PATH ${${VAR}TOOL})
  else()
    set(${VAR}PATH ${VAR})
  endif()
endforeach()
if(DTI-Reg_BUILD_SLICER_EXTENSION AND ${LOCAL_PROJECT_NAME}_SUPERBUILD)
   get_property(dtiprocessPATH TARGET dtiprocess PROPERTY IMPORTED_LOCATION_RELEASE )
   get_property(ResampleDTIlogEuclideanPATH TARGET ResampleDTIlogEuclidean PROPERTY IMPORTED_LOCATION_RELEASE )
endif()


# Finds DTI-Reg and runs it to show help
add_test(NAME ${CLP}HelpTest COMMAND ${CLP} --help )
# Finds ANTS and runs it to show help
add_test(NAME ANTSHelpTest COMMAND ${ANTSPATH} -h )
# Finds ResampleDTIlogEuclidean and runs it to show help
add_test(NAME ResampleDTIlogEuclideanHelpTest COMMAND ${SEM_LAUNCH_COMMAND} ${ResampleDTIlogEuclideanPATH} --help )
# Finds dtiprocess and runs it to show help
add_test(NAME dtiprocessHelpTest COMMAND ${SEM_LAUNCH_COMMAND} ${dtiprocessPATH} --help )
# Finds ITKTransformTools and runs it to show help
add_test(NAME ITKTransformToolsHelpTest COMMAND ${ITKTransformToolsPATH} version )
# Finds BRAINSFit and runs it to show help
add_test(NAME BRAINSFitHelpTest COMMAND ${SEM_LAUNCH_COMMAND} ${BRAINSFitPATH} --help )
# Finds BRAINSDemonWarp and runs it to show help
add_test(NAME BRAINSDemonWarpHelpTest COMMAND ${SEM_LAUNCH_COMMAND} ${BRAINSDemonWarpPATH} --help )

set(TEMP_DIR ${${CLP}_BINARY_DIR}/Testing/Temporary )

set(MovingImage DATA{Data/Input/AdultData_DWI_QCed_DTI.nrrd})
set(MovingMask DATA{Data/Input/AdultData_DWI_QCed_DTI_mask.nrrd})
set(FixedImage DATA{Data/Input/AdultAtlas.nrrd})
set(FixedMask DATA{Data/Input/AdultAtlas_BrainMask.nrrd})

# ANTS
set(BaselineImage DATA{Data/Baseline/ANTS-GreedyDiffeo-Baseline.nrrd})
set(OutputImage ${TEMP_DIR}/ANTS-GreedyDiffeo-Registered.nrrd )
# We start the command with ${SEM_LAUNCH_COMMAND} so that it sets the environment variables correctly for other tools that might require them (eg dtiprocess)
ExternalData_add_test(${CLP}Data NAME ${CLP}ANTSRegistration COMMAND ${SEM_LAUNCH_COMMAND} ${${CLP}TestPATH}
--compare
${BaselineImage}
${OutputImage}
--compareIntensityTolerance 0.000000001
--compareRadiusTolerance 1
ModuleEntryPoint
--movingVolume ${MovingImage}
--fixedVolume ${FixedImage}
--fixedMaskVolume ${FixedMask}
--movingMaskVolume ${MovingMask}
--method useScalar-ANTS
--outputVolume ${OutputImage}
--ANTSRegistrationType GreedyDiffeo
--ANTSPath ${ANTSPATH}
--dtiprocessPath ${dtiprocessPATH}
--ResampleDTIPath ${ResampleDTIlogEuclideanPATH}
--ITKTransformToolsPath ${ITKTransformToolsPATH}
)

# No Output - error if build as extension
# We start the command with ${SEM_LAUNCH_COMMAND} so that it sets the environment variables correctly for other tools that might require them (eg dtiprocess)
ExternalData_add_test(${CLP}Data NAME ${CLP}NoOutput COMMAND ${SEM_LAUNCH_COMMAND} ${${CLP}TestPATH}
--compare
${BaselineImage}
${OutputImage}
ModuleEntryPoint
--movingVolume ${MovingImage}
--fixedVolume ${FixedImage}
)
if( DTI-Reg_BUILD_SLICER_EXTENSION )
  set_tests_properties(${CLP}NoOutput PROPERTIES WILL_FAIL true)
endif()

#useScalar-BRAINS
#--BRAINSFitPath
#--BRAINSDemonWarpPath
# ANTS
set(BaselineImage DATA{Data/Baseline/BRAINS-Affine-Baseline.nrrd})
set(OutputImage ${TEMP_DIR}/BRAINS-Affine-Registered.nrrd )
# We start the command with ${SEM_LAUNCH_COMMAND} so that it sets the environment variables correctly for other tools that might require them (e.g. dtiprocess, BRAINSFit,...)
ExternalData_add_test(${CLP}Data NAME ${CLP}BRAINSRegistration COMMAND ${SEM_LAUNCH_COMMAND} ${${CLP}TestPATH}
--compare
${BaselineImage}
${OutputImage}
--compareIntensityTolerance 0.001
--compareRadiusTolerance 1
ModuleEntryPoint
--movingVolume ${MovingImage}
--fixedVolume ${FixedImage}
--fixedMaskVolume ${FixedMask}
--movingMaskVolume ${MovingMask}
--method useScalar-BRAINS
--BRAINSRegistrationType Affine
--outputVolume ${OutputImage}
--dtiprocessPath ${dtiprocessPATH}
--ResampleDTIPath ${ResampleDTIlogEuclideanPATH}
--ITKTransformToolsPath ${ITKTransformToolsPATH}
)


ExternalData_add_target(${CLP}Data)

