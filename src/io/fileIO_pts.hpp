#ifndef _UTK_FILEIO_PTS_HPP_
#define _UTK_FILEIO_PTS_HPP_

#include "fileIO_model.hpp"
#include <sstream>

namespace utk
{

template <unsigned int D, typename T, typename P>
class PointsetReader_PTS : public PointsetReader_Model<D, T, P>
{
public:
	PointsetReader_PTS() {}
	virtual ~PointsetReader_PTS() {}
	
	virtual bool readPointset(Pointset<D, T, P>& arg_pointset) 
	{
		if(!arg_pointset.empty())
		{
			WARNING("fileIO_PTS::readPointset the pointset to fill is not empty, clearing it ...");
			arg_pointset.clear();
		}
		
		if(this->m_istream.eof())
			return false;
		
		//header
		char line[1000];
		this->m_istream.getline(line, 1000);
		
		unsigned int i=0;
		while(!this->m_istream.eof())
		{
			char line[1000];
			this->m_istream.getline(line, 1000);
			
			if(line[0] == '#' || line[0] == 0)
				break;
			
			P pt;
			std::stringstream ss;
			ss.str(line);
			ss >> pt;
			arg_pointset.push_back(pt);
			i++;
		}

		return true;
	};
};

template <unsigned int D, typename T, typename P>
class PointsetWriter_PTS : public PointsetWriter_Model<D, T, P>
{
public:
	PointsetWriter_PTS() { first=true; }
	virtual ~PointsetWriter_PTS() {}
	
	virtual bool writePointset(const Pointset<D, T, P>& arg_pointset) 
	{
		if(!first)
			this->m_ostream << "#patch-separator" << std::endl;
		else
			this->m_ostream << "mpts2 2 n unit-tor" << std::endl;
		
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
