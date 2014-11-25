#ifndef JOINT_HPP
#define JOINT_HPP

#include "Object.hpp"
#include "RobotSimulator.hpp"

#include <cstring>
#include <cmath>

#define PRECISION 6
#define RAD_CONV (double)(M_PI/180.0)

namespace ANN_USM
{	
	class Joint : public Object
	{
		double * positions;
		double current_position;
		double initial_position;
		double max_value;
		double min_value;

		int pass_slope_sign;
		int next_slope_sign;
		bool joint_change_direction;

		double truncValue(double value, int precision);
		double convertToRadFrom(const char * unit, double value);
		double convertFromRadTo(const char * unit, double value);

	public:

		Joint(RobotSimulator * simulator, char name[], double max_value, double min_value, double position);
		Joint(RobotSimulator * simulator, char name[], double max_value, double min_value);
		~Joint();

		void setJointPosition(double position, const char * unit);
		void setJointInitialPosition();
		void setJointInitialPosition(double position, const char * unit);

		double getJointCurrentPosition(const char * unit);
		double getJointGoalPosition(const char * unit);
		double getJointInitialPosition(const char * unit);
		double getMaxAngle();
		double getMinAngle();
		bool getJointChangeDirection();
	};
}

#endif