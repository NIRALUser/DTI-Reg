#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <itksys/SystemTools.hxx>

#include <bmScriptParser.h>
#include <algorithm>
#include <math.h>
#include "DTI-RegCLP.h"
#include "DTI-Reg_Config.h"
#include "DTI-Reg-bms.h"

#ifdef Slicer_Extension
  #include <vtkSlicerConfigure.h> //configured in the inner-build directory of Slicer
#endif

int SetPath( std::string &pathString , const char* name , std::vector< std::string >  ProgramsPathsVector , std::string givenPath )
{
  if( !givenPath.empty() )
  {
    if( itksys::SystemTools::FileExists( givenPath , true ) )
    {
      pathString = givenPath ;
      return 0 ;
    }
    else
    {
      std::cerr << givenPath << " is not a file" << std::endl ;
      return 1 ;
    }
  }
  std::string cmakePathString = pathString ;
  if( !ProgramsPathsVector.empty() )
  {
    pathString= itksys::SystemTools::FindProgram( name , ProgramsPathsVector, true ) ; // Does not look in PATH, only in "ProgramsPathsVector"
    if( !pathString.empty() )
    {
      return 0 ;
    }
  }
  pathString = cmakePathString ;
  if( pathString.size() >= 9 && !pathString.substr(pathString.size() - 9 , 9 ).compare( "-NOTFOUND" ) )
  {
    pathString = "" ;
  }
  if(!pathString.empty()) // has been set to the value in the cmake cache -< check that it exists
  {
    mode_t ITKmode_X_OK = 1;
    if( !itksys::SystemTools::GetPermissions(pathString.c_str(), ITKmode_X_OK) )
    {
       pathString = ""; // if not executable, empty it so it can be found by FindProgram()
    }
  }
  if( pathString.empty() )
  {
    pathString= itksys::SystemTools::FindProgram( name ) ;
    if( pathString.empty() )
    {
      std::cerr << name << " is missing or its PATH is not set" << std::endl ;
      return 1 ;
    }
  }
  return 0 ;
}

int main (int argc, char *argv[])
{
  PARSE_ARGS;
  if( numberOfThreads != 0 )
  {
    std::ostringstream ssNumberOfThreads ;
    ssNumberOfThreads << "ITK_GLOBAL_DEFAULT_NUMBER_OF_THREADS=" << numberOfThreads ;
    itksys::SystemTools::PutEnv( ssNumberOfThreads.str().c_str() ) ;
  }
  //  std::vector< std::string > path_vec ; // initialized by the cmd line vector ProgramsPathsVector

  // Added by Adrien Kaiser : the tools are either in the same directory than the DTI-Reg executable ran or in the PATH
  // Get the directory where the DTI-Reg executable is
  std::string RanCommandDirectory = itksys::SystemTools::GetRealPath( itksys::SystemTools::GetFilenamePath(argv[0]).c_str() );
  if(RanCommandDirectory=="")
  {
    RanCommandDirectory="."; // If called by itself = either in the PATH or in the current directory : will be find either way by find_program
  }
#ifdef Slicer_Extension
  //We add "ExternalBin" to the directory that have to be looked in for the other software
  ProgramsPathsVector.push_back( RanCommandDirectory + "/../ExternalBin" ) ;
  //Since Slicer r23212, dependent extensions can be downloaded automatically. DTIProcess will therefore not be installed in ExternalBin
  // but it will be in the DTIProcess extension directory. We add this path to where DTIReg looks for other software
  ProgramsPathsVector.push_back( RanCommandDirectory + "/../../../../DTIProcess/" + std::string(Slicer_CLIMODULES_BIN_DIR) ) ;
  //We also need to find ResampleDTIlogEuclidean which is also an extension
  ProgramsPathsVector.push_back( RanCommandDirectory + "/../../../../ResampleDTIlogEuclidean/" + std::string(Slicer_CLIMODULES_BIN_DIR) ) ;
#ifdef __APPLE__
  // on Mac, slicer does not provide a PATH variable that includes the built-in CLIs
  // so we add it here (for BRAINS).
  std::string slicerHome ;
  if( itksys::SystemTools::GetEnv("SLICER_HOME", slicerHome) )
  {
    // Slicer_CLIMODULES_BIN_DIR is defined in vtkSlicerConfigure.h which is configured in the inner-build
    // directory of Slicer
    ProgramsPathsVector.push_back( slicerHome + "/" + Slicer_CLIMODULES_BIN_DIR ) ;
  }
#endif
#else
  // Add it in the ProgramsPathsVector, if DTI-Reg is not built as an extension.
  ProgramsPathsVector.push_back(RanCommandDirectory);
#endif

  std::cout<<"DTI-Reg: ";

  if( ( fixedVolume.empty() && scalarFixedVolume.empty() ) || movingVolume.empty() )
  {
    std::cerr << "Error: Fixed and moving volumes must be set!" << std::endl;
    std::cerr << "To display help, please type: DTI-Reg --help"<<std::endl;
    return EXIT_FAILURE ;
  }
  if( !fixedVolume.empty() && !scalarFixedVolume.empty() )
  {
    std::cerr << "Error: Only one fixed image should be set!" << std::endl;
    std::cerr << "To display help, please type: DTI-Reg --help"<<std::endl;
    return EXIT_FAILURE ;    
  }
#ifdef Slicer_Extension
  //If it is an extension, than we want to have access to either the transformed DTI or
  //to a transform
  //We verify that either are defined
  bool transformDefined = false ;
  if( !outputVolume.empty() || !outputResampledScalarVolume.empty() )
  {
    transformDefined = true ;
  }
  bool linear = false ;
  bool bspline = false ;
  bool nonlinear = false ;
  if( !method.compare("useScalar-BRAINS" ) )
  {
    if( !BRAINSRegistrationType.compare("Rigid")
          || !BRAINSRegistrationType.compare("Affine")
      )
    {
      linear = true ;
    }
    else if( !BRAINSRegistrationType.compare("BSpline") )
    {
      bspline = true ;
    }
    else
    {
      nonlinear = true ;
    }
  }
  else if( !method.compare("useScalar-ANTS" ) )
  {
    if( !ANTSRegistrationType.compare("Affine")
        || !ANTSRegistrationType.compare("Rigid")
      )
    {
      linear = true ;
    }
    else
    {
      nonlinear = true ;
    }
  }
  if( ( linear && !outputTransform.empty() )
   || ( nonlinear && !outputDeformationFieldVolume.empty() )
   || ( bspline && !outputBSplineTransform.empty() )
    )
  {
    transformDefined = true ;
  }
  if( !transformDefined )
  {
      std::cout<<"No output specified. Either define an output image ('Output DTI Volume' or 'Resampled Scalar Measurement Volume'),\
 or 'Linear Output Transform' for rigid or affine registration, 'BSpline Output Transform' for BSpline registration, or\
 or 'Output Displacement Field Volume' for any non-linear registration"<<std::endl;
    return EXIT_FAILURE;
  }
#endif
  // Write BatchMake script
  std::string outputDir, bmsScriptPrefix;
  bmsScriptPrefix = itksys::SystemTools::GetFilenameWithoutExtension(movingVolume);
  if( selectedOutputFolder.compare("") )
  {
    outputDir = itksys::SystemTools::GetRealPath( selectedOutputFolder ) ;
  }
  else if ( outputVolume.compare("") ) // outputVolume NON empty
  {
    outputDir = itksys::SystemTools::GetRealPath( itksys::SystemTools::GetFilenamePath(outputVolume).c_str() );
    if( outputDir.empty() )
    {
        outputDir = "." ;
    }
    bmsScriptPrefix = itksys::SystemTools::GetFilenameWithoutExtension(outputVolume);
  }
  else  // No outputVolume given
  {
    outputDir = ".";
  }
  std::string BatchMakeScriptFile = outputDir + "/" + bmsScriptPrefix + "_DTI-Reg.bms";
  std::ofstream file( BatchMakeScriptFile.c_str());

  file <<"# Inputs"<<std::endl;
  if( !fixedVolume.empty() )
  {
    file <<"set (useScalar FALSE)"<<std::endl;
    file <<"set (fixedVolume "<<fixedVolume<<")"<<std::endl;
  }
  else
  {
    file <<"set (useScalar TRUE)"<<std::endl;
    file <<"set (fixedScalarMap "<<scalarFixedVolume<<")"<<std::endl;
    file <<"set (fixedVolume "<<scalarFixedVolume<<")"<<std::endl;
  }
  file <<"set (movingVolume "<<movingVolume<<")"<<std::endl;
  if (initialAffine.compare(""))
  {
    file <<"set (initialAffine "<<initialAffine<<")"<<std::endl;
  }
  else
  {
    file <<"set (initialAffine \'\')"<<std::endl;
  }
#ifdef Slicer_Extension
  //if it is an extension, and if no output volume is given, we want to use the input
  // directory (ie: Slicer temp directory) as our output directory
  if( outputVolume.empty() )
  {
    outputDir = itksys::SystemTools::GetRealPath( itksys::SystemTools::GetFilenamePath(movingVolume).c_str() );
    bmsScriptPrefix = itksys::SystemTools::GetFilenameWithoutExtension(outputVolume);
  }
#endif
  if( outputBSplineTransform.empty() )
  {
      outputBSplineTransform = "\'\'" ;
  }
  file <<"set (outputBSplineTransform "<<outputBSplineTransform<<")"<<std::endl;
  file <<"set (OutputDir "<<outputDir<<")"<<std::endl;
  file <<"\n# Optional input mask volumes"<<std::endl;
  if (fixedMaskVolume.compare(""))
  {
    file <<"set (fixedMaskVolume "<<fixedMaskVolume<<")"<<std::endl;
  }
  else
  {
    file <<"set (fixedMaskVolume \'\')"<<std::endl;
  }
  if (movingMaskVolume.compare(""))
  {
    file <<"set (movingMaskVolume "<<movingMaskVolume<<")"<<std::endl;
  }
  else
  {
    file <<"set (movingMaskVolume \'\')"<<std::endl;
  }
  file <<"\n# Registration type"<<std::endl;
  file <<"set (Method "<<method<<")"<<std::endl;
  file <<"set (ANTSRegistrationType "<<ANTSRegistrationType<<")"<<std::endl;
  file <<"set (BRAINSRegistrationType "<<BRAINSRegistrationType<<")"<<std::endl;

  file <<"\n# Outputs"<<std::endl;
  if (outputVolume.compare(""))
  {
    file <<"set (outputVolume "<<outputVolume<<")"<<std::endl;
  }
  else
  {
    file <<"set (outputVolume \'\')"<<std::endl;
  }
  if (outputFixedScalarVolume.compare(""))
  {
    file <<"set (outputFixedScalarVolume "<<outputFixedScalarVolume<<")"<<std::endl;
  }
  else
  {
    file <<"set (outputFixedScalarVolume \'\')"<<std::endl;
  }
  if (outputMovingScalarVolume.compare(""))
  {
    file <<"set (outputMovingScalarVolume "<<outputMovingScalarVolume<<")"<<std::endl;
  }
  else
  {
    file <<"set (outputMovingScalarVolume \'\')"<<std::endl;
  }
  if (outputResampledScalarVolume.compare(""))
  {
    file <<"set (outputResampledScalarVolume "<<outputResampledScalarVolume<<")"<<std::endl;
  }
  else
  {
    file <<"set (outputResampledScalarVolume \'\')"<<std::endl;
  }
  if (outputTransform.compare(""))
  {
    file <<"set (outputTransform "<<outputTransform<<")"<<std::endl;
  }
  else
  {
    file <<"set (outputTransform \'\')"<<std::endl;
  }
  if (outputDeformationFieldVolume.compare(""))
  {
    file <<"set (outputDeformationFieldVolume "<<outputDeformationFieldVolume<<")"<<std::endl;
  }
  else
  {
    file <<"set (outputDeformationFieldVolume \'\')"<<std::endl;
  }
  if (outputInverseDeformationFieldVolume.compare(""))
  {
    file <<"set (outputInverseDeformationFieldVolume "<<outputInverseDeformationFieldVolume<<")"<<std::endl;
  }
  else
  {
    file <<"set (outputInverseDeformationFieldVolume \'\')"<<std::endl;
  }
  if (!method.compare("useScalar-BRAINS"))
  {
    file <<"\n# BRAINS Registration Parameters"<<std::endl;
    file <<"set (BRAINSnumberOfPyramidLevels "<<BRAINSnumberOfPyramidLevels<<")"<<std::endl;
    file <<"set (BRAINSarrayOfPyramidLevelIterations "<<BRAINSarrayOfPyramidLevelIterations<<")"<<std::endl;
       
    file <<"set (BRAINSinitializeTransformMode "<<BRAINSinitializeTransformMode<<")"<<std::endl;
    if (BRAINSinitialDeformationField.compare(""))
    {
      file <<"set (BRAINSinitialDeformationField "<<BRAINSinitialDeformationField<<")"<<std::endl;
    }
    else
    {
      file <<"set (BRAINSinitialDeformationField \'\')"<<std::endl;
    }
    file <<"# Histogram matching"<<std::endl;
    file <<"set (BRAINSnumberOfHistogramLevels "<<BRAINSnumberOfHistogramLevels<<")"<<std::endl;
    file <<"set (BRAINSnumberOfMatchPoints "<<BRAINSnumberOfMatchPoints<<")"<<std::endl;
       
    file <<"\n#External Tools"<<std::endl;
    std::string BRAINSFitCmd = BRAINSFitTool;
    if( SetPath(BRAINSFitCmd, "BRAINSFit" , ProgramsPathsVector , BRAINSFitPath ) )
    {
      return EXIT_FAILURE;
    }
    else
    {
      file <<"set (BRAINSFitCmd "<<BRAINSFitCmd<<")"<<std::endl;
    }
    std::string BRAINSDemonWarpCmd = BRAINSDemonWarpTool;
    if( SetPath(BRAINSDemonWarpCmd, "BRAINSDemonWarp" , ProgramsPathsVector , BRAINSDemonWarpPath ) )
    {
      return EXIT_FAILURE;
    }
    else
    {
      file <<"set (BRAINSDemonWarpCmd "<<BRAINSDemonWarpCmd<<")"<<std::endl;
    }
  }
  if (!method.compare("useScalar-ANTS"))
  {
    if (ANTSOutbase.compare(""))
    {
	    file <<"set (ANTSOutbase "<<ANTSOutbase<<")"<<std::endl;
    }
    else
    {
      file <<"set (ANTSOutbase \'\')"<<std::endl;
    }
   
    file <<"\n# ANTS Registration Parameters"<<std::endl;
    file <<"set (ANTSIterations "<<ANTSIterations<<")"<<std::endl;
    file <<"set (ANTSSimilarityMetric "<<ANTSSimilarityMetric<<")"<<std::endl;
    file <<"set (ANTSSimilarityParameter "<<ANTSSimilarityParameter<<")"<<std::endl;
    file <<"set (ANTSTransformationStep "<<ANTSTransformationStep<<")"<<std::endl;
    file <<"set (ANTSGaussianSmoothingOff "<<ANTSGaussianSmoothingOff<<")"<<std::endl;
    file <<"set (ANTSGaussianSigma "<<ANTSGaussianSigma<<")"<<std::endl;
    file <<"set (ANTSUseHistogramMatching "<<ANTSUseHistogramMatching<<")"<<std::endl;
    file <<"\n#External Tools"<<std::endl;
    std::string ANTSCmd = ANTSTool;
    if( SetPath(ANTSCmd, "ANTS" , ProgramsPathsVector , ANTSPath ) )
    {
	    return EXIT_FAILURE;
    }
    else
    {
      file <<"set (ANTSCmd "<<ANTSCmd<<")"<<std::endl;
    }
      
    std::string WarpImageMultiTransformCmd = WarpImageMultiTransformTool;
    if( SetPath(WarpImageMultiTransformCmd, "WarpImageMultiTransform" , ProgramsPathsVector , WarpImageMultiTransformPath ) )
    {
      return EXIT_FAILURE;
    }
    else
    {
      file <<"set (WarpImageMultiTransformCmd "<<WarpImageMultiTransformCmd<<")"<<std::endl; 
    }
      
  } // if (!method.compare("useScalar-ANTS"))
  std::string dtiprocessCmd = dtiprocessTool;
  if( SetPath(dtiprocessCmd, "dtiprocess" , ProgramsPathsVector , dtiprocessPath ) )
  {
    return EXIT_FAILURE;
  }
  else
  {
    file <<"set (dtiprocessCmd "<<dtiprocessCmd<<")"<<std::endl;
  }
  std::string ResampleDTICmd = ResampleDTITool;
  if( SetPath(ResampleDTICmd, "ResampleDTIlogEuclidean" , ProgramsPathsVector , ResampleDTIPath ) )
  {
    return EXIT_FAILURE;
  }
  else
  {
    file <<"set (ResampleDTICmd "<<ResampleDTICmd<<")"<<std::endl;
  }
  std::string ITKTransformToolsCmd = ITKTransformToolsTool;
  if( SetPath(ITKTransformToolsCmd, "ITKTransformTools" , ProgramsPathsVector , ITKTransformToolsPath ) )
  {
    return EXIT_FAILURE;
  }
  else
  {
    file <<"set (ITKTransformToolsCmd "<<ITKTransformToolsCmd<<")"<<std::endl;
  }
  file <<"set (TensorCorrection "<<correction<<")"<<std::endl;
  file <<"set (ScalarMeasurement "<<scalarMeasurement<<")"<<std::endl;
  // Include main BatchMake script
  file <<"\n#Include main batchMake script"<<std::endl;
  if (!method.compare("useScalar-ANTS"))
  {
    std::cout<<"Registration via ANTS..."<<std::endl;
    file <<DTIReg_Scalar_ANTS<<std::endl; // Added by Adrien Kaiser : String variable defined in DTI-Reg-bms.h that contains the bms script for ANTS
  }
  else if (!method.compare("useScalar-BRAINS"))
  {
    std::cout<<"Registration via BRAINS..."<<std::endl;
    file <<DTIReg_Scalar_BRAINS<<std::endl; // Added by Adrien Kaiser : String variable defined in DTI-Reg-bms.h that contains the bms script for BRAINS
  }
  file.close();
  // Execute BatchMake
  bm::ScriptParser m_Parser;
  bool val = m_Parser.Execute(BatchMakeScriptFile);
  std::cout<<"DTI-Reg: Done!"<<std::endl;
  if( val )
  {
    return EXIT_SUCCESS ;
  }
  else
  {
    return EXIT_FAILURE ;
  }
}
