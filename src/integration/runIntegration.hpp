/*
 * Hélène Perrier helene.perrier@liris.cnrs.fr
 * and David Coeurjolly david.coeurjolly@liris.cnrs.fr
 *
 * Copyright (c) 2018 CNRS Université de Lyon
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the UTK project.
 */
#include "../parameters/ParamParser.hpp"
#include "../io/messageStream.hpp"

bool param_help = false;
bool param_verbose = true;
bool param_brute = false;
std::string param_input;
std::string param_output;
unsigned int param_npts = 0;

class IntegrationStatistics
{
public:
	long double iter;
	long double nbpts;
	long double mean_integration;
	long double var_integration;
	long double max_integration;
	long double min_integration;
	long double ana_value;
	long double mse_integration;
	
	IntegrationStatistics() {
		iter = 0;
		nbpts = 0;
		mean_integration = 0;
		var_integration = 0;
		min_integration = 0;
		max_integration = 0;
		mse_integration = 0;
	}
	void setAnalytical(double analytical)
	{
	  ana_value = analytical;
	}
	void addValue(double integration)
	{
		iter++;
		
		double delta = integration - mean_integration;
		mean_integration += delta/iter;
		var_integration += delta*(integration - mean_integration);
		mse_integration += (integration-ana_value)*(integration-ana_value);
		
		if(iter == 1) {
			min_integration = integration;
			max_integration = integration;
		} else {
			if(integration > max_integration)
				max_integration = integration;
			if(integration < min_integration)
				min_integration = integration;
		}
	}
	void computeStatistics()
	{
		if(iter <= 1)
			var_integration = 0;
		else
			var_integration /= iter-1;
		mse_integration /= iter;
	}
};
std::ostream& operator<<(std::ostream& os, const IntegrationStatistics& e) 
{
  os<< std::setprecision(std::numeric_limits<long double>::digits10 + 1);
	if(e.var_integration == 0)
	{
		os << "#Nbpts\t#Mean\t\t#Var\t#Min\t\t#Max\t\t#Analytical\t#MSE\t\t#NbPtsets" << std::endl;
		os << e.nbpts << "\t";
		os << e.mean_integration << "\t";
		os << e.var_integration << "\t";
		os << e.min_integration << "\t";
		os << e.max_integration << "\t";
		os << e.ana_value << "\t\t";
		os << e.mse_integration << "\t";
		os << e.iter;
	}
	else
	{
		os << "#Nbpts\t#Mean\t\t#Var\t\t#Min\t\t#Max\t\t#Analytical\t#MSE\t\t#NbPtsets" << std::endl;
		os << e.nbpts << "\t";
		os << e.mean_integration << "\t";
		os << e.var_integration << "\t";
		os << e.min_integration << "\t";
		os << e.max_integration << "\t";
		os << e.ana_value << "\t\t";
		os << e.mse_integration << "\t";
		os << e.iter;
	}
	return os; 
}

int initParserIntegration(utk::ParamParser* parser)
{
	parser->addShortOption('h', &param_help, 0, utk::assignBoolTrue, NULL, "<string>\t\tDisplays this help message");
	parser->addShortOption('i', &param_input, 1, utk::assignString, utk::displayString, "<string>\t\tThe input pointsets", "Input");
	parser->addShortOption('o', &param_output, 1, utk::assignString, utk::displayString, "<string>\t\tThe output integration results", "Output");
	parser->addShortOption('s', &param_npts, 1, utk::assignInt, utk::displayInt, "<uint>\t\tThe number of samples to use", "NbPts");
	parser->addLongOption((char*)"silent", &param_verbose, 0, utk::assignBoolFalse, utk::displayBool, (char*)"\t\tSilent mode", (char*)"Verbose");
	parser->addLongOption((char*)"brute", &param_brute, 0, utk::assignBoolTrue, utk::displayBool, (char*)"\t\tOutput brute values instead of computing the statistics", (char*)"Brute");
    
    return 0;
}

int dealParamParserIntegration(utk::ParamParser* parser)
{
	if(param_input.empty())
	{
		ERROR("Argument -i required");
		std::cout << parser->getHelp() << std::endl;
		return 0;
	}
	
	if(param_help)
		std::cout << parser->getHelp() << std::endl;
	
	if(param_verbose)
		parser->displayState();
	
	return 1;
}
