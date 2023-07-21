#ifndef KERNELCODES_H
#define KERNELCODES_H

// This macro is used to define the kernel codes as multi-line strings without quotation marks,
// which would prevent syntax highlighting in Visual Studio.
#define MULTILINE(...) #__VA_ARGS__

namespace KernelCodes
{
    const char* GetForceMinimumKernel = MULTILINE(
        #define A 16807L\n
        #define M 2147483647L\n
        #define HALF 1073741824\n
    
        // Implementation of the Park-Miller Pseudo-Random Number Generator. For an explanation, see:
        // Park, Steven K. and Miller, Keith W., Random Number Generators: Good Ones are Hard to Find,
        // Communications of the ACM, volume 31, number 10, pp. 1192 - 1201, 1988.
        int ParkMillerPRNG(int seed)
        {
            return (int) ((long) seed * A % M);
        }

        __kernel void GetForceMinimum(
            const int threadsCount,
            __global int* syncBuffer,
            __global int* maskBuffer,
            __global int* randomSeedBuffer,
            const __global int* forceFieldBuffer,
            const __global int* freeLocationsBuffer)
        {
            int threadId = get_global_id(0);
            
            if (threadId < threadsCount)
            {
                int rand = ParkMillerPRNG(randomSeedBuffer[threadId]);
                randomSeedBuffer[threadId] = rand;

                if ((rand < HALF) || (threadId == 0))
                {
                    int locationIndex = freeLocationsBuffer[threadId];
                    atomic_min(&syncBuffer[0], forceFieldBuffer[locationIndex]);
                    maskBuffer[threadId] = 1;
                }
                else
                {
                    maskBuffer[threadId] = 0;
                }
            }
        }
    );

    const char* GetForceCountKernel = MULTILINE(
        __kernel void GetForceCount(
            const int threadsCount,
            __global int* syncBuffer,
            __global int* maskBuffer,
            __global int* candidatesBuffer,
            const __global int* forceFieldBuffer,
            const __global int* freeLocationsBuffer)
        {
            int threadId = get_global_id(0);

            if (threadId < threadsCount)
            {
                if (maskBuffer[threadId] == 1)
                {
                    int locationIndex = freeLocationsBuffer[threadId];
                    int force = forceFieldBuffer[locationIndex];

                    if (syncBuffer[0] == force)
                    {
                        // The order in the candidates buffer is not deterministic because of arbitrary
						// thread execution, but this is not required here because we randomly select a
						// candidate anyway
                        candidatesBuffer[atomic_inc(&syncBuffer[1])] = threadId;
                    }
                }
            }
        }
    );

    const char* AddForceFunctionKernel = MULTILINE(
        __kernel void AddForceFunction(
            const int threadsCount,
            const __global int* forceFunctionBuffer,
            __global int* forceFieldBuffer,
            const int locationIndex,
            const int n)
        {
            int threadId = get_global_id(0);

            if (threadId < threadsCount)
            {
                int yLoc = locationIndex / n; // Integer truncation intended
                int xLoc = locationIndex - yLoc * n;
                int yForce = threadId / n; // Integer truncation intended
                int xForce = threadId - yForce * n;
                int deltaX = abs_diff(xLoc, xForce);
                int deltaY = abs_diff(yLoc, yForce);

                forceFieldBuffer[threadId] += forceFunctionBuffer[deltaY * n + deltaX];
            }
        }
    );
}

#endif