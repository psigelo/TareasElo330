#ifndef SPATIALNODE_H
#define SPATIALNODE_H

#include <vector>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;
/**
 * \namespace ANN_USM
 * \brief Dedicated to artificial intelligence development in Santa María University.
 */
namespace ANN_USM{
	/**
	 * \class SpatialNode
	 * \brief The SpatialNode class is used to create nodes in a neural network.
	 */
	class SpatialNode
	{	
		// node sheet id value 
		int sheet_id;

		// node type value. Input: 0, Hidden: 1, Output: 2 
		int node_type;

		// coordenate node vector 
		vector < double > coordenates;

		// vector of weight associated with each input node connection 
		vector < double > inputs_weight;

		// vector of input node pointer 
		vector < SpatialNode * > inputs_nodes;

		// substrate input pointer. Only if the node is in input mode 
		double * input;

		// Id value of Substrate input 
		int input_id;

		// output node pointer 
		double * output;

		// Id value of Substrate output 
		int output_id;

		bool active;

		char node_function[40];
		
	public:
		/**
		 * \brief Constructor with parameters
		 * \param node_type Node type value, 0 for input, 1 for hidden and 2 for output type
		 * \param sheet_id Sheet id value
		 * \param coordenates Cartesian coordinate vector of node
		 */
		SpatialNode(int node_type, int sheet_id, vector < double > coordenates, char * node_function);
		/**
		 * \brief Void constructor
		 */
		SpatialNode();
		/**
		 * \brief Destructor
		 */
		~SpatialNode();
		/**
		 * \brief Assign input to input type node
		 * \param input Input pointer
		 * \param input_id HyperNeat input id
		 */
		void SetInputToInputNode(double * input, int input_id);
		/**
		 * \brief Assign output to output type node
		 * \param output Output pointer
		 * \param output_id HyperNeat output id
		 */
		void SetOutputToOutputNode(double * output, int output_id);
		/**
		 * \brief Add input to node from other node
		 * \param input_node Input node pointer
		 * \param input_weight Weight associated to connection
		 */
		void AddInputToNode(SpatialNode * input_node, double input_weight);
		/**
		 * \brief Calcule of node output value
		 */
		void OutputCalcule();
		/**
		 * \brief Get coordenates node
		 * \return Coordenate node vector
		 */
		vector < double > GetCoordenates();
		/**
		 * \brief Get node type
		 * \return node type value
		 */
		int GetNodeType();
		/**
		 * \brief Get sheet node id
		 * \return Sheet node id value
		 */
		int GetSheetNodeId();
		/**
		 * \brief Get node output
		 * \return node output value
		 */
		double GetOuput();
		/**
		 * \brief Clear inputs node and weight vector
		 */
		void ClearInputs();
		/**
		 * \brief Get output function from this node recursively backwards
		 * \param plataform Name of the target platform function
		 * \return Function in string format
		 */
		string GetNodeFunction(string plataform);
		/**
		 * \brief Get input node id
		 */
		int GetInputId();
		/**
		 * \brief Get output node id
		 */
		int GetOutputId();

		bool ActiveNode();
	};
}
#endif
