#include <iostream>
#include <fstream>

#include <itksys/SystemTools.hxx>

#include <bmScriptParser.h>

#include "DTI-RegCLP.h"
#include "DTI-Reg_Config.h"


int SetPath( std::string &pathString , const char* name , std::vector< std::string >  path_vec )
{
  if( pathString.empty() || !pathString.substr(pathString.size() - 10 , 9 ).compare( "-NOTFOUND" ) )
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

  std::cout<<"DTI-Reg..."<<std::endl;

  if( fixedVolume.empty() || movingVolume.empty() )
    {
      std::cerr << "Error: Fixed and moving volumes must be set!" << std::endl ;
      std::cerr << "To display help, please type: DTI-Reg --help"<<std::endl;
      return EXIT_FAILURE ;
    }  
  if (!method.compare("useTensor"))
    {
      std::cout<<"Computational method not implemented yet..."<<std::endl;
      return EXIT_FAILURE;
    }
  if (!linearRegType.compare("None") && !warpingType.compare("None"))
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
  file <<"set (linearRegType "<<linearRegType<<")"<<std::endl;
  if (!linearRegType.compare("None"))
    file <<"set (IsLinearRegistration 0)"<<std::endl;
  else
    file <<"set (IsLinearRegistration 1)"<<std::endl;

  file <<"set (warpingType "<<warpingType<<")"<<std::endl;
  if (!warpingType.compare("None") || !warpingType.compare("BSpline"))
    file <<"set (IsDemonsWarping 0)"<<std::endl;
  else
    file <<"set (IsDemonsWarping 1)"<<std::endl;
  
  file <<"\n# Registration initialization"<<std::endl;
    file <<"set (initializeTransformMode "<<initializeTransformMode<<")"<<std::endl;
  if (initialTransform.compare(""))
    file <<"set (initialTransform "<<initialTransform<<")"<<std::endl;
  else
    file <<"set (initialTransform \'\')"<<std::endl;
  if (initialDeformationField.compare(""))
    file <<"set (initialDeformationField "<<initialDeformationField<<")"<<std::endl;
  else
    file <<"set (initialDeformationField \'\')"<<std::endl;

  file <<"\n# Outputs"<<std::endl;
  if (outputVolume.compare(""))
  file <<"set (outputVolume "<<outputVolume<<")"<<std::endl;
  else
    file <<"set (outputVolume \'\')"<<std::endl;
  
  if (outputTransform.compare(""))
    file <<"set (outputTransform "<<outputTransform<<")"<<std::endl;
  else
    file <<"set (outputTransform \'\')"<<std::endl;
  if (outputDeformationFieldVolume.compare(""))
    file <<"set (outputDeformationFieldVolume "<<outputDeformationFieldVolume<<")"<<std::endl;
  else
    file <<"set (outputDeformationFieldVolume \'\')"<<std::endl;
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
  
  file <<"\n# Advanced Histogram matching parameters"<<std::endl;
  file <<"set (numberOfHistogramLevels "<<numberOfHistogramLevels<<")"<<std::endl;
  file <<"set (numberOfMatchPoints "<<numberOfMatchPoints<<")"<<std::endl;

  file <<"\n#External Tools"<<std::endl;
  std::string dtiprocessCmd = dtiprocessTool;
  if( SetPath(dtiprocessCmd, "dtiprocess" , path_vec ) )
    return EXIT_FAILURE;
  else
    file <<"set (dtiprocessCmd "<<dtiprocessCmd<<")"<<std::endl;
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
  std::string ResampleDTICmd = ResampleDTITool;
  if( SetPath(ResampleDTICmd, "ResampleDTI" , path_vec ) )
    return EXIT_FAILURE;
  else
    file <<"set (ResampleDTICmd "<<ResampleDTICmd<<")"<<std::endl;

  // Include main BatchMake script
  file <<"\n#Include main batchMake script"<<std::endl;
  file <<"include("<<ScriptDir<<"/DTI-Reg_Scalar.bms)"<<std::endl;

  file.close();


  // Execute BatchMake
  bm::ScriptParser m_Parser;
  m_Parser.Execute(BatchMakeScriptFile);

  std::cout<<"DTI-Reg: Done!"<<std::endl;
  return EXIT_SUCCESS;
}
