#ifndef _UTK_FILEIO_TIKZ_HPP_
#define _UTK_FILEIO_TIKZ_HPP_

#include "fileIO_model.hpp"
#include <sstream>

namespace utk
{

template <unsigned int D, typename T, typename P>
class PointsetReader_TIKZ : public PointsetReader_Model<D, T, P>
{
public:
	PointsetReader_TIKZ() {}
	virtual ~PointsetReader_TIKZ() {}
	
	virtual bool readPointset(Pointset<D, T, P>& arg_pointset) 
	{	
		ERROR("Cannot read tikz pointset");
		return false;
	};
};

template <unsigned int D, typename T, typename P>
class PointsetWriter_TIKZ : public PointsetWriter_Model<D, T, P>
{
public:
	PointsetWriter_TIKZ() { first=true; }
	virtual ~PointsetWriter_TIKZ() {}
	
	virtual bool writePointset(const Pointset<D, T, P>& arg_pointset) 
	{
		for(uint i=0; i<arg_pointset.size(); i++)
		{
			double xp = arg_pointset[i].pos()[0];
			double yp = arg_pointset[i].pos()[1];
			
			this->m_ostream << "\\filldraw[black] (" << xp << "," << yp << ") circle (10pt);" << std::endl;
		}
		
		first = false;
		
		return true;
	};
	
protected:
	bool first;
};

};// end namespace utk

#endif
