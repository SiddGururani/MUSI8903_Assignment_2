//
//  LFO.h
//  MUSI8903
//
//  Created by Som on 05/02/16.
//
//

#ifndef LFO_h
#define LFO_h

#include <cassert>
#include <algorithm>
#include <cmath>

#define PI 3.14159265359

using namespace std;

class LFO
{
public:
    
    LFO(float mod_freq)
    {
        _mod_freq = mod_freq;
    }
    ~LFO()
    {
        _mod_freq = 0;
    }
    /* returns the LFO output at a given time in seconds
    float time: time in seconds
    */
    float getOscOutput(float time)
    {
        return sin(2*PI*_mod_freq*time);
    }
    
    void setOscFreq(float mod_freq)
    {
        _mod_freq = mod_freq;
    }
private:
    float _mod_freq;
};



#endif /* LFO_h */
