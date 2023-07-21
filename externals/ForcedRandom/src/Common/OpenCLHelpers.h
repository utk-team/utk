#ifndef OPENCLHELPERS_H
#define OPENCLHELPERS_H

#include <CL/cl.h>
#include <iostream>

#include "MathTypes.h"

namespace cl
{
    void CheckError(cl_int error);
    void CheckBuildStatus(cl_device_id device, cl_program program, cl_int error);
    cl_int DownloadBuffer(cl_command_queue queue, cl_mem buffer, int size, double* destination);
    cl_int DownloadBuffer(cl_command_queue queue, cl_mem buffer, int size, V2d* destination);
    cl_int DownloadBuffer(cl_command_queue queue, cl_mem buffer, int size, int* destination);
    cl_int DownloadBuffer(cl_command_queue queue, cl_mem buffer, int size, __int64* destination);
    V2d DownloadV2dValue(cl_command_queue queue, cl_mem buffer, int index);
    int DownloadIntValue(cl_command_queue queue, cl_mem buffer, int index);
    __int64 DownloadInt64Value(cl_command_queue queue, cl_mem buffer, int index);
    void MoveIntValue(cl_command_queue queue, cl_mem buffer, int oldIndex, int newIndex);
    cl_int RunKernel(cl_command_queue queue, cl_kernel kernel, int threads, int localSize);
    cl_int UploadValue(cl_command_queue, cl_mem buffer, int index, int value);
}

#endif