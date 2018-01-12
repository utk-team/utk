#ifndef _UTK_FILEIO_EBIN_HPP_
#define _UTK_FILEIO_EBIN_HPP_

#include "fileIO_model.hpp"
#include <sstream>

namespace utk
{

template <unsigned int D, typename T, typename P>
class PointsetReader_EBIN : public PointsetReader_Model<D, T, P>
{
public:
	virtual ~PointsetReader_EBIN() {}
	
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
			WARNING("fileIO_EBIN::readPointset the pointset to fill is not empty, clearing it ...");
			arg_pointset.clear();
		}
		
		this->m_istream.read((char*)&(arg_pointset.domain.pMin), sizeof(arg_pointset.domain.pMin));
		this->m_istream.read((char*)&(arg_pointset.domain.pMax), sizeof(arg_pointset.domain.pMax));
		this->m_istream.read((char*)&(arg_pointset.toricity), sizeof(arg_pointset.toricity));
		
		if(this->m_istream.eof())
		  return false;
		
		unsigned int s = 0;
		this->m_istream.read((char*)&s, sizeof(unsigned int));
		arg_pointset.resize(s);
		this->m_istream.read((char*)&arg_pointset[0], sizeof(arg_pointset[0])*s);
		//this->m_istream.read((char*)arg_pointset.data(), sizeof(arg_pointset[0])*s);
		
		return true;
	};
};

template <unsigned int D, typename T, typename P>
class PointsetWriter_EBIN : public PointsetWriter_Model<D, T, P>
{
public:
	PointsetWriter_EBIN() { first=true; }
	virtual ~PointsetWriter_EBIN() {}
	
	virtual bool open(const char* arg_filename)
	{
		assert(!this->m_ostream.is_open());
		this->m_ostream.open(arg_filename, std::ios::binary);
		return this->m_ostream.is_open();
	}
	
	virtual bool writePointset(const Pointset<D, T, P>& arg_pointset) 
	{
		this->m_ostream.write((char*)&(arg_pointset.domain.pMin), sizeof(arg_pointset.domain.pMin));
		this->m_ostream.write((char*)&(arg_pointset.domain.pMax), sizeof(arg_pointset.domain.pMax));
		this->m_ostream.write((char*)&(arg_pointset.toricity), sizeof(arg_pointset.toricity));
		unsigned int s = arg_pointset.size();
		this->m_ostream.write((char*)&s, sizeof(unsigned int));
		this->m_ostream.write((char*)&arg_pointset[0], sizeof(arg_pointset[0])*s);
		//this->m_ostream.write((char*)arg_pointset.data(), sizeof(arg_pointset[0])*s);
		return true;
	};
	
protected:
	bool first;
};

};// end namespace utk

#endif