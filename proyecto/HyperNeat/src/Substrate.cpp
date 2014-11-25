#ifndef SUBSTRATE_CPP
#define SUBSTRATE_CPP

#include "Substrate.hpp"
#include "UserFunctions.hpp"
using namespace ANN_USM;

Substrate::Substrate(vector < double * > inputs, vector < double * > outputs){
	this->inputs = inputs;
	this->outputs = outputs;	
}
Substrate::Substrate(){

}
Substrate::~Substrate(){
	vector < vector < SpatialNode * > >().swap(nodes);
}
char * Substrate::SJsonDeserialize(char * substrate_info)
{
	const char delimeters[] = "{\"\t\n:,[ ]}";
	char * node_function;

	int n_layers = 0;
	int coordenate_type = 0;
	int n_nodes = 0;


	if(!strcmp(substrate_info,(char *)"n_layers"))
	{						
		substrate_info = strtok(NULL, delimeters);
		n_layers = atoi(substrate_info);
		substrate_info = strtok(NULL, delimeters);
	}
	if(!strcmp(substrate_info,(char *)"coordenate_type"))
	{
		substrate_info = strtok(NULL, delimeters);
		coordenate_type = atoi(substrate_info);
		substrate_info = strtok(NULL, delimeters);
	}
	if(!strcmp(substrate_info,(char *)"node_function"))
	{
		substrate_info = strtok(NULL, delimeters);
		node_function = substrate_info;
		substrate_info = strtok(NULL, delimeters);
	}
	if (!strcmp(substrate_info,(char *)"Layers"))
	{
		substrate_info = strtok(NULL, delimeters);
		for(int i = 0; i < n_layers; i++)
		{
			if(!strcmp(substrate_info,(char *)"n_nodes"))
			{
				substrate_info = strtok(NULL, delimeters);
				n_nodes = atoi(substrate_info);
				substrate_info = strtok(NULL, delimeters);
				if(i == 0 && n_nodes != (int)inputs.size())
				{
					cerr << "ERROR: The size of inputs vector mismatch with input nodes number in the substrate configuration" << endl;
					cerr << "The size of inputs vector is " << (int)inputs.size() << " but should be " << n_nodes << endl;
					exit(0);
				}
				else if(i == n_layers-1 && n_nodes != (int)outputs.size())
				{
					cerr << "ERROR: The size of outputs vector mismatch with output nodes number in the substrate configuration" << endl;
					cerr << "The size of outputs vector is " << (int)outputs.size() << " but should be " << n_nodes << endl;
					exit(0);
				}
			}
			if(!strcmp(substrate_info,(char *)"nodes_info"))
			{
				vector < SpatialNode * > aux1;
				for(int j = 0; j < n_nodes; j++)
				{
					int node_type;
					int IO_id;
					vector < double > coordenates; 

					substrate_info = strtok(NULL, delimeters);
					node_type = atoi(substrate_info);
					substrate_info = strtok(NULL, delimeters);
					IO_id = atoi(substrate_info);

					for(int k = 0; k < coordenate_type; k++)
					{	
						substrate_info = strtok(NULL, delimeters);
						coordenates.push_back(atof(substrate_info));
					}			

					aux1.push_back(new SpatialNode(node_type,i,coordenates, node_function));

					if(node_type == 0)
						aux1.at(j)->SetInputToInputNode(inputs.at(IO_id), IO_id);
					else
						if(node_type == 2)
							aux1.at(j)->SetOutputToOutputNode(outputs.at(IO_id), IO_id);

				}
				nodes.push_back(aux1);
				substrate_info = strtok(NULL, delimeters);
			}
		}
	}
	
	return substrate_info;
}

int Substrate::GetLayersNumber()
{
	return (int)nodes.size();
}

int Substrate::GetLayerNodesNumber(int layer_num)
{
	if(layer_num + 1 > (int)nodes.size() || layer_num < 0)
	{
		cerr << "ERROR: The layer " << layer_num << " does not exist" << endl;
		return 0;
	}

	return (int)nodes.at(layer_num).size();
}

SpatialNode * Substrate::GetSpatialNode(int layer_num, int node_num)
{
	if(layer_num + 1 > (int)nodes.size() || layer_num < 0)
	{
		cerr << "ERROR: The layer " << layer_num << " does not exist" << endl;
		return NULL;
	}

	if(node_num + 1 > (int)nodes.at(layer_num).size())
	{
		cerr << "ERROR: The node " << node_num << " in the layer "<< layer_num << " does not exist" << endl;
		return NULL;
	}

	return nodes.at(layer_num).at(node_num);
}

void Substrate::EvaluateSpatialNode(int layer_num)
{
	if(layer_num + 1 > (int)nodes.size() || layer_num < 0)
	{
		cerr << "ERROR: The layer " << layer_num << " does not exist" << endl;
		return;
	}

	for(int i = 0; i < (int)nodes.at(layer_num).size(); i++){
		nodes.at(layer_num).at(i)->OutputCalcule();
	}
}

void Substrate::ClearSpatialNodeInputs(int layer_num)
{	
	if(layer_num + 1 > (int)nodes.size() || layer_num < 0)
	{
		cerr << "ERROR: The layer " << layer_num << " does not exist" << endl;
		return;
	}

	for(int i = 0; i < (int)nodes.at(layer_num).size(); i++){
		nodes.at(layer_num).at(i)->ClearInputs();
	}
}

vector < string > Substrate::GetSubstrateOutputFunctions(string plataform)
{
	vector < string > functions;
	
	for(int i = 0; i < (int)nodes.size(); i++)
		for(int j = 0; j < (int)nodes.at(i).size(); j++)
			if(nodes.at(i).at(j)->GetNodeType() == 2)
				functions.push_back(nodes.at(i).at(j)->GetNodeFunction(plataform));
	
	return functions;
}
vector < int > Substrate::GetInputOrder()
{
	vector < int > input_order;
	
	for(int i = 0; i < (int)nodes.size(); i++)
		for(int j = 0; j < (int)nodes.at(i).size(); j++)
			if(nodes.at(i).at(j)->GetNodeType() == 0)
				input_order.push_back(nodes.at(i).at(j)->GetInputId());
	
	return input_order;
}
vector < int > Substrate::GetOutputOrder()
{
	vector < int > output_order;
	
	for(int i = 0; i < (int)nodes.size(); i++)
		for(int j = 0; j < (int)nodes.at(i).size(); j++)
			if(nodes.at(i).at(j)->GetNodeType() == 2)
				output_order.push_back(nodes.at(i).at(j)->GetOutputId());
	
	return output_order;
}

#endif