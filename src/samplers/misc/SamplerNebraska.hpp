/*
 * Coded by Hélène Perrier helene.perrier@liris.cnrs.fr
 * and Bastien Doignies bastien.doignies@liris.cnrs.fr 
 * and David Coeurjolly David.coeurjolly@liris.cnrs.fr
 *
 * Copyright (c) 2023 CNRS Université de Lyon
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the UTK project.
 */
#pragma once

#include <utk/utils/Pointset.hpp>
#include <random>
#include <array>

namespace utk
{

class SamplerNebraska
{
protected:
public:

	SamplerNebraska() {  }

	template<typename T>
	bool generateSamples(Pointset<T>& arg_pts, uint32_t N)
	{
        using Point = std::array<T, 2>;
        std::vector<Point> points;

        T domain = std::sqrt(2);
        T nbdisk = std::sqrt(2 * N);
        T step   = domain / nbdisk;

        T off = 0.5;
        
        T x0_orig = -off;
        T y0_orig = -off;
        
        T x1_orig = -off;
        T y1_orig = 1 + off;

        T diag0 = sqrt(x0_orig * x0_orig + y0_orig * y0_orig);
        T diag1 = sqrt(x1_orig * x1_orig + (1 - y1_orig) * (1 - y1_orig));

        int r0i = 0;
        // int r1i = 0;

        for(T r0_orig = diag0 + (step / 2); r0_orig <= std::sqrt(2) + diag0; r0_orig += step)
        {
            T x0 = x0_orig;
            T y0 = y0_orig;
            T r0 = r0_orig;
            // r1i = 0;
            r0i++;

            for(T r1_orig = diag1 + (step / 2); r1_orig <= std::sqrt(2) + diag1; r1_orig += step)
            {
                T x1 = x1_orig;
                T y1 = y1_orig;
                T r1 = r1_orig;
                /*r1i++;
                if(r1i%3==1)
                    r1_orig += 0.04;*/
                
                T R02 = r0*r0;
                T R12 = r1*r1;
                
                T x02 = x0*x0;
                T x12 = x1*x1;
                
                T y02 = y0*y0;
                T y12 = y1*y1;
            
                if(y0 == y1)
                {
                    Point pt0, pt1;
                    T x = (R12 - R02 - x12 + x02) / 2*(x0-x1);
                    
                    T A = 1;
                    T B = -2*y1;
                    T C = x12 + x*x - 2*x1*x + y12 - R12;
                    
                    T D2 = sqrt(B*B - 4*A*C);
                    
                    if(D2 > 0)
                    {
                        pt0[1] = (-B + D2) / (2.0*A);
                        pt1[1] = (-B - D2) / (2.0*A);
                        
                        pt0[0] = x;
                        pt1[0] = x;
                        
                        if(pt0[0] >= 0 && pt0[0] < 1)
                        if(pt0[1] >= 0 && pt0[1] < 1)
                            points.push_back(pt0);
                        
                        if(pt1[0] >= 0 && pt1[0] < 1)
                        if(pt1[1] >= 0 && pt1[1] < 1)
                            points.push_back(pt1);
                    }
                    else if (D2 == 0)
                    {
                        pt0[1] = (-B + D2) / (2.0*A);
                        pt0[0] = x;
                        
                        if(pt0[0] >= 0 && pt0[0] < 1)
                        if(pt0[1] >= 0 && pt0[1] < 1)
                            points.push_back(pt0);
                    }
                }
                else
                {
                    T d = (x0 - x1) / (y0 - y1);
                    T A = d*d + 1;
                    
                    T N = R12 - R02 - x12 + x02 - y12 + y02;
                    N /= (2 * (y0 - y1));
                    
                    T B = 2*y0*d - 2*N*d - 2*x0;
                    T C = x02 + y02 + N*N - R02 - 2*y0*N;
                    
                    T D2 = sqrt(B*B - 4*A*C);
                    
                    Point pt0, pt1;
                    
                    if(D2 > 0)
                    {
                        pt0[0] = (-B + D2) / (2.0*A);
                        pt1[0] = (-B - D2) / (2.0*A);
                        
                        pt0[1] = N - pt0[0]*d;
                        pt1[1] = N - pt1[0]*d;
                        
                        if(pt0[0] >= 0 && pt0[0] < 1)
                        if(pt0[1] >= 0 && pt0[1] < 1)
                            points.push_back(pt0);
                        
                        if(pt1[0] >= 0 && pt1[0] < 1)
                        if(pt1[1] >= 0 && pt1[1] < 1)
                            points.push_back(pt1);
                    }
                    else if (D2 == 0)
                    {
                        pt0[0] = (-B + D2) / (2.0*A);
                        pt0[1] = N - pt0[0]*d;
                        
                        if(pt0[0] >= 0 && pt0[0] < 1)
                        if(pt0[1] >= 0 && pt0[1] < 1)
                            points.push_back(pt0);
                    }
                }
            }
        }

        arg_pts.Resize(points.size(), 2);
        for (uint32_t i = 0; i < points.size(); i++)
        {
            arg_pts[i][0] = points[i][0];
            arg_pts[i][1] = points[i][1];
        }
		return true;
	};

protected:
};

}
