/*
 * CRoutine_Chi_test.cpp
 *
 *  Created on: Nov 27, 2012
 *      Author: bkloppen
 */

#include "gtest/gtest.h"
#include "liboi_tests.h"
#include "COpenCL.h"
#include "CRoutine_Chi.h"
#include "CModel.h"

using namespace std;

extern string LIBOI_KERNEL_PATH;

/// Checks that the CPU chi algorithm evaluates to zero
/// when data and model are equal.
TEST(CRoutine_Chi, CPU_Chi_ZERO)
{
	unsigned int test_size = 10000;
	// Create buffers
	valarray<cl_float> data(test_size);
	valarray<cl_float> data_err(test_size);
	valarray<cl_float> model(test_size);
	valarray<cl_float> output(test_size);
	cl_float value = 0;

	//
	// # Test 1: Chi evaluates to zero.
	//

	// Init, chi should evaluate to zero
	for(int i = 0; i < test_size; i++)
	{
		value = i+1;
		data[i] = value;
		data_err[i] = 0.1 * value;
		model[i] = value;
	}

	// Run the chi algorithm
	CRoutine_Chi::Chi(data, data_err, model, 0, test_size, output);

	// Compare results. Because data = model every chi element should be zero
	for(int i = 0; i < test_size; i++)
		EXPECT_EQ(output[i], 0);
}

/// Tests that the CPU Chi algorithm evaluates to one when data and model differ
/// by one standard deviation.
TEST(CRoutine_Chi, CPU_Chi_ONE)
{
	unsigned int test_size = 10000;
	// Create buffers
	valarray<cl_float> data(test_size);
	valarray<cl_float> data_err(test_size);
	valarray<cl_float> model(test_size);
	valarray<cl_float> output(test_size);
	cl_float value = 0;

	// Offset the model by one standard deviation.
	for(int i = 0; i < test_size; i++)
	{
		value = i+1;
		data[i] = value;
		data_err[i] = 0.1 * value;
		model[i] = value + data_err[i];
	}

	// Run the chi algorithm
	CRoutine_Chi::Chi(data, data_err, model, 0, test_size, output);

	// Compare results. Because data = model every chi element should be of unit magnitude
	for(int i = 0; i < test_size; i++)
		EXPECT_NEAR(fabs(output[i]), 1.0, MAX_REL_ERROR);

}

/// Tests the CPU implementation of the convex chi approximation
TEST(CRoutine_Chi, CPU_Chi_convex_ZERO)
{
	unsigned int test_size = 10000;

	unsigned int n = 2*test_size;
	// Create buffers
	valarray<cl_float> data(n);
	valarray<cl_float> data_err(n);
	valarray<cl_float> model(n);
	valarray<cl_float> output(n);

	valarray<cl_float2> temp = CModel::GenerateUVSpiral_CL(test_size);

	// Set data = model to produce a zero chi result.
	for(int i = 0; i < test_size; i++)
	{
		data[i] = temp[i].s0;
		data[test_size + i] = temp[i].s1;

		model[i] = temp[i].s0;
		model[test_size + i] = temp[i].s1;

		// 1% error on amplitudes, 10% error on phases
		data_err[i] = 0.01 * data[i];
		data_err[test_size + i] = 0.1 * data[i];
	}

	// Run the chi algorithm
	CRoutine_Chi::Chi_complex_convex(data, data_err, model, 0, test_size, output);

	// Compare results. Because data = model every chi element should be of unit magnitude
	for(int i = 0; i < test_size; i++)
	{
		// Check the real and imaginary chi values
		EXPECT_NEAR(fabs(output[i]), 0, MAX_REL_ERROR);
		EXPECT_NEAR(fabs(output[test_size + i]), 0, MAX_REL_ERROR);
	}
}

/// Tests the CPU implementation of the convex chi approximation
TEST(CRoutine_Chi, CPU_Chi_nonconvex_ZERO)
{
	unsigned int test_size = 10000;

	unsigned int n = 2*test_size;
	// Create buffers
	valarray<cl_float> data(n);
	valarray<cl_float> data_err(n);
	valarray<cl_float> model(n);
	valarray<cl_float> output(n);

	valarray<cl_float2> t_data = CModel::GenerateUVSpiral_CL(test_size);

	cl_float amp_err = 0.01;	// 1% error on amplitudes
	cl_float phi_err = 0.1;		// 10% error on phases.
	// Generate test data.
	for(int i = 0; i < test_size; i++)
	{
		complex<float> c_data(t_data[i].s0, t_data[i].s1);

		// Data are stored as amplitude and phase.
		data[i] = abs(c_data);
		data[test_size + i] = arg(c_data);

		model[i] = abs(c_data);
		model[test_size + i] = arg(c_data);

		// Set the errors to something non-zero.
		data_err[i] = amp_err;
		data_err[test_size + i] = phi_err;
	}

	// Run the chi algorithm
	CRoutine_Chi::Chi_complex_nonconvex(data, data_err, model, 0, test_size, output);

	// Compare results. Because data = model every chi element should be of unit magnitude
	for(int i = 0; i < test_size; i++)
	{
		// Check the real and imaginary chi values
		EXPECT_EQ(fabs(output[i]), 0);
		EXPECT_EQ(fabs(output[test_size + i]), 0);
	}
}

/// Tests the CPU implementation of the convex chi approximation
TEST(CRoutine_Chi, CPU_Chi_nonconvex_ONE)
{
	unsigned int test_size = 10000;

	unsigned int n = 2*test_size;
	// Create buffers
	valarray<cl_float> data(n);
	valarray<cl_float> data_err(n);
	valarray<cl_float> model(n);
	valarray<cl_float> output(n);

	valarray<cl_float2> t_data = CModel::GenerateUVSpiral_CL(test_size);

	cl_float amp_err = 0.01;	// 1% error on amplitudes
	cl_float phi_err = 0.1;		// 10% error on phases.
	// Generate test data.
	for(int i = 0; i < test_size; i++)
	{
		complex<float> c_data(t_data[i].s0, t_data[i].s1);

		// Data are stored as amplitude and phase.
		data[i] = abs(c_data);
		data[test_size + i] = arg(c_data);

		model[i] = abs(c_data) * (1 + amp_err);
		model[test_size + i] = arg(c_data) * (1 + phi_err);

		// Set the errors, avoid zero error
		data_err[i] = max(fabs(amp_err * data[i]), amp_err);
		data_err[test_size + i] = max(fabs(phi_err * data[test_size + i]), phi_err);
	}

	// Run the chi algorithm
	CRoutine_Chi::Chi_complex_nonconvex(data, data_err, model, 0, test_size, output);

	// Compare results. Because data = model every chi element should be of unit magnitude
	for(int i = 0; i < test_size; i++)
	{
		// Check the real and imaginary chi values
		EXPECT_LT(fabs(output[i]), 1 + amp_err) << "Amp error exceeded.";
		EXPECT_LT(fabs(output[test_size + i]), 1 + phi_err) << "Phase error exceeded.";
	}
}