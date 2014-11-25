#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <stdlib.h>
#include <iostream>
#include <cstring>

#include "RobotSimulator.hpp"

using namespace std;

namespace ANN_USM
{
	class Object
	{
	protected:
		RobotSimulator * simulator;

		int handle;
		char * name;
		double * position;
		double * orientation;

	public:
		Object(RobotSimulator * simulator, char name[]);
		Object();
		~Object();	

		void setHandle(int handle);	
		void setPosition(float position[3]);
		void setOrientation(float orientation[3]);

		int getHandle();
		char * getName();
		double * getPosition(int relativeTo);		
		double * getOrientation(int relativeTo);
	};
}

#endif