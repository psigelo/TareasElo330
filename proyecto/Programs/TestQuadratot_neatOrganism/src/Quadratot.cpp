#ifndef Quadratot_CPP
#define Quadratot_CPP

#include "Quadratot.hpp"

int main(int argc, char* argv[])
{	
	srand (time(0));

	RobotSimulator * simulator = new RobotSimulator();
	Fitness * fitness = new Fitness();
	bool flag = true;

	if(argc < 4)
	{
		cerr << "ERROR: The number of arguments is incorrect" << endl;
		return -1;	
	} 
	
	simulator->simStart();
	// ============= VREP INITIALIZATIONS ============= //			

	vector < Joint * > joints;
	vector < CollisionObject * > body_parts;
	Dummy * center_dummy = new Dummy(simulator, (char*)"center");

	double max_angle_limit[] = MAX_ANGLE_LIMIT;
	double min_angle_limit[] = MIN_ANGLE_LIMIT;

	for(int i = 0; i < N_LEGS*GRA_LIB + GRA_LIB_EXT; i++)
	{
		stringstream joint;
		joint << "joint" << i;
		joints.push_back(new Joint(simulator, (char*)joint.str().c_str(), max_angle_limit[i], min_angle_limit[i]));
	}

	for(int i = 0; i < 16; i++)
	{
		stringstream body_part;
		body_part << "Collision" << i << "#";
		body_parts.push_back(new CollisionObject(simulator, (char*)body_part.str().c_str()));
	}
	
	// ================================================ //

	// ========== HYPERNEAT INITIALIZATIONS =========== //

	vector < double * > next;
	vector < double * > pass;

	for(int i = 0; i < N_LEGS*GRA_LIB + GRA_LIB_EXT; i++)
	{
		double joint_pos = joints.at(i)->getJointInitialPosition((char *)"SCALE");

		double * next_pos = new double(joint_pos);
		double * pass_pos = new double(joint_pos);

		next.push_back(next_pos);
		pass.push_back(pass_pos);
	}

	for(int i = 0; i < ADITIONAL_HYPERNEAT_INPUTS; i++)
	{
		double * aux_pass = new double(SIN(0,i));
		pass.push_back(aux_pass);
	}

	HyperNeat * hyperneat = new HyperNeat(pass, next, argv[1], argv[2], argv[3]);

	// ================================================ //

	if (simulator->clientIdStatus())
	{
		while (simulator->simGetConnectionId() != -1)
		{
			double sim_time = 0;					
			int step = 0;
			vector < double > sum_next ((int)joints.size(),0.0);

			fitness->resetPopulationValues();

			if(!hyperneat->CreateSubstrateConnections(argv[3])) continue;

			for(int i = 0; i < (int)joints.size(); i++)
			{
				double joint_pos = joints.at(i)->getJointInitialPosition((char *)"SCALE");
				joints.at(i)->setJointInitialPosition();
				*next.at(i) = joint_pos;
				*pass.at(i) = joint_pos;
			}

			simulator->simStartSimulation(simx_opmode_oneshot_wait);

			while(sim_time < TIME_SIMULATION)
			{						
				for(int i = 0; i < ADITIONAL_HYPERNEAT_INPUTS; i++)
				{
					*pass.at((int)joints.size()+i) = SIN(sim_time,i);
				}

				hyperneat->EvaluateSubstrateConnections();

				for(int i = 0; i < (int)joints.size(); i++)
				{
					sum_next.at(i) = sum_next.at(i) + *next.at(i);
					*pass.at(i) = *next.at(i);
				}		
				step++;
				
				if(step >= STEP_CALC)
				{
					simulator->simPauseCommunication(1);

					for(int i = 0; i < (int)joints.size(); i++)
					{
						joints.at(i)->setJointPosition((double)sum_next.at(i)/STEP_CALC,(char *)"SCALE");
						sum_next.at(i) = 0;
					}

					simulator->simPauseCommunication(0);

					for(int i = 4; i < (int)body_parts.size(); i++)
					{
						if(body_parts.at(i)->getCollisionState() != 0)
						{
							flag = false;
							break;
						}
					}

					if(sim_time > TIME_INIT_MEASURING)
					{
						fitness->measuringValues(joints, center_dummy);
					}				

					step = 0;
				}

				usleep((int)(DELTA_T*1000000.0));
				sim_time += DELTA_T;
			}

			simulator->simStopSimulation(simx_opmode_oneshot_wait);

			break;
		}
	}

	if(flag)
	{						
		fitness->calcFitness();
		//fitness->getFitness();

		clog << "Joint direction change number: " << fitness->getJointDirectionChangeNumber() << endl;
		clog << "Joint distance change number frecuency: " << fitness->getFrecuency() << endl;
		clog << "Traveled distance : " << fitness->getDistance() << endl;
		clog << "Distance penalization : " << fitness->getDistancePenalization() << endl;
		clog << "Penalized distance : " << fitness->getPenalizedDistance() << endl;
		clog << "Fitness: " << fitness->getFitness() << endl;
	}

	//ENVIAR FITNESS!!!!!

	simulator->simFinish();

	delete(simulator);
	delete(hyperneat);
	
	return(0);

}

#endif