#ifndef DUMMY_CPP
#define DUMMY_CPP

#include "Dummy.hpp"
using namespace ANN_USM;

Dummy::Dummy(RobotSimulator * simulator, char name[]) : Object(simulator, name)
{
	velocity = new double[3];
}

Dummy::Dummy()
{

}

Dummy::~Dummy()
{

}

void Dummy::setVelocity(float velocity[3])
{
	for(int i = 0; i < 3; i++)
		this->velocity[i] = (double)velocity[i];
}

double * Dummy::getVelocity()
{
	simulator->simGetObjectVelocity(handle, velocity, simx_opmode_oneshot);
	
	return velocity;
}

#endif

