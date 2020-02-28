////////////////////////////////////////////////////////////////////////////////
// Waveform methods implementation
//
// mailto:ascarpell@bnl.gov
////////////////////////////////////////////////////////////////////////////////

#include "Waveform.h"

//------------------------------------------------------------------------------

Waveform::Waveform(){};

//------------------------------------------------------------------------------

Waveform::Waveform(int run, int subrun, int event, int board, int channel)
  : m_run(run)
  , m_subrun(subrun)
  , m_event(event)
  , m_board(board)
  , m_channel(channel)
{ };

//------------------------------------------------------------------------------

Waveform::Waveform( int run, int subrun, int event ,int board, int channel,
                                                      Rawdigits_t raw_waveform )
  : Waveform(run, subrun, event, board, channel)
{
  this->loadData(raw_waveform);
};

//------------------------------------------------------------------------------

Waveform::~Waveform(){};

//------------------------------------------------------------------------------

void Waveform::loadData( Rawdigits_t raw_waveform )
{
  m_nsamples=int(raw_waveform.size());
  m_raw_waveform = raw_waveform;
  for( auto w : raw_waveform ){ m_waveform.push_back( double(w) ); }
  removeBaseline();
};

//------------------------------------------------------------------------------

void Waveform::removeBaseline()
{
  // Calculate the baseline as the mean values on the last part of the spectrum
  for(int t=m_nsamples-1; t>m_nsamples-1-n_sample_baseline; t--)
  {
    baseline_mean += m_waveform.at(t);
  }
  baseline_mean /= n_sample_baseline;

  // Calculate the stdev of the baseline
  for(int t=m_nsamples-1; t>m_nsamples-1-m_nsamples; t--)
  {
    baseline_width += pow(m_waveform.at(t)-baseline_mean, 2);
  }
  baseline_width = sqrt( baseline_width / (n_sample_baseline-1) );

  // Subtract the baseline from the signal waveform
  std::transform(m_waveform.begin(), m_waveform.end(), m_waveform.begin(),
                                [ & ] (double x) { return x - baseline_mean; });

};

//------------------------------------------------------------------------------

bool Waveform::find(int run, int subrun, int event, int board, int channel )
{
  return (run==m_run && subrun==m_subrun && event==m_event
                                       && board==m_board && channel==m_channel);
};

//------------------------------------------------------------------------------

bool Waveform::hasSignal(double n_sigma)
{
  // Find if the waveform has n consecutive counts above a threshold expressed
  // in number of sigmas

  bool has_signal=false;
  int n_counts=5.0;
  int counts=0;

  for( double value : m_waveform )
  {
    if( abs(value) > n_sigma*baseline_width ){ counts++; }
    else{ counts=0; } // Reset the counts

    if( counts > n_counts ){ has_signal=true; break; }
  }

  return has_signal;
};

//------------------------------------------------------------------------------

bool Waveform::hasPulse( double n_sigma )
{
  // Define the Pulse region of the signal as the region with >n consecutive
  // counts above a threshold expressed in number of sigmas.

  bool has_pulse=false;
  int counts=0;
  int n_counts=5.0;

  int t_start=0, t_end=0;

  for(int t=0; t<m_nsamples; t++)
  {
    double value = m_waveform.at(t);

    if( abs(value) > n_sigma*baseline_width )
    {
      counts++;
      if( counts==1 ){ t_start=t-2; }
    }
    else
    {
      // if it is a pulse region, then this is the ending clause
      if( counts>n_counts && t_start>0 )
      {
        t_end=t+3;
        has_pulse=true;
        break; // Force to have only one pulse
      }
      else
      {
        // reset everything
        counts=0; t_start=0; t_end=0;
      }
    }
  }

  // If no pulse is found, we end the games here..
  if( !has_pulse ) { return has_pulse; }

  // Here we define the characteristics of the pulse
  double amp = 0., charge=0;
  for( int t=t_start; t<t_end; t++ )
  {
    if( m_waveform.at(t) < amp ){ amp = m_waveform.at(t); }
    charge += abs(m_waveform.at(t));
  }

  m_start_time = t_start;
  m_width = (t_end-t_start);
  m_amplitude = amp;
  m_integral = charge;

  return has_pulse;
};

//------------------------------------------------------------------------------

  // Use fit to find the pulse instead

  // Better way to find the pulse

  // Noise and reflecting pulses subtraction ?

//------------------------------------------------------------------------------

TH1D* Waveform::getWaveformHist()
{
  char hname[100];
  sprintf(hname, "Run%d-Subrun%d-Event%d-Board%d-Channel%d_hist", m_run,
                                         m_subrun, m_event, m_board, m_channel);

  TH1D *hist = new TH1D(hname, hname, m_nsamples,
                                               0, m_nsamples*m_sampling_period);

  for(int t=0; t<m_nsamples; t++){ hist->Fill( t, m_waveform.at(t) ); }

  return hist;
};

//------------------------------------------------------------------------------