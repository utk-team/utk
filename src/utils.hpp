#ifndef _UTK_UTILS_HPP_
#define _UTK_UTILS_HPP_

#include <typeinfo>
#include <cstdlib>
#include <cmath>
#include <string>

namespace utk
{
	
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

template<typename T>
bool isIntegerType()
{
	return typeid(T) == typeid(int) ||
		typeid(T) == typeid(unsigned int) ||
		typeid(T) == typeid(char) ||
		typeid(T) == typeid(short) ||
		typeid(T) == typeid(long int) ||
		typeid(T) == typeid(long unsigned int) ||
		typeid(T) == typeid(unsigned long long int);
}

template<typename T>
bool isFloatingType()
{
	return typeid(T) == typeid(float) ||
		typeid(T) == typeid(double);
}

bool* toBitList(unsigned long long int a, unsigned int size)
{
	bool* digits = (bool*)malloc(sizeof(bool)*size);
	for(unsigned int i=0; i<size; i++)
		digits[size-i-1] = (a >> i) & 1;
	return digits;
}

}

#endif

