//
//  Vibrato.cpp
//  MUSI8903
//
//  Created by Som on 05/02/16.
//
//

// standard headers
#include <iostream>

#include "MUSI8903Config.h"

// project headers
#include "ErrorDef.h"
#include "Ringbuffer.h"

#include "Vibrato.h"

using namespace std;

static const char*  kVibratoBuildDate             = __DATE__;


Vibrato::Vibrato (int num_channels, long int sample_rate)
{
    _sample_rate = sample_rate;
    _num_channels = num_channels;
    _num_samples_processed = 0;
    //convertTimeToSamples(max_delay_width_secs, _max_delay_width);
     _sin_osc = new LFO(0.F);
    _ring_delay_line = new CRingBuffer<float>*[num_channels];
    
    for (int i = 0; i < _num_channels; i++) {
        _ring_delay_line[i] = new CRingBuffer<float>(static_cast<int>(_max_delay_width_secs*_sample_rate));
    }
    reset();
}


Vibrato::~Vibrato () {}

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

Error_t Vibrato::create (Vibrato*& pVibrato, long int sample_rate, int num_channels)
{
	// Hard limit on the maximum delay buffer size set to 2 seconds.
	float max_delay_width_sec = 2.0F;
    pVibrato = new Vibrato (num_channels, sample_rate);
    
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

Error_t Vibrato::init(float mod_freq, float delay_width_secs, float mod_amp_secs)
{
    reset();
    
    // initialise parameters
    _mod_freq = mod_freq;
    convertTimeToSamples(delay_width_secs, _delay_width);
    convertTimeToSamples(mod_amp_secs, _mod_amp);
    _sin_osc->setOscFreq(mod_freq);
    
    // check parameter validity
	if (_delay_width * 2 > _max_delay_width_secs*_sample_rate)
		return kFunctionInvalidArgsError;

    if (_mod_amp < 0 || _delay_width < 0)
        return kFunctionInvalidArgsError;
    
    if (_mod_amp > _delay_width)
        return kFunctionInvalidArgsError;
    
    if (_delay_width + _mod_amp > _max_delay_width_secs*_sample_rate)
        return kFunctionInvalidArgsError;

    // set ring buffer write index
    int delay_length = _delay_width + _mod_amp;
    for (int i = 0; i < _num_channels; i++) {
        
        _ring_delay_line[i]->setWriteIdx(_ring_delay_line[i]->getReadIdx() + delay_length + 1);
    }
    
    return kNoError;
}

Error_t Vibrato::reset ()
{
    // reset buffers and variables to default values
    _mod_freq = 0.F;
    _delay_width = 0;
    _mod_amp = 0;
    _num_samples_processed = 0;
    _sin_osc->setOscFreq(0);
    for (int i = 0; i < _num_channels; i++) {
        _ring_delay_line[i]->reset();
    }
    return kNoError;
}

Error_t Vibrato::convertTimeToSamples(float paramValue, int& param)
{
    param = static_cast<int>(round(paramValue * _sample_rate));
    return kNoError;
}

Error_t Vibrato::setModAmp(float delay_width_secs)
{
    int temp = 0;
    convertTimeToSamples(delay_width_secs, temp);
    if (temp > _delay_width || temp < 0)
        return kFunctionInvalidArgsError;
    
    int num_additional_taps  = temp - _mod_amp;
    if (num_additional_taps < 0)
    {
        for (int i = 0; i < _num_channels; i++)
        {
            _ring_delay_line[i]->setWriteIdx(temp + _mod_amp + _ring_delay_line[i]->getReadIdx() + 1);
        }
    }
    else
    {
        
        for (int i = 0; i < _num_channels; i++)
        {
            for (int j = 0; j < num_additional_taps; j++)
            {
                _ring_delay_line[i]->putPostInc(0.F);
            }
        }
    }
    
    _mod_amp = temp;
    return kNoError;
}

Error_t Vibrato::setModFreq(float mod_freq)
{
    if (mod_freq < 0)
        return kFunctionInvalidArgsError;
    _mod_freq = mod_freq;
    _sin_osc->setOscFreq(_mod_freq);
    return kNoError;
}

Error_t Vibrato::getDelayWidth(int &delay_width)
{
    delay_width = _delay_width;
    return kNoError;
}

Error_t Vibrato::getModAmp(int &mod_amp)
{
    mod_amp = _mod_amp;
    return kNoError;
}

Error_t Vibrato::getModFreq(float &mod_freq)
{
    mod_freq = _mod_freq;
    return kNoError;
}

Error_t Vibrato::process (float **input_buffer, float **output_buffer, int number_of_frames)
{
    float temp = 0;
    double tap = 0;
    
    for (int data_id = 0; data_id < number_of_frames; data_id++)
    {
        for (int channel_id = 0; channel_id < _num_channels; channel_id++)
        {
            _ring_delay_line[channel_id]->putPostInc(input_buffer[channel_id][data_id]);
            temp = _ring_delay_line[channel_id]->getPostInc();
            double time = _num_samples_processed/static_cast<double>(_sample_rate);
            tap  = 1 + _delay_width + _mod_amp * _sin_osc->getOscOutput(time);
            tap = _ring_delay_line[channel_id]->getWriteIdx() - tap - _ring_delay_line[channel_id]->getReadIdx();
            output_buffer[channel_id][data_id] = _ring_delay_line[channel_id]->get(tap);
        }
        _num_samples_processed++;
    }
    return kNoError;
}

