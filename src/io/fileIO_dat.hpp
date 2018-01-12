#ifndef _UTK_FILEIO_DAT_HPP_
#define _UTK_FILEIO_DAT_HPP_

#include "fileIO_model.hpp"
#include <sstream>

namespace utk
{

template <unsigned int D, typename T, typename P>
class PointsetReader_DAT : public PointsetReader_Model<D, T, P>
{
public:
	PointsetReader_DAT() {}
	virtual ~PointsetReader_DAT() {}
	
	virtual bool readPointset(Pointset<D, T, P>& arg_pointset) 
	{	
		if(this->m_istream.eof())
			return false;
		
		unsigned int i=0;
		//unsigned int s=0;
		while(!this->m_istream.eof())
		{
			char line[1000];
			this->m_istream.getline(line, 1000);
			
			if(line[0] == '#' || line[0] == 0)
				break;
			
			if( i==0 && !arg_pointset.empty())
			{
				WARNING("fileIO_DAT::readPointset the pointset to fill is not empty, clearing it ...");
				arg_pointset.clear();
			}
			
			P pt;
			std::stringstream ss;
			ss.str(line);
			ss >> pt;
			arg_pointset.push_back(pt);
			i++;
		}

		if(i==0)
			return false;
		
		return true;
	};
};

template <unsigned int D, typename T, typename P>
class PointsetWriter_DAT : public PointsetWriter_Model<D, T, P>
{
public:
	PointsetWriter_DAT() { first=true; }
	virtual ~PointsetWriter_DAT() {}
	
	virtual bool writePointset(const Pointset<D, T, P>& arg_pointset) 
	{
		if(!first)
			this->m_ostream << "#" << std::endl;
		
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
