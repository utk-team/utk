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
#ifndef _UTK_FILEIO_EDAT_HPP_
#define _UTK_FILEIO_EDAT_HPP_

#include "fileIO_model.hpp"
#include <sstream>

namespace utk
{

template <unsigned int D, typename T, typename P>
class PointsetReader_EDAT : public PointsetReader_Model<D, T, P>
{
public:
	PointsetReader_EDAT() {}
	virtual ~PointsetReader_EDAT() {}
	
	virtual bool readPointset(Pointset<D, T, P>& arg_pointset) 
	{
		
		if(this->m_istream.eof())
			return false;
		
		unsigned int i=0;
		while(!this->m_istream.eof())
		{
			char line[1000];
			this->m_istream.getline(line, 1000);
			
			if(line[0] == '#' || line[0] == 0)
				break;
			
			if( i==0 && !arg_pointset.empty())
			{
				WARNING("fileIO_EDAT::readPointset the pointset to fill is not empty, clearing it ...");
				arg_pointset.clear();
			}
			
			if(i==0)
			{
			  P pt;
			  std::stringstream ss;
			  ss.str(line);
			  ss >> pt;
			  arg_pointset.domain.pMin = pt;
			}
			else if(i==1)
			{
			  P pt;
			  std::stringstream ss;
			  ss.str(line);
			  ss >> pt;
			  arg_pointset.domain.pMax = pt;
			}
			else if (i==2)
			{
			  int tor;
			  std::stringstream ss;
			  ss.str(line);
			  ss >> tor;
			  arg_pointset.toricity = tor;
			}
			else
			{
			  P pt;
			  std::stringstream ss;
			  ss.str(line);
			  ss >> pt;
			  arg_pointset.push_back(pt);
			}
			i++;
		}

		return true;
	};
};

template <unsigned int D, typename T, typename P>
class PointsetWriter_EDAT : public PointsetWriter_Model<D, T, P>
{
public:
	PointsetWriter_EDAT() { first=true; }
	virtual ~PointsetWriter_EDAT() {}
	
	virtual bool writePointset(const Pointset<D, T, P>& arg_pointset) 
	{
		if(!first)
			this->m_ostream << "#" << std::endl;
		
		this->m_ostream << arg_pointset.domain.pMin << std::endl;
		this->m_ostream << arg_pointset.domain.pMax << std::endl;
		this->m_ostream << arg_pointset.toricity << std::endl;
		for(uint i=0; i<arg_pointset.size(); i++)
			this->m_ostream << arg_pointset[i] << std::endl;
		
		first = false;
		
		return true;
	};
	
protected:
	bool first;
};

};// end namespace utk

#endif
