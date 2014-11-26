#ifndef Quadratot_HPP
#define Quadratot_HPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <cmath>
#include <time.h>
#include <sys/time.h>
#include <fstream>
#include "HyperNeat.hpp"
#include "Joint.hpp"
#include "Dummy.hpp"
#include "Object.hpp"
#include "CollisionObject.hpp"
#include "RobotSimulator.hpp"
#include "CalcFunctions.hpp"
#include "Fitness.hpp"
#include "SimFiles.hpp"
#include "Simulation.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <poll.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>

using namespace std;
using namespace ANN_USM;

#define NAME_TEST "QUADRATOT"
#define PORTNUMBER  12345
#define TIME_OUT_POLL_MSEC 100

FILE * archivoHYPERNEAT;
FILE * archivoNEAT;
int s, n, ns;

#endif