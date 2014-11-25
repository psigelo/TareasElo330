#ifndef Quadratot_CPP
#define Quadratot_CPP

#include "Quadratot.hpp"

int main(int argc, char* argv[])
{	
	srand (time(0));

	SimFiles * simfile = new SimFiles(); 
	Fitness * fitness = new Fitness();

	if(argc < 4)
	{
		cerr << "ERROR: The number of arguments is incorrect" << endl;
		return -1;	
	} 

	// ========== HYPERNEAT INITIALIZATIONS =========== //

	HyperNeat * hyperneat = new HyperNeat(pass, next, argv[1], argv[2], argv[3]);

	// ================================================ //

	for(int g = 0; g < hyperneat->cppn_neat->GENERATIONS; g++)
	{
		fitness->resetGenerationValues();

		for(int p = 0; p < hyperneat->cppn_neat->POPULATION_MAX; p++)
		{
			fitness->resetPopulationValues();






			//ACA SE DEBE LLAMAR AL QLO DEL CLIENTE!!!		





			

			hyperneat->HyperNeatFitness(fitness->getFitness(), p);	
		}				
		hyperneat->HyperNeatEvolve();
		simfile->addFileFitness(fitness, g);
		simfile->addFileFrecuency(fitness, g);
	}
	clog << endl << "BEST RESULT ------------------------------------" << endl << endl;
	clog << "\t-> " << hyperneat->cppn_neat->fitness_champion << endl << endl;

	delete(fitness);
	delete(simfile);
	delete(hyperneat);
	
	return(0);

}

#endif