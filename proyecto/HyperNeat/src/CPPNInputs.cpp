#ifndef CPPNINPUTS_CPP
#define CPPNINPUTS_CPP

#include "CPPNInputs.hpp"
using namespace ANN_USM;

CPPNInputs::CPPNInputs(char * type, double bias)
{
	this->bias = bias;
	this->type = type;
	if(!strcmp(type,(char *)"BIAS")){
		function = &CPPNInputs::Bias;
	}
	else if(!strcmp(type,(char *)"RADIUS1")){
		function = &CPPNInputs::Radius1;
	}
	else if(!strcmp(type,(char *)"RADIUS2")){
		function = &CPPNInputs::Radius2;
	}
	else if(!strcmp(type,(char *)"EUCLIDIANDISTANCE")){
		function = &CPPNInputs::EuclidianDistance;
	}
	//else if(type == "USER_CUSTOM"){
	//	function = CPPNInputs::user_custom;
	//}
}
CPPNInputs::~CPPNInputs(){	
}
double CPPNInputs::Eval(vector < double > point){
	return (*this.*function)(point);
}
double CPPNInputs::Bias(vector < double > point){
	return bias;
}
double CPPNInputs::Radius1(vector < double > point){
	double tmp = 0;
	for(int i = 0; i < (int)point.size()-2; i++)
		tmp += pow(point.at(i),2);
	return sqrt(tmp);
}
double CPPNInputs::Radius2(vector < double > point){
	double tmp = 0;
	for(int i = 2; i < (int)point.size(); i++)
		tmp += pow(point.at(i),2);
	return sqrt(tmp);
}
double CPPNInputs::EuclidianDistance(vector < double > point){
	double tmp = 0;
	for(int i = 0; i < (int)point.size(); i = i + 2)
		tmp += pow(point.at(i)-point.at(i+1),2);
	return sqrt(tmp);
}
char * CPPNInputs::GetType(){
	return type;
}



#endif