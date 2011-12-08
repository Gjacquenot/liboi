/*
 * CRoutine_FTtoT3.cpp
 *
 *  Created on: Dec 5, 2011
 *      Author: bkloppenborg
 */

#include "CRoutine_FTtoT3.h"
#include <cstdio>

CRoutine_FTtoT3::CRoutine_FTtoT3(cl_device_id device, cl_context context, cl_command_queue queue)
	:COpenCLRoutine(device, context, queue)
{
	// Specify the source location for the kernel.
	mSource.push_back("ft_to_t3.cl");
}

CRoutine_FTtoT3::~CRoutine_FTtoT3()
{
	// TODO Auto-generated destructor stub
}

void CRoutine_FTtoT3::Init(void)
{
	// Read the kernel, compile it
	string source = ReadSource(mSource[0]);
    BuildKernel(source, "ft_to_t3");
}

void CRoutine_FTtoT3::FTtoT3(cl_mem ft_loc, cl_mem data_phasor, cl_mem data_bsref, cl_mem data_sign, int n_t3, int n_v2, cl_mem output)
{
	int err = 0;
	size_t global = (size_t) n_t3;
	size_t local = 0;

	// Get the maximum work-group size for executing the kernel on the device
	err = clGetKernelWorkGroupInfo(mKernels[0], mDeviceID, CL_KERNEL_WORK_GROUP_SIZE , sizeof(size_t), &local, NULL);
	COpenCL::CheckOCLError("Failed to determine local size for ft_to_t3 kernel.", err);

	// Set kernel arguments:
	err  = clSetKernelArg(mKernels[0], 0, sizeof(cl_mem), &ft_loc);
	err |= clSetKernelArg(mKernels[0], 1, sizeof(cl_mem), &data_phasor);
	err |= clSetKernelArg(mKernels[0], 2, sizeof(cl_mem), &data_bsref);
	err |= clSetKernelArg(mKernels[0], 3, sizeof(cl_mem), &data_sign);
	err |= clSetKernelArg(mKernels[0], 4, sizeof(int), &n_v2);
	err |= clSetKernelArg(mKernels[0], 5, sizeof(cl_mem), &output);      // Output is stored on the GPU.
	COpenCL::CheckOCLError("Failed to set ft_to_t3 kernel arguments.", err);

	// Execute the kernel over the entire range of the data set
	err = clEnqueueNDRangeKernel(mQueue, mKernels[0], 1, NULL, &global, NULL, 0, NULL, NULL);
	COpenCL::CheckOCLError("Failed to set ft_to_t3 kernel arguments.", err);

#ifdef DEBUG_VERBOSE
	// Copy back the input/output buffers.
	cl_float2 * tmp = new cl_float2[n_t3];
	err = clEnqueueReadBuffer(mQueue, output, CL_TRUE, n_v2 * sizeof(cl_float), n_t3 * sizeof(cl_float2), tmp, 0, NULL, NULL);

	printf("T3 Buffer elements:\n");
	for(int i = 0; i < n_t3; i++)
		printf("(%f %f) ", tmp[i].s0, tmp[i].s1);

	// End the line, free memory.
	printf("\n");
	delete tmp;

#endif //DEBUG_VERBOSE

}

