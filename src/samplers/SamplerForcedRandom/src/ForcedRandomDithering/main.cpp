#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <math.h>
#include <memory>
#include <numeric>
#include <sstream>
#include <vector>

#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
#endif

#include "KernelCodes.h"
#include "OpenCLHelpers.h"
#include "Random.h"

#define LOCALSIZE 32

cl_int error;
cl_device_id device;
cl_platform_id platform;
cl_context context;
cl_command_queue queue;

cl_mem forceFieldBuffer;
cl_mem forceFieldFunctionBuffer;
cl_mem freeLocationsBuffer;
cl_mem maskBuffer;
cl_mem candidatesBuffer;
cl_mem randomSeedBuffer;
cl_mem syncBuffer;

cl_program getForceMinimumProgram;
cl_kernel getForceMinimumKernel;
cl_program getForceCountProgram;
cl_kernel getForceCountKernel;
cl_program addForceFunctionProgram;
cl_kernel addForceFunctionKernel;

void SetupOpenCL(int n)
{
    cl::CheckError(clGetPlatformIDs(1, &platform, nullptr));
    cl::CheckError(clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr));

    context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &error);
    cl::CheckError(error);

    queue = clCreateCommandQueue(context, device, 0, &error);
    cl::CheckError(error);
    
    forceFieldBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, n * n * sizeof(int), nullptr, &error);
    cl::CheckError(error);
    forceFieldFunctionBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, n * n * sizeof(int), nullptr, &error);
    cl::CheckError(error);
    freeLocationsBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, n * n * sizeof(int), nullptr, &error);
    cl::CheckError(error);
    maskBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, n * n * sizeof(int), nullptr, &error);
    cl::CheckError(error);
    candidatesBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, n * n * sizeof(int), nullptr, &error);
    cl::CheckError(error);
    randomSeedBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, n * n * sizeof(int), nullptr, &error);
    cl::CheckError(error);
    syncBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, 2 * sizeof(int), nullptr, &error);
    cl::CheckError(error);

    getForceMinimumProgram = clCreateProgramWithSource(context, 1, &KernelCodes::GetForceMinimumKernel, nullptr, &error);
    cl::CheckError(error);
    cl::CheckError(clBuildProgram(getForceMinimumProgram, 0, nullptr, nullptr, nullptr, nullptr)); 
    getForceMinimumKernel = clCreateKernel(getForceMinimumProgram, "GetForceMinimum", &error);
    cl::CheckError(error);
    cl::CheckError(clSetKernelArg(getForceMinimumKernel, 1, sizeof(int*), &syncBuffer));
    cl::CheckError(clSetKernelArg(getForceMinimumKernel, 2, sizeof(int*), &maskBuffer));
    cl::CheckError(clSetKernelArg(getForceMinimumKernel, 3, sizeof(int*), &randomSeedBuffer));
    cl::CheckError(clSetKernelArg(getForceMinimumKernel, 4, sizeof(int*), &forceFieldBuffer));
    cl::CheckError(clSetKernelArg(getForceMinimumKernel, 5, sizeof(int*), &freeLocationsBuffer));

    getForceCountProgram = clCreateProgramWithSource(context, 1, &KernelCodes::GetForceCountKernel, nullptr, &error);
    cl::CheckError(error);
    cl::CheckError(clBuildProgram(getForceCountProgram, 0, nullptr, nullptr, nullptr, nullptr)); 
    getForceCountKernel = clCreateKernel(getForceCountProgram, "GetForceCount", &error);
    cl::CheckError(error);
    cl::CheckError(clSetKernelArg(getForceCountKernel, 1, sizeof(int*), &syncBuffer));
    cl::CheckError(clSetKernelArg(getForceCountKernel, 2, sizeof(int*), &maskBuffer));
    cl::CheckError(clSetKernelArg(getForceCountKernel, 3, sizeof(int*), &candidatesBuffer));
    cl::CheckError(clSetKernelArg(getForceCountKernel, 4, sizeof(int*), &forceFieldBuffer));
    cl::CheckError(clSetKernelArg(getForceCountKernel, 5, sizeof(int*), &freeLocationsBuffer));

    addForceFunctionProgram = clCreateProgramWithSource(context, 1, &KernelCodes::AddForceFunctionKernel, nullptr, &error);
    cl::CheckError(error);
    cl::CheckError(clBuildProgram(addForceFunctionProgram, 0, nullptr, nullptr, nullptr, nullptr)); 
    addForceFunctionKernel = clCreateKernel(addForceFunctionProgram, "AddForceFunction", &error);
    cl::CheckError(error);
    cl::CheckError(clSetKernelArg(addForceFunctionKernel, 1, sizeof(int*), &forceFieldFunctionBuffer));
    cl::CheckError(clSetKernelArg(addForceFunctionKernel, 2, sizeof(int*), &forceFieldBuffer));
}

double ForceFunction(double radius)
{
    return std::exp(-std::sqrt(2.0 * radius));
}

void PrecalculateForceFunction(int n, std::vector<int>& forceFieldFunction)
{
    // The integral over the whole force field is smaller than 20. The force field function is therefore
    // scaled such that the sum can be represented in one int and its influence ranges over half the matrix.
    double scale = std::min(static_cast<double>(INT_MAX) / 20.0, 1.0 / ForceFunction(0.5 * n));

    for (int deltaY = 0; deltaY < n; deltaY++)
    {
        for (int deltaX = 0; deltaX < n; deltaX++)
        {
            int minDeltaX = std::min(deltaX, n - deltaX);
            int minDeltaY = std::min(deltaY, n - deltaY);
            int r2 = minDeltaX * minDeltaX + minDeltaY * minDeltaY;

            forceFieldFunction[deltaY * n + deltaX] = static_cast<int>(scale * ForceFunction(std::sqrt((double) r2)));
        }
    }
}

std::vector<int> CreateDitherMatrix(int n)
{
    int n2 = n * n;
    int elements = n2;
    std::vector<int> ditherMatrix(n2, INT_MAX);
    
    clSetKernelArg(addForceFunctionKernel, 0, sizeof(int), &n2);
    clSetKernelArg(addForceFunctionKernel, 4, sizeof(int), &n);

    // Precalculate all possible squared distances between any two locations
    // to avoid double-precision calculations at runtime
    std::vector<int> forceFieldFunction(n2);
    PrecalculateForceFunction(n, forceFieldFunction);
    clEnqueueWriteBuffer(queue, forceFieldFunctionBuffer, CL_TRUE, 0, n2 * sizeof(int), forceFieldFunction.data(), 0, nullptr, nullptr);

    // Initialize the free locations buffer with all locations
    std::vector<int> freeLocations(n2);
    std::iota(freeLocations.begin(), freeLocations.end(), 0);
    clEnqueueWriteBuffer(queue, freeLocationsBuffer, CL_TRUE, 0, n2 * sizeof(int), freeLocations.data(), 0, nullptr, nullptr);

    // Initialize the RNG and the random seed buffer
    Random random(static_cast<int>(time(nullptr)));
    std::vector<int> randomSeeds(n2);
    randomSeeds[0] = random.NextInt();

    for (int i = 1; i < n2; i++)
    {
        randomSeeds[i] = random.RandomizeSeed(randomSeeds[0] + i);
    }

    clEnqueueWriteBuffer(queue, randomSeedBuffer, CL_TRUE, 0, n2 * sizeof(int), randomSeeds.data(), 0, nullptr, nullptr);

    // Values used to initialize the synchronization buffer: { minimumForceValue, numberOfCellsWithMinimumForce }
    int initialSyncValues[] = { INT_MAX, 0 };

    int progress = 0;
    int ditherValue = 0;

    while (ditherValue < n2)
    {
        if (elements == 1)
        {
            ditherMatrix[freeLocations[0]] = ditherValue;

            break;
        }

        // (Re)set the synchronization buffer
        clEnqueueWriteBuffer(queue, syncBuffer, CL_TRUE, 0, 2 * sizeof(int), initialSyncValues, 0, nullptr, nullptr);

        // Randomly select approximately half of all free locations and determine the minimum force value among them
        clSetKernelArg(getForceMinimumKernel, 0, sizeof(int), &elements);
        cl::RunKernel(queue, getForceMinimumKernel, elements, LOCALSIZE);

        // Get the number of all locations with the minimum force value
        clSetKernelArg(getForceCountKernel, 0, sizeof(int), &elements);
        cl::RunKernel(queue, getForceCountKernel, elements, LOCALSIZE);

        // Read back the count and select one location randomly out of all candidates
        int locationsWithIdenticalForce = cl::DownloadIntValue(queue, syncBuffer, 1);
        int randomSelection = random.NextInt(locationsWithIdenticalForce);
        int minimumBufferIndex = cl::DownloadIntValue(queue, candidatesBuffer, randomSelection);
        int minimumLocationIndex = freeLocations[minimumBufferIndex];

        ditherMatrix[minimumLocationIndex] = ditherValue;
        ditherValue++;

        // Remove the chosen location from the buffer
        freeLocations[minimumBufferIndex] = freeLocations[elements - 1];
        cl::MoveIntValue(queue, freeLocationsBuffer, elements - 1, minimumBufferIndex);
        elements--;

        // Update the force field
        clSetKernelArg(addForceFunctionKernel, 3, sizeof(int), &minimumLocationIndex);
        cl::RunKernel(queue, addForceFunctionKernel, n2, LOCALSIZE);

        if (static_cast<int>(static_cast<float>(ditherValue) / static_cast<float>(n2) * 100.0f) > progress)
        {
            progress++;
            std::cout << progress << " %\r";
        }
    }

    return ditherMatrix;
}

void SaveMatrix(const std::vector<int> ditherMatrix, int length, std::string path)
{
    std::ofstream outputStream(path.data(), std::ofstream::binary);

    if (!outputStream.is_open())
	{
        std::cout << "\nThe output stream " << path.data() << " cannot be opened.\n";

        exit(EXIT_FAILURE);
    }

    outputStream.write(reinterpret_cast<const char*>(ditherMatrix.data()), std::streamsize(length * sizeof(int)));
    outputStream.close();
}

void OutputMatrixAsImage(const std::vector<int>& ditherMatrix, int n, std::string path)
{
    std::ofstream outputStream(path.data(), std::ofstream::binary);

    if (!outputStream.is_open())
    {
        std::cout << "\nThe output file " << path.data() << " cannot be opened.\n";

        exit(EXIT_FAILURE);
    }

    // Write tga header
    outputStream << (char) 0 << (char) 0 << (char) 2 << (char) 0 << (char) 0 << (char) 0 << (char) 0 << (char) 0;
    outputStream << (char) 0 << (char) 0 << (char) 0 << (char) 0 << (char) (n & 0x00FF) << (char) ((n & 0xFF00) / 256);
    outputStream << (char) (n & 0x00FF) << (char) ((n & 0xFF00) / 256) << (char) 32 << (char) 0;

    // Write image data
    for (int value : ditherMatrix)
    {
        char grayscale = static_cast<char>(value * 256L / (n * n));

        outputStream << (char) grayscale << (char) grayscale << (char) grayscale << (char) 0xFF;
    }   

    // Close the file
    outputStream.close();
}

void CleanupOpenCL()
{
    clReleaseMemObject(forceFieldBuffer);
    clReleaseMemObject(forceFieldFunctionBuffer);
    clReleaseMemObject(freeLocationsBuffer);
    clReleaseMemObject(maskBuffer);
    clReleaseMemObject(candidatesBuffer);
    clReleaseMemObject(randomSeedBuffer);
    clReleaseMemObject(syncBuffer);

    clReleaseProgram(getForceMinimumProgram);
    clReleaseKernel(getForceMinimumKernel);
    clReleaseProgram(getForceCountProgram);
    clReleaseKernel(getForceCountKernel);
    clReleaseProgram(addForceFunctionProgram);
    clReleaseKernel(addForceFunctionKernel);

    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}

int Usage()
{
    std::cout << "Usage:\n" << "ForcedRandomDithering [matrixSize]\n\n";

    exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
    int n = 512; // Default matrix size

    if (argc > 2)
    {
        return Usage();
    }
    else if (argc == 2)
    {
        try
        {
            n = std::stoi(argv[1]);
        }
        catch (...)
        {
            std::cout << "\"" << argv[1] << "\" is not a valid matrix size. The matrix size must be a single positive integer.\n";

            return Usage();
        }
    }

    std::stringstream stringStream;
    std::stringstream imageStringStream;

#ifdef WIN32
	// For correct relative paths when starting the application from within Visual Studio
	if (IsDebuggerPresent() == TRUE)
	{
		stringStream << "../";
		imageStringStream << "../";
	}
#endif

    stringStream << "../Matrices/" << n << "x" << n << "_" << static_cast<int>(time(nullptr)) << ".matrix";
    std::string outputPath = stringStream.str();

    imageStringStream << "../Matrices/" << n << "x" << n << ".tga";
    std::string imageOutputPath = imageStringStream.str();

    std::cout << "Creating a dither matrix of size " << n << " * " << n << " ...\n";
    std::cout << "Path: " << outputPath << "\n";
    std::cout << "Image path: " << imageOutputPath << "\n";

    clock_t start = clock();

    SetupOpenCL(n);

    std::vector<int> ditherMatrix = CreateDitherMatrix(n);
    SaveMatrix(ditherMatrix, n * n, outputPath);
    OutputMatrixAsImage(ditherMatrix, n, imageOutputPath);

    CleanupOpenCL();

    clock_t end = clock();
    int elapsedSeconds = (end - start) / CLOCKS_PER_SEC;
    int hours = elapsedSeconds / 3600;
    int minutes = (elapsedSeconds - hours * 3600) / 60;
    int seconds = elapsedSeconds - minutes * 60 - hours * 3600;

    std::cout << "Done. Elapsed time: " << hours << " h, " << minutes << " min, " << seconds << " s\n";

	system("pause");

    return EXIT_SUCCESS;
}