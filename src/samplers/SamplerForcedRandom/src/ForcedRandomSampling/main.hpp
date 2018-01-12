#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
//#include <process.h>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

#include "../Common/MathHelpers.h"
#include "../Common/MathTypes.h"
#include "../Common/Random.h"

#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#endif

// Remove this definition to compile without the OpenCL code
//#define USEOPENCL

#ifdef USEOPENCL
#include "OpenCLHelpers.h"
#endif

#define MULTILINE(...) #__VA_ARGS__
#define w_I 128
#define h_I 128
#define LOCALSIZE 32

namespace forcedrandom
{
	
// Input parameters
int n;					// Desired number of samples
int sigma;				// Sample sparsity
std::vector<int> M;		// Dither matrix
int w_M;				// Dither matrix size
std::vector<double> I;	// Importance map
V2i s_I;				// Importance map size
V2d s_I_inv;			// 1 / s_I

// Setup results
V2i s_W;            // Dither matix window size
V2d s_W_inv;        // 1 / s_W
int w_B;            // Block width, i.e. number of window elements corresponding to each importance map element
double tau;         // Threshold scaling factor
V2i deltaW;         // Random dither matrix window offset
V2i blockDeltaW;    // Block offset
int blockW_M;		// Size of w_M in blocks

std::vector<V2i> L;	// Blockwise sorted list
int t;				// Number of importance map pixels/threads
int n_B;			// Maximum number of samples per block/thread

std::unique_ptr<Random> randomX;
std::unique_ptr<Random> randomY;

void LoadMatrix(std::string path)
{
    // Note: Only the first 2 * w_M * w_M / (sparsity * sparsity) elements of the matrix are used!
	std::streamsize dataSize = w_M * w_M * sizeof(int);
	M = std::vector<int>(w_M * w_M);

    std::ifstream inputStream(path.data(), std::ios::binary);

    if (!inputStream.is_open()) {
    	std::cout << "\nThe dither matrix " << path.data() << " cannot be opened.\n";

        exit(EXIT_FAILURE);
    }

    inputStream.read(reinterpret_cast<char*>(M.data()), dataSize);
    inputStream.close();
}

void GenerateImportanceMap()
{
	I = std::vector<double>(w_I * h_I);

	for (int y = 0; y < h_I; y++)
	{
		for (int x = 0; x < w_I; x++)
		{
			I[y * w_I + x] = 1; //std::pow(static_cast<double>(x + 1) / static_cast<double>(w_I), 3.0);
        }
    }
}

void Setup()
{
    randomX = std::unique_ptr<Random>(new Random((12 + 1337) << 16));		// Two different
    randomY = std::unique_ptr<Random>(new Random(static_cast<int>(time(nullptr))));	// random seeds

    t = s_I.x * s_I.y;
	s_I_inv = V2d(1.0 / s_I.x, 1.0 / s_I.y);
    double maxValue = MathHelpers::GetMax(I);
    double meanValue = MathHelpers::GetMean(I);

    // Determine which side of the importance map is the smaller one
    int smallS = std::min(s_I.x, s_I.y);
    int largeS = std::max(s_I.x, s_I.y);
    double aspectRatio = static_cast<double>(largeS) / static_cast<double>(smallS);

    // Calculate the minimum size the window has to have to provide enough dither values
    int minSizeForSamples = static_cast<int>(std::ceil(static_cast<double>(sigma) * std::sqrt(static_cast<double>(n) * maxValue / (meanValue * aspectRatio))));

    // Get next power-of-two multiple of the minimum window size
    int smallWindowSize = smallS * static_cast<int>(std::pow(2.0, std::max(0.0,
		std::ceil(MathHelpers::log2(static_cast<double>(minSizeForSamples) / static_cast<double>(smallS))))));
    int largeWindowSize = static_cast<int>(std::ceil(static_cast<double>(smallWindowSize) * aspectRatio));
    s_W = (s_I.x > s_I.y) ? V2i(largeWindowSize, smallWindowSize) : V2i(smallWindowSize, largeWindowSize);
	s_W_inv = V2d(1.0 / s_W.x, 1.0 / s_W.y);

	std::cout << "Window size: " << s_W.x << ", " << s_W.y << "\n";

	w_B = s_W.x / s_I.x;

	if (w_B > w_M)
    {
        std::cout << "\nThe dither matrix is too small to provide enough subpixel samples!\n";

        exit(EXIT_FAILURE);
    }

    V2d randomOffset = V2d(randomX->NextDouble(), randomY->NextDouble());
	V2d offsetScale = 1.0 / static_cast<double>(sigma) * randomOffset * V2d(static_cast<double>(w_M)) / static_cast<double>(w_B);
	deltaW = ToV2i(static_cast<double>(w_B) * offsetScale) * sigma;
	blockDeltaW = deltaW / w_B;
	tau = static_cast<double>(static_cast<__int64_t>(n) * static_cast<__int64_t>(w_M) * static_cast<__int64_t>(w_M)) / (static_cast<double>(s_W.x * s_W.y) * meanValue);
}

void CreateBlockwiseSortedList()
{
	// Creates the block-wise sorted list from the dither matrix. This is included
	// in this sample application for experimenting, but the list can and should be
	// pre-calculated for use in an actual application.

	n_B = (sigma <= w_B) ? std::min(w_B * w_B, 2 * (w_B * w_B / (sigma * sigma))) : w_B * w_B;

	blockW_M = w_M / w_B;
    int blocks = blockW_M * blockW_M;
	L = std::vector<V2i>(blocks * n_B);

    for (int blockId = 0; blockId < blocks; blockId++)
	{
		V2i blockOffset = V2i(blockId % blockW_M, blockId / blockW_M) * w_B; // Integer truncation intended
        std::vector<V2i> blockList;

        // Get all elements of the submatrix of M corresponding to the current w_B x w_B block
		for (int y = blockOffset.y; y < blockOffset.y + w_B; y++)
		{
			for (int x = blockOffset.x; x < blockOffset.x + w_B; x++)
			{
				int matrixElementIndex = y * w_M + x;
				int blockElementIndex = (y - blockOffset.y) * w_B + x - blockOffset.x;
                blockList.push_back(V2i(M[matrixElementIndex], blockElementIndex));
            }
        }

        // Add the first n_B sorted dither values of the submatrix
		std::sort(blockList.begin(), blockList.end(), [](const V2i& lhs, const V2i& rhs) { return (lhs.x < rhs.x); });
		std::copy(blockList.begin(), blockList.begin() + n_B, L.begin() + blockId * n_B);
    }
}

std::vector<V2d> GenerateSamples()
{
    std::vector<V2d> samples;
    samples.reserve(static_cast<int>(static_cast<double>(n) * 1.1));

    for (int y = 0; y < s_I.y; y++)     // These two loops
	{
        for (int x = 0; x < s_I.x; x++) // can be parallelized, but access to "samples" needs to be synchronized
		{
			V2d relativeCoords = V2d(x * s_I_inv.x, y * s_I_inv.y);

            int blockStartY = (blockDeltaW.y + y) % blockW_M;
            int blockStartX = (blockDeltaW.x + x) % blockW_M;
            int blockIndex = blockStartY * blockW_M + blockStartX;
			int blockStartIndex = blockIndex * n_B;

            double weight = I[y * s_I.x + x];
            int weightedThreshold = static_cast<int>(tau * weight + 0.5);

			for (int i = 0; i < n_B; i++)
			{
                V2i ditherSample = L[blockStartIndex + i];

                if (ditherSample.x < weightedThreshold)
				{
					int subY = ditherSample.y / w_B; // Integer truncation intended
					int subX = ditherSample.y - subY * w_B;
                    V2d deltaS = V2d(static_cast<double>(subX) + 0.5, static_cast<double>(subY) + 0.5); // This is the right place to apply jitter instead

					samples.push_back(relativeCoords + deltaS * s_W_inv);
                }
                else
				{
                    break;
                }
            }
        }
    }

    return std::move(samples);
}

void OutputSamplesAsTxt(std::vector<V2d> samples, std::string path)
{
    std::ofstream outputStream(path.data(), std::ofstream::binary);

    if (!outputStream.is_open()) {
    	std::cout << "\nThe output file " << path.data() << " cannot be opened.\n";

        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < samples.size(); i++)
	{
        outputStream << samples.at(i).x << "\t" << samples.at(i).y << std::endl;
    }

    outputStream.close();
}

void OutputSamplesAsImage(const std::vector<V2d>& samples, const std::string& path)
{
    int imageSize = 512;
    std::vector<int> image(imageSize * imageSize * 4);

    for (int i = 0; i < imageSize * imageSize; i++)
	{
        image[4 * i] = 255;
        image[4 * i + 1] = 255;
        image[4 * i + 2] = 255;
        image[4 * i + 3] = 255;
    }

    for (size_t i = 0; i < samples.size(); i++)
	{
        const V2d& sample = samples.at(i);
        int x = static_cast<int>(sample.x * static_cast<double>(imageSize - 1) + 0.5);
        int y = static_cast<int>(sample.y * static_cast<double>(imageSize - 1) + 0.5);

        image[4 * (y * imageSize + x)] = 0;
        image[4 * (y * imageSize + x) + 1] = 0;
        image[4 * (y * imageSize + x) + 2] = 0;
    }

    std::ofstream outputStream(path.data(), std::ofstream::binary);

    if (!outputStream.is_open())
	{
    	std::cout << "\nThe output file " << path.data() << " cannot be opened.\n";

        exit(EXIT_FAILURE);
    }

	// Write tga header
	outputStream << (char) 0 << (char) 0 << (char) 2 << (char) 0 << (char) 0 << (char) 0 << (char) 0 << (char) 0;
   	outputStream << (char) 0 << (char) 0 << (char) 0 << (char) 0 << (char) (imageSize & 0x00FF) << (char) ((imageSize & 0xFF00) / 256);
   	outputStream << (char) (imageSize & 0x00FF) << (char) ((imageSize & 0xFF00) / 256) << (char) 32 << (char) 0;

	// Write image data
	for (int i = 0; i < imageSize * imageSize; i++)
	{
		outputStream << (char) image[4 * i] << (char) image[4 * i + 1] << (char) image[4 * i + 2] << (char) image[4 * i + 3];
	}

	outputStream.close();
}

#ifdef USEOPENCL

cl_int error;
cl_device_id device;
cl_platform_id platform;
cl_context context;
cl_command_queue queue;

cl_mem syncBuffer;
cl_mem samplesBuffer;
cl_mem importanceMapBuffer;
cl_mem sortedListBuffer;

cl_program forcedRandomSamplingProgram;
cl_kernel forcedRandomSamplingKernel;

const char* ForcedRandomSamplingKernel = MULTILINE(
    #pragma OPENCL EXTENSION cl_khr_fp64 : enable\n

    __kernel void ForcedRandomSampling(
        const int t,
        __global double2* samplesBuffer,
        __global int* syncBuffer,
        const __global int2* L,
        const __global double* I,
        const int2 s_I,
		const double2 s_I_inv,
        const int blockW_M,
		const double2 s_W_inv,
        const int2 blockDeltaW,
		const int w_B,
		const int n_B,
        const double tau)
    {
        int threadId = get_global_id(0);

        if (threadId < t)
        {
            int y = threadId / s_I.x; // Integer truncation intended
            int x = threadId - y * s_I.x;

			double2 relativeCoords = (double2) (x * s_I_inv.x, y * s_I_inv.y);

            int blockStartY = (blockDeltaW.y + y) % blockW_M;
            int blockStartX = (blockDeltaW.x + x) % blockW_M;
			int blockStartIndex = (blockStartY * blockW_M + blockStartX) * n_B;

            int weightedThreshold = (int) (tau * I[threadId] + 0.5);

			for (int i = 0; i < n_B; i++)
            {
                int2 ditherSample = L[blockStartIndex + i];

                if (ditherSample.x < weightedThreshold)
                {
					int subY = ditherSample.y / w_B; // Integer truncation intended
					int subX = ditherSample.y - subY * w_B;
					double2 subPixelOffset = s_W_inv * (double2) (subX + 0.5, subY + 0.5); // This is the right place to apply jitter instead

                    samplesBuffer[atomic_inc(syncBuffer)] = (double2) (relativeCoords + subPixelOffset);
                }
                else
                {
                    return;
                }
            }
        }
    }
);

void SetupOpenCL()
{
    cl::CheckError(clGetPlatformIDs(1, &platform, nullptr));
    cl::CheckError(clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr));

    context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &error);
    cl::CheckError(error);

    queue = clCreateCommandQueue(context, device, 0, &error);
    cl::CheckError(error);

    syncBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int), nullptr, &error);
    cl::CheckError(error);

	samplesBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, t * n_B * sizeof(cl_double2), nullptr, &error);
    cl::CheckError(error);

    importanceMapBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, t * sizeof(double), nullptr, &error);
    cl::CheckError(error);
    cl::CheckError(clEnqueueWriteBuffer(queue, importanceMapBuffer, CL_TRUE, 0, t * sizeof(double), I.data(), 0, nullptr, nullptr));

	sortedListBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, blockW_M * blockW_M * n_B * sizeof(cl_int2), nullptr, &error);

    cl::CheckError(error);
	cl::CheckError(clEnqueueWriteBuffer(queue, sortedListBuffer, CL_TRUE, 0, blockW_M * blockW_M * n_B * sizeof(cl_int2), L.data(), 0, nullptr, nullptr));

    forcedRandomSamplingProgram = clCreateProgramWithSource(context, 1, &ForcedRandomSamplingKernel, nullptr, &error);
    cl::CheckError(error);
    cl::CheckError(clBuildProgram(forcedRandomSamplingProgram, 0, nullptr, nullptr, nullptr, nullptr));
    forcedRandomSamplingKernel = clCreateKernel(forcedRandomSamplingProgram, "ForcedRandomSampling", &error);
    cl::CheckError(error);
    cl::CheckError(clSetKernelArg(forcedRandomSamplingKernel, 0, sizeof(int), &t));
    cl::CheckError(clSetKernelArg(forcedRandomSamplingKernel, 1, sizeof(cl_double2*), &samplesBuffer));
    cl::CheckError(clSetKernelArg(forcedRandomSamplingKernel, 2, sizeof(int*), &syncBuffer));
    cl::CheckError(clSetKernelArg(forcedRandomSamplingKernel, 3, sizeof(cl_int2*), &sortedListBuffer));
    cl::CheckError(clSetKernelArg(forcedRandomSamplingKernel, 4, sizeof(double*), &importanceMapBuffer));
    cl::CheckError(clSetKernelArg(forcedRandomSamplingKernel, 5, sizeof(cl_int2), &s_I));
	cl::CheckError(clSetKernelArg(forcedRandomSamplingKernel, 6, sizeof(cl_double2), &s_I_inv));
    cl::CheckError(clSetKernelArg(forcedRandomSamplingKernel, 7, sizeof(int), &blockW_M));
	cl::CheckError(clSetKernelArg(forcedRandomSamplingKernel, 8, sizeof(cl_double2), &s_W_inv));
    cl::CheckError(clSetKernelArg(forcedRandomSamplingKernel, 9, sizeof(cl_int2), &blockDeltaW));
	cl::CheckError(clSetKernelArg(forcedRandomSamplingKernel, 10, sizeof(int), &w_B));
	cl::CheckError(clSetKernelArg(forcedRandomSamplingKernel, 11, sizeof(int), &n_B));
    cl::CheckError(clSetKernelArg(forcedRandomSamplingKernel, 12, sizeof(double), &tau));
}

std::vector<V2d> GenerateSamplesOpenCL()
{
    cl::RunKernel(queue, forcedRandomSamplingKernel, t, LOCALSIZE);

    int samplesCount = cl::DownloadIntValue(queue, syncBuffer, 0);
    std::vector<V2d> samples(samplesCount);
    cl::DownloadBuffer(queue, samplesBuffer, samplesCount, samples.data());

    return std::move(samples);
}

void CleanupOpenCL()
{
    clReleaseMemObject(syncBuffer);
    clReleaseMemObject(samplesBuffer);
    clReleaseMemObject(importanceMapBuffer);
    clReleaseMemObject(sortedListBuffer);

    clReleaseProgram(forcedRandomSamplingProgram);
    clReleaseKernel(forcedRandomSamplingKernel);

    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}

#endif

}
