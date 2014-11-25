#ifndef ROBOTSIMULATOR_HPP
#define ROBOTSIMULATOR_HPP

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <unistd.h>

using namespace std;

extern "C" {
    #include "extApi.h"
}

#define PORTNB 19998

namespace ANN_USM
{
	class RobotSimulator
	{
		int clientID;
		ofstream vrep_error;

	public:
		RobotSimulator();
		~RobotSimulator();

		bool clientIdStatus();

		void simStart();
		void simFinish();
		int simGetConnectionId();
		void simPauseCommunication(int action);

		void simStartSimulation(simxInt operationMode);
		void simStopSimulation(simxInt operationMode);

		void simGetObjectHandle(char name[], int * handle, simxInt operationMode);
		void simGetObjectPosition(int object_handle, int relativeTo, double * position, simxInt operationMode);
		void simGetObjectVelocity(int object_handle, double * velocity, simxInt operationMode);
		void simGetObjectOrientation(int object_handle, int relativeTo, double * orientation, simxInt operationMode);

		double simGetJointPosition(int object_handle, simxInt operationMode);
		void simSetJointTargetPosition(int object_handle, double joint_pos, simxInt operationMode);

		void simAddStatusbarMessage(char * message, simxInt operationMode);

		void simReadCollision(int collisionHandle, int * collisionState, simxInt operationMode);
		void simGetCollisionHandle(char name[], int * collisionHandle, simxInt operationMode);

	};
}

#endif