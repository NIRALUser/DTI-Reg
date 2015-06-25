# Add standard remote object stores to user's
# configuration.
list(APPEND ExternalData_URL_TEMPLATES
"http://slicer.kitware.com/midas3/api/rest?method=midas.bitstream.download&checksum=%(hash)"
)

set(CLP DTI-Reg)


macro(FIND_EXTERNAL_TOOLS Tool)
  find_program( ${Tool}PATH ${Tool}
    PATHS ${${Tool}_DIR} ${${Tool}_INSTALL_DIRECTORY}
    PATH_SUFFIXES ${SlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION} ${INSTALL_RUNTIME_DESTINATION}
    NO_SYSTEM_ENVIRONMENT_PATH
    )
endmacro()

set( LIST_TOOLS_TESTS
    dtiprocess
    ANTS
    ResampleDTIlogEuclidean
    ITKTransformTools
    BRAINSFit
    BRAINSDemonWarp
    )

if( NOT ${LOCAL_PROJECT_NAME}_SUPERBUILD )
  add_executable(${CLP}Test ${CLP}Test.cxx)
  add_dependencies(${CLP}Test ${CLP})
  target_link_libraries(${CLP}Test ${CLP}Lib ${ITK_LIBRARIES} ${BatchMake_LIBRARIES})
  set_target_properties(${CLP}Test PROPERTIES LABELS ${CLP})
  install(TARGETS ${CLP}Test
          DESTINATION ${INSTALL_RUNTIME_DESTINATION} )
  foreach( VAR ${LIST_TOOLS_TESTS} )
    FIND_EXTERNAL_TOOLS(${VAR})
    message(WARNING ${${VAR}PATH} )
  endforeach()
else()
  foreach( VAR ${LIST_TOOLS_TESTS} )
    if( ${VAR}TOOL )
      set(${VAR}PATH ${${VAR}TOOL})
    else()
      set(${VAR}PATH ${VAR})
    endif()
    message(WARNING ${${VAR}PATH} )
  endforeach()
# Finds DTI-Reg and runs it to show help
add_test(NAME ${CLP}HelpTest COMMAND ${CLP} --help )
# Finds ANTS and runs it to show help
add_test(NAME ANTSHelpTest COMMAND ${ANTSPATH} -h )
# Finds WarpImageMultiTransform and runs it to show help
add_test(NAME WarpImageMultiTransformHelpTest COMMAND ${WarpImageMultiTransformPATH} -h )
# Finds ResampleDTIlogEuclidean and runs it to show help
add_test(NAME ResampleDTIlogEuclideanHelpTest COMMAND ${ResampleDTIlogEuclideanPATH} --help )
# Finds dtiprocess and runs it to show help
add_test(NAME dtiprocessHelpTest COMMAND ${dtiprocessPATH} --help )
# Finds ITKTransformTools and runs it to show help
add_test(NAME ITKTransformToolsHelpTest COMMAND ${ITKTransformToolsPATH} version )

set(TEMP_DIR ${${CLP}_BINARY_DIR}/Testing/Temporary )

#set(BaselineImage DATA{Data/Baseline/ANTS-GreedyDiffeo-Baseline.nrrd})
set(BaselineImage Data/Baseline/ANTS-GreedyDiffeo-Baseline.nrrd)
set(MovingImage DATA{Data/Input/AdultData_DWI_QCed_DTI.nrrd})
set(MovingMask DATA{Data/Input/AdultData_DWI_QCed_DTI_mask.nrrd})
set(FixedImage DATA{Data/Input/AdultAtlas.nrrd})
set(FixedMask DATA{Data/Input/AdultAtlas_BrainMask.nrrd})

set(OutputImage ${TEMP_DIR}/ANTS-GreedyDiffeo-Baseline.nrrd )

ExternalData_add_test(${CLP}Data NAME ${CLP}ANTSRegistration COMMAND ${DTI-Reg_INSTALL_DIRECTORY}/${INSTALL_RUNTIME_DESTINATION}/DTI-RegTest
--compare
${BaselineImage}
${RegisteredImage}
--compareIntensityTolerance 0
ModuleEntryPoint
--movingVolume ${MovingImage}
--fixedVolume ${FixedImage}
--fixedMaskVolume ${FixedMask}
--movingMaskVolume ${MovingMask}
--method useScalar-ANTS
--outputVolume ${OutputImage}
--ANTSRegistrationType GreedyDiffeo
--ANTSPath ${ANTSPATH}
--dtiprocessPath ${SEM_LAUNCH_COMMAND} ${dtiprocessPATH}
--ResampleDTIPath ${SEM_LAUNCH_COMMAND} ${ResampleDTIlogEuclideanPATH}
--ITKTransformToolsPath ${ITKTransformToolsPATH}
--WarpImageMultiTransformPath ${WarpImageMultiTransformPATH}
)

#useScalar-BRAINS
#--BRAINSFitPath
#--BRAINSDemonWarpPath
ExternalData_add_target(${CLP}Data)

endif()
