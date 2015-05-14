// if using ANTS

#define DTIReg_Scalar_ANTS "GetFilename(fixedVolumeHead ${fixedVolume} NAME_WITHOUT_EXTENSION)\n\
GetFilename(fixedVolumeTail ${fixedVolume} NAME)\n\
\n\
GetFilename(movingVolumeHead ${movingVolume} NAME_WITHOUT_EXTENSION)\n\
GetFilename(movingVolumeTail ${movingVolume} NAME)\n\
\n\
If( ${ScalarMeasurement} == 'FA')\n\
  set(ScalarMeasurementFlag -f)\n\
Else( ${ScalarMeasurement} )\n\
  set(ScalarMeasurementFlag -m)\n\
EndIf( ${ScalarMeasurement} )\n\
If(${useScalar} == FALSE )\n\
# Scalar measurement (FA or MD) map creation\n\
  echo()\n\
  echo( ${ScalarMeasurement}' map creation...')\n\
  echo('fixed '${ScalarMeasurement}' map creation...')\n\
  If (${outputFixedScalarVolume} == '')\n\
    set (fixedScalarMap ${OutputDir}/${fixedVolumeHead}_${ScalarMeasurement}.nrrd)\n\
  Else(${outputFixedScalarVolume})\n\
    set (fixedScalarMap ${outputFixedScalarVolume})\n\
  EndIf(${outputFixedScalarVolume})\n\
  If (${fixedMaskVolume} != '')\n\
    set (commanddtiprocess ${dtiprocessCmd} --dti_image ${fixedVolume} ${ScalarMeasurementFlag} ${fixedScalarMap} --mask ${fixedMaskVolume} --correction ${TensorCorrection})\n\
  Else(${fixedMaskVolume})\n\
   set (commanddtiprocess ${dtiprocessCmd} --dti_image ${fixedVolume} ${ScalarMeasurementFlag} ${fixedScalarMap} --correction ${TensorCorrection})\n\
  EndIf (${fixedMaskVolume})\n\
  Run (outputdtiprocess ${commanddtiprocess} errordtiprocess)\n\
  If(${errordtiprocess} != '')\n\
    echo('Error dtiprocess: ' ${errordtiprocess})\n\
    exit()\n\
  Endif(${errordtiprocess})\n\
Endif(${useScalar} == FALSE )\n\
echo('moving '${ScalarMeasurement}' map creation...')\n\
If (${outputMovingScalarVolume} == '')\n\
  set (movingScalarMap ${OutputDir}/${movingVolumeHead}_${ScalarMeasurement}.nrrd)\n\
Else(${outputMovingScalarVolume})\n\
  set (movingScalarMap ${outputMovingScalarVolume})\n\
EndIf(${outputMovingScalarVolume})\n\
If (${movingMaskVolume} != '')\n\
  set (commanddtiprocess ${dtiprocessCmd} --dti_image ${movingVolume} ${ScalarMeasurementFlag} ${movingScalarMap} --mask ${movingMaskVolume} --correction ${TensorCorrection})\n\
Else(${movingMaskVolume})\n\
  set (commanddtiprocess ${dtiprocessCmd} --dti_image ${movingVolume} ${ScalarMeasurementFlag} ${movingScalarMap} --correction ${TensorCorrection})\n\
EndIf(${movingMaskVolume})\n\
Run (outputdtiprocess ${commanddtiprocess} errordtiprocess)\n\
If(${errordtiprocess} != '')\n\
  echo('Error dtiprocess: ' ${errordtiprocess})\n\
  exit()\n\
Endif(${errordtiprocess})\n\
\n\
# Registration via ANTS\n\
\n\
echo()\n\
echo('Registration...')\n\
set(Transform '')\n\
set (DeformationField '')\n\
\n\
  If (${ANTSOutbase} == '')\n\
    set (ANTSOutbase ${OutputDir}/${movingVolumeHead}_ANTS_)\n\
  EndIf (${ANTSOutbase})\n\
\n\
  set(IsWarping 1)\n\
  If (${ANTSRegistrationType} == 'Affine' || ${ANTSRegistrationType} == 'Rigid')\n\
    set(IsWarping 0)\n\
  EndIf()\n\
\n\
  set (commandANTS ${ANTSCmd} 3 -m ${ANTSSimilarityMetric}[${fixedScalarMap},${movingScalarMap},1,${ANTSSimilarityParameter}] -o ${ANTSOutbase} )\n\
\n\
  If (${initialAffine} != '')\n\
    set (commandANTS ${commandANTS} -a ${initialAffine})\n\
  EndIf(${initialAffine})\n\
\n\
  If (${ANTSRegistrationType} == 'Affine')\n\
    set (commandANTS ${commandANTS} -i 0)\n\
  EndIf(${ANTSRegistrationType})\n\
\n\
  If (${ANTSRegistrationType} == 'Rigid')\n\
    set (commandANTS ${commandANTS} -i 0 --rigid-affine true)\n\
  EndIf(${ANTSRegistrationType})\n\
\n\
  If (${ANTSRegistrationType} == 'GreedyDiffeo')\n\
    set (commandANTS ${commandANTS} -i ${ANTSIterations} -t SyN[${ANTSTransformationStep}])\n\
    If (${ANTSGaussianSmoothingOff} == 0)\n\
      set (commandANTS ${commandANTS} -r Gauss[${ANTSGaussianSigma},0])\n\
    EndIf(${ANTSGaussianSmoothingOff})\n\
  EndIf(${ANTSRegistrationType})\n\
\n\
  If (${ANTSRegistrationType} == 'SpatioTempDiffeo')\n\
    set (commandANTS ${commandANTS} -i ${ANTSIterations} -t SyN[${ANTSTransformationStep}])\n\
    If (${ANTSGaussianSmoothingOff} == 0)\n\
      set (commandANTS ${commandANTS} -r Gauss[${ANTSGaussianSigma},0.5])\n\
    EndIf(${ANTSGaussianSmoothingOff})\n\
  EndIf(${ANTSRegistrationType})\n\
  If (${ANTSUseHistogramMatching} == 1)\n\
    set (commandANTS ${commandANTS} --use-Histogram-Matching )\n\
  EndIf(${ANTSUseHistogramMatching})\n\
\n\
  If (${ANTSRegistrationType} == 'Elast')\n\
    set (commandANTS ${commandANTS} -i ${ANTSIterations} -t Elast)\n\
  EndIf(${ANTSRegistrationType})\n\
\n\
  If (${ANTSRegistrationType} == 'Exp')\n\
    set (commandANTS ${commandANTS} -i ${ANTSIterations} -t Exp[${ANTSTransformationStep},5])\n\
  EndIf(${ANTSRegistrationType})\n\
\n\
  If (${ANTSRegistrationType} == 'GreedyExp')\n\
    set (commandANTS ${commandANTS} -i ${ANTSIterations} -t GreedyExp)\n\
  EndIf(${ANTSRegistrationType})\n\
\n\
  Run (outputANTS ${commandANTS} errorANTS)\n\
  echo('ANTS: ' ${outputANTS})\n\
  If(${errorANTS} != '')\n\
    echo('Error ANTS: ' ${errorANTS})\n\
#    exit()\n\
  Endif(${errorANTS})\n\
\n\
# Define deformation files\n\
set(Transform ${ANTSOutbase}Affine.txt)\n\
set (DeformationField ${ANTSOutbase}Warp.nii.gz)\n\
\n\
# Applying deformation to scalar measurement (FA or MD) map\n\
If (${outputResampledScalarVolume} != '')\n\
  echo()\n\
  echo(${ScalarMeasurement}' resampling...')\n\
  set (commandWarpImageMultiTransform ${WarpImageMultiTransformCmd} 3 ${movingScalarMap} ${outputResampledScalarVolume} -R ${outputFixedScalarVolume})\n\
  If(${IsWarping} == 1)\n\
    set (commandWarpImageMultiTransform ${commandWarpImageMultiTransform} ${DeformationField} ${Transform})\n\
  Else(${IsWarping})\n\
    set (commandWarpImageMultiTransform ${commandWarpImageMultiTransform} ${Transform})\n\
  EndIf(${IsWarping})\n\
\n\
  Run (outputWarpImageMultiTransform ${commandWarpImageMultiTransform} errorWarpImageMultiTransform)\n\
  If(${errorWarpImageMultiTransform} != '')\n\
    echo('Error WarpImageMultiTransform: ' ${errorWarpImageMultiTransform})\n\
    exit()\n\
  Endif(${errorWarpImageMultiTransform})\n\
EndIf(${outputResampledScalarVolume})\n\
\n\
# DTI resampling\n\
echo()\n\
echo('DTI resampling...')\n\
If (${outputVolume} != '')\n\
  set (ResampledDTI ${outputVolume})\n\
Else(${outputVolume})\n\
    set (ResampledDTI ${OutputDir}/${movingVolumeHead}_warp.nrrd)\n\
EndIf(${outputVolume})\n\
\n\
set (commandWarpTensorImageMultiTransform ${ResampleDTICmd} ${movingVolume} ${ResampledDTI} -R ${fixedVolume} --correction ${TensorCorrection})\n\
If(${IsWarping} == 1)\n\
  set (commandWarpTensorImageMultiTransform ${commandWarpTensorImageMultiTransform} --defField ${DeformationField} --hfieldtype displacement -f ${Transform})\n\
Else(${IsWarping})\n\
  set (commandWarpTensorImageMultiTransform ${commandWarpTensorImageMultiTransform} -f ${Transform})\n\
EndIf(${IsWarping})\n\
Run (outputWarpTensorImageMultiTransform ${commandWarpTensorImageMultiTransform} errorWarpTensorImageMultiTransform)\n\
FileExists(fileCreated ${ResampledDTI})\n\
If(${fileCreated} != 1)\n\
  echo('Error WarpTensorImageMultiTransform: ' ${errorWarpTensorImageMultiTransform})\n\
  exit()\n\
Endif(${errorWarpTensorImageMultiTransform})\n\
\n\
If(${IsWarping} == 1)\n\
  If (${outputDeformationFieldVolume} != '')\n\
    echo()\n\
    echo('Computing deformation field...')\n\
    set(ConcatenationCmd ${ITKTransformToolsCmd} concatenate ${outputDeformationFieldVolume} -r ${fixedVolume} ${DeformationField} displacement ${Transform})\n\
    Run(outputConcatenationCmd ${ConcatenationCmd} errorConcatenationCmd)\n\
    echo(${outputConcatenationCmd})\n\
    If(${errorConcatenationCmd} != '')\n\
      echo('Error ITKTransformTools: ' ${errorConcatenationCmd})\n\
      exit()\n\
    Endif(${errorConcatenationCmd})\n\
  EndIf(${outputDeformationFieldVolume})\n\
  If (${outputInverseDeformationFieldVolume} != '')\n\
    echo()\n\
    echo('Computing inverse deformation field...')\n\
    set(Transform_inv ${ANTSOutbase}Affine_inv.txt)\n\
    set( CmdInvert ${ITKTransformToolsCmd} invert ${Transform} ${Transform_inv} )\n\
    run( outputCmdInvert ${CmdInvert} errorCmdInvert)\n\
    echo( ${outputCmdInvert} )\n\
    If(${errorCmdInvert} != '')\n\
      echo('Error ITKTransformTools: ' ${errorCmdInvert})\n\
      exit()\n\
    Endif(${errorCmdInvert})\n\
    set(InverseDeformationField ${ANTSOutbase}InverseWarp.nii.gz)\n\
    set(ConcatenationCmd ${ITKTransformToolsCmd} concatenate ${outputInverseDeformationFieldVolume} -r ${movingVolume} ${Transform_inv} ${InverseDeformationField} displacement )\n\
    Run(outputConcatenationCmd ${ConcatenationCmd} errorConcatenationCmd)\n\
    echo(${outputConcatenationCmd})\n\
    If(${errorConcatenationCmd} != '')\n\
      echo('Error ITKTransformTools: ' ${errorConcatenationCmd})\n\
      exit()\n\
    Endif(${errorConcatenationCmd})\n\
  EndIf(${outputInverseDeformationFieldVolume})\n\
Else(${IsWarping} == 1)\n\
  If (${outputDeformationFieldVolume} != '')\n\
    echo('Warning: '${outputDeformationFieldVolume}' is specified but registration is only rigid or affine. This value will not be used.')\n\
  EndIf(${outputDeformationFieldVolume})\n\
  If (${outputInverseDeformationFieldVolume} != '')\n\
    echo('Warning: '${outputInverseDeformationFieldVolume}' is specified but registration is only rigid or affine. This value will not be used.')\n\
  EndIf(${outputInverseDeformationFieldVolume})\n\
EndIf(${IsWarping})\n\
\n\
If (${outputTransform} != '')\n\
  echo()\n\
  echo('Copying Transform file...')\n\
#  set( Cmd /Applications/Slicer.app/Contents/Extensions-22599/DTIAtlasBuilder/lib/Slicer-4.3/ExternalBin/MO2Aff ${Transform} ${outputTransform} )\n\
  set( Cmd ${ITKTransformToolsCmd} MO2Aff ${Transform} ${outputTransform} )\n\
  echo(${Cmd})\n\
  Run(outputCmd ${Cmd} errorCmd)\n\
  If(${errorCmd} != '')\n\
    echo('Error ITKTransformTools: ' ${errorCmd})\n\
    exit()\n\
  Endif(${errorCmd})\n\
#  DeleteFile(${Transform})\n\
EndIf (${outputTransform})\n\
\n\
#Delete temporary files\n\
If (${outputFixedScalarVolume} == '')\n\
#  DeleteFile(${fixedScalarMap})\n\
EndIf(${outputFixedScalarVolume})\n\
If (${outputMovingScalarVolume} == '')\n\
#  DeleteFile(${movingScalarMap})\n\
EndIf(${outputMovingScalarVolume})"

////////////////////////////////////////////////////////////////////////
// if using BRAINS

#define DTIReg_Scalar_BRAINS "GetFilename(fixedVolumeHead ${fixedVolume} NAME_WITHOUT_EXTENSION)\n\
GetFilename(fixedVolumeTail ${fixedVolume} NAME)\n\
\n\
GetFilename(movingVolumeHead ${movingVolume} NAME_WITHOUT_EXTENSION)\n\
GetFilename(movingVolumeTail ${movingVolume} NAME)\n\
\n\
If( ${outputInverseDeformationFieldVolume} != '' )\n\
  echo('Warning: '${outputInverseDeformationFieldVolume}' is specified but registration is performed with BRAINSDemonWarp. This value will not be used.')\n\
EndIf()\n\
If( ${ScalarMeasurement} == 'FA')\n\
  set(ScalarMeasurementFlag -f)\n\
Else( ${ScalarMeasurement} )\n\
  set(ScalarMeasurementFlag -m)\n\
EndIf( ${ScalarMeasurement} )\n\
If(${useScalar} == FALSE )\n\
  # Scalar Measurement (FA or MD) map creation\n\
  echo()\n\
  echo(${ScalarMeasurement}' map creation...')\n\
  echo('fixed '${ScalarMeasurement}' map creation...')\n\
  If (${outputFixedScalarVolume} == '')\n\
    set (fixedScalarMap ${OutputDir}/${fixedVolumeHead}_${ScalarMeasurement}.nrrd)\n\
  Else(${outputFixedScalarVolume})\n\
    set (fixedScalarMap ${outputFixedScalarVolume})\n\
  EndIf(${outputFixedScalarVolume})\n\
  If (${fixedMaskVolume} != '')\n\
    set (commanddtiprocess ${dtiprocessCmd} --dti_image ${fixedVolume} ${ScalarMeasurementFlag} ${fixedScalarMap} --mask ${fixedMaskVolume} --correction ${TensorCorrection})\n\
  Else(${fixedMaskVolume})\n\
   set (commanddtiprocess ${dtiprocessCmd} --dti_image ${fixedVolume} ${ScalarMeasurementFlag} ${fixedScalarMap} --correction ${TensorCorrection})\n\
  EndIf (${fixedMaskVolume})\n\
  Run (outputdtiprocess ${commanddtiprocess} errordtiprocess)\n\
  If(${errordtiprocess} != '')\n\
    echo('Error dtiprocess: ' ${errordtiprocess})\n\
    exit()\n\
  Endif(${errordtiprocess})\n\
EndIf(${useScalar} == FALSE )\n\
\n\
echo('moving '${ScalarMeasurement}' map creation...')\n\
If (${outputMovingScalarVolume} == '')\n\
  set (movingScalarMap ${OutputDir}/${movingVolumeHead}_${ScalarMeasurement}.nrrd)\n\
Else(${outputMovingScalarVolume})\n\
  set (movingScalarMap ${outputMovingScalarVolume})\n\
EndIf(${outputMovingScalarVolume})\n\
If (${movingMaskVolume} != '')\n\
  set (commanddtiprocess ${dtiprocessCmd} --dti_image ${movingVolume} ${ScalarMeasurementFlag} ${movingScalarMap} --mask ${movingMaskVolume} --correction ${TensorCorrection})\n\
Else(${movingMaskVolume})\n\
  set (commanddtiprocess ${dtiprocessCmd} --dti_image ${movingVolume} ${ScalarMeasurementFlag} ${movingScalarMap} --correction ${TensorCorrection})\n\
EndIf(${movingMaskVolume})\n\
Run (outputdtiprocess ${commanddtiprocess} errordtiprocess)\n\
If(${errordtiprocess} != '')\n\
  echo('Error dtiprocess: ' ${errordtiprocess})\n\
  exit()\n\
Endif(${errordtiprocess})\n\
\n\
\n\
#Registration\n\
If (${BRAINSRegistrationType} == 'Rigid' || ${BRAINSRegistrationType} == 'Affine' || ${BRAINSRegistrationType} == 'BSpline')\n\
  set(IsDemonsWarping 0)\n\
Else (${BRAINSRegistrationType})\n\
  set(IsDemonsWarping 1)\n\
EndIf(${BRAINSRegistrationType})\n\
\n\
# Registration via BRAINSFit \n\
set (Transform '')\n\
If (${BRAINSRegistrationType} == 'Rigid')\n\
  set (TransformType 'Rigid')\n\
  set (RegSuffix 'RReg')\n\
Else(${BRAINSRegistrationType})\n\
  If (${BRAINSRegistrationType} == 'BSpline')\n\
    set (TransformType 'Rigid,Affine,BSpline')\n\
    set (RegSuffix 'BSpline')\n\
    set (outputTransform ${outputBSplineTransform} )\n\
  Else()\n\
     set (TransformType 'Rigid,Affine')\n\
     set (RegSuffix 'AffReg')\n\
  EndIf()\n\
EndIf(${BRAINSRegistrationType})\n\
echo()\n\
echo(${TransformType} ' registration...')\n\
\n\
If (${outputResampledScalarVolume} != '')\n\
  set (ResampledScalarMap ${outputResampledScalarVolume})\n\
Else(${outputResampledScalarVolume})\n\
  set (ResampledScalarMap ${OutputDir}/${movingVolumeHead}_${ScalarMeasurement}_${RegSuffix}.nrrd)\n\
Endif(${outputResampledScalarVolume})\n\
If (${outputTransform} != '')\n\
  set (Transform ${outputTransform})\n\
Else(${outputTransform})\n\
  set (Transform ${OutputDir}/${movingVolumeHead}_BRAINSFit_${RegSuffix}.txt)\n\
Endif(${outputTransform})\n\
If (${initialAffine} != '')\n\
  set (commandBRAINSFit ${BRAINSFitCmd} --fixedVolume ${fixedScalarMap} --movingVolume ${movingScalarMap} --initialTransform ${initialAffine} --outputTransform ${Transform} --outputVolume ${ResampledScalarMap} --outputVolumePixelType ushort --transformType ${TransformType} --interpolationMode Linear)\n\
Else(${initialAffine})\n\
  set (commandBRAINSFit ${BRAINSFitCmd} --fixedVolume ${fixedScalarMap} --movingVolume ${movingScalarMap} --initializeTransformMode ${BRAINSinitializeTransformMode} --outputTransform ${Transform} --outputVolume ${ResampledScalarMap} --outputVolumePixelType ushort --transformType ${TransformType} --interpolationMode Linear)\n\
Endif(${initialAffine})\n\
Run (outputBRAINSFit ${commandBRAINSFit} errorBRAINSFit)\n\
If(${errorBRAINSFit} != '')\n\
  echo('Error BRAINSFit: ' ${errorBRAINSFit})\n\
#  exit()\n\
Endif(${errorBRAINSFit})\n\
If (${IsDemonsWarping} == 1)\n\
  DeleteFile(${ResampledScalarMap})\n\
EndIf(${IsDemonsWarping})\n\
\n\
# Warping\n\
If (${IsDemonsWarping} == 1)\n\
  echo()\n\
  echo('Warping...')\n\
  If (${outputResampledScalarVolume} != '')\n\
    set (ResampledScalarMap ${outputResampledScalarVolume})\n\
  Else(${outputResampledScalarVolume})\n\
    set (ResampledScalarMap ${OutputDir}/${movingVolumeHead}_${ScalarMeasurement}_warp.nrrd)\n\
  EndIf(${outputResampledScalarVolume})\n\
  If (${outputDeformationFieldVolume} != '')\n\
    set (DeformationField ${outputDeformationFieldVolume})\n\
  Else(${outputDeformationFieldVolume})\n\
    set (DeformationField ${OutputDir}/${movingVolumeHead}_${ScalarMeasurement}_warpfield.nrrd)\n\
  EndIf(${outputDeformationFieldVolume})\n\
  set (commandBRAINSDemonWarp ${BRAINSDemonWarpCmd} --fixedVolume ${fixedScalarMap} --movingVolume ${movingScalarMap} --outputVolume ${ResampledScalarMap} --outputDeformationFieldVolume ${DeformationField} --outputPixelType ushort --interpolationMode Linear --registrationFilterType ${BRAINSRegistrationType} --histogramMatch --numberOfHistogramBins ${BRAINSnumberOfHistogramLevels} --numberOfMatchPoints ${BRAINSnumberOfMatchPoints} --initializeWithTransform ${Transform} --numberOfPyramidLevels ${BRAINSnumberOfPyramidLevels} --arrayOfPyramidLevelIterations ${BRAINSarrayOfPyramidLevelIterations})\n\
  \n\
  If (${BRAINSinitialDeformationField} != '')\n\
    set (commandBRAINSDemonWarp ${commandBRAINSDemonWarp} --initializeWithDeformationField ${BRAINSinitialDeformationField})\n\
  EndIf(${BRAINSinitialDeformationField})\n\
  Run (outputBRAINSDemonWarp ${commandBRAINSDemonWarp} errorBRAINSDemonWarp)\n\
  If(${errorBRAINSDemonWarp} != '')\n\
    echo('Error BRAINSDemonWarp: ' ${errorBRAINSDemonWarp})\n\
    exit()\n\
  Endif(${errorBRAINSDemonWarp})\n\
Endif(${IsDemonsWarping})\n\
\n\
# DTI resampling\n\
echo()\n\
echo('DTI resampling...')\n\
If (${outputVolume} != '')\n\
  set (ResampledDTI ${outputVolume})\n\
Else(${outputVolume})\n\
  set (ResampledDTI ${OutputDir}/${movingVolumeHead}_warp.nrrd)\n\
EndIf(${outputVolume})\n\
set (commandResampleDTI ${ResampleDTICmd} ${movingVolume} ${ResampledDTI} --Reference ${fixedVolume} --correction ${TensorCorrection})\n\
If (${IsDemonsWarping} == 1)\n\
  set (commandResampleDTI ${commandResampleDTI} --defField ${DeformationField} --hfieldtype displacement)\n\
Else(${IsDemonsWarping})\n\
  set (commandResampleDTI ${commandResampleDTI} --transformationFile ${Transform})\n\
EndIf(${IsDemonsWarping})\n\
Run (outputResampleDTI ${commandResampleDTI} errorResampleDTI)\n\
If(${errorResampleDTI} != '')\n\
  echo('Error ResampleDTI: ' ${errorResampleDTI})\n\
  exit()\n\
Endif(${errorResampleDTI})\n\
\n\
#Delete temporary files\n\
#If (${outputVolume} == '')\n\
#  DeleteFile(${ResampledDTI})\n\
#EndIf(${outputVolume})\n\
#If (${outputTransform} == '')\n\
#  DeleteFile(${Transform})\n\
#EndIf(${outputTransform})\n\
#If (${outputDeformationFieldVolume} == '')\n\
#  DeleteFile(${DeformationField})\n\
#EndIf(${outputDeformationFieldVolume})\n\
If (${outputFixedScalarVolume} == '')\n\
#  DeleteFile(${fixedScalarMap})\n\
EndIf(${outputFixedScalarVolume})\n\
If (${outputMovingScalarVolume} == '')\n\
#  DeleteFile(${movingScalarMap})\n\
EndIf(${outputMovingScalarVolume})\n\
If (${outputResampledScalarVolume} == '')\n\
#  DeleteFile(${ResampledScalarMap})\n\
EndIf(${outputResampledScalarVolume})"


////////////////////////////////////////////////////////////////////////
// if using DTITK
/*
#define DTIReg_DTITK "GetFilename(fixedVolumeHead ${fixedVolume} NAME_WITHOUT_EXTENSION)\n\
GetFilename(fixedVolumeTail ${fixedVolume} NAME)\n\
GetFilename(OutputDir ${outputVolume} PATH)\n\
If (${OutputDir} == '')\n\
  set (OutputDir '.')\n\
EndIf(${OutputDir})\n\
\n\
GetFilename(movingVolumeHead ${movingVolume} NAME_WITHOUT_EXTENSION)\n\
GetFilename(movingVolumeTail ${movingVolume} NAME)"
*/
//Convert format: from nrrd to nii - Using DTIConvert (included in DTITK)
//Convert units (by default x1000)
//Affine transform (use default parameters)
//Diffeomorphic transform (use default parameters)

