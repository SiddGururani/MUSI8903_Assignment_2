//
//  Vibrato.h
//  MUSI8903
//
//  Created by Som on 05/02/16.
//
//

#ifndef Vibrato_h
#define Vibrato_h

#include "LFO.h"
#include "RingBuffer.h"

class Vibrato
{
public:
    /*! version number */
    enum Version_t
    {
        kMajor,                         //!< major version number
        kMinor,                         //!< minor version number
        kPatch,                         //!< patch version number
        
        kNumVersionInts
    };
    
    static const int  getVersion (const Version_t eVersionIdx);
    static const char* getBuildDate ();
    static Error_t create (Vibrato*& pCKortIf, float max_delay_width_sec, long int sample_rate, int num_channels);
    static Error_t destroy (Vibrato*& pCKortIf);
    
    Error_t init (float mod_freq, int delay_width, int mod_amp);
    Error_t reset ();
    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
    Error_t setDelayWidth(float delay_width_secs);
    Error_t setModAmp(float mod_amp_secs);
    Error_t setModFreq(float mod_freq);
    
private:
    Vibrato (int max_delay, int num_channels);
    virtual ~Vibrato ();
    
    CRingBuffer<float>** _ring_delay_line;
    LFO* _sin_osc;
    float _mod_freq;
    int _delay_width;
    int _max_delay_width;
    int _mod_amp;
    long int _sample_rate;
    int _num_channels;
   
    
    Error_t convertTimeToSamples(int paramValue, int& param);
};


#endif /* Vibrato_h */
