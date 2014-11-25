#ifndef USERFUNCTIONS_CPP
#define USERFUNCTIONS_CPP

#include <iostream>
#include <sstream>
#include <string>
#include "UserFunctions.hpp"

namespace ANN_USM{
	
	void GetNodeFunction(string plataform)
	{
		if(!strcmp(plataform.c_str(),(char *)"octave"))
		{
			ofstream myfile ("functions_files/SIGMOID.m");

			if (myfile.is_open())
			{
		    	myfile << OCTAVE_SIGMOID_STATEMENT << endl;
			    myfile << OCTAVE_SIGMOID_CONST_LETTER << " = " << OCTAVE_SIGMOID_CONST << ";" << endl;
			    myfile << OCTAVE_SIGMOID_FUNC << ";" << endl;
			    myfile.close();

		  	}else 
		  		cerr << "Unable to open file: functions_files/SIGMOID.m" << endl;

			/*
			ofstream myfile ("functions_files/USER_CUSTOM.m");

			if (myfile.is_open())
			{
		    	myfile << OCTAVE_USER_CUSTOM_STATEMENT << endl;
			    myfile << OCTAVE_USER_CUSTOM_CONST_LETTER << " = " << OCTAVE_USER_CUSTOM_CONST << ";" << endl;
			    myfile << OCTAVE_USER_CUSTOM_FUNC << ";" << endl;
			    myfile.close();

		  	}else 
		  		cerr << "Unable to open file: functions_files/USER_CUSTOM" << endl;		
		  	*/
		}
		else if(!strcmp(plataform.c_str(),(char *)"c++"))
		{
			
		}
		
	}

	double OutputNodeFunction(char * node_function, double input)
	{
		if(!strcmp(node_function,(char *)"SIGMOID"))
		{
			return SIGMOID(input);			
		}
		/*
		else if(!strcmp(node_function,(char *)"USER_CUSTOM"))
		{
			return USER_CUSTOM(input);
		}
		*/
		else
		{
			cerr << "ERROR: Does not exist any function called " << node_function << endl;
			return 0.0;
		}
	}
}

#endif