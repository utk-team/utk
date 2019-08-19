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
 * either expressed or implied, of the UTK project.
 */
#ifndef _UTK_HISTOGRAMIO_HPP_
#define _UTK_HISTOGRAMIO_HPP_

#include "histogramIO_dat.hpp"

#include <iostream>
#include <cassert>

#include "../utils.hpp"

namespace utk
{

/**
 * \class HistogramWriter
 * \brief Outputs an histogram in ascii mode.
*/
template <uint D, typename T0, typename T1>
class HistogramWriter
{
public:
	HistogramWriter() { m_writer = NULL; }
	~HistogramWriter() { if(m_writer!=NULL) delete m_writer; }

	bool open(const std::string arg_filename)
	{
		assert(!m_writer);
		std::string ext = getExt(arg_filename, ".dat");
		
		if(ext == ".dat" || ext == ".pcf")
			m_writer = new HistogramWriter_DAT<D, T0, T1>;
		else
		{
			ERROR("HistogramWriter::open unknown file format "+ext);
			return false;
		}
		
		return m_writer->open(arg_filename.c_str());
	}
	
	virtual bool writeHistogram(const Histogram<D, T0, T1>& arg_histo) { return m_writer->writeHistogram(arg_histo); }
	
	void close()
	{
		assert(m_writer);
		m_writer->close();
		delete m_writer;
		m_writer=NULL;
	}
	
private:
	HistogramWriter_Model<D, T0, T1>* m_writer;
};


/**
 * \class HistogramReader
 * \brief Fills an instance of the Histogram class from an ascii file.
*/
template <uint D, typename T0, typename T1>
class HistogramReader
{
public:
	HistogramReader() { m_reader = NULL; }
	~HistogramReader() { if(m_reader!=NULL) delete m_reader; }

	bool open(const std::string arg_filename)
	{
		assert(!m_reader);
		std::string ext = getExt(arg_filename, ".dat");
		
		if(ext == ".dat")
			m_reader = new HistogramReader_DAT<D, T0, T1>;
		else
		{
			ERROR("HistogramReader::open unknown file format "+ext);
			return false;
		}
		
		return m_reader->open(arg_filename.c_str());
	}
	
	virtual bool readHistogram(Histogram<D, T0, T1>& arg_histo) { return m_reader->readHistogram(arg_histo); }
	
	void close()
	{
		assert(m_reader);
		m_reader->close();
		delete m_reader;
		m_reader=NULL;
	}
	
private:
	HistogramReader_Model<D, T0, T1>* m_reader;
};


}//end namespace utk

#endif