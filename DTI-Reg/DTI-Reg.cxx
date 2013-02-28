#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <itksys/SystemTools.hxx>

#include <bmScriptParser.h>

#include "DTI-RegCLP.h"
#include "DTI-Reg_Config.h"
#include "DTI-Reg-bms.h"


int SetPath( std::string &pathString , const char* name , std::vector< std::string >  ProgramsPathsVector )
{
  if(!pathString.empty()) // has been set to the value in the cmake cache -< check that it exists
  {
    mode_t ITKmode_X_OK = 1;
    if(! itksys::SystemTools::GetPermissions(pathString.c_str(), ITKmode_X_OK)) pathString = ""; // if not executable, empty it so it can be found by FindProgram()
  }

  if( pathString.empty() || !pathString.substr(pathString.size() - 9 , 9 ).compare( "-NOTFOUND" ) )
  {
    pathString= itksys::SystemTools::FindProgram( name , ProgramsPathsVector ) ; // will look in the PATH first AND then in the extra path "ProgramsPathsVector"
    if( !pathString.compare( "" ) )
    {
      std::cerr << name << " is missing or its PATH is not set" << std::endl ;
      return 1 ;
    }
  }

  return 0 ;
}

void WriteReplaceTransformNamePythonScript (std::string PythonScriptPath)
{
  std::string PythonScriptText="#!/usr/bin/python\n\
\n\
import os\n\
import sys # to get the cmd line arguments\n\
\n\
NewTransformFile = sys.argv[1] + \"/TempName.txt\"\n\
OldTransformFile = sys.argv[1] + \"/\" + sys.argv[2]\n\
\n\
if os.path.isfile(NewTransformFile) : os.remove(NewTransformFile) # If for any reason it was already here\n\
NewFile = open(NewTransformFile,\"a\") #open for append\n\
\n\
# Search all lines for the text\n\
for line in open(OldTransformFile):\n\
  line = line.replace(\"MatrixOffsetTransformBase\",\"AffineTransform\")\n\
  NewFile.write(line)\n\
\n\
NewFile.close()\n\
\n\
# Replace the old wrong file by the new corrected file by removing and renaming\n\
os.remove(OldTransformFile) # Remove \"CaseX_FA_Affine.txt\"\n\
os.rename(NewTransformFile,OldTransformFile) # Rename \"TempName.txt\" to \"CaseX_FA_Affine.txt\"";

  // Create File
  std::ofstream PythonScriptfile( PythonScriptPath.c_str());
  PythonScriptfile << PythonScriptText <<std::endl;
  PythonScriptfile.close();

//  Script executed with python executable so no need to make the script executable
//  mode_t ITKmode_X_OK = 1;
//  itksys::SystemTools::SetPermissions(PythonScriptPath.c_str(), ITKmode_X_OK);
}

int main (int argc, char *argv[])
{
  PARSE_ARGS;

//  std::vector< std::string > path_vec ; // initialized by the cmd line vector ProgramsPathsVector

  // Added by Adrien Kaiser : the tools are either in the same directory than the DTI-Reg executable ran or in the PATH
  // Get the directory where the DTI-Reg executable is
  std::string RanCommandDirectory = itksys::SystemTools::GetRealPath( itksys::SystemTools::GetFilenamePath(argv[0]).c_str() );
  if(RanCommandDirectory=="") RanCommandDirectory="."; // If called by itself = either in the PATH or in the current directory : will be found either way by find_program
  // Add it in the ProgramsPathsVector
  ProgramsPathsVector.push_back(RanCommandDirectory);

  // If DTI-Reg is a Slicer Extension in the DTIAtlasBuilder package, give the path to the folder containing external non cli tools
  // If no SicerExtension, find_program will just search there and find nothing -> not an issue
  std::string LinuxWindowsExternalBinDir = RanCommandDirectory + "/../../../ExternalBin"; // On linux or windows, the executable will be in Ext/lib/Slicer4.2/cli_modules and the tools will be in Ext/ExternalBin
  std::string MacExternalBinDir = RanCommandDirectory + "/../ExternalBin"; // On mac, the executable will be in Ext/cli_modules and the tools will be in Ext/ExternalBin
  ProgramsPathsVector.push_back(LinuxWindowsExternalBinDir);
  ProgramsPathsVector.push_back(MacExternalBinDir);

// for(int i=0;i<ProgramsPathsVector.size();i++) std::cout<<ProgramsPathsVector[i]<<std::endl;

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
  if (initialAffine.compare(""))
    file <<"set (initialAffine "<<initialAffine<<")"<<std::endl;
  else
    file <<"set (initialAffine \'\')"<<std::endl;

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
       file <<"set (BRAINSnumberOfPyramidLevels "<<BRAINSnumberOfPyramidLevels<<")"<<std::endl;
       file <<"set (BRAINSarrayOfPyramidLevelIterations "<<BRAINSarrayOfPyramidLevelIterations<<")"<<std::endl;
       
       file <<"set (BRAINSinitializeTransformMode "<<BRAINSinitializeTransformMode<<")"<<std::endl;
        if (BRAINSinitialDeformationField.compare(""))
	 file <<"set (BRAINSinitialDeformationField "<<BRAINSinitialDeformationField<<")"<<std::endl;
       else
	 file <<"set (BRAINSinitialDeformationField \'\')"<<std::endl;  
       file <<"# Histogram matching"<<std::endl;
       file <<"set (BRAINSnumberOfHistogramLevels "<<BRAINSnumberOfHistogramLevels<<")"<<std::endl;
       file <<"set (BRAINSnumberOfMatchPoints "<<BRAINSnumberOfMatchPoints<<")"<<std::endl;
       
       file <<"\n#External Tools"<<std::endl;
       std::string BRAINSFitCmd = BRAINSFitTool;
       if( SetPath(BRAINSFitCmd, "BRAINSFit" , ProgramsPathsVector ) )
	 return EXIT_FAILURE;
       else
	 file <<"set (BRAINSFitCmd "<<BRAINSFitCmd<<")"<<std::endl;
       
       std::string BRAINSDemonWarpCmd = BRAINSDemonWarpTool;
       if( SetPath(BRAINSDemonWarpCmd, "BRAINSDemonWarp" , ProgramsPathsVector ) )
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
      if( SetPath(ANTSCmd, "ANTS" , ProgramsPathsVector ) )
	return EXIT_FAILURE;
      else
	file <<"set (ANTSCmd "<<ANTSCmd<<")"<<std::endl; 
      
      std::string WarpImageMultiTransformCmd = WarpImageMultiTransformTool;
      if( SetPath(WarpImageMultiTransformCmd, "WarpImageMultiTransform" , ProgramsPathsVector ) )
	return EXIT_FAILURE;
      else
	file <<"set (WarpImageMultiTransformCmd "<<WarpImageMultiTransformCmd<<")"<<std::endl; 
      
      std::string WarpTensorImageMultiTransformCmd = WarpTensorImageMultiTransformTool;
      if( SetPath(WarpTensorImageMultiTransformCmd, "WarpTensorImageMultiTransform" , ProgramsPathsVector ) )
	return EXIT_FAILURE;
      else
	file <<"set (WarpTensorImageMultiTransformCmd "<<WarpTensorImageMultiTransformCmd<<")"<<std::endl; 

      // Create Python Script and set path
      std::string PythonExecutable;
      if( SetPath(PythonExecutable, "python" , ProgramsPathsVector ) )
      {
        std::cout<<"Python not found. Abort."<<std::endl;
        return EXIT_FAILURE;
      }
      else
        file <<"set (PythonExecutable "<<PythonExecutable<<")"<<std::endl;

      std::string PythonScriptPath = "ReplaceTransformName.py";
      WriteReplaceTransformNamePythonScript(PythonScriptPath); // will be deleted in the bms file
      file <<"set (PythonScriptPath "<<PythonScriptPath<<")"<<std::endl; 

    } // if (!method.compare("useScalar-ANTS"))
  
  std::string dtiprocessCmd = dtiprocessTool;
  if( SetPath(dtiprocessCmd, "dtiprocess" , ProgramsPathsVector ) )
    return EXIT_FAILURE;
  else
    file <<"set (dtiprocessCmd "<<dtiprocessCmd<<")"<<std::endl;
  
  std::string ResampleDTICmd = ResampleDTITool;
  if( SetPath(ResampleDTICmd, "ResampleDTIlogEuclidean" , ProgramsPathsVector ) )
    return EXIT_FAILURE;
  else
    file <<"set (ResampleDTICmd "<<ResampleDTICmd<<")"<<std::endl;

  // Include main BatchMake script
  file <<"\n#Include main batchMake script"<<std::endl;
  if (!method.compare("useScalar-ANTS"))
    {
      std::cout<<"Registration via ANTS..."<<std::endl;
//      file <<"include("<<ScriptDir<<"/DTI-Reg_Scalar_ANTS.bms)"<<std::endl;
      file <<DTIReg_Scalar_ANTS<<std::endl; // Added by Adrien Kaiser : String variable defined in DTI-Reg-bms.h that contains the bms script for ANTS
    }
  else if (!method.compare("useScalar-BRAINS"))
    {
      std::cout<<"Registration via BRAINS..."<<std::endl;
//      file <<"include("<<ScriptDir<<"/DTI-Reg_Scalar_BRAINS.bms)"<<std::endl;
      file <<DTIReg_Scalar_BRAINS<<std::endl; // Added by Adrien Kaiser : String variable defined in DTI-Reg-bms.h that contains the bms script for BRAINS
    }

  file.close();

  // Execute BatchMake
  bm::ScriptParser m_Parser;
  m_Parser.Execute(BatchMakeScriptFile);

  std::cout<<"DTI-Reg: Done!"<<std::endl;
  return EXIT_SUCCESS;
}
