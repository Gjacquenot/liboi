/*
 * COILibData.h
 *
 *  Created on: Nov 14, 2011
 *      Author: bkloppenborg
 *
 * A class for reading in and storing data in a convenient format for liboi operations.
 * NOTE: Right now this class is compatible with only Optical Interferometric Data.
 * It should be generalized if radio interferometry routines are implmenented in liboi.
 */

#ifndef COILIBDATA_H_
#define COILIBDATA_H_

#if defined(__APPLE__) || defined(__MACOSX)
	#include <OpenCL/cl.hpp>
#else
	#include <CL/cl.hpp>
#endif

// TODO: This class should be able to manipulate the order of the data BEFORE uploading
// to the OpenCL device to promote coalesced memory loads.

#include <string>

#include "COpenCL.h"
extern "C" {
    #include "exchange.h"
    #include "oifile.h"
	#include "getoifits.h"
}

using namespace std;

class COILibData
{
private:
	// Location and size of data loaded into OpenCL memory objects.
	int n_uv;
	int n_v2;
	cl_mem v2_loc;
	cl_mem v2_uv;
	int n_t3;
	cl_mem t3_loc;
	cl_mem t3_phasor;
	cl_mem t3_uv;

	// Storage containers for OIFITS data.
	oi_data * mData;

public:
	COILibData(oi_data * data);
	~COILibData();

	void CopyToOpenCLDevice(cl_command_queue * queue);

	cl_mem GetLocation_V2();
	cl_mem GetLocation_V2UV();
	cl_mem GetLocation_T3();
	cl_mem GetLocation_T3Phase();
	cl_mem GetLocation_T3UV();

	int GetNumV2();
	int GetNumT3();
	int GetNumUV();
};

#endif /* COILIBDATA_H_ */
