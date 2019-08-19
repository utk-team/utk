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
#ifndef _UTK_HISTOGRAMIO_MODEL_HPP_
#define _UTK_HISTOGRAMIO_MODEL_HPP_

#include <fstream>
#include <cassert>

#include "../pointsets/Histogram.hpp"

namespace utk
{

template <uint D, typename T0, typename T1>
class HistogramReader_Model
{
public:
	virtual ~HistogramReader_Model() {}
	
	virtual bool open(const char* arg_filename)
	{
		assert(!m_istream.is_open());
		m_istream.open(arg_filename);
		return m_istream.is_open();
	}
	
	virtual bool readHistogram(Histogram<D, T0, T1>& arg_histo) { return false; }
	
	virtual void close()
	{
		assert(m_istream.is_open());
		m_istream.close();
	}

protected:
	std::ifstream m_istream;
};


template <uint D, typename T0, typename T1>
class HistogramWriter_Model
{
public:
	virtual ~HistogramWriter_Model() {}
	
	virtual bool open(const char* arg_filename)
	{
		assert(!m_ostream.is_open());
		m_ostream.open(arg_filename);
		return m_ostream.is_open();
	}
	
	virtual bool writeHistogram(const Histogram<D, T0, T1>& arg_histo) { return false; }
	
	virtual void close()
	{
		assert(m_ostream.is_open());
		m_ostream.close();
	}

protected:
	std::ofstream m_ostream;
};

};//end namespace utk

#endif