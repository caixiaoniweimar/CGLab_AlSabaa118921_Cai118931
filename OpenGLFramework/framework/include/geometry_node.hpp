#ifndef GEOMETRY_NODE_HPP
#define GEOMETRY_NODE_HPP
#include <iostream>
#include <string>
using namespace std;
#include "model.hpp"
#include "node.hpp"

class GeometryNode : public Node{

public:
	GeometryNode();
	~GeometryNode();
	model getGeometry() const;
	void setGeometry(model const& model);

private:
	model model_;

};
#endif