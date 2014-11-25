#ifndef DUMMY_HPP
#define DUMMY_HPP

#include "Object.hpp"
#include "RobotSimulator.hpp"

namespace ANN_USM
{
	class Dummy : public Object
	{
		double * velocity;

	public:

		Dummy(RobotSimulator * simulator, char name[]);
		Dummy();
		~Dummy();

		void setVelocity(float velocity[3]);

		double * getVelocity();
	};
}

#endif