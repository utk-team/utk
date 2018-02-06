// API-independent utilities for making normalization cube maps and creating 
// normal maps from heightmaps.
//
// -Mark Harris, March 2004

#ifndef __NORMALMAPUTILS_H__
#define __NORMALMAPUTILS_H__

#include <math.h>

#define NORMALIZE_EPSILON 1e-6

void normalize(float n[3]);
void scaleBias(const float vec[3], float* texel);
void scaleBias2D(const float vec[3], float* texel);
void scaleBiasHiLoZ(const float vec[3], float* texel);

enum CubeFace
{
    CUBE_MAP_POSITIVE_X,
    CUBE_MAP_NEGATIVE_X,
    CUBE_MAP_POSITIVE_Y,
    CUBE_MAP_NEGATIVE_Y,
    CUBE_MAP_POSITIVE_Z,
    CUBE_MAP_NEGATIVE_Z,
};

// ported from NVIDIA glh-dependent glh_cube_map.h to avoid library/API dependencies.
// This version is also much more compact, and handles HiLo 16-bit textures.
inline void MakeNormalizationCubeMapFace(CubeFace face,
                                         float*   &data,
                                         int      size, 
                                         int      components,
                                         bool     scaleAndBias = true, 
                                         bool     hiloZ        = false)
{
    const int signX[6] = { -1,  1,  1,  1,  1, -1 };
    const int signY[6] = { -1, -1,  1, -1, -1, -1 };

	float *ip = data; // image pointer

	float offset   = .5;
	float delta    = 1;
	float halfsize = 0.5f * size;
	float v[3];

    int signZ = (face % 2) ? -1 : 1; // POS or NEG
    
    for(int j = 0; j < size; j++)
    {
        for(int i=0; i < size; i++)
        {
            v[2 * (int)(face < 2)]           = signX[face] * (i*delta + offset - halfsize);
            v[1 + (int)(face==2 || face==3)] = signY[face] * (j*delta + offset - halfsize);
            v[face / 2]                      = signZ * halfsize;
            normalize(v);
            
            if (3 == components)
            {
                if (scaleAndBias)
                    scaleBias(v, ip);
                else
                {
                    ip[0] = v[0];
                    ip[1] = v[1];
                    ip[2] = v[2];
                }
                
            }
            else if (2 == components)
            {
                if (!hiloZ)
                {
                    if (scaleAndBias)
                        scaleBias2D(v, ip);
                    else
                    {
                        ip[0] = v[0];
                        ip[1] = v[1];
                    }
                }
                else
                {
                    if (scaleAndBias)
                        scaleBiasHiLoZ(v, ip);
                    else
                    {
                        ip[0] = v[2];
                        ip[1] = 0;
                    }
                }
            }
            
            ip += components;
            
        }
    }   
}


// Ported from NVIDIA glh-dependent bumpmap_to_normalmap.h to avoid 
// dependencies on glh. (a bit faster, too, for reduced load times)
inline void BumpmapToNormalmap(const unsigned char* src, 
                               float*               dst, 
                               float                scale[3],
                               int                  iWidth, 
                               int                  iHeight, 
                               bool                 bScaleAndBias = false)
{
    if(scale[0] == 0.f || scale[1] == 0.f || scale[2] == 0.f)
    {
        float rAspect = float(iWidth) / float(iHeight);
        
        if(rAspect < 1.0f)
        {
            scale[0] = 1.0f;
            scale[1] = 1.0f / rAspect;
        }
        else
        {
            scale[0] = rAspect;
            scale[1] = 1.0f;
        }
        scale[2] = 1.0f;
    }
    
    int index = (iWidth + 1) * 3;
    int i, j;
    for(j = 1; j < iHeight - 1; ++j)
    {
        for(i = 1; i < iWidth - 1; ++i)
        {
            float dfdiZ = float(src[j * iWidth + i + 1] - src[j * iWidth + i - 1]) / 255.0f;
            float dfdjZ = float(src[(j + 1) * iWidth + i] - src[(j - 1) * iWidth + i]) / 255.0f;
            
            // cross product, optimized.  Also scaled by the scale parameter
            float n[3];
            n[0] = scale[0] * (-dfdiZ * 2);
            n[1] = scale[1] * (-2 * dfdjZ);
            n[2] = scale[2] * 4;
            
            normalize(n);
            
            if (bScaleAndBias)
            {
                dst[index++] = 0.5f * n[0] + 0.5f;
                dst[index++] = 0.5f * n[1] + 0.5f;
                dst[index++] = 0.5f * n[2] + 0.5f;
            }
            else
            {
                dst[index++] = n[0];
                dst[index++] = n[1];
                dst[index++] = n[2];
            }
        }
        index += 6;
    }
    
    // cheesy boundary cop-out
    for(i = 0; i < iWidth; i++)
    {
        dst[i * 3]     = dst[(iWidth + i) * 3];
        dst[i * 3 + 1] = dst[(iWidth + i) * 3 + 1];
        dst[i * 3 + 2] = dst[(iWidth + i) * 3 + 2];
        
        dst[((iHeight - 1) * iWidth + i) * 3]     = dst[((iHeight - 2) * iWidth + i) * 3];
        dst[((iHeight - 1) * iWidth + i) * 3 + 1] = dst[((iHeight - 2) * iWidth + i) * 3 + 1];
        dst[((iHeight - 1) * iWidth + i) * 3 + 2] = dst[((iHeight - 2) * iWidth + i) * 3 + 2];
    }
    for(j = 0; j < iHeight; j++)
    {
        dst[(j * iWidth) * 3]     = dst[(j * iWidth + 1) * 3];
        dst[(j * iWidth) * 3 + 1] = dst[(j * iWidth + 1) * 3 + 1];
        dst[(j * iWidth) * 3 + 2] = dst[(j * iWidth + 1) * 3 + 2];
        
        dst[(j * iWidth + iWidth - 1) * 3]     = dst[(j * iWidth + iWidth - 2) * 3];
        dst[(j * iWidth + iWidth - 1) * 3 + 1] = dst[(j * iWidth + iWidth - 2) * 3 + 1];
        dst[(j * iWidth + iWidth - 1) * 3 + 2] = dst[(j * iWidth + iWidth - 2) * 3 + 2];
    }	
}


inline void normalize(float n[3])
{
    float sum = 0;
    for(int i = 0; i < 3; i++) 
        sum += n[i]*n[i];
    
    sum = float(sqrt(sum));
    
    if (sum > NORMALIZE_EPSILON)
    {
        for(int i = 0; i < 3; i++) 
            n[i] /= sum;
    }
}

inline void scaleBias(const float vec[3], float* texel)
{
    for (int i = 0; i < 3; ++i)
    {
        texel[i] = vec[i];
        texel[i] *= 0.5;
        texel[i] += 0.5;
    }
}

inline void scaleBias2D(const float vec[3], float* texel)
{
    for (int i = 0; i < 2; ++i)
    {
        texel[i] = vec[i];
        texel[i] *= 0.5;
        texel[i] += 0.5;
    }
}

inline void scaleBiasHiLoZ(const float vec[3], float* texel)
{
    texel[0] = vec[2];
    texel[0] *= 0.5;
    texel[0] += 0.5;
    texel[1] = 0.0;
}





#endif //__NORMALMAPUTILS_H__
