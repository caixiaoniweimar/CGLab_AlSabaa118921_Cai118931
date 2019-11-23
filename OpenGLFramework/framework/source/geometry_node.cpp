#include "geometry_node.hpp"

GeometryNode::GeometryNode():
	Node{},
	model_{} {}
GeometryNode::~GeometryNode(){}

GeometryNode::GeometryNode(shared_ptr<Node> const& parent, string const& name, string const& path, int const& depth, 
		   float const& size, float const& speed, float const& distance):
	Node{parent, name, path, depth, size, speed, distance},
	model_{} {}

model GeometryNode::getGeometry() const{
	return model_;
}

void GeometryNode::setGeometry(model const& model){
	this -> model_ = model;
}

ostream& GeometryNode::print(ostream& os) const{
	Node::print(os);
	return os;
}