#ifndef OBJECT_CPP
#define OBJECT_CPP

#include "Object.hpp"
using namespace ANN_USM;

Object::Object(RobotSimulator * simulator, char name[])
{
	this->simulator = NULL;
	this->simulator = simulator;

	int size = strlen(name)+1;
	this->name = new char[size];
	strncpy(this->name, name, size);

	position = new double[3];
	orientation = new double[3];

	simulator->simGetObjectHandle(this->name, &handle, simx_opmode_oneshot_wait);
	
	this->getPosition(-1);
	this->getOrientation(-1);
}

Object::Object()
{

}

Object::~Object()
{

}

void Object::setHandle(int handle)
{
	this->handle = handle;
}

void Object::setPosition(float position[3])
{
	for(int i = 0; i < 3; i++)
		this->position[i] = (double)position[i];
}

void Object::setOrientation(float orientation[3])
{
	for(int i = 0; i < 3; i++)
		this->orientation[i] = (double)orientation[i];
}

int Object::getHandle()
{
	return handle;
}

char * Object::getName()
{
	return name;
}

double * Object::getPosition(int relativeTo)
{
	simulator->simGetObjectPosition(handle, relativeTo, position, simx_opmode_oneshot);

	return position;
}

double * Object::getOrientation(int relativeTo)
{
	simulator->simGetObjectOrientation(handle, relativeTo, orientation, simx_opmode_oneshot);

	return orientation;
}

#endif