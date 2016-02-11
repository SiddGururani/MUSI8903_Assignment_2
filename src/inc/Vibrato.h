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

	// Function to instantiate a Vibrato object pointer with given sample rate and channel specifications
    static Error_t create (Vibrato*& pCKortIf, long int sample_rate, int num_channels);

	// Function to clear and delete buffers and delete the Vibrato object pointer
    static Error_t destroy (Vibrato*& pCKortIf);
    
    //initialization and reset functions
    Error_t init (float mod_freq, float delay_width_secs, float mod_amp_secs);
    Error_t reset ();
    
    
    //set functions for parameter member variables
    Error_t setModAmp(float mod_amp_secs);
    Error_t setModFreq(float mod_freq);
    
    //get functions for member variables
    Error_t getDelayWidth(int& delay_width);
    Error_t getModAmp(int& mod_amp);
    Error_t getModFreq(float& mod_freq);
    
    //process function
    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
    
private:
    Vibrato (int num_channels, long int sample_rate);
    virtual ~Vibrato ();
    
	//Limit for the maximum ring buffer size that may be used by the developer.
	const float _max_delay_width_secs = 2.0F;

    CRingBuffer<float>** _ring_delay_line;
    LFO* _sin_osc;
    float _mod_freq;
    int _delay_width;
    int _mod_amp;
    long int _sample_rate;
    int _num_channels;
    long long int _num_samples_processed;
   
    Error_t convertTimeToSamples(float paramValue, int& param);
};


#endif /* Vibrato_h */
