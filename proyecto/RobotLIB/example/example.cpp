#ifndef PRUEBA_CPP
#define PRUEBA_CPP

#include "example.hpp"

int main(int argc, char* argv[])
{	
	RobotSimulator * simulator = new RobotSimulator();
	simulator->simStart();

	Joint * joint = new Joint(simulator,(char*)"joint7",45*RAD,-90*RAD);

	if (simulator->clientIdStatus())
	{
		simulator->simStartSimulation(simx_opmode_oneshot_wait);

		while (simulator->simGetConnectionId() != -1)
		{
			double position;
			cout << "Ingrese un valor para el angulo del motor #1 en grados ó -1 para terminar:" << endl;
			cin >> position;

			if(position == 999) break;
			else
			{
				clog << "joint_pos: " << joint->getJointCurrentPosition((char *)"RAD") << endl;
				double * pos;
				double * ori;
				pos = joint->getPosition(-1);
				ori = joint->getOrientation(-1);
				clog << "pos: " << pos[0] << " - " << pos[1] << " - " << pos[2] << endl;
				clog << "ori: " << ori[0] << " - " << ori[1] << " - " << ori[2] << endl;
				clog << "moving to position: " << position << endl;
				joint->setJointPosition(position,(char *)"DEG");
			} 
				

			sleep(2);
		}

		simulator->simStopSimulation(simx_opmode_oneshot_wait);
	}

	simulator->simFinish();

	delete(simulator);

	return(0);
}

#endif
