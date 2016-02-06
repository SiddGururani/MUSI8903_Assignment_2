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
    
    static Error_t create (Vibrato*& pCKortIf,float mod_freq, int delay_width, int mod_amp);
    static Error_t destroy (Vibrato*& pCKortIf);
    
    Error_t init (float mod_freq, int delay_width, int mod_amp);
    Error_t reset ();
    
    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);
    
private:
    Vibrato ();
    virtual ~Vibrato ();
    
    CRingBuffer<float>* _ring_delay_line;
    LFO* _sin_osc;
    float _mod_freq;
    int _delay_width;
    int _mod_amp;
};


#endif /* Vibrato_h */
