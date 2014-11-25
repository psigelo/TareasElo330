#ifndef USERFUNCTIONS_H
#define USERFUNCTIONS_H

#include "HyperNeat.hpp"
#include <vector>
using namespace std;
using namespace ANN_USM;

// --------- SIGMOID FUNCTION DEFINE --------- //
#define SIGMOID_CONST 4.9 
#define SIGMOID(X) (double)((1.0 / (1.0 + exp(-SIGMOID_CONST*X)))-0.5)*2.0
// ---------------- FOR OCTAVE --------------- //
#define OCTAVE_SIGMOID_STATEMENT "function [y] = SIGMOID(x)"
#define OCTAVE_SIGMOID_CONST_LETTER "K"
#define OCTAVE_SIGMOID_CONST SIGMOID_CONST 
#define OCTAVE_SIGMOID_FUNC "y = (1/(1+exp(-K*x)) - 0.5)*2"
// ---------------- FOR C++ -- --------------- //
#define SIGMOID_STRING "#define SIGMOID(X) (double)((1.0 / (1.0 + exp(-SIGMOID_CONST*X)))-0.5)*2.0"
#define SIGMOID_CONST_STRING "#define SIGMOID_CONST 4.9"
// =========================================== //

#define HYPERNEAT_TEST "QUADRATOT"

namespace ANN_USM{

	void GetNodeFunction(string plataform);
	double OutputNodeFunction(char * node_function, double input);

}
#endif