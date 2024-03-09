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
#include <limits>

#include "Log.hpp"
#include "Pointset.hpp"

namespace utk
{

std::string rstrip(const std::string& s)
{
    std::string rslt = s;
    rslt.erase(rslt.find_last_not_of (" \n\r\t") + 1);
    return rslt;
}

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

    file << std::setprecision(std::numeric_limits<T>::digits10 + 2) << std::fixed;
    write_text_pointset_stream(file, pts);
    return true;
}

template<typename T>
inline bool write_text_pointsets(const std::string& dest, const std::vector<Pointset<T>>& pts)
{
    std::ofstream file(dest);
    if (!file.is_open()) return false;
    
    file << std::setprecision(std::numeric_limits<T>::digits10 + 2) << std::fixed;
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

    std::istringstream sstream(rstrip(line));
    while(sstream.good()) sstream >> pts.PushBack();
    uint32_t d = pts.Npts();
    
    while(std::getline(st, line))
    {
        if (line[0] == '#') break;
        std::istringstream tmp(rstrip(line));

        while(tmp.good()) tmp >> pts.PushBack();
        
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

const uint32_t UTK_MAGIC_NUMBER = 0x214B5455; // "UTK!" in binary

enum class DATATYPE : uint8_t
{
    INT32  = 0,
    INT64  = 1,
    UINT32 = 2,
    UINT64 = 3,
    FLOAT  = 4, 
    DOUBLE = 5
};

struct MetaData
{
    uint8_t dtype      = 0;
    uint8_t reserved_1 = 0;
    uint8_t reserved_2 = 0;
    uint8_t reserved_3 = 0;
};

template <typename T> struct DATATYPE_FROM_TYPE { static constexpr DATATYPE CODE = DATATYPE::DOUBLE; };
template <> struct DATATYPE_FROM_TYPE<int32_t>  { static constexpr DATATYPE CODE = DATATYPE::INT32;  };
template <> struct DATATYPE_FROM_TYPE<int64_t>  { static constexpr DATATYPE CODE = DATATYPE::INT64;  };
template <> struct DATATYPE_FROM_TYPE<uint32_t> { static constexpr DATATYPE CODE = DATATYPE::UINT32; };
template <> struct DATATYPE_FROM_TYPE<uint64_t> { static constexpr DATATYPE CODE = DATATYPE::UINT64; };
template <> struct DATATYPE_FROM_TYPE<float>    { static constexpr DATATYPE CODE = DATATYPE::FLOAT;  };
template <> struct DATATYPE_FROM_TYPE<double>   { static constexpr DATATYPE CODE = DATATYPE::DOUBLE; };


template<class Stream, typename T>
inline void write_bin_pointset_stream(Stream& st, const Pointset<T>& pts)
{
    st.write(reinterpret_cast<const char*>(pts.Data()), sizeof(T) * pts.Ndim() * pts.Npts());
}

template<typename T>
inline bool write_bin_pointset(const char* dest, const Pointset<T>& pts)
{
    std::ofstream file(dest, std::ios::binary); 

    if (!file.is_open()) return false;

    uint32_t m = 1;
    uint32_t n = pts.Npts();
    uint32_t d = pts.Ndim();

    MetaData meta; 
    meta.dtype = static_cast<uint32_t>(DATATYPE_FROM_TYPE<T>::CODE); 
    
    file.write(reinterpret_cast<const char*>(&UTK_MAGIC_NUMBER), sizeof(UTK_MAGIC_NUMBER));
    file.write(reinterpret_cast<const char*>(&meta), sizeof(MetaData));

    file.write(reinterpret_cast<const char*>(&m), sizeof(uint32_t));
    file.write(reinterpret_cast<const char*>(&n), sizeof(uint32_t));
    file.write(reinterpret_cast<const char*>(&d), sizeof(uint32_t));
    write_bin_pointset_stream(file, pts);

    return true;
}

template<typename T>
inline bool write_bin_pointsets(const char* dest, const std::vector<Pointset<T>>& ptss)
{
    std::ofstream file(dest, std::ios::binary); 

    if (!file.is_open()) return false;
    if (ptss.size() == 0) return true;

    MetaData meta; 
    meta.dtype = static_cast<uint32_t>(DATATYPE_FROM_TYPE<T>::CODE); 
    
    uint32_t m = ptss.size();
    uint32_t n = ptss[0].Npts();
    uint32_t d = ptss[0].Ndim();

    file.write(reinterpret_cast<const char*>(&UTK_MAGIC_NUMBER), sizeof(UTK_MAGIC_NUMBER));
    file.write(reinterpret_cast<const char*>(&meta), sizeof(MetaData));

    file.write(reinterpret_cast<const char*>(&m), sizeof(uint32_t));
    file.write(reinterpret_cast<const char*>(&n), sizeof(uint32_t));
    file.write(reinterpret_cast<const char*>(&d), sizeof(uint32_t));
    
    for (uint32_t i = 0; i < ptss.size(); i++)
        write_bin_pointset_stream(file, ptss[0]);
    
    return true;
}

template<class Stream, typename Source, typename T>
inline Pointset<T> read_bin_pointset_stream(Stream& st, uint32_t N, uint32_t d)
{
    Pointset<T> pts(N, d);

    if constexpr (std::is_same_v<Source, T>)
    {
        // Read directly to pointset
        st.read(reinterpret_cast<char*>(pts.Data()), N * d * sizeof(T));
    }
    else
    {
        // Type mismatch, read to tmp buffer and convert.
        Source* data = new Source[N * d];
        st.read(reinterpret_cast<char*>(data), N * d * sizeof(Source));
    
        for (uint32_t i = 0; i < N; i++)
            for (uint32_t j = 0; j < d; j++)
                pts[i][j] = static_cast<T>(data[j + d * i]);
        delete[] data; 
    }
    return pts;
}

template<class Stream, typename Source, typename T>
inline std::vector<Pointset<T>> read_bin_pointsets_stream(Stream& st, uint32_t m, uint32_t n, uint32_t d)
{
    std::vector<Pointset<T>> pointsets;
    for (uint32_t i = 0; i < m; i++)
        pointsets.push_back(read_bin_pointset_stream<Stream, Source, T>(st, n, d));
    return pointsets;
}

template<typename T>
inline std::vector<Pointset<T>> read_bin_pointsets(const char* filename)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) return {};
    
    MetaData meta;
    uint32_t magic, m, N, d;

    file.read(reinterpret_cast<char*>(&magic), sizeof(uint32_t));
    if (magic != UTK_MAGIC_NUMBER)
    {
        UTK_ERROR("Error, incorrect magic number in file {} ({}, expected {})", filename, magic, UTK_MAGIC_NUMBER);
        return {};
    }

    file.read(reinterpret_cast<char*>(&meta), sizeof(MetaData));
    file.read(reinterpret_cast<char*>(&m), sizeof(uint32_t));
    file.read(reinterpret_cast<char*>(&N), sizeof(uint32_t));
    file.read(reinterpret_cast<char*>(&d), sizeof(uint32_t));

    switch (static_cast<DATATYPE>(meta.dtype))
    {
    case DATATYPE::INT32:
        return read_bin_pointsets_stream<decltype(file), int32_t, T>(file, m, N, d);
    case DATATYPE::INT64:
        return read_bin_pointsets_stream<decltype(file), int64_t, T>(file, m, N, d);
    case DATATYPE::UINT32:
        return read_bin_pointsets_stream<decltype(file), uint32_t, T>(file, m, N, d);
    case DATATYPE::UINT64:
        return read_bin_pointsets_stream<decltype(file), uint64_t, T>(file, m, N, d);
    case DATATYPE::FLOAT:
        return read_bin_pointsets_stream<decltype(file), float, T>(file, m, N, d);
    case DATATYPE::DOUBLE:
        return read_bin_pointsets_stream<decltype(file), double, T>(file, m, N, d);
    default:
        break;
    }

    return {};
}

template<typename T>
inline std::vector<Pointset<T>> read_pointsets(const char* filename)
{
    uint32_t length = strlen(filename);
    if (length > 4 && !strcmp(filename + length - 4, ".bin"))
        return read_bin_pointsets<T>(filename);
    else
        return read_text_pointset<T>(filename);
}

template<typename T>
inline bool write_pointset(const char* filename, const Pointset<T>& pts)
{
    uint32_t length = strlen(filename);
    if (length > 4 && !strcmp(filename + length - 4, ".bin"))
        return write_bin_pointset<T>(filename, pts);
    else
        return write_text_pointset<T>(filename, pts);
}

template<typename T>
inline bool write_pointsets(const char* filename, const std::vector<Pointset<T>>& ptss)
{
    uint32_t length = strlen(filename);
    if (length > 4 && !strcmp(filename + length - 4, ".bin"))
        return write_bin_pointsets<T>(filename, ptss);
    else
        return write_text_pointsets<T>(filename, ptss);
}

};
