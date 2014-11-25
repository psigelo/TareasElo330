#ifndef COLLISIONOBJECT_HPP
#define COLLISIONOBJECT_HPP

#include <stdlib.h>
#include <iostream>
#include <cstring>

#include "RobotSimulator.hpp"

using namespace std;

namespace ANN_USM
{
	class CollisionObject
	{
		RobotSimulator * simulator;

		char * name;
		int collisionState;
		int collisionHandle;

	public:
		CollisionObject(RobotSimulator * simulator, char name[]);
		CollisionObject();
		~CollisionObject();	

		void setCollisionHandle(int collisionHandle);
		void setCollisionState(int collisionState);

		char * getName();
		int getCollisionState();
		int getCollisionHandle();
	};
}

#endif