#include "MUSI8903Config.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>

#include "UnitTest++.h"
#include "Synthesis.h"
#include "Vector.h"
#include "Vibrato.h"

SUITE(Vibrato_Test)
{
	struct VibratoData
	{
		VibratoData():
			pVibrato(0),
			inputData(0),
			outputData(0),
			dataLength(35131),
			maxDelayLength(3.F),
			blockLength(171),
			numChannels(3),
			sampleRate(8000),
			delay_width(.02F),
            mod_freq(10.F),
			mod_amp(.01F)
			
		{
			Vibrato::create(pVibrato, maxDelayLength, sampleRate, numChannels);
			
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
		float   maxDelayLength;
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
			for (int j = delay_width*sampleRate+1; j < dataLength; j++)
			{
				CHECK_CLOSE(inputData[i][j], outputData[i][j], 1e-3F);
			}
		}

	}

	TEST_FIXTURE(VibratoData, DC_input)
	{
		pVibrato->init(mod_freq, delay_width, mod_amp);

		//Generate DC input
		for (int i = 0; i < numChannels; i++)
		{
			for (int j = 0; j < dataLength; j++)
				inputData[i][j] = 0.5;
		}

		TestProcess();

		for (int i=0; i < numChannels; i++)
		{
			CHECK_ARRAY_CLOSE(inputData[i], outputData[i], dataLength, 1e-3);
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
			CHECK_ARRAY_CLOSE(outputData[i], outputData2[i], dataLength, 1e-3);
		}
	}
}

#endif //WITH_TESTS