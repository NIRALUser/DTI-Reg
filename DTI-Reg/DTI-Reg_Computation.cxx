#include "DTI-Reg_Computation.h"

std::string Computation::RemoveExtension( std::string data , std::string &ext )
{
  int found = data.rfind( "." ) ;
  ext = data.substr( found + 1 , data.size() - found - 1 ) ;
  data.resize( found ) ;
  if( !ext.compare( "gz" ) )
  {
     found = data.rfind( "." ) ;
     ext = data.substr( found + 1 , data.size() - found - 1 ) ;
     ext = ext + ".gz" ;
     data.resize( found ) ;
  }
  return data ;
}

std::string Computation::GetDirectory( std::string &data )
{
  size_t found = data.rfind('/') ;
  if( found != std::string::npos )
  {
    itksys_stl::string dir1 = data.substr( 0 , found ) ;
    data.erase( data.begin() , data.begin() + found + 1 ) ;
    return dir1 ;
  }
  return "." ;
}

void Computation::ComputeFA()
{
  pathFiberProcess= itksys::SystemTools::FindProgram("fiberprocess");
  // enter the path for fiberprocess if it didn't find it
  if(pathFiberProcess.empty()==true)
    {
      std::cout<<"Give the path for fiberprocess : "<<std::endl;
      std::cin>>pathFiberProcess;
    }
  //Call fiberprocess
  Applyfiberprocess(CSVFile, pathFiberProcess, AtlasFiberDir, OutputFolder, DataCol, DefCol,
		    NameCol, SelectedFibers,parameters,transposeColRow,true);
  
}

  std::vector<const char*> args;  
  args.push_back("BatchMake");
  args.push_back("-e");
  args.push_back(_Input);
  // don't forget this line at the end of the argument list
  args.push_back(0);


  // Run the application
  m_Process = itksysProcess_New();
  itksysProcess_SetCommand(m_Process, &*args.begin());
  itksysProcess_SetOption(m_Process,itksysProcess_Option_HideWindow,1);
  itksysProcess_Execute(m_Process);   
