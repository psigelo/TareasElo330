#ifndef JOINT_CPP
#define JOINT_CPP

#include "Joint.hpp"
#include <iostream>
using namespace ANN_USM;

double Joint::truncValue(double value, int precision)
{
	return (double)floor(value*pow(10,precision))/pow(10,precision);
}

double Joint::convertToRadFrom(const char * unit, double value)
{
	if (!strcmp(unit,(char *)"RAD")) return value;
	else if (!strcmp(unit,(char *)"DEG")) return value*RAD_CONV;
	else if (!strcmp(unit,(char *)"SCALE")) return (max_value - min_value )*(value + 1.0)/2.0 + min_value;
	else
	{
		cerr << "Joint ERROR: Unit not valid" << endl;
		return 0.0;
	}
}

double Joint::convertFromRadTo(const char * unit, double value)
{
	if (!strcmp(unit,(char *)"RAD")) return value;
	else if (!strcmp(unit,(char *)"DEG")) return value/RAD_CONV;
	else if (!strcmp(unit,(char *)"SCALE")) return 2.0*(value - min_value)/(max_value - min_value) - 1.0;
	else
	{
		cerr << "Joint ERROR: Unit not valid" << endl;
		return 0.0;
	}
}

Joint::Joint(RobotSimulator * simulator, char name[], double max_value, double min_value, double position) : Object(simulator, name)
{
	this->max_value = max_value;
	this->min_value = min_value;

	positions = new double[3];

	setJointInitialPosition(position, (char *)"RAD");

}

Joint::Joint(RobotSimulator * simulator, char name[], double max_value, double min_value) : Object(simulator, name)
{
	this->simulator = simulator;

	this->max_value = max_value;
	this->min_value = min_value;

	positions = new double[3];

	initial_position = truncValue(simulator->simGetJointPosition(handle, simx_opmode_oneshot_wait), PRECISION);	

	setJointInitialPosition();
}

Joint::~Joint()
{

}

void Joint::setJointPosition(double position, const char * unit)
{
	double aux_slope = 0;

	positions[2] = positions[1];
	positions[1] = positions[0];
	positions[0] = truncValue(convertToRadFrom(unit, position)*0.7 + positions[1]*0.2 + positions[2]*0.1,PRECISION);	

	pass_slope_sign = next_slope_sign;
	aux_slope = positions[0] - positions[1];

	if(abs(aux_slope) > 0.001)
	{
		if(aux_slope < 0) next_slope_sign = -1;
		else next_slope_sign = 1; 
	}		

	if(next_slope_sign != pass_slope_sign) joint_change_direction = true;
	else joint_change_direction = false;

	simulator->simSetJointTargetPosition(handle, positions[0], simx_opmode_oneshot);
}

void Joint::setJointInitialPosition()
{
	positions[2] = positions[1] = positions[0] = initial_position;	
	pass_slope_sign = 1;
	next_slope_sign = 1;	
	joint_change_direction = false;
}

void Joint::setJointInitialPosition(double position, const char * unit)
{
	positions[2] = positions[1] = positions[0] = initial_position = truncValue(convertToRadFrom(unit, position), PRECISION);		
	pass_slope_sign = 1;
	next_slope_sign = 1;	
	joint_change_direction = false;

	simulator->simSetJointTargetPosition(handle, initial_position, simx_opmode_oneshot_wait);
}

double Joint::getJointCurrentPosition(const char * unit)
{
	current_position = simulator->simGetJointPosition(handle, simx_opmode_oneshot_wait);

	return convertFromRadTo(unit, current_position);
}

double Joint::getJointGoalPosition(const char * unit)
{
	return convertFromRadTo(unit, positions[0]);
}

double Joint::getJointInitialPosition(const char * unit)
{
	return convertFromRadTo(unit, initial_position);
}

double Joint::getMaxAngle()
{
	return max_value;
}

double Joint::getMinAngle()
{
	return min_value;
}

bool Joint::getJointChangeDirection()
{
	return joint_change_direction;
}

#endif

