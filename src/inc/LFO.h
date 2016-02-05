//
//  LFO.h
//  MUSI8903
//
//  Created by Som on 05/02/16.
//
//

#ifndef LFO
#define LFO

#include <cassert>
#include <algorithm>
#include <cmath>

#define PI 3.14159265359

using namespace std;

class LFO
{
public:
    
    /* returns the LFO output at a given time in seconds
    float time: time in seconds
    */
    float getOscOutput(float time)
    {
        return sin(2*PI*_mod_freq*time);
    }
    
private:
    float _mod_freq;
};



#endif /* LFO */
