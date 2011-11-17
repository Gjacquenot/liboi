/*
 * CRoutine_Normalize.h
 *
 *  Created on: Nov 17, 2011
 *      Author: bkloppenborg
 */

#ifndef CROUTINE_NORMALIZE_H_
#define CROUTINE_NORMALIZE_H_

#include "COpenCLRoutine.h"

class CRoutine_Normalize: public COpenCLRoutine
{
public:
	CRoutine_Normalize(cl_device_id device, cl_context context, cl_command_queue queue);
	virtual ~CRoutine_Normalize();

	void Init();

	void Normalize(cl_mem image, cl_mem divisor, int width, int height, int depth);
};

#endif /* CROUTINE_NORMALIZE_H_ */
