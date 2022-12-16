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
#ifndef _UTK_FILEIO_BIN_HPP_
#define _UTK_FILEIO_BIN_HPP_

#include "fileIO_model.hpp"
#include <sstream>

namespace utk
{

template <unsigned int D, typename T, typename P>
class PointsetReader_BIN : public PointsetReader_Model<D, T, P>
{
public:
	virtual ~PointsetReader_BIN() {}

	virtual bool open(const char* arg_filename)
	{
		assert(!this->m_istream.is_open());
		this->m_istream.open(arg_filename, std::ios::binary);
		return this->m_istream.is_open();
	}

	virtual bool readPointset(Pointset<D, T, P>& arg_pointset)
	{
		if(!arg_pointset.empty())
		{
			WARNING("fileIO_BIN::readPointset the pointset to fill is not empty, clearing it ...");
			arg_pointset.clear();
		}
		unsigned int s = 0;
		this->m_istream.read((char*)&s, sizeof(unsigned int));

		if(this->m_istream.eof())
		  return false;

		arg_pointset.resize(s);
		if(s > 0)
			this->m_istream.read((char*)&arg_pointset[0], sizeof(arg_pointset[0])*s);
			//this->m_istream.read((char*)arg_pointset.data(), sizeof(arg_pointset[0])*s);

		return true;
	};
};

template <unsigned int D, typename T, typename P>
class PointsetWriter_BIN : public PointsetWriter_Model<D, T, P>
{
public:
	PointsetWriter_BIN() { first=true; }
	virtual ~PointsetWriter_BIN() {}

	virtual bool open(const char* arg_filename)
	{
		assert(!this->m_ostream.is_open());
		this->m_ostream.open(arg_filename, std::ios::binary);
		return this->m_ostream.is_open();
	}

	virtual bool writePointset(const Pointset<D, T, P>& arg_pointset)
	{
		auto s = arg_pointset.size();

		this->m_ostream.write((char*)&s, sizeof(unsigned int));

		if(s > 0)
			this->m_ostream.write((char*)&arg_pointset[0], sizeof(arg_pointset[0])*s);
			//this->m_ostream.write((char*)arg_pointset.data(), sizeof(arg_pointset[0])*s);

		return true;
	};

protected:
	bool first;
};

};// end namespace utk

#endif
