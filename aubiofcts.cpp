#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <aubio/aubio.h>

#include "aubiofcts.hpp"

/*
 * FROM : https://aubio.org/doc/latest/tempo_2test-tempo_8c-example.html
 */

float getBPMFromFile(std::string songPath)
{
	//uint_t win_size = 10240;	// 10ko
	uint_t win_size = 1024;	// 1ko
	uint_t hop_size = win_size / 4;

	aubio_source_t* aubiosource = new_aubio_source((char*)songPath.c_str(), 0, hop_size);
		if(!aubiosource) { std::cerr << "Problem with song at : " << songPath << std::endl; return -1; }
	uint_t samplerate = aubio_source_get_samplerate(aubiosource);
	uint_t nFrames = 0, read = 0;

	fvec_t* in = new_fvec(hop_size);	// Input buffer
	fvec_t* out = new_fvec(1);		// Output buffer

	aubio_tempo_t* aubiotempo = new_aubio_tempo((char*)"default", win_size, hop_size, samplerate);

	std::vector<smpl_t> allBPM;
	float bpm = 0;

	do {
	// Getting new data
	aubio_source_do(aubiosource, in, &read);
	// Execute tempo
	aubio_tempo_do(aubiotempo, in, out);
	if(out->data[0] != 0) {
		// Getting the tempo -> allBPM (vector array)
		allBPM.push_back(float(aubio_tempo_get_bpm(aubiotempo)));
	}

	nFrames += read;
	} while (read == hop_size);

	del_aubio_tempo(aubiotempo);
	del_fvec(in);
	del_fvec(out);
	del_aubio_source(aubiosource);
	aubio_cleanup();

	for(std::vector<smpl_t>::iterator it = allBPM.begin(); it != allBPM.end(); it++)
	{
		bpm += *it;
	}
	bpm /= allBPM.size();

	return bpm;
}
