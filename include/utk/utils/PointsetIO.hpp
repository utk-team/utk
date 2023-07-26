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

#include <iomanip>
#include <fstream>
#include <string>

#include "Pointset.hpp"

namespace utk
{

template<class Stream, typename T>
inline void write_text_pointset_stream(Stream& st, const Pointset<T>& pts)
{
    
    for (uint32_t i = 0; i < pts.Npts(); i++)
    {
        for (uint32_t d = 0; d < pts.Ndim() - 1; d++)
        {
            st << pts[i][d] << ' ';
        }
        st << pts[i][pts.Ndim() - 1] << '\n';
    }
}
    
template<typename T>
inline bool write_text_pointset(const std::string& dest, const Pointset<T>& pts)
{
    std::ofstream file(dest); 
    
    if (!file.is_open()) return false;

    file << std::setprecision(16) << std::fixed;
    write_text_pointset_stream(file, pts);
    return true;
}

template<typename T>
inline bool write_text_pointsets(const std::string& dest, const std::vector<Pointset<T>>& pts)
{
    std::ofstream file(dest);
    if (!file.is_open()) return false;
    
    file << std::setprecision(16) << std::fixed;
    for (uint32_t i = 0; i < pts.size() - 1; i++)
    {
        write_text_pointset_stream(file, pts[i]);
        file << "#\n";
    }
    
    if (pts.size() != 0)
        write_text_pointset_stream(file, pts.back());

    return true;
}

template<class Stream, typename T>
inline Pointset<T> read_text_pointset_stream(Stream& st)
{
    // At least one element
    Pointset<T> pts = Pointset<T>(0, 1);
    std::string line = "#";

    // Skips comments (at the beginning only) (if any)
    while (line[0] == '#' && std::getline(st, line));

    // Check for end of file. Not sure if empty line is possible
    // Better safe than sorry
    if (st.eof() || line.empty())
        return pts;

    std::istringstream sstream(line);
    while(sstream.good()) sstream >> pts.PushBack();
    
    uint32_t d = pts.Npts();
    
    while(std::getline(st, line))
    {
        if (line[0] == '#') break;
        std::istringstream tmp(line);
        while(tmp.good()) 
            tmp >> pts.PushBack();
    }

    pts.Resize(pts.Npts() / d, d);
    // pts.Shrink();
    // return pts;
    return pts;
}

template<class Stream, typename T>
inline std::vector<Pointset<T>> read_text_pointsets_stream(Stream& st)
{
    std::vector<Pointset<T>> pointsets;
    while (st.good())
    {
        Pointset<T> pointset = read_text_pointset_stream<decltype(st), T>(st);
        if (pointset.Npts() > 0)
            pointsets.push_back(pointset);
    }
    return pointsets;
}

template<typename T>
inline std::vector<Pointset<T>> read_text_pointset(const char* filepath)
{
    std::ifstream file(filepath);
    return read_text_pointsets_stream<decltype(file), T>(file);
}

};