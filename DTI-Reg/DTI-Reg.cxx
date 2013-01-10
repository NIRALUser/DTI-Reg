#include <iostream>
#include <fstream>

#include <itksys/SystemTools.hxx>
#include "DTI-Reg_Config.h"
#include <bmScriptParser.h>

#include "DTI-RegCLP.h"


int SetPath( std::string &pathString , const char* name , std::vector< std::string >  path_vec )
{
  pathString= itksys::SystemTools::FindProgram( name , path_vec ) ; // will look in the PATH AND in the extra path "path_vec"
  if( !pathString.compare( "" ) )
  {
    std::cerr << name << " is missing or its PATH is not set" << std::endl ;
    return -1 ;
  }
  return 0 ;
}

int main (int argc, char *argv[])
{
  PARSE_ARGS;
  
  std::vector< std::string > path_vec ;
  path_vec.push_back( "." ) ;
  path_vec.push_back( ScriptDir ) ;
  std::string currentDir = itksys::SystemTools::GetCurrentWorkingDirectory( false ) ;
  currentDir += argv[ 0 ] ;
  itksys::SystemTools::ConvertToUnixSlashes( currentDir ) ;
  size_t pos = currentDir.rfind('/');
  if( pos > 0 && pos < currentDir.size() )
  {
    currentDir.resize( pos ) ;
    path_vec.push_back( currentDir ) ;
  }

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
  file <<"set (fixedVolume "<<fixedVolume<<")"<<std::endl;
  file <<"set (movingVolume "<<movingVolume<<")"<<std::endl;

  file <<"\n# Optional input mask volumes"<<std::endl;
  if (fixedMaskVolume.compare(""))
    file <<"set (fixedMaskVolume "<<fixedMaskVolume<<")"<<std::endl;
  else
    file <<"set (fixedMaskVolume \'\')"<<std::endl;
  if (movingMaskVolume.compare(""))
    file <<"set (movingMaskVolume "<<movingMaskVolume<<")"<<std::endl;
  else
    file <<"set (movingMaskVolume \'\')"<<std::endl;

  file <<"\n# Registration type"<<std::endl;
  file <<"set (Method "<<method<<")"<<std::endl;
  file <<"set (ANTSRegistrationType "<<ANTSRegistrationType<<")"<<std::endl;
  file <<"set (BRAINSRegistrationType "<<BRAINSRegistrationType<<")"<<std::endl;

  file <<"\n# Outputs"<<std::endl;
  if (outputVolume.compare(""))
    file <<"set (outputVolume "<<outputVolume<<")"<<std::endl;
  else
    file <<"set (outputVolume \'\')"<<std::endl;  

  if (outputFixedFAVolume.compare(""))
    file <<"set (outputFixedFAVolume "<<outputFixedFAVolume<<")"<<std::endl;
  else
    file <<"set (outputFixedFAVolume \'\')"<<std::endl;
  if (outputMovingFAVolume.compare(""))
    file <<"set (outputMovingFAVolume "<<outputMovingFAVolume<<")"<<std::endl;
  else
    file <<"set (outputMovingFAVolume \'\')"<<std::endl;
  if (outputResampledFAVolume.compare(""))
    file <<"set (outputResampledFAVolume "<<outputResampledFAVolume<<")"<<std::endl;
  else
    file <<"set (outputResampledFAVolume \'\')"<<std::endl;

  if (outputTransform.compare(""))
    file <<"set (outputTransform "<<outputTransform<<")"<<std::endl;
  else
    file <<"set (outputTransform \'\')"<<std::endl;
  if (outputDeformationFieldVolume.compare(""))
    file <<"set (outputDeformationFieldVolume "<<outputDeformationFieldVolume<<")"<<std::endl;
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
	 file <<"set (BRAINSinitialTransform "<<BRAINSinitialTransform<<")"<<std::endl;
       else
	 file <<"set (BRAINSinitialTransform \'\')"<<std::endl;
       if (BRAINSinitialDeformationField.compare(""))
	 file <<"set (BRAINSinitialDeformationField "<<BRAINSinitialDeformationField<<")"<<std::endl;
       else
	 file <<"set (BRAINSinitialDeformationField \'\')"<<std::endl;  
       file <<"# Histogram matching"<<std::endl;
       file <<"set (BRAINSnumberOfHistogramLevels "<<BRAINSnumberOfHistogramLevels<<")"<<std::endl;
       file <<"set (BRAINSnumberOfMatchPoints "<<BRAINSnumberOfMatchPoints<<")"<<std::endl;
       
       file <<"\n#External Tools"<<std::endl;
       std::string BRAINSFitCmd ;
       if( SetPath(BRAINSFitCmd, "BRAINSFit" , path_vec ) )
	 return EXIT_FAILURE;
       else
	 file <<"set (BRAINSFitCmd "<<BRAINSFitCmd<<")"<<std::endl;
       
       std::string BRAINSDemonWarpCmd ;
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
      std::string ANTSCmd ;
      if( SetPath(ANTSCmd, "ANTS" , path_vec ) )
	return EXIT_FAILURE;
      else
	file <<"set (ANTSCmd "<<ANTSCmd<<")"<<std::endl; 
      
      std::string WarpImageMultiTransformCmd ;
      if( SetPath(WarpImageMultiTransformCmd, "WarpImageMultiTransform" , path_vec ) )
	return EXIT_FAILURE;
      else
	file <<"set (WarpImageMultiTransformCmd "<<WarpImageMultiTransformCmd<<")"<<std::endl; 
      
      std::string WarpTensorImageMultiTransformCmd ;
      if( SetPath(WarpTensorImageMultiTransformCmd, "WarpTensorImageMultiTransform" , path_vec ) )
	return EXIT_FAILURE;
      else
	file <<"set (WarpTensorImageMultiTransformCmd "<<WarpTensorImageMultiTransformCmd<<")"<<std::endl; 
    }
  
  std::string dtiprocessCmd ;
  if( SetPath(dtiprocessCmd, "dtiprocess" , path_vec ) )
    return EXIT_FAILURE;
  else
    file <<"set (dtiprocessCmd "<<dtiprocessCmd<<")"<<std::endl;
  
  std::string ResampleDTICmd ;
  if( SetPath(ResampleDTICmd, "ResampleDTI" , path_vec ) )
    return EXIT_FAILURE;
  else
    file <<"set (ResampleDTICmd "<<ResampleDTICmd<<")"<<std::endl;

  //find scripts paths
  std::string DTI-Reg_Scalar_ANTSCmd ;
  if( SetPath(DTI-Reg_Scalar_ANTSCmd, "DTI-Reg_Scalar_ANTS.bms" , path_vec ) )
  {
    return EXIT_FAILURE ;
  }
  std::string DTI-Reg_Scalar_BRAINSCmd ;
  if( SetPath(DTI-Reg_Scalar_BRAINSCmd, "DTI-Reg_Scalar_BRAINS.bms" , path_vec ) )
  {
    return EXIT_FAILURE ;
  }
  // Include main BatchMake script
  file <<"\n#Include main batchMake script"<<std::endl;
  if (!method.compare("useScalar-ANTS"))
    {
      std::cout<<"Registration via ANTS..."<<std::endl;
      file <<"include("<<DTI-Reg_Scalar_ANTSCmd<<")"<<std::endl;
    }
  else if (!method.compare("useScalar-BRAINS"))
    {
      std::cout<<"Registration via BRAINS..."<<std::endl;
      file <<"include("<<DTI-Reg_Scalar_BRAINSCmd<<")"<<std::endl;
    }

  file.close();


  // Execute BatchMake
  bm::ScriptParser m_Parser;
  m_Parser.Execute(BatchMakeScriptFile);

  std::cout<<"DTI-Reg: Done!"<<std::endl;
  return EXIT_SUCCESS;
}
