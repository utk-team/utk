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
