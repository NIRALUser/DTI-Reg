// if using ANTS

#define DTIReg_Scalar_ANTS "GetFilename(fixedVolumeHead ${fixedVolume} NAME_WITHOUT_EXTENSION)\n\
GetFilename(fixedVolumeTail ${fixedVolume} NAME)\n\
#GetFilename(OutputDir ${fixedVolume} PATH)\n\
GetFilename(OutputDir ${outputVolume} PATH)\n\
If (${OutputDir} == '')\n\
  set (OutputDir '.')\n\
EndIf(${OutputDir})\n\
\n\
GetFilename(movingVolumeHead ${movingVolume} NAME_WITHOUT_EXTENSION)\n\
GetFilename(movingVolumeTail ${movingVolume} NAME)\n\
\n\
# FA map creation\n\
echo()\n\
echo('FA map creation...')\n\
echo('fixed FA map creation...')\n\
If (${outputFixedFAVolume} == '')\n\
  set (fixedFAMap ${OutputDir}/${fixedVolumeHead}_FA.nrrd)\n\
Else(${outputFixedFAVolume})\n\
  set (fixedFAMap ${outputFixedFAVolume})\n\
EndIf(${outputFixedFAVolume})\n\
If (${fixedMaskVolume} != '')\n\
  set (commanddtiprocess ${dtiprocessCmd} --dti_image ${fixedVolume} -f ${fixedFAMap} --mask ${fixedMaskVolume})\n\
Else(${fixedMaskVolume})\n\
 set (commanddtiprocess ${dtiprocessCmd} --dti_image ${fixedVolume} -f ${fixedFAMap}) \n\
EndIf (${fixedMaskVolume})\n\
Run (outputdtiprocess ${commanddtiprocess} errordtiprocess)\n\
If(${errordtiprocess} != '')\n\
  echo('Error dtiprocess: ' ${errordtiprocess})\n\
  exit()\n\
Endif(${errordtiprocess})\n\
\n\
echo('moving FA map creation...')\n\
If (${outputMovingFAVolume} == '')\n\
  set (movingFAMap ${OutputDir}/${movingVolumeHead}_FA.nrrd)\n\
Else(${outputMovingFAVolume})\n\
  set (movingFAMap ${outputMovingFAVolume})\n\
EndIf(${outputMovingFAVolume})\n\
If (${movingMaskVolume} != '')\n\
  set (commanddtiprocess ${dtiprocessCmd} --dti_image ${movingVolume} -f ${movingFAMap} --mask ${movingMaskVolume})\n\
Else(${movingMaskVolume})\n\
  set (commanddtiprocess ${dtiprocessCmd} --dti_image ${movingVolume} -f ${movingFAMap})\n\
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
    set (ANTSOutbase ${OutputDir}/${movingVolumeHead}_FA_)\n\
  EndIf (${ANTSOutbase})\n\
\n\
  set(IsWarping 1)\n\
  If (${ANTSRegistrationType} == 'Affine' || ${ANTSRegistrationType} == 'Rigid')\n\
    set(IsWarping 0)\n\
  EndIf()\n\
\n\
  set (commandANTS ${ANTSCmd} 3 -m ${ANTSSimilarityMetric}[${fixedFAMap},${movingFAMap},1,${ANTSSimilarityParameter}] -o ${ANTSOutbase})\n\
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
    exit()\n\
  Endif(${errorANTS})\n\
\n\
# Correcting Affine transform file using python script\n\
GetFilename(ANTSOutbasePath ${ANTSOutbase} PATH)\n\
GetFilename(ANTSOutbaseName ${ANTSOutbase} NAME)\n\
set (PythonScriptCommand ${PythonExecutable} ${PythonScriptPath} ${ANTSOutbasePath} ${ANTSOutbaseName}Affine.txt)\n\
echo('Correcting Affine transform file using python script...')\n\
Run (outputPythonScript ${PythonScriptCommand} errorPythonScript)\n\
DeleteFile(${PythonScriptPath})\n\
\n\
# Define deformation files\n\
set(Transform ${ANTSOutbase}Affine.txt)\n\
set (DeformationField ${ANTSOutbase}Warp.nii.gz)\n\
\n\
# Applying deformation to FA map\n\
If (${outputResampledFAVolume} != '')\n\
  echo()\n\
  echo('FA resampling...')\n\
  set (commandWarpImageMultiTransform ${WarpImageMultiTransformCmd} 3 ${movingFAMap} ${outputResampledFAVolume} -R ${outputFixedFAVolume})\n\
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
EndIf(${outputResampledFAVolume})\n\
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
set (commandWarpTensorImageMultiTransform ${WarpTensorImageMultiTransformCmd} 3 ${movingVolume} ${ResampledDTI} -R ${fixedVolume})\n\
If(${IsWarping} == 1)\n\
  set (commandWarpTensorImageMultiTransform ${commandWarpTensorImageMultiTransform} ${DeformationField} ${Transform})\n\
Else(${IsWarping})\n\
  set (commandWarpTensorImageMultiTransform ${commandWarpTensorImageMultiTransform} ${Transform})\n\
EndIf(${IsWarping})\n\
Run (outputWarpTensorImageMultiTransform ${commandWarpTensorImageMultiTransform} errorWarpTensorImageMultiTransform)\n\
If(${errorWarpTensorImageMultiTransform} != '')\n\
  echo('Error WarpTensorImageMultiTransform: ' ${errorWarpTensorImageMultiTransform})\n\
  exit()\n\
Endif(${errorWarpTensorImageMultiTransform})\n\
\n\
If (${outputTransform} != '')\n\
  echo()\n\
  echo('Copying Transform file...')\n\
  CopyFile(${Transform} ${outputTransform})\n\
  DeleteFile(${Transform})\n\
EndIf (${outputTransform})\n\
\n\
If (${outputDeformationFieldVolume} != '')\n\
  echo()\n\
  echo('Computing deformation field...')\n\
  # the ${fixedVolume} and ${movingVolume} are not actually used, but ResampleDTI needs real files given to work\n\
  set(ResampleDTIConcatenationCmd ${ResampleDTICmd} -f ${Transform} -H ${DeformationField} --hfieldtype displacement ${fixedVolume} ${movingVolume} --concatenationOnly --outputDisplacementField ${outputDeformationFieldVolume})\n\
  Run(outputResampleDTIConcatenationCmd ${ResampleDTIConcatenationCmd} errorResampleDTIConcatenationCmd)\n\
  If(${errorResampleDTIConcatenationCmd} != '')\n\
    echo('Error ResampleDTIlogEuclidean: ' ${errorResampleDTIConcatenationCmd})\n\
    exit()\n\
  Endif(${errorResampleDTIConcatenationCmd})\n\
#  echo('Copying deformation field...')\n\
#  CopyFile(${DeformationField} ${outputDeformationFieldVolume})\n\
#  DeleteFile(${DeformationField})\n\
EndIf(${outputDeformationFieldVolume})\n\
\n\
#Delete temporary files\n\
If (${outputFixedFAVolume} == '')\n\
#  DeleteFile(${fixedFAMap})\n\
EndIf(${outputFixedFAVolume})\n\
If (${outputMovingFAVolume} == '')\n\
#  DeleteFile(${movingFAMap})\n\
EndIf(${outputMovingFAVolume})"

////////////////////////////////////////////////////////////////////////
// if using BRAINS

#define DTIReg_Scalar_BRAINS "GetFilename(fixedVolumeHead ${fixedVolume} NAME_WITHOUT_EXTENSION)\n\
GetFilename(fixedVolumeTail ${fixedVolume} NAME)\n\
#GetFilename(OutputDir ${fixedVolume} PATH)\n\
GetFilename(OutputDir ${outputVolume} PATH)\n\
If (${OutputDir} == '')\n\
  set (OutputDir '.')\n\
EndIf(${OutputDir})\n\
\n\
GetFilename(movingVolumeHead ${movingVolume} NAME_WITHOUT_EXTENSION)\n\
GetFilename(movingVolumeTail ${movingVolume} NAME)\n\
\n\
# FA map creation\n\
echo()\n\
echo('FA map creation...')\n\
echo('fixed FA map creation...')\n\
If (${outputFixedFAVolume} == '')\n\
  set (fixedFAMap ${OutputDir}/${fixedVolumeHead}_FA.nrrd)\n\
Else(${outputFixedFAVolume})\n\
  set (fixedFAMap ${outputFixedFAVolume})\n\
EndIf(${outputFixedFAVolume})\n\
If (${fixedMaskVolume} != '')\n\
  set (commanddtiprocess ${dtiprocessCmd} --dti_image ${fixedVolume} -f ${fixedFAMap} --mask ${fixedMaskVolume})\n\
Else(${fixedMaskVolume})\n\
 set (commanddtiprocess ${dtiprocessCmd} --dti_image ${fixedVolume} -f ${fixedFAMap})\n\
EndIf (${fixedMaskVolume})\n\
Run (outputdtiprocess ${commanddtiprocess} errordtiprocess)\n\
If(${errordtiprocess} != '')\n\
  echo('Error dtiprocess: ' ${errordtiprocess})\n\
  exit()\n\
Endif(${errordtiprocess})\n\
\n\
echo('moving FA map creation...')\n\
If (${outputMovingFAVolume} == '')\n\
  set (movingFAMap ${OutputDir}/${movingVolumeHead}_FA.nrrd)\n\
Else(${outputMovingFAVolume})\n\
  set (movingFAMap ${outputMovingFAVolume})\n\
EndIf(${outputMovingFAVolume})\n\
If (${movingMaskVolume} != '')\n\
  set (commanddtiprocess ${dtiprocessCmd} --dti_image ${movingVolume} -f ${movingFAMap} --mask ${movingMaskVolume})\n\
Else(${movingMaskVolume})\n\
  set (commanddtiprocess ${dtiprocessCmd} --dti_image ${movingVolume} -f ${movingFAMap})\n\
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
  Else()\n\
     set (TransformType 'Rigid,Affine')\n\
     set (RegSuffix 'AffReg')\n\
  EndIf()\n\
EndIf(${BRAINSRegistrationType})\n\
echo()\n\
echo(${TransformType} ' registration...')\n\
\n\
If (${outputResampledFAVolume} != '')\n\
  set (ResampledFAMap ${outputResampledFAVolume})\n\
Else(${outputResampledFAVolume})\n\
  set (ResampledFAMap ${OutputDir}/${movingVolumeHead}_FA_${RegSuffix}.nrrd)\n\
Endif(${outputResampledFAVolume})\n\
If (${outputTransform} != '')\n\
  set (Transform ${outputTransform})\n\
Else(${outputTransform})\n\
  set (Transform ${OutputDir}/${movingVolumeHead}_FA_${RegSuffix}.txt)\n\
Endif(${outputTransform})\n\
If (${initialAffine} != '')\n\
  set (commandBRAINSFit ${BRAINSFitCmd} --fixedVolume ${fixedFAMap} --movingVolume ${movingFAMap} --initialTransform ${initialAffine} --outputTransform ${Transform} --outputVolume ${ResampledFAMap} --outputVolumePixelType ushort --transformType ${TransformType} --interpolationMode Linear)\n\
Else(${initialAffine})\n\
  set (commandBRAINSFit ${BRAINSFitCmd} --fixedVolume ${fixedFAMap} --movingVolume ${movingFAMap} --initializeTransformMode ${BRAINSinitializeTransformMode} --outputTransform ${Transform} --outputVolume ${ResampledFAMap} --outputVolumePixelType ushort --transformType ${TransformType} --interpolationMode Linear)\n\
Endif(${initialAffine})\n\
Run (outputBRAINSFit ${commandBRAINSFit} errorBRAINSFit)\n\
#If(${errorBRAINSFit} != '')\n\
#  echo('Error BRAINSFit: ' ${errorBRAINSFit})\n\
#  exit()\n\
#Endif(${errorBRAINSFit})\n\
If (${IsDemonsWarping} == 1)\n\
  DeleteFile(${ResampledFAMap})\n\
EndIf(${IsDemonsWarping})\n\
\n\
# Warping\n\
If (${IsDemonsWarping} == 1)\n\
  echo()\n\
  echo('Warping...')\n\
  If (${outputResampledFAVolume} != '')\n\
    set (ResampledFAMap ${outputResampledFAVolume})\n\
  Else(${outputResampledFAVolume})\n\
    set (ResampledFAMap ${OutputDir}/${movingVolumeHead}_FA_warp.nrrd)\n\
  EndIf(${outputResampledFAVolume})\n\
  If (${outputDeformationFieldVolume} != '')\n\
    set (DeformationField ${outputDeformationFieldVolume})\n\
  Else(${outputDeformationFieldVolume})\n\
    set (DeformationField ${OutputDir}/${movingVolumeHead}_FA_warpfield.nrrd)\n\
  EndIf(${outputDeformationFieldVolume})\n\
  set (commandBRAINSDemonWarp ${BRAINSDemonWarpCmd} --fixedVolume ${fixedFAMap} --movingVolume ${movingFAMap} --outputVolume ${ResampledFAMap} --outputDeformationFieldVolume ${DeformationField} --outputPixelType ushort --interpolationMode Linear --registrationFilterType ${BRAINSRegistrationType} --histogramMatch --numberOfHistogramBins ${BRAINSnumberOfHistogramLevels} --numberOfMatchPoints ${BRAINSnumberOfMatchPoints} --initializeWithTransform ${Transform} --numberOfPyramidLevels ${BRAINSnumberOfPyramidLevels} --arrayOfPyramidLevelIterations ${BRAINSarrayOfPyramidLevelIterations})\n\
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
set (commandResampleDTI ${ResampleDTICmd} ${movingVolume} ${ResampledDTI} --Reference ${fixedVolume})\n\
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
If (${outputFixedFAVolume} == '')\n\
#  DeleteFile(${fixedFAMap})\n\
EndIf(${outputFixedFAVolume})\n\
If (${outputMovingFAVolume} == '')\n\
#  DeleteFile(${movingFAMap})\n\
EndIf(${outputMovingFAVolume})\n\
If (${outputResampledFAVolume} == '')\n\
#  DeleteFile(${ResampledFAMap})\n\
EndIf(${outputResampledFAVolume})"