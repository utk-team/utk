#ifndef _UTK_FILEIO_HPP_
#define _UTK_FILEIO_HPP_

#include "fileIO_dat.hpp"
#include "fileIO_bin.hpp"
#include "fileIO_edat.hpp"
#include "fileIO_ebin.hpp"
#include "fileIO_pts.hpp"
#include "fileIO_tex.hpp"

#include <iostream>
#include <cassert>

namespace utk
{

template <unsigned int D, typename T, typename P>
class PointsetWriter
{
public:
	PointsetWriter() { m_writer = NULL; }
	~PointsetWriter() { if(m_writer!=NULL) delete m_writer; }

	bool open(const std::string arg_filename)
	{
		assert(!m_writer);
		std::string ext = getExt(arg_filename, ".dat");
		
		if(ext == ".dat" || ext == ".txt")
			m_writer = new PointsetWriter_DAT<D, T, P>;
		else if(ext == ".bin")
			m_writer = new PointsetWriter_BIN<D, T, P>;
		else if(ext == ".edat")
			m_writer = new PointsetWriter_EDAT<D, T, P>;
		else if(ext == ".ebin")
			m_writer = new PointsetWriter_EBIN<D, T, P>;
		else if(ext == ".pts")
			m_writer = new PointsetWriter_PTS<D, T, P>;
		else if(ext == ".tex")
			m_writer = new PointsetWriter_TIKZ<D, T, P>;
		else
		{
			ERROR("PointsetWriter::open unknown file format "+ext);
			return false;
		}
		
		return m_writer->open(arg_filename.c_str());
	}
	
	virtual bool writePointset(Pointset<D, T, P>& arg_pointset) { return m_writer->writePointset(arg_pointset); }
	
	void close()
	{
		assert(m_writer);
		m_writer->close();
		delete m_writer;
		m_writer=NULL;
	}
	
private:
	PointsetWriter_Model<D, T, P>* m_writer;
	
	std::string getExt(std::string f, std::string default_value)
	{
		size_t pos_ext = f.find_last_of('.');
		if (pos_ext == std::string::npos)
			return default_value;
		
		return f.substr(pos_ext);
	}

	std::string getBasename(std::string f)
	{
		unsigned int pos_ext = f.find_last_of('.');
		return f.substr(0, pos_ext);
	}
};

template <unsigned int D, typename T, typename P>
class PointsetReader
{
public:
	PointsetReader() { m_reader = NULL; }
	~PointsetReader() { if(m_reader!=NULL) delete m_reader; }

	bool open(const std::string arg_filename)
	{
		assert(!m_reader);
		std::string ext = getExt(arg_filename, ".dat");
		
		if(ext == ".dat")
			m_reader = new PointsetReader_DAT<D, T, P>;
		else if(ext == ".bin")
			m_reader = new PointsetReader_BIN<D, T, P>;
		else if(ext == ".edat")
			m_reader = new PointsetReader_EDAT<D, T, P>;
		else if(ext == ".ebin")
			m_reader = new PointsetReader_EBIN<D, T, P>;
		else
		{
			ERROR("PointsetReader::open unknown file format "+ext);
			return false;
		}
		
		return m_reader->open(arg_filename.c_str());
	}
	
	virtual bool readPointset(Pointset<D, T, P>& arg_pointset) { return m_reader->readPointset(arg_pointset); }
	
	void close()
	{
		assert(m_reader);
		m_reader->close();
		delete m_reader;
		m_reader=NULL;
	}
	
private:
	PointsetReader_Model<D, T, P>* m_reader;
	
	std::string getExt(std::string f, std::string default_value)
	{
		size_t pos_ext = f.find_last_of('.');
		if (pos_ext == std::string::npos)
			return default_value;
		
		return f.substr(pos_ext);
	}

	std::string getBasename(std::string f)
	{
		unsigned int pos_ext = f.find_last_of('.');
		return f.substr(0, pos_ext);
	}
};


}//end namespace utk

#endif