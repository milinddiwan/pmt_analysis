////////////////////////////////////////////////////////////////////////////////
// Basic library loader into ROOT
// mailto:ascarpell@bnl.gov
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>

void loadLib(){

  char cwd[1024];
  getcwd(cwd, sizeof(cwd));

  //gStyle->SetPalette(kRainBow);
  //gStyle->SetOptFit(11111);

  std::string currentdir( cwd );
  std::string includepath=currentdir+"/../inc/";

  cout << includepath << endl;

  gSystem->SetBuildDir("obj",true);

  // For some reasons ROOT has not a dictionary for vector of vectors
  gInterpreter->GenerateDictionary("vector<vector<unsigned short>>", "vector");

  // Custom classes
  gInterpreter->AddIncludePath(includepath.c_str());
  gROOT->LoadMacro((currentdir+"/../src/Waveform.cc+").c_str());
  //gROOT->LoadMacro((currentdir+"/../src/Event.cc+").c_str());
  gROOT->LoadMacro((currentdir+"/../src/Pmt.cc+").c_str());

  #define __INITIALIZED__

}//end loadLib
