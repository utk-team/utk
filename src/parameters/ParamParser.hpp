#ifndef _UTK_PARAMPARSER_
#define _UTK_PARAMPARSER_

#include <string>
#include <cstdlib>
#include <iostream>

namespace utk
{
	
void assignDouble(void* variable, char* str)
{
	*((double*)variable) = atof(str);
}
void assignInt(void* variable, char* str)
{
	*((int*)variable) = atoi(str);
}
void assignUInt(void* variable, char* str)
{
	*((unsigned int*)variable) = atol(str);
}
void assignLLInt(void* variable, char* str)
{
	*((long long int*)variable) = strtoll(str, NULL, 10);
}
void assignULLInt(void* variable, char* str)
{
	*((unsigned long long int*)variable) = strtoull(str, NULL, 10);
}
void assignString(void* variable, char* str)
{
	*((std::string*)variable) = std::string(str);
}
void assignBoolTrue(void* variable, char* str)
{
	*((bool*)variable) = true;
}
void assignBoolFalse(void* variable, char* str)
{
	*((bool*)variable) = false;
}


void displayDouble(void* variable)
{
	std::cout << *((double*)variable);
}
void displayInt(void* variable)
{
	std::cout << *((int*)variable);
}
void displayUInt(void* variable)
{
	std::cout << *((int*)variable);
}
void displayLLInt(void* variable)
{
	std::cout << *((long long int*)variable);
}
void displayULLInt(void* variable)
{
	std::cout << *((unsigned long long int*)variable);
}
void displayString(void* variable)
{
	std::cout << *((std::string*)variable);
}
void displayBool(void* variable)
{
	std::cout << *((bool*)variable);
}


class ParamParser
{
public:
	virtual void addShortOption(char string, void* variable, int has_arg, void (*assign)(void*, char*), void (*arg_display) (void*) = NULL, std::string help_msg="", std::string display_name="") = 0;
	virtual void addLongOption(char* string, void* variable, int has_arg, void (*assign)(void*, char*), void (*arg_display) (void*) = NULL, std::string help_msg="", std::string display_name="") = 0;
	virtual void parse(int argc, char** argv) = 0;
	virtual std::string getHelp() = 0;
	virtual void displayState() = 0;
};

} //end namespace

#endif