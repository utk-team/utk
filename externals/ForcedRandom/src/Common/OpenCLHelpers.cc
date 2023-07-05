#include "OpenCLHelpers.h"

#include <vector>

namespace cl
{
    void CheckError(cl_int error)
    {
        if (error != CL_SUCCESS)
        {
            switch (error)
            {
                case CL_DEVICE_NOT_FOUND: std::cerr << "ERROR: " << "Device not found\n"; break;
                case CL_DEVICE_NOT_AVAILABLE: std::cerr << "ERROR: " << "Device not available\n"; break;
                case CL_COMPILER_NOT_AVAILABLE: std::cerr << "ERROR: " << "Compiler not available\n"; break;
                case CL_MEM_OBJECT_ALLOCATION_FAILURE: std::cerr << "ERROR: " << "Mem object allocation failure\n"; break;
                case CL_OUT_OF_RESOURCES: std::cerr << "ERROR: " << "Out of resources\n"; break;
                case CL_OUT_OF_HOST_MEMORY: std::cerr << "ERROR: " << "Out of host memory\n"; break;
                case CL_PROFILING_INFO_NOT_AVAILABLE: std::cerr << "ERROR: " << "Profiling info not available\n"; break;
                case CL_MEM_COPY_OVERLAP: std::cerr << "ERROR: " << "Mem copy overlap\n"; break;
                case CL_IMAGE_FORMAT_MISMATCH: std::cerr << "ERROR: " << "Image format mismatch\n"; break;
                case CL_IMAGE_FORMAT_NOT_SUPPORTED: std::cerr << "ERROR: " << "Image format not supported\n"; break;
                case CL_MAP_FAILURE: std::cerr << "ERROR: " << "Map failure\n"; break;
                case CL_INVALID_VALUE: std::cerr << "ERROR: " << "Invalid value\n"; break;
                case CL_INVALID_DEVICE_TYPE: std::cerr << "ERROR: " << "Invalid device type\n"; break;
                case CL_INVALID_PLATFORM: std::cerr << "ERROR: " << "Invalid platform\n"; break;
                case CL_INVALID_DEVICE: std::cerr << "ERROR: " << "Invalid device\n"; break;
                case CL_INVALID_CONTEXT: std::cerr << "ERROR: " << "Invalid context\n"; break;
                case CL_INVALID_QUEUE_PROPERTIES: std::cerr << "ERROR: " << "Invalid queue properties\n"; break;
                case CL_INVALID_COMMAND_QUEUE: std::cerr << "ERROR: " << "Invalid command queue\n"; break;
                case CL_INVALID_HOST_PTR: std::cerr << "ERROR: " << "Invalid host ptr\n"; break;
                case CL_INVALID_MEM_OBJECT: std::cerr << "ERROR: " << "Invalid mem object\n"; break;
                case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: std::cerr << "ERROR: " << "Invalid image format descriptor\n"; break;
                case CL_INVALID_IMAGE_SIZE: std::cerr << "ERROR: " << "Invalid image size\n"; break;
                case CL_INVALID_SAMPLER: std::cerr << "ERROR: " << "Invalid sampler\n"; break;
                case CL_INVALID_BINARY: std::cerr << "ERROR: " << "Invalid binary\n"; break;
                case CL_INVALID_BUILD_OPTIONS: std::cerr << "ERROR: " << "Invalid build options\n"; break;
                case CL_INVALID_PROGRAM: std::cerr << "ERROR: " << "Invalid program\n"; break;
                case CL_INVALID_PROGRAM_EXECUTABLE: std::cerr << "ERROR: " << "Invalid program executable\n"; break;
                case CL_INVALID_KERNEL_NAME: std::cerr << "ERROR: " << "Invalid kernel name\n"; break;
                case CL_INVALID_KERNEL_DEFINITION: std::cerr << "ERROR: " << "Invalid kernel definition\n"; break;
                case CL_INVALID_KERNEL: std::cerr << "ERROR: " << "Invalid kernel\n"; break;
                case CL_INVALID_ARG_INDEX: std::cerr << "ERROR: " << "Invalid arg index\n"; break;
                case CL_INVALID_ARG_VALUE: std::cerr << "ERROR: " << "Invalid arg value\n"; break;
                case CL_INVALID_ARG_SIZE: std::cerr << "ERROR: " << "Invalid arg size\n"; break;
                case CL_INVALID_KERNEL_ARGS: std::cerr << "ERROR: " << "Invalid kernel args\n"; break;
                case CL_INVALID_WORK_DIMENSION: std::cerr << "ERROR: " << "Invalid work dimension\n"; break;
                case CL_INVALID_WORK_GROUP_SIZE: std::cerr << "ERROR: " << "Invalid work group size\n"; break;
                case CL_INVALID_WORK_ITEM_SIZE: std::cerr << "ERROR: " << "Invalid work item size\n"; break;
                case CL_INVALID_GLOBAL_OFFSET: std::cerr << "ERROR: " << "Invalid global offset\n"; break;
                case CL_INVALID_EVENT_WAIT_LIST: std::cerr << "ERROR: " << "Invalid event wait list\n"; break;
                case CL_INVALID_EVENT: std::cerr << "ERROR: " << "Invalid event\n"; break;
                case CL_INVALID_OPERATION: std::cerr << "ERROR: " << "Invalid operation\n"; break;
                case CL_INVALID_GL_OBJECT: std::cerr << "ERROR: " << "Invalid GL object\n"; break;
                case CL_INVALID_BUFFER_SIZE: std::cerr << "ERROR: " << "Invalid buffer size\n"; break;
                case CL_INVALID_MIP_LEVEL: std::cerr << "ERROR: " << "Invalid mip level\n"; break;
                case CL_BUILD_PROGRAM_FAILURE: std::cerr << "ERROR: " << "Build program failure\n"; break;
                default: std::cerr << "ERROR: " << "Unknown\n";
            }

            exit(EXIT_FAILURE);
        }
    }

    void CheckBuildStatus(cl_device_id device, cl_program program, cl_int error)
    {
        size_t logSize;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);

		if (logSize > 2)
		{
			std::vector<char> log(logSize);
			clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, log.data(), nullptr);

			std::cout << std::endl << "Build log:" << std::endl;
			std::cout << log.data() << '\0' << std::endl;
		}

        CheckError(error);
    }

    cl_int DownloadBuffer(cl_command_queue queue, cl_mem buffer, int size, double* destination)
    {
        return clEnqueueReadBuffer(queue, buffer, CL_TRUE, 0, size * sizeof(double), destination, 0, nullptr, nullptr);
    }

    cl_int DownloadBuffer(cl_command_queue queue, cl_mem buffer, int size, V2d* destination)
    {
        return clEnqueueReadBuffer(queue, buffer, CL_TRUE, 0, size * sizeof(cl_double2), destination, 0, nullptr, nullptr);
    }

    cl_int DownloadBuffer(cl_command_queue queue, cl_mem buffer, int size, int* destination)
    {
        return clEnqueueReadBuffer(queue, buffer, CL_TRUE, 0, size * sizeof(int), destination, 0, nullptr, nullptr);
    }

    cl_int DownloadBuffer(cl_command_queue queue, cl_mem buffer, int size, __int64* destination)
    {
        return clEnqueueReadBuffer(queue, buffer, CL_TRUE, 0, size * sizeof(__int64), destination, 0, nullptr, nullptr);
    }

    V2d DownloadV2dValue(cl_command_queue queue, cl_mem buffer, int index)
    {
        V2d returnValue;

        clEnqueueReadBuffer(queue, buffer, CL_TRUE, index * sizeof(cl_double2), sizeof(cl_double2), &returnValue, 0, nullptr, nullptr);

        return returnValue;
    }

    int DownloadIntValue(cl_command_queue queue, cl_mem buffer, int index)
    {
        int returnValue;

        clEnqueueReadBuffer(queue, buffer, CL_TRUE, index * sizeof(int), sizeof(int), &returnValue, 0, nullptr, nullptr);

        return returnValue;
    }

    __int64 DownloadInt64Value(cl_command_queue queue, cl_mem buffer, int index)
    {
        __int64 returnValue;

        clEnqueueReadBuffer(queue, buffer, CL_TRUE, index * sizeof(__int64), sizeof(__int64), &returnValue, 0, nullptr, nullptr);

        return returnValue;
    }

    void MoveIntValue(cl_command_queue queue, cl_mem buffer, int oldIndex, int newIndex)
    {
        UploadValue(queue, buffer, newIndex, DownloadIntValue(queue, buffer, oldIndex));
    }

    cl_int RunKernel(cl_command_queue queue, cl_kernel kernel, int threads, int localSize)
    {
        size_t local = static_cast<size_t>(localSize);
        size_t global = static_cast<size_t>((threads % localSize == 0) ? threads : threads + localSize - threads % localSize);
        clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &global, &local, 0, nullptr, nullptr);

        return clFinish(queue);
    }

    cl_int UploadValue(cl_command_queue queue, cl_mem buffer, int index, int value)
    {
        return clEnqueueWriteBuffer(queue, buffer, CL_TRUE, index * sizeof(int), sizeof(int), &value, 0, nullptr, nullptr);
    }
}