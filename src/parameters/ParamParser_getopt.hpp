#ifndef _UTK_PARAMPARSER_GETOPT_
#define _UTK_PARAMPARSER_GETOPT_

#include "ParamParser.hpp"
#include "../io/messageStream.hpp"
#include <getopt.h>
#include <vector>
#include <string>
#include <iostream>
#include <cstring>

namespace utk
{
	
struct long_option
{
	char* name;
	void* variable;
	void (*assign)(void*, char*);
	int has_arg;
	
	std::string state_name;
	void (*display)(void*);
	
};
struct short_option
{
	char name;
	void* variable;
	void (*assign)(void*, char*);
	int has_arg;
	
	std::string state_name;
	void (*display)(void*);
};

class ParamParser_getopt : public ParamParser
{
public:
	ParamParser_getopt()
	{
		full_help_msg = "[HELP]\n";
	}
	
	~ParamParser_getopt()
	{
		for(uint i=0; i<long_options.size(); i++)
			free(long_options[i].name);
	}
	
	virtual void addShortOption(char string, void* variable, int has_arg, void (*assign)(void*, char*), void (*arg_display) (void*)= NULL, std::string help_msg="", std::string display_name="");
	virtual void addLongOption(char* string, void* variable, int has_arg, void (*assign)(void*, char*), void (*arg_display) (void*)= NULL, std::string help_msg="", std::string display_name="");

	virtual void parse(int argc, char** argv);
	
	virtual std::string getHelp() { return full_help_msg; }
	virtual void displayState();
	
protected:
	std::vector< long_option > long_options;
	std::vector< short_option > short_options;
	std::string full_help_msg;
};

void ParamParser_getopt::addLongOption(char* arg_name, void* arg_variable, int has_arg, void (*arg_assign) (void*, char*), void (*arg_display) (void*), std::string help_msg, std::string display_name)
{
	long_option l;
	l.name = (char*)malloc(sizeof(char)*(strlen(arg_name)+1));
	strcpy(l.name, arg_name);
	l.has_arg = has_arg;
	l.variable = arg_variable;
	l.assign = arg_assign;
	l.state_name = display_name;
	l.display = arg_display;
	long_options.push_back(l);
	
 	full_help_msg += std::string("\t--") + std::string(l.name) + std::string(" ") + help_msg + std::string("\n");
}

void ParamParser_getopt::addShortOption(char arg_name, void* arg_variable, int has_arg, void (*arg_assign) (void*, char*), void (*arg_display) (void*), std::string help_msg, std::string display_name)
{
	short_option l;
	l.name = arg_name;
	l.has_arg = has_arg;
	l.variable = arg_variable;
	l.assign = arg_assign;
	l.state_name = display_name;
	l.display = arg_display;
	short_options.push_back(l);
	
	std::string name_tmp("a");
	name_tmp[0] = arg_name;
	full_help_msg += std::string("\t-") + name_tmp + std::string(" ") + help_msg + std::string("\n");
}

void ParamParser_getopt::displayState()
{
	std::cout << "[State]" << std::endl;
	for(uint i=0; i<short_options.size(); i++)
	{
		if(short_options[i].display != NULL && short_options[i].state_name != "")
		{
			std::cout << short_options[i].state_name + ":";
			short_options[i].display(short_options[i].variable);
			std::cout << std::endl;
		}
	}
	for(uint i=0; i<long_options.size(); i++)
	{
		if(long_options[i].display != NULL)
		{
			std::cout << long_options[i].state_name + ":";
			long_options[i].display(long_options[i].variable);
			std::cout << std::endl;
		}
	}
}


void ParamParser_getopt::parse(int argc, char** argv)
{
	int c;
	optind=1;
	opterr=0;
	
	//Tab for long option parsing
	static struct option* long_options_tab = (struct option*)malloc(sizeof(struct option)*(long_options.size()+1));
	
	for(uint i=0; i<long_options.size(); i++)
	{
		long_options_tab[i].name = long_options[i].name;
		long_options_tab[i].has_arg = long_options[i].has_arg;
		long_options_tab[i].flag = 0;
		long_options_tab[i].val = 0;
	}
	long_options_tab[long_options.size()].name = 0;
	long_options_tab[long_options.size()].has_arg = 0;
	long_options_tab[long_options.size()].flag = 0;
	long_options_tab[long_options.size()].val = 0;

	//String for short option parsing
	std::string short_option_str;
	for(uint i=0; i<short_options.size();i++)
	{
		short_option_str+=short_options[i].name;
		if(short_options[i].has_arg)
			short_option_str+=":";
	}
	
	while (1) 
	{
		int option_index = 0;
	
		c = getopt_long(argc, argv, short_option_str.c_str(),
					long_options_tab, &option_index);
		
		if (c == -1)
		{
			free(long_options_tab);
			break;
		}
		else if(c=='?')
		{
			if(optopt==0)
				ERROR("Unknown parameter long");
			else 
            {
                std::string c(" ");
                c[0] = (char)optopt;
                ERROR("Unknown parameter "+c);
            }
				
			
			//std::cout << full_help_msg << std::endl;
			free(long_options_tab);
			exit(0);
		}
		else if(c == 0)
		{
			std::string s(long_options[option_index].name);
			for(uint i=0; i<long_options.size(); i++)
			{
				if(s == long_options[i].name)
					long_options[i].assign(long_options[i].variable, optarg);
			}
		}
		else
		{
			for(uint i=0; i<short_options.size();i++)
			{
				if(c == short_options[i].name)
					short_options[i].assign(short_options[i].variable, optarg);
			}
		}
	}
}


} //end namespace

#endif
