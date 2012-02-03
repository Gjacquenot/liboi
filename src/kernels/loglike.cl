__kernel void loglike(
    __global float * data,
    __global float * data_err,
    __global float * mock_data,
    __global float * output,
    __private int n)
{
    int i = get_global_id(0);
    float temp = 0;
    
   	// Required since we may swap out data sets and they may have different sizes
    if(i < n)
	{
		temp = (data[i] - mock_data[i]) / data_err[i];
	    temp = -2 * native_log(data_err[i]) - temp * temp;
    }
	else
		temp = 0;
    
    output[i] = temp;
}