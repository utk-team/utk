#ifndef _UTK_FILEIO_MODEL_HPP_
#define _UTK_FILEIO_MODEL_HPP_

#include <fstream>
#include <cassert>

#include "../pointsets/Pointset.hpp"

namespace utk
{

template <unsigned int D, typename T, typename P>
class PointsetReader_Model
{
public:
	virtual ~PointsetReader_Model() {}
	
	virtual bool open(const char* arg_filename)
	{
		assert(!m_istream.is_open());
		m_istream.open(arg_filename);
		return m_istream.is_open();
	}
	
	virtual bool readPointset(Pointset<D, T, P>& arg_pointset) { return false; }
	
	virtual void close()
	{
		assert(m_istream.is_open());
		m_istream.close();
	}

protected:
	std::ifstream m_istream;
};


template <unsigned int D, typename T, typename P>
class PointsetWriter_Model
{
public:
	virtual ~PointsetWriter_Model() {}
	
	virtual bool open(const char* arg_filename)
	{
		assert(!m_ostream.is_open());
		m_ostream.open(arg_filename);
		return m_ostream.is_open();
	}
	
	virtual bool writePointset(const Pointset<D, T, P>& arg_pointset) { return false; }
	
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