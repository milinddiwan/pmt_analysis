////////////////////////////////////////////////////////////////////////////////
// Waveform class definition
//
// mailto:ascarpell@bnl.gov
////////////////////////////////////////////////////////////////////////////////

#ifndef  __WAVEFORM_H
#define __WAVEFORM_H

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>

#include "TH1D.h"

using namespace std;

class Waveform
{

    public:

      typedef vector<unsigned short> Rawdigits_t;
      typedef vector<double> Waveform_t;

      Waveform();
      Waveform(int run, int subrun, int event ,int board, int channel);
      Waveform( int run, int subrun, int event ,int board, int channel,
                                                     Rawdigits_t raw_waveform );
      ~Waveform();

      // Import data
      void loadData( Rawdigits_t raw_waveform );

      // Getters
      int getRun(){ return m_run; }
      int getSubrun(){ return m_subrun; }
      int getEvent(){ return m_event; }
      int getBoard(){ return m_board; }
      int getChannel(){ return m_channel; }

      Rawdigits_t getRawWaveform(){ return m_raw_waveform; };
      Waveform_t getWaveform(){ return m_waveform; };

      double getBaselineMean(){return baseline_mean;};
      double getBaselineWidth(){return baseline_width;}

      // Indentify and characterize pulses
      bool hasSignal(double n_sigma);
      bool hasPulse(double n_sigma);

      double getStartTime(){ return m_start_time; };
      double getWidth(){ return m_width; };
      double getAmplitude(){ return m_amplitude; }
      double getIntegral(){ return m_integral; }

      // Helpers
      bool find(int run, int subrun, int event, int board, int channel );
      TH1D* getWaveformHist();

    private:

      int m_run;
      int m_subrun;
      int m_event;
      int m_board;
      int m_channel;

      int m_nsamples;
      double m_sampling_period=1.0;

      int n_sample_baseline=200;
      double baseline_mean=0.0;
      double baseline_width=0.0;

      Rawdigits_t m_raw_waveform;
      Waveform_t m_waveform;

      double m_start_time=0.0;
      double m_width=0.0;
      double m_amplitude=0.0;
      double m_integral=0.0;

      void removeBaseline();
};


#endif //__WAVEFORM_H