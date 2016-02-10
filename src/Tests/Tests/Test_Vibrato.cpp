#include "MUSI8903Config.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>
#include <iostream>

#include "UnitTest++.h"
#include "Synthesis.h"
#include "Vector.h"
#include "Vibrato.h"

using namespace std;

SUITE(Vibrato_Test)
{
	struct VibratoData
	{
		VibratoData():
			pVibrato(0),
			inputData(0),
			outputData(0),
			dataLength(35131),
			//maxDelayLength(3.F),
			blockLength(171),
			numChannels(3),
			sampleRate(8000),
			delay_width(.002F),
            mod_freq(13.F),
			mod_amp(.002F)
			
		{
			Vibrato::create(pVibrato, sampleRate, numChannels);
			
			inputData = new float*[numChannels];
			outputData = new float*[numChannels];
			inputTmp = new float*[numChannels];
			outputTmp = new float*[numChannels];
			for (int i = 0;i < numChannels; i++)
			{
				inputData[i] = new float[dataLength];
				outputData[i] = new float[dataLength];
				CVectorFloat::setZero(inputData[i], dataLength);
				CVectorFloat::setZero(outputData[i], dataLength);
			}
		}

		~VibratoData()
		{
			for (int i = 0;i < numChannels; i++)
			{
				delete[] inputData[i];
				delete[] outputData[i];
				//delete[] inputTmp[i];
				//delete[] outputTmp[i];
			}
			delete[] inputData;
			delete[] outputData;
			delete[] inputTmp;
			delete[] outputTmp;

			Vibrato::destroy(pVibrato);
		}

		void TestProcess()
		{
			int numFramesRemaining = dataLength;
			while (numFramesRemaining > 0)
			{
				int numFrames = std::min(numFramesRemaining, blockLength);
				for (int i = 0; i < numChannels; i++)
				{
					inputTmp[i] = &inputData[i][dataLength - numFramesRemaining];
					outputTmp[i] = &outputData[i][dataLength - numFramesRemaining];
				}
				pVibrato->process(inputTmp, outputTmp, numFrames);
				numFramesRemaining -= numFrames;
			}
		}

		Vibrato *pVibrato;
		float   **inputData,
                **outputData,
                **inputTmp,
                **outputTmp;
		int     dataLength;
		//float   maxDelayLength;
		int     blockLength;
		int     numChannels;
		float   sampleRate;
		float   delay_width,
				mod_freq,
                mod_amp;
	};

	TEST_FIXTURE(VibratoData, zero_mod_amp)
	{
		//Set modulation amplitude to 0
		pVibrato->init(mod_freq, delay_width, 0.F); 
		
		//Generate arbitrary sine wave
		for (int c = 0; c < numChannels; c++)
			CSynthesis::generateSine(inputData[c], 50, sampleRate, dataLength, .8F, static_cast<float>(c*M_PI_2));

		TestProcess();

		for (int i = 0; i < numChannels; i++)
		{
			for (int j = delay_width*sampleRate; j < dataLength; j++)
			{
				CHECK_CLOSE(inputData[i][j-(int)(delay_width*sampleRate)], outputData[i][j], 1e-3F);
			}
		}
	}

	TEST_FIXTURE(VibratoData, DC_input)
	{
		pVibrato->init(mod_freq, delay_width, mod_amp);

		//Generate DC input
        float dc_value = 0.5;
		for (int i = 0; i < numChannels; i++)
		{
			for (int j = 0; j < dataLength; j++)
				inputData[i][j] = dc_value;
		}

		TestProcess();

		for (int i=0; i < numChannels; i++)
		{
            for (int j = (delay_width + mod_amp)*sampleRate; j < dataLength; j++) {
                CHECK_EQUAL(dc_value, outputData[i][j]);
            }
		}
	}

	TEST_FIXTURE(VibratoData, varying_block_size)
	{
		float **outputData2;
		pVibrato->init(mod_freq, delay_width, mod_amp);

		//Generate arbitrary sine wave
		for (int c = 0; c < numChannels; c++)
			CSynthesis::generateSine(inputData[c], 50, sampleRate, dataLength, .8F, static_cast<float>(c*M_PI_2));

		TestProcess();

        pVibrato->init(mod_freq, delay_width, mod_amp);
		outputData2 = new float*[numChannels];
		for (int i = 0; i < numChannels; i++)
		{
			outputData2[i] = new float[dataLength];
		}

		//Processing again with new block Length = 366;
		blockLength = 366;
		int numFramesRemaining = dataLength;
		while (numFramesRemaining > 0)
		{
			int numFrames = std::min(numFramesRemaining, blockLength);
			for (int i = 0; i < numChannels; i++)
			{
				inputTmp[i] = &inputData[i][dataLength - numFramesRemaining];
				outputTmp[i] = &outputData2[i][dataLength - numFramesRemaining];
			}
			pVibrato->process(inputTmp, outputTmp, numFrames);
			numFramesRemaining -= numFrames;
		}
		for (int i=0; i < numChannels; i++)
		{
            for (int j = 0; j < dataLength; j++)
            {
                CHECK_CLOSE(outputData2[i][j], outputData[i][j], 1e-3F);
            }
		}
        
        for (int i = 0;i < numChannels; i++)
        {
            delete[] outputData2[i];
        }
        delete[] outputData2;
	}

	TEST_FIXTURE(VibratoData, zero_input)
	{
		pVibrato->init(mod_freq, delay_width, mod_amp);

		//Generate zero input
		for (int i = 0; i < numChannels; i++)
		{
			for (int j = 0; j < dataLength; j++)
				inputData[i][j] = 0.F;
		}

		TestProcess();

		for (int i = 0; i < numChannels; i++)
		{
			for (int j = (delay_width + mod_amp)*sampleRate; j < dataLength; j++) {
				CHECK_EQUAL(0, outputData[i][j]);
			}
		}
	}

	TEST_FIXTURE(VibratoData, zero_mod_freq)
	{
		//Set modulation amplitude to 0
		pVibrato->init(0.F, delay_width, mod_amp);

		//Generate arbitrary sine wave
		for (int c = 0; c < numChannels; c++)
			CSynthesis::generateSine(inputData[c], 50, sampleRate, dataLength, .8F, static_cast<float>(c*M_PI_2));

		TestProcess();

		for (int i = 0; i < numChannels; i++)
		{
			for (int j = delay_width*sampleRate; j < dataLength; j++)
			{
				CHECK_CLOSE(inputData[i][j - (int)(delay_width*sampleRate)], outputData[i][j], 1e-3F);
			}
		}
	}
}

#endif //WITH_TESTS