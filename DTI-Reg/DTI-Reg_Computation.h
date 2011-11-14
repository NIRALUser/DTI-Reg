#ifndef COMPUTATION_H
#define COMPUTATION_H

class Computation
{
 public:
  Computation(){};
  ~Computation(){};

  ComputeFA();
  HistogramMatching();
  AffineRegistration();
  Warping();
  ResampleDTI();

 private:
  std::string RemoveExtension( std::string data , std::string &ext );
  std::string GetDirectory( std::string &data );
   
};
