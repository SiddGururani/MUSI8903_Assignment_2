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


Vibrato::Vibrato () {}


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

Error_t Vibrato::create (Vibrato*& pVibrato, float mod_freq, int delay_width)
{
    pVibrato = new Vibrato ();
    
    if (!pVibrato)
        return kUnknownError;
    pVibrato->_mod_freq = mod_freq;
    pVibrato->_delay_width = delay_width;
    pVibrato->_ring_delay_line = new CRingBuffer<float>(2*delay_width+1);
    pVibrato->_sin_osc = new LFO(mod_freq);
    return kNoError;
}

Error_t Vibrato::destroy (Vibrato*& pVibrato)
{
    if (!pVibrato)
        return kUnknownError;
    
    pVibrato->reset ();
    
    delete pVibrato->_sin_osc;
    delete pVibrato->_ring_delay_line;
    delete pVibrato;
    pVibrato = 0;
    
    return kNoError;
}

Error_t Vibrato::init(float mod_freq, int delay_width)
{
    // allocate memory
    
    // initialize variables and buffers
    _mod_freq = mod_freq;
    _delay_width = delay_width;
    _ring_delay_line = new CRingBuffer<float>(2*delay_width+1);
    _sin_osc = new LFO(mod_freq);
    
    return kNoError;
}

Error_t Vibrato::reset ()
{
    // reset buffers and variables to default values
    _mod_freq = 0.F;
    _delay_width = 0;
    _sin_osc->setOscFreq(0);
    _ring_delay_line->reset();
    return kNoError;
}
