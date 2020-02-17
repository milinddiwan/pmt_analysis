////////////////////////////////////////////////////////////////////////////////
// Macro that interfaces the output of the CAEN decoder script and produces
// a file format compatible with this small analysis framework
//
// mailto:ascarpell@bnl.gov
////////////////////////////////////////////////////////////////////////////////

#include "Waveform.h"
//#include "Event.h"
#include "Pmt.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"

using namespace std;

PMT* locatePMT(int run, int board, int channel , vector<PMT*> pmts)
{

  int index = -1;

  auto find = [&]( PMT *pmt )
  {
    return (pmt->getRun()==run && pmt->getBoard()==board && pmt->getChannel()==channel);
  };

  auto it = std::find_if( pmts.begin(), pmts.end(), find );

  return *it;
}

void load_pmt()
{
  //****************************************************************************
  // General definitions ( ..import from DB )

  int run=1067;
  int subrun=11;
  int nboards=12;
  int nchannels=16;

  //****************************************************************************
  // Data structures definitions

  vector<PMT*> pmts;

  for( int board=0; board<nboards; board++ )
  {
    for( int channel=0; channel<nchannels; channel++ )
    {
      PMT *pmt = new PMT( run, board, channel );
      pmts.push_back(pmt);
    }
  }

  //****************************************************************************
  // Input

  string filename="../data/data_dl1_run1067_11_20200204T230459_dl3.root";
  string treename="caenv1730dump/events";

  // Open TFile
  TFile* ifile = new TFile(filename.c_str(), "READ");
  cout << "Open TFile"+filename << endl;

  // Get the TTres
  TTree* tevents = (TTree*)ifile->Get(treename.c_str());
  int n_events = tevents->GetEntries();
  cout << "TTree"+treename << " has " << n_events << " events" << endl;

  // Set Branch address
  std::vector<std::vector<uint16_t> > *data=0; //unsigned short
  tevents->SetBranchAddress("fWvfmsVec", &data);

  //****************************************************************************
  // Event loop

  // Loop over the events
  for(int e=0; e<n_events; e++)
  {
    cout << "Processing event: " << e << endl;

    tevents->GetEvent(e);

    const int n_channels = 16;
    const int n_samples = (*data)[0].size();
    const int n_boards = (*data).size()/n_channels;

    for(int board=0; board<n_boards; board++)
    {
      for(int channel=0; channel<n_channels; channel++)
      {

        // Create the PMT object
        Waveform *waveform = new Waveform(run, subrun ,e, board, channel);
        waveform->loadData((*data).at(channel+n_channels*board));

        if(waveform->hasSignal(1.0))
        {
          PMT *pmt_temp = locatePMT(run, board, channel, pmts);
          pmt_temp->loadWaveform(waveform);
        }

      } // end channel
    } // end boards

  } // end events

  ifile->Close();

  //****************************************************************************
  // Now save the PMTs

  // Open TFile
  TFile ofile("../data/data_run1067_11_pmt.root", "RECREATE"); ofile.cd();

  TTree pmt_tree("pmts", "Calibration information for each PMT");
  PMT *pmt;
  pmt_tree.Branch( "pmt", &pmt );

  for(auto pmt_it : pmts)
  {
    if( pmt_it->hasWaveform() )
    {
      pmt = pmt_it;
      pmt_tree.Fill();
    }
  }

  pmt_tree.Write("pmts");
  pmt_tree.Print();
  ofile.Close();

  cout << "All done ..." << endl;

} //end macro
