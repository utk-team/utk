/*
 * Hélène Perrier helene.perrier@liris.cnrs.fr
 * and David Coeurjolly david.coeurjolly@liris.cnrs.fr
 *
 * Copyright (c) 2018 CNRS Université de Lyon
 * All rights reserved.
 *
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
 * either expressed or implied, of the Halton project.
 */
#ifndef _UTK_FILEIO_HPP_
#define _UTK_FILEIO_HPP_

#include "fileIO_dat.hpp"
#include "fileIO_bin.hpp"
#include "fileIO_edat.hpp"
#include "fileIO_ebin.hpp"
#include "fileIO_pts.hpp"
#include "fileIO_tex.hpp"

#include <iostream>
#include <cassert>

namespace utk
{

/**
 * \class PointsetWriter
 * \brief Outputs an ascii or binary point set
*/
template <unsigned int D, typename T, typename P>
class PointsetWriter
{
public:
	PointsetWriter() { m_writer = NULL; }
	~PointsetWriter() { if(m_writer!=NULL) delete m_writer; }

	bool open(const std::string arg_filename)
	{
		assert(!m_writer);
		std::string ext = getExt(arg_filename, ".dat");
		
		if(ext == ".dat" || ext == ".txt")
			m_writer = new PointsetWriter_DAT<D, T, P>;
		else if(ext == ".bin")
			m_writer = new PointsetWriter_BIN<D, T, P>;
		else if(ext == ".edat")
			m_writer = new PointsetWriter_EDAT<D, T, P>;
		else if(ext == ".ebin")
			m_writer = new PointsetWriter_EBIN<D, T, P>;
		else if(ext == ".pts")
			m_writer = new PointsetWriter_PTS<D, T, P>;
		else if(ext == ".tex")
			m_writer = new PointsetWriter_TIKZ<D, T, P>;
		else
		{
			ERROR("PointsetWriter::open unknown file format "+ext);
			return false;
		}
		
		return m_writer->open(arg_filename.c_str());
	}
	
	virtual bool writePointset(Pointset<D, T, P>& arg_pointset) { return m_writer->writePointset(arg_pointset); }
	
	void close()
	{
		assert(m_writer);
		m_writer->close();
		delete m_writer;
		m_writer=NULL;
	}
	
private:
	PointsetWriter_Model<D, T, P>* m_writer;
	
	std::string getExt(std::string f, std::string default_value)
	{
		size_t pos_ext = f.find_last_of('.');
		if (pos_ext == std::string::npos)
			return default_value;
		
		return f.substr(pos_ext);
	}

	std::string getBasename(std::string f)
	{
		unsigned int pos_ext = f.find_last_of('.');
		return f.substr(0, pos_ext);
	}
};


/**
 * \class PointsetReader
 * \brief Fills an instance of the Pointset class from an ascii or binary file.
*/
template <unsigned int D, typename T, typename P>
class PointsetReader
{
public:
	PointsetReader() { m_reader = NULL; }
	~PointsetReader() { if(m_reader!=NULL) delete m_reader; }

	bool open(const std::string arg_filename)
	{
		assert(!m_reader);
		std::string ext = getExt(arg_filename, ".dat");
		
		if(ext == ".dat")
			m_reader = new PointsetReader_DAT<D, T, P>;
		else if(ext == ".bin")
			m_reader = new PointsetReader_BIN<D, T, P>;
		else if(ext == ".edat")
			m_reader = new PointsetReader_EDAT<D, T, P>;
		else if(ext == ".ebin")
			m_reader = new PointsetReader_EBIN<D, T, P>;
		else
		{
			ERROR("PointsetReader::open unknown file format "+ext);
			return false;
		}
		
		return m_reader->open(arg_filename.c_str());
	}
	
	virtual bool readPointset(Pointset<D, T, P>& arg_pointset) { return m_reader->readPointset(arg_pointset); }
	
	void close()
	{
		assert(m_reader);
		m_reader->close();
		delete m_reader;
		m_reader=NULL;
	}
	
private:
	PointsetReader_Model<D, T, P>* m_reader;
	
	std::string getExt(std::string f, std::string default_value)
	{
		size_t pos_ext = f.find_last_of('.');
		if (pos_ext == std::string::npos)
			return default_value;
		
		return f.substr(pos_ext);
	}

	std::string getBasename(std::string f)
	{
		unsigned int pos_ext = f.find_last_of('.');
		return f.substr(0, pos_ext);
	}
};


}//end namespace utk

#endif
