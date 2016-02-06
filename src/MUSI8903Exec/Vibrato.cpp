//
//  Vibrato.cpp
//  MUSI8903
//
//  Created by Som on 05/02/16.
//
//

// standard headers

#include "MUSI8903Config.h"

// project headers
#include "ErrorDef.h"
#include "Ringbuffer.h"

#include "Vibrato.h"

static const char*  kVibratoBuildDate             = __DATE__;


Vibrato::Vibrato (int max_delay_width, int num_channels)
{
    _ring_delay_line = new CRingBuffer<float>*[num_channels];
    for (int i = 0; i < _num_channels; i++) {
        _ring_delay_line[i] = new CRingBuffer<float>(_max_delay_width);
    }
}


Vibrato::~Vibrato ()
{
    this->reset ();
}

const int  Vibrato::getVersion (const Version_t eVersionIdx)
{
    int iVersion = 0;
    
    switch (eVersionIdx)
    {
        case kMajor:
            iVersion    = MUSI8903_VERSION_MAJOR;
            break;
        case kMinor:
            iVersion    = MUSI8903_VERSION_MINOR;
            break;
        case kPatch:
            iVersion    = MUSI8903_VERSION_PATCH;
            break;
        case kNumVersionInts:
            iVersion    = -1;
            break;
    }
    
    return iVersion;
}
const char*  Vibrato::getBuildDate ()
{
    return kVibratoBuildDate;
}

Error_t Vibrato::create (Vibrato*& pVibrato, float max_delay_width_sec, long int sample_rate, int num_channels)
{
    pVibrato->convertTimeToSamples(max_delay_width_sec, pVibrato->_max_delay_width);
    pVibrato->_num_channels = num_channels;
    pVibrato->_sample_rate = sample_rate;
    pVibrato = new Vibrato (pVibrato->_max_delay_width, num_channels);
    
    if (!pVibrato)
        return kUnknownError;

    return kNoError;
}

Error_t Vibrato::destroy (Vibrato*& pVibrato)
{
    if (!pVibrato)
        return kUnknownError;
    
    pVibrato->reset ();
    
    delete pVibrato->_sin_osc;
    for (int i = 0; i < pVibrato->_num_channels; i++) {
        delete pVibrato->_ring_delay_line[i];
    }
    delete[] pVibrato->_ring_delay_line;
    delete pVibrato;
    pVibrato = 0;
    
    return kNoError;
}

Error_t Vibrato::init(float mod_freq, int delay_width_secs, int mod_amp_secs)
{
    reset();
    
    // initialise parameters
    _mod_freq = mod_freq;
    convertTimeToSamples(delay_width_secs, _delay_width);
    convertTimeToSamples(mod_amp_secs, _mod_amp);
    _sin_osc = new LFO(mod_freq);
    
    // check parameter validity
    if (_mod_amp > _delay_width)
        return kFunctionInvalidArgsError;
    
    if (2*_delay_width+1 > _max_delay_width)
        return kFunctionInvalidArgsError;

    // set ring buffer write index
    int delay_length = _delay_width + _mod_amp + 1;
    for (int i = 0; i < _num_channels; i++) {
        
        _ring_delay_line[i]->setWriteIdx(_ring_delay_line[i]->getReadIdx() + delay_length);
    }
    
    return kNoError;
}

Error_t Vibrato::reset ()
{
    // reset buffers and variables to default values
    _mod_freq = 0.F;
    _delay_width = 0;
    _mod_amp = 0;
    _sin_osc->setOscFreq(0);
    for (int i = 0; i < _num_channels; i++) {
        _ring_delay_line[i]->reset();
    }
    return kNoError;
}
Error_t Vibrato::process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
    
    return kNoError;
}

Error_t Vibrato::convertTimeToSamples(int paramValue, int& param)
{
    param = paramValue*_sample_rate;
    return kNoError;
}
