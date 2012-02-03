#include <iostream>
#include <fstream>

#include <itksys/SystemTools.hxx>

#include <bmScriptParser.h>

#include "DTI-RegCLP.h"
#include "DTI-Reg_Config.h"


int SetPath( std::string &pathString , const char* name , std::vector< std::string >  path_vec )
{
  if( pathString.empty() || !pathString.substr(pathString.size() - 9 , 9 ).compare( "-NOTFOUND" ) )
  {
    pathString= itksys::SystemTools::FindProgram( name , path_vec ) ;
    if( !pathString.compare( "" ) )
    {
      std::cerr << name << " is missing or its PATH is not set" << std::endl ;
      return -1 ;
    }
  }
  return 0 ;
}

int main (int argc, char *argv[])
{
  PARSE_ARGS;
  
  std::vector< std::string > path_vec ;

  std::cout<<"DTI-Reg: ";

  if( fixedVolume.empty() || movingVolume.empty() )
    {
      std::cerr << "Error: Fixed and moving volumes must be set!" << std::endl;
      std::cerr << "To display help, please type: DTI-Reg --help"<<std::endl;
      return EXIT_FAILURE ;
     }
  if (!ANTSRegistrationType.compare("None") && !BRAINSRegistrationType.compare("None"))
    {
      std::cout<<"No registration to be performed..."<<std::endl;
      return EXIT_FAILURE;
    }

  // Write BatchMake script
  std::string BatchMakeScriptFile = "DTI-Reg.bms";
  std::ofstream file( BatchMakeScriptFile.c_str());

  file <<"# Inputs"<<std::endl;
  file <<"set (fixedVolume "<<itksys::SystemTools::ConvertToUnixOutputPath(fixedVolume.c_str())<<")"<<std::endl;
  file <<"set (movingVolume "<<itksys::SystemTools::ConvertToUnixOutputPath(movingVolume.c_str())<<")"<<std::endl;

  file <<"\n# Optional input mask volumes"<<std::endl;
  if (fixedMaskVolume.compare(""))
    file <<"set (fixedMaskVolume "<<itksys::SystemTools::ConvertToUnixOutputPath(fixedMaskVolume.c_str())<<")"<<std::endl;
  else
    file <<"set (fixedMaskVolume \'\')"<<std::endl;
  if (movingMaskVolume.compare(""))
    file <<"set (movingMaskVolume "<<itksys::SystemTools::ConvertToUnixOutputPath(movingMaskVolume.c_str())<<")"<<std::endl;
  else
    file <<"set (movingMaskVolume \'\')"<<std::endl;

  file <<"\n# Registration type"<<std::endl;
  file <<"set (Method "<<method<<")"<<std::endl;
  file <<"set (ANTSRegistrationType "<<ANTSRegistrationType<<")"<<std::endl;
  file <<"set (BRAINSRegistrationType "<<BRAINSRegistrationType<<")"<<std::endl;

  file <<"\n# Outputs"<<std::endl;
  if (outputVolume.compare(""))
    file <<"set (outputVolume "<<itksys::SystemTools::ConvertToUnixOutputPath(outputVolume.c_str())<<")"<<std::endl;
  else
    file <<"set (outputVolume \'\')"<<std::endl;  

  if (outputFixedFAVolume.compare(""))
    file <<"set (outputFixedFAVolume "<<itksys::SystemTools::ConvertToUnixOutputPath(outputFixedFAVolume.c_str())<<")"<<std::endl;
  else
    file <<"set (outputFixedFAVolume \'\')"<<std::endl;
  if (outputMovingFAVolume.compare(""))
    file <<"set (outputMovingFAVolume "<<itksys::SystemTools::ConvertToUnixOutputPath(outputMovingFAVolume.c_str())<<")"<<std::endl;
  else
    file <<"set (outputMovingFAVolume \'\')"<<std::endl;
  if (outputResampledFAVolume.compare(""))
    file <<"set (outputResampledFAVolume "<<itksys::SystemTools::ConvertToUnixOutputPath(outputResampledFAVolume.c_str())<<")"<<std::endl;
  else
    file <<"set (outputResampledFAVolume \'\')"<<std::endl;

  if (outputTransform.compare(""))
    file <<"set (outputTransform "<<itksys::SystemTools::ConvertToUnixOutputPath(outputTransform.c_str())<<")"<<std::endl;
  else
    file <<"set (outputTransform \'\')"<<std::endl;
  if (outputDeformationFieldVolume.compare(""))
    file <<"set (outputDeformationFieldVolume "<<itksys::SystemTools::ConvertToUnixOutputPath(outputDeformationFieldVolume.c_str())<<")"<<std::endl;
  else
    file <<"set (outputDeformationFieldVolume \'\')"<<std::endl;
  
   if (!method.compare("useScalar-BRAINS"))
     {
       file <<"\n# BRAINS Registration Parameters"<<std::endl;
       file <<"set (BRAINSsmoothDefFieldSigma "<<BRAINSsmoothDefFieldSigma<<")"<<std::endl;
       file <<"set (BRAINSnumberOfPyramidLevels "<<BRAINSnumberOfPyramidLevels<<")"<<std::endl;
       file <<"set (BRAINSarrayOfPyramidLevelIterations "<<BRAINSarrayOfPyramidLevelIterations<<")"<<std::endl;
       
       file <<"set (BRAINSinitializeTransformMode "<<BRAINSinitializeTransformMode<<")"<<std::endl;
       if (BRAINSinitialTransform.compare(""))
	 file <<"set (BRAINSinitialTransform "<<itksys::SystemTools::ConvertToUnixOutputPath(BRAINSinitialTransform.c_str())<<")"<<std::endl;
       else
	 file <<"set (BRAINSinitialTransform \'\')"<<std::endl;
       if (BRAINSinitialDeformationField.compare(""))
	 file <<"set (BRAINSinitialDeformationField "<<itksys::SystemTools::ConvertToUnixOutputPath(BRAINSinitialDeformationField.c_str())<<")"<<std::endl;
       else
	 file <<"set (BRAINSinitialDeformationField \'\')"<<std::endl;  
       file <<"# Histogram matching"<<std::endl;
       file <<"set (BRAINSnumberOfHistogramLevels "<<BRAINSnumberOfHistogramLevels<<")"<<std::endl;
       file <<"set (BRAINSnumberOfMatchPoints "<<BRAINSnumberOfMatchPoints<<")"<<std::endl;
       
       file <<"\n#External Tools"<<std::endl;
       std::string BRAINSFitCmd = BRAINSFitTool;
       if( SetPath(BRAINSFitCmd, "BRAINSFit" , path_vec ) )
	 return EXIT_FAILURE;
       else
	 file <<"set (BRAINSFitCmd "<<BRAINSFitCmd<<")"<<std::endl;
       
       std::string BRAINSDemonWarpCmd = BRAINSDemonWarpTool;
       if( SetPath(BRAINSDemonWarpCmd, "BRAINSDemonWarp" , path_vec ) )
	 return EXIT_FAILURE;
       else
	 file <<"set (BRAINSDemonWarpCmd "<<BRAINSDemonWarpCmd<<")"<<std::endl;  
     }

  if (!method.compare("useScalar-ANTS"))
    {
      if (ANTSOutbase.compare(""))
	file <<"set (ANTSOutbase "<<ANTSOutbase<<")"<<std::endl;
      else
	file <<"set (ANTSOutbase \'\')"<<std::endl;  
      
      file <<"\n# ANTS Registration Parameters"<<std::endl;
      file <<"set (ANTSIterations "<<ANTSIterations<<")"<<std::endl;
      file <<"set (ANTSSimilarityMetric "<<ANTSSimilarityMetric<<")"<<std::endl;
      file <<"set (ANTSSimilarityParameter "<<ANTSSimilarityParameter<<")"<<std::endl;
      file <<"set (ANTSTransformationStep "<<ANTSTransformationStep<<")"<<std::endl;
      file <<"set (ANTSGaussianSmoothingOff "<<ANTSGaussianSmoothingOff<<")"<<std::endl;
      file <<"set (ANTSGaussianSigma "<<ANTSGaussianSigma<<")"<<std::endl;
      
      file <<"\n#External Tools"<<std::endl;
      std::string ANTSCmd = ANTSTool;
      if( SetPath(ANTSCmd, "ANTS" , path_vec ) )
	return EXIT_FAILURE;
      else
	file <<"set (ANTSCmd "<<ANTSCmd<<")"<<std::endl; 
      
      std::string WarpImageMultiTransformCmd = WarpImageMultiTransformTool;
      if( SetPath(WarpImageMultiTransformCmd, "WarpImageMultiTransform" , path_vec ) )
	return EXIT_FAILURE;
      else
	file <<"set (WarpImageMultiTransformCmd "<<WarpImageMultiTransformCmd<<")"<<std::endl; 
      
      std::string WarpTensorImageMultiTransformCmd = WarpTensorImageMultiTransformTool;
      if( SetPath(WarpTensorImageMultiTransformCmd, "WarpTensorImageMultiTransform" , path_vec ) )
	return EXIT_FAILURE;
      else
	file <<"set (WarpTensorImageMultiTransformCmd "<<WarpTensorImageMultiTransformCmd<<")"<<std::endl; 
    }
  
  std::string DiffusionTensorMathematicsCmd = DiffusionTensorMathematicsTool;
  if( SetPath(DiffusionTensorMathematicsCmd, "DiffusionTensorMathematics" , path_vec ) )
    return EXIT_FAILURE;
  else
    file <<"set (DiffusionTensorMathematicsCmd "<<DiffusionTensorMathematicsCmd<<")"<<std::endl;
  
  std::string ResampleDTICmd = ResampleDTITool;
  if( SetPath(ResampleDTICmd, "ResampleDTI" , path_vec ) )
    return EXIT_FAILURE;
  else
    file <<"set (ResampleDTICmd "<<ResampleDTICmd<<")"<<std::endl;

  std::string MaskCmd = MaskTool;
  if( SetPath(MaskCmd, "Mask" , path_vec ) )
    return EXIT_FAILURE;
  else
    file <<"set (MaskCmd "<<MaskCmd<<")"<<std::endl;

  //Pipeline
    file <<"GetFilename(fixedVolumeHead ${fixedVolume} NAME_WITHOUT_EXTENSION)"<<std::endl;
    file <<"GetFilename(fixedVolumeTail ${fixedVolume} NAME)"<<std::endl;
    file <<"GetFilename(OutputDir ${fixedVolume} PATH)"<<std::endl;
    file <<"If (${OutputDir} == '')"<<std::endl;
    file <<"  set (OutputDir '.')"<<std::endl;
    file <<"EndIf(${OutputDir})"<<std::endl<<std::endl;
    file <<"GetFilename(movingVolumeHead ${movingVolume} NAME_WITHOUT_EXTENSION)"<<std::endl;
    file <<"GetFilename(movingVolumeTail ${movingVolume} NAME)"<<std::endl<<std::endl;
    file <<"# FA map creation"<<std::endl;
    file <<"echo()"<<std::endl;
    file <<"echo('FA map creation...')"<<std::endl;
    file <<"echo('fixed FA map creation...')"<<std::endl;
    file <<"If (${outputFixedFAVolume} == '')"<<std::endl;
    file <<"  set (fixedFAMap ${OutputDir}/${fixedVolumeHead}_FA.nrrd)"<<std::endl;
    file <<"Else(${outputFixedFAVolume})"<<std::endl;
    file <<"  set (fixedFAMap ${outputFixedFAVolume})"<<std::endl;
    file <<"EndIf(${outputFixedFAVolume})"<<std::endl;
    file <<"set (commandDiffusionTensorMathematics ${DiffusionTensorMathematicsCmd} ${fixedVolume} ${fixedFAMap})"<<std::endl;
    file <<"Run (outputDiffusionTensorMathematics ${commandDiffusionTensorMathematics} errorDiffusionTensorMathematics)"<<std::endl;
    file <<"If(${errorDiffusionTensorMathematics} != '')"<<std::endl;
    file <<"  echo('Error DiffusionTensorMathematics: ' ${errorDiffusionTensorMathematics})"<<std::endl;
    file <<"#  exit()"<<std::endl;
    file <<"Endif(${errorDiffusionTensorMathematics})"<<std::endl<<std::endl;
    file <<"echo('moving FA map creation...')"<<std::endl;
    file <<"If (${outputMovingFAVolume} == '')"<<std::endl;
    file <<"  set (movingFAMap ${OutputDir}/${movingVolumeHead}_FA.nrrd)"<<std::endl;
    file <<"Else(${outputMovingFAVolume})"<<std::endl;
    file <<"  set (movingFAMap ${outputMovingFAVolume})"<<std::endl;
    file <<"EndIf(${outputMovingFAVolume})"<<std::endl;
    file <<"set (commandDiffusionTensorMathematics ${DiffusionTensorMathematicsCmd} ${movingVolume} ${movingFAMap})"<<std::endl;
    file <<"Run (outputDiffusionTensorMathematics ${commandDiffusionTensorMathematics} errorDiffusionTensorMathematics)"<<std::endl;
    file <<"If(${errorDiffusionTensorMathematics} != '')"<<std::endl;
    file <<"  echo('Error DiffusionTensorMathematics: ' ${errorDiffusionTensorMathematics})"<<std::endl;
    file <<"#  exit()"<<std::endl;
    file <<"Endif(${errorDiffusionTensorMathematics})"<<std::endl<<std::endl;
    file <<"#Optional: FA Masking"<<std::endl;
    file <<"If (${fixedMaskVolume} != '')"<<std::endl;
    file <<"  set (commandMask ${MaskCmd} ${fixedFAMap} ${fixedMaskVolume} ${fixedFAMap})"<<std::endl;
    file <<"  Run (outputMask ${commandMask} errorMask)"<<std::endl;
    file <<"  If(${errorMask} != '')"<<std::endl;
    file <<"    echo('Error Mask: ' ${errorMask})"<<std::endl;
    file <<"#    exit()"<<std::endl;
    file <<"  Endif(${errorMask})"<<std::endl;
    file <<"EndIf (${fixedMaskVolume})"<<std::endl<<std::endl;
    file <<"If (${movingMaskVolume} != '')"<<std::endl;
    file <<"  set (commandMask ${MaskCmd} ${movingFAMap} ${movingMaskVolume} ${movingFAMap})"<<std::endl;
    file <<"  Run (outputMask ${commandMask} errorMask)"<<std::endl;
    file <<"  If(${errorMask} != '')"<<std::endl;
    file <<"    echo('Error Mask: ' ${errorMask})"<<std::endl;
    file <<"#    exit()"<<std::endl;
    file <<"  Endif(${errorMask})"<<std::endl;
    file <<"EndIf(${movingMaskVolume})"<<std::endl<<std::endl;

  if (!method.compare("useScalar-ANTS"))
    {
      std::cout<<"Registration via ANTS..."<<std::endl;
      //      file <<"include("<<ScriptDir<<"/DTI-Reg_Scalar_ANTS.bms)"<<std::endl;

    file <<"# Registration via ANTS"<<std::endl<<std::endl;
    file <<"echo()"<<std::endl;
    file <<"echo('Registration...')"<<std::endl;
    file <<"set(Transform '')"<<std::endl;
    file <<"set (DeformationField '')"<<std::endl<<std::endl;
    file <<"  If (${ANTSOutbase} == '')"<<std::endl;
    file <<"    set (ANTSOutbase ${OutputDir}/${movingVolumeHead}_FA_)"<<std::endl;
    file <<"  EndIf (${ANTSOutbase})"<<std::endl;
    file <<"  set(IsWarping 1)"<<std::endl;
    file <<"  If (${ANTSRegistrationType} == 'Affine' || ${ANTSRegistrationType} == 'Rigid')"<<std::endl;
    file <<"    set(IsWarping 0)"<<std::endl;
    file <<"  EndIf()"<<std::endl;
    file <<"  set (commandANTS ${ANTSCmd} 3 -m ${ANTSSimilarityMetric}[${fixedFAMap},${movingFAMap},1,${ANTSSimilarityParameter}] -o ${ANTSOutbase})"<<std::endl;
    file <<"  If (${ANTSRegistrationType} == 'Affine')"<<std::endl;
    file <<"    set (commandANTS ${commandANTS} -i 0)"<<std::endl;
    file <<"  EndIf(${ANTSRegistrationType})"<<std::endl;
    file <<"  If (${ANTSRegistrationType} == 'Rigid')"<<std::endl;
    file <<"    set (commandANTS ${commandANTS} -i 0 --rigid-affine true)"<<std::endl;
    file <<"  EndIf(${ANTSRegistrationType})"<<std::endl;
    file <<"  If (${ANTSRegistrationType} == 'GreedyDiffeo')"<<std::endl;
    file <<"    set (commandANTS ${commandANTS} -i ${ANTSIterations} -t SyN[${ANTSTransformationStep}])"<<std::endl;
    file <<"    If (${ANTSGaussianSmoothingOff} == 0)"<<std::endl;
    file <<"      set (commandANTS ${commandANTS} -r Gauss[${ANTSGaussianSigma},0])"<<std::endl;
    file <<"    EndIf(${ANTSGaussianSmoothingOff})"<<std::endl;
    file <<"  EndIf(${ANTSRegistrationType})"<<std::endl;
    file <<"  If (${ANTSRegistrationType} == 'SpatioTempDiffeo')"<<std::endl;
    file <<"    set (commandANTS ${commandANTS} -i ${ANTSIterations} -t SyN[${ANTSTransformationStep})"<<std::endl;
    file <<"    If (${ANTSGaussianSmoothingOff} == 0)"<<std::endl;
    file <<"      set (commandANTS ${commandANTS} -r Gauss[${ANTSGaussianSigma},0.5])"<<std::endl;
    file <<"    EndIf(${ANTSGaussianSmoothingOff})"<<std::endl;
    file <<"  EndIf(${ANTSRegistrationType})"<<std::endl;
    file <<"  Run (outputANTS ${commandANTS} errorANTS)"<<std::endl;
    file <<"  echo('ANTS: ' ${outputANTS})"<<std::endl;
    file <<"  If(${errorANTS} != '')"<<std::endl;
    file <<"    echo('Error ANTS: ' ${errorANTS})"<<std::endl;
    file <<"#    exit()"<<std::endl;
    file <<"  Endif(${errorANTS})"<<std::endl<<std::endl;
    file <<"# Define deformation files"<<std::endl;
    file <<"set(Transform ${ANTSOutbase}Affine.txt)"<<std::endl;
    file <<"set (DeformationField ${ANTSOutbase}Warp.nii.gz)"<<std::endl<<std::endl;
    file <<"# Applying deformation to FA map"<<std::endl;
    file <<"If (${outputResampledFAVolume} != '')"<<std::endl;
    file <<"  echo()"<<std::endl;
    file <<"  echo('FA resampling...')"<<std::endl;
    file <<"  set (commandWarpImageMultiTransform ${WarpImageMultiTransformCmd} 3 ${movingFAMap} ${outputResampledFAVolume} -R ${outputFixedFAVolume})"<<std::endl;
    file <<"  If(${IsWarping} == 1)"<<std::endl;
    file <<"    set (commandWarpImageMultiTransform ${commandWarpImageMultiTransform} ${DeformationField} ${Transform})"<<std::endl;
    file <<"  Else(${IsWarping})"<<std::endl;
    file <<"    set (commandWarpImageMultiTransform ${commandWarpImageMultiTransform} ${Transform})"<<std::endl;
    file <<"  EndIf(${IsWarping})"<<std::endl;
    file <<"  Run (outputWarpImageMultiTransform ${commandWarpImageMultiTransform} errorWarpImageMultiTransform)"<<std::endl;
    file <<"  If(${errorWarpImageMultiTransform} != '')"<<std::endl;
    file <<"    echo('Error WarpImageMultiTransform: ' ${errorWarpImageMultiTransform})"<<std::endl;
    file <<"#    exit()"<<std::endl;
    file <<"  Endif(${errorWarpImageMultiTransform})"<<std::endl;
    file <<"EndIf(${outputResampledFAVolume})"<<std::endl<<std::endl;
    file <<"# DTI resampling"<<std::endl;
    file <<"echo()"<<std::endl;
    file <<"echo('DTI resampling...')"<<std::endl;
    file <<"If (${outputVolume} != '')"<<std::endl;
    file <<"  set (ResampledDTI ${outputVolume})"<<std::endl;
    file <<"Else(${outputVolume})"<<std::endl;
    file <<"    set (ResampledDTI ${OutputDir}/${movingVolumeHead}_warp.nrrd)"<<std::endl;
    file <<"EndIf(${outputVolume})"<<std::endl;
    file <<"set (commandWarpTensorImageMultiTransform ${WarpTensorImageMultiTransformCmd} 3 ${movingVolume} ${ResampledDTI} -R ${fixedVolume})"<<std::endl;
    file <<"If(${IsWarping} == 1)"<<std::endl;
    file <<"  set (commandWarpTensorImageMultiTransform ${commandWarpTensorImageMultiTransform} ${DeformationField} ${Transform})"<<std::endl;
    file <<"Else(${IsWarping})"<<std::endl;
    file <<"  set (commandWarpTensorImageMultiTransform ${commandWarpTensorImageMultiTransform} ${Transform})"<<std::endl;
    file <<"EndIf(${IsWarping})"<<std::endl;
    file <<"Run (outputWarpTensorImageMultiTransform ${commandWarpTensorImageMultiTransform} errorWarpTensorImageMultiTransform)"<<std::endl;
    file <<"If(${errorWarpTensorImageMultiTransform} != '')"<<std::endl;
    file <<"  echo('Error WarpTensorImageMultiTransform: ' ${errorWarpTensorImageMultiTransform})"<<std::endl;
    file <<"#  exit()"<<std::endl;
    file <<"Endif(${errorWarpTensorImageMultiTransform})"<<std::endl<<std::endl;
    file <<"If (${outputTransform} != '')"<<std::endl;
    file <<"  echo()"<<std::endl;
    file <<"  echo('Copying Transform file...')"<<std::endl;
    file <<"  CopyFile(${Transform} ${outputTransform})"<<std::endl;
    file <<"  DeleteFile(${Transform})"<<std::endl;
    file <<"EndIf (${outputTransform})"<<std::endl;
    file <<"If (${outputDeformationFieldVolume} != '')"<<std::endl;
    file <<"  echo()"<<std::endl;
    file <<"  echo('Copying deformation field...')"<<std::endl;
    file <<"  CopyFile(${DeformationField} ${outputDeformationFieldVolume})"<<std::endl;
    file <<"  DeleteFile(${DeformationField})"<<std::endl;
    file <<"EndIf(${outputDeformationFieldVolume})"<<std::endl;
    file <<"#Delete temporary files"<<std::endl;
    file <<"If (${outputFixedFAVolume} == '')"<<std::endl;
    file <<"  DeleteFile(${fixedFAMap})"<<std::endl;
    file <<"EndIf(${outputFixedFAVolume})"<<std::endl;
    file <<"If (${outputMovingFAVolume} == '')"<<std::endl;
    file <<"  DeleteFile(${movingFAMap})"<<std::endl;
    file <<"EndIf(${outputMovingFAVolume})"<<std::endl;

    }
  else if (!method.compare("useScalar-BRAINS"))
    {
      std::cout<<"Registration via BRAINS..."<<std::endl;
      //      file <<"include("<<ScriptDir<<"/DTI-Reg_Scalar_BRAINS.bms)"<<std::endl;

    file <<"#Registration"<<std::endl;
    file <<"If (${BRAINSRegistrationType} == 'Rigid' || ${BRAINSRegistrationType} == 'Affine' || ${BRAINSRegistrationType} == 'BSpline')"<<std::endl;
    file <<"  set(IsDemonsWarping 0)"<<std::endl;
    file <<"Else (${BRAINSRegistrationType})"<<std::endl;
    file <<"  set(IsDemonsWarping 1)"<<std::endl;
    file <<"EndIf(${BRAINSRegistrationType})"<<std::endl<<std::endl;
    file <<"# Registration via BRAINSFit "<<std::endl;
    file <<"set (Transform '')"<<std::endl;
    file <<"If (${BRAINSRegistrationType} == 'Rigid')"<<std::endl;
    file <<"  set (TransformType 'Rigid')"<<std::endl;
    file <<"  set (RegSuffix 'RReg')"<<std::endl;
    file <<"Else(${BRAINSRegistrationType})"<<std::endl;
    file <<"  If (${BRAINSRegistrationType} == 'BSpline')"<<std::endl;
    file <<"    set (TransformType 'Rigid,Affine,BSpline')"<<std::endl;
    file <<"    set (RegSuffix 'BSpline')"<<std::endl;
    file <<"  Else()"<<std::endl;
    file <<"     set (TransformType 'Rigid,Affine')"<<std::endl;
    file <<"     set (RegSuffix 'AffReg')"<<std::endl;
    file <<"  EndIf()"<<std::endl;
    file <<"EndIf(${BRAINSRegistrationType})"<<std::endl;
    file <<"echo()"<<std::endl;
    file <<"echo(${TransformType} ' registration...')"<<std::endl;
    file <<"If (${outputResampledFAVolume} != '')"<<std::endl;
    file <<"  set (ResampledFAMap ${outputResampledFAVolume})"<<std::endl;
    file <<"Else(${outputResampledFAVolume})"<<std::endl;
    file <<"  set (ResampledFAMap ${OutputDir}/${movingVolumeHead}_FA_${RegSuffix}.nrrd)"<<std::endl;
    file <<"Endif(${outputResampledFAVolume})"<<std::endl;
    file <<"If (${outputTransform} != '')"<<std::endl;
    file <<"  set (Transform ${outputTransform})"<<std::endl;
    file <<"Else(${outputTransform})"<<std::endl;
    file <<"  set (Transform ${OutputDir}/${movingVolumeHead}_FA_${RegSuffix}.txt)"<<std::endl;
    file <<"Endif(${outputTransform})"<<std::endl;
    file <<"If (${BRAINSinitialTransform} != '')"<<std::endl;
    file <<"  set (commandBRAINSFit ${BRAINSFitCmd} --fixedVolume ${fixedFAMap} --movingVolume ${movingFAMap} --initialTransform ${BRAINSinitialTransform} --outputTransform ${Transform} --outputVolume ${ResampledFAMap} --outputVolumePixelType float --transformType ${TransformType} --interpolationMode Linear)"<<std::endl;
    file <<"Else(${BRAINSinitialTransform})"<<std::endl;
    file <<"  set (commandBRAINSFit ${BRAINSFitCmd} --fixedVolume ${fixedFAMap} --movingVolume ${movingFAMap} --initializeTransformMode ${BRAINSinitializeTransformMode} --outputTransform ${Transform} --outputVolume ${ResampledFAMap} --outputVolumePixelType float --transformType ${TransformType} --interpolationMode Linear)"<<std::endl;
    file <<"Endif(${BRAINSinitialTransform})"<<std::endl;
    file <<"Run (outputBRAINSFit ${commandBRAINSFit} errorBRAINSFit)"<<std::endl;
    file <<"If(${errorBRAINSFit} != '')"<<std::endl;
    file <<"  echo('Error BRAINSFit: ' ${errorBRAINSFit})"<<std::endl;
    file <<"#  exit()"<<std::endl;
    file <<"Endif(${errorBRAINSFit})"<<std::endl;
    file <<"If (${IsDemonsWarping} == 1)"<<std::endl;
    file <<"  DeleteFile(${ResampledFAMap})"<<std::endl;
    file <<"EndIf(${IsDemonsWarping})"<<std::endl<<std::endl;
    file <<"# Warping"<<std::endl;
    file <<"If (${IsDemonsWarping} == 1)"<<std::endl;
    file <<"  echo()"<<std::endl;
    file <<"  echo('Warping...')"<<std::endl;
    file <<"  If (${outputResampledFAVolume} != '')"<<std::endl;
    file <<"    set (ResampledFAMap ${outputResampledFAVolume})"<<std::endl;
    file <<"  Else(${outputResampledFAVolume})"<<std::endl;
    file <<"    set (ResampledFAMap ${OutputDir}/${movingVolumeHead}_FA_warp.nrrd)"<<std::endl;
    file <<"  EndIf(${outputResampledFAVolume})"<<std::endl;
    file <<"  If (${outputDeformationFieldVolume} != '')"<<std::endl;
    file <<"    set (DeformationField ${outputDeformationFieldVolume})"<<std::endl;
    file <<"  Else(${outputDeformationFieldVolume})"<<std::endl;
    file <<"    set (DeformationField ${OutputDir}/${movingVolumeHead}_FA_warpfield.nrrd)"<<std::endl;
    file <<"  EndIf(${outputDeformationFieldVolume})"<<std::endl;
    file <<"  set (commandBRAINSDemonWarp ${BRAINSDemonWarpCmd} --fixedVolume ${fixedFAMap} --movingVolume ${movingFAMap} --outputVolume ${ResampledFAMap} --outputDisplacementFieldVolume ${DeformationField} --outputPixelType float --interpolationMode Linear --registrationFilterType ${BRAINSRegistrationType} --histogramMatch --numberOfHistogramBins ${BRAINSnumberOfHistogramLevels} --numberOfMatchPoints ${BRAINSnumberOfMatchPoints} --initializeWithTransform ${Transform} --smoothDisplacementFieldSigma ${BRAINSsmoothDefFieldSigma} --numberOfPyramidLevels ${BRAINSnumberOfPyramidLevels} --arrayOfPyramidLevelIterations ${BRAINSarrayOfPyramidLevelIterations})"<<std::endl;
    file <<"  "<<std::endl;
    file <<"  If (${BRAINSinitialDeformationField} != '')"<<std::endl;
    file <<"    set (commandBRAINSDemonWarp ${commandBRAINSDemonWarp} --initializeWithDisplacementField ${BRAINSinitialDeformationField})"<<std::endl;
    file <<"  EndIf(${BRAINSinitialDeformationField})"<<std::endl;
    file <<"  Run (outputBRAINSDemonWarp ${commandBRAINSDemonWarp} errorBRAINSDemonWarp)"<<std::endl;
    file <<"  If(${errorBRAINSDemonWarp} != '')"<<std::endl;
    file <<"    echo('Error BRAINSDemonWarp: ' ${errorBRAINSDemonWarp})"<<std::endl;
    file <<"#    exit()"<<std::endl;
    file <<"  Endif(${errorBRAINSDemonWarp})"<<std::endl;
    file <<"Endif(${IsDemonsWarping})"<<std::endl<<std::endl;
    file <<"# DTI resampling"<<std::endl;
    file <<"echo()"<<std::endl;
    file <<"echo('DTI resampling...')"<<std::endl;
    file <<"If (${outputVolume} != '')"<<std::endl;
    file <<"  set (ResampledDTI ${outputVolume})"<<std::endl;
    file <<"Else(${outputVolume})"<<std::endl;
    file <<"  set (ResampledDTI ${OutputDir}/${movingVolumeHead}_warp.nrrd)"<<std::endl;
    file <<"EndIf(${outputVolume})"<<std::endl;
    file <<"set (commandResampleDTI ${ResampleDTICmd} ${movingVolume} ${ResampledDTI} --Reference ${fixedVolume})"<<std::endl;
    file <<"If (${IsDemonsWarping} == 1)"<<std::endl;
    file <<"  set (commandResampleDTI ${commandResampleDTI} --defField ${DeformationField} --hfieldtype displacement)"<<std::endl;
    file <<"Else(${IsDemonsWarping})"<<std::endl;
    file <<"  set (commandResampleDTI ${commandResampleDTI} --transformationFile ${Transform})"<<std::endl;
    file <<"EndIf(${IsDemonsWarping})"<<std::endl;
    file <<"Run (outputResampleDTI ${commandResampleDTI} errorResampleDTI)"<<std::endl;
    file <<"If(${errorResampleDTI} != '')"<<std::endl;
    file <<"  echo('Error ResampleDTI: ' ${errorResampleDTI})"<<std::endl;
    file <<"#  exit()"<<std::endl;
    file <<"Endif(${errorResampleDTI})"<<std::endl<<std::endl;
    file <<"#Delete temporary files"<<std::endl;
    file <<"#If (${outputVolume} == '')"<<std::endl;
    file <<"#  DeleteFile(${ResampledDTI})"<<std::endl;
    file <<"#EndIf(${outputVolume})"<<std::endl;
    file <<"#If (${outputTransform} == '')"<<std::endl;
    file <<"#  DeleteFile(${Transform})"<<std::endl;
    file <<"#EndIf(${outputTransform})"<<std::endl;
    file <<"#If (${outputDeformationFieldVolume} == '')"<<std::endl;
    file <<"#  DeleteFile(${DeformationField})"<<std::endl;
    file <<"#EndIf(${outputDeformationFieldVolume})"<<std::endl;
    file <<"If (${outputFixedFAVolume} == '')"<<std::endl;
    file <<"  DeleteFile(${fixedFAMap})"<<std::endl;
    file <<"EndIf(${outputFixedFAVolume})"<<std::endl;
    file <<"If (${outputMovingFAVolume} == '')"<<std::endl;
    file <<"  DeleteFile(${movingFAMap})"<<std::endl;
    file <<"EndIf(${outputMovingFAVolume})"<<std::endl;
    file <<"If (${outputResampledFAVolume} == '')"<<std::endl;
    file <<"  DeleteFile(${ResampledFAMap})"<<std::endl;
    file <<"EndIf(${outputResampledFAVolume})"<<std::endl;

    }

  file.close();


  // Execute BatchMake
  bm::ScriptParser m_Parser;
  m_Parser.Execute(BatchMakeScriptFile);

  std::cout<<"DTI-Reg: Done!"<<std::endl;
  return EXIT_SUCCESS;
}
