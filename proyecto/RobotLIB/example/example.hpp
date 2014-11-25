#ifndef PRUEBA_HPP
#define PRUEBA_HPP

#include <stdio.h>
#include <vector>
#include <unistd.h>
#include <cmath>
#include <sstream>

#include "Joint.hpp"
#include "Dummy.hpp"
#include "Object.hpp"
#include "RobotSimulator.hpp"

using namespace std;
using namespace ANN_USM;

#define RAD M_PI/180.0

#define MAX_ANGLE_1 160.0*RAD
#define MIN_ANGLE_1 -160.0*RAD
#define MAX_ANGLE_2 0.2
#define MIN_ANGLE_2 0
#define MAX_ANGLE_3 180.0*RAD
#define MIN_ANGLE_3 -180.0*RAD

#define MAX_ANGLE_LIMIT {MAX_ANGLE_1, MAX_ANGLE_1, MAX_ANGLE_2, MAX_ANGLE_3}
#define MIN_ANGLE_LIMIT {MIN_ANGLE_1, MIN_ANGLE_1, MIN_ANGLE_2, MIN_ANGLE_3}

#endif