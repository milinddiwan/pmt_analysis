////////////////////////////////////////////////////////////////////////////////
// PMT Class definition
//
// mailto:ascarpell@bnl.gov
////////////////////////////////////////////////////////////////////////////////

#ifndef  __PMT_H
#define __PMT_H

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>

#include "Geometry.h"
#include "Waveform.h"
#include "TH1D.h"

using namespace std;

class PMT
{

  public:

    PMT();
    PMT(int run, int board, int channel );
    PMT(int run, int pmt_number );
    ~PMT();

    // Loaders
    void loadWaveform( Waveform *waveform );

    // Getters
    int getRun(){ return m_run; };
    int getBoard(){ return m_board; };
    int getChannel(){ return m_channel; }
    int getPmtNumber(){ return m_pmt_number; }
    void getBoardAndChannel( int pmt_number, int & board, int & channel);

    // vector<double> getAmplitudes(){ return m_amplitude_array; };

    // Helpers
    void clean();
    void initHist();
    void writeHist();
    bool isIlluminated( int optical_channel );

  private:

    int m_run;
    int m_board;
    int m_channel;
    int m_pmt_number;

    // vector<double> m_amplitude_array;

    TH1D *h_amplitude;
    TH1D *h_amplitude_low;

};

#endif //__PMT_H
