#include "geometry_node.hpp"

GeometryNode::GeometryNode():
	Node(),
	model_{} {}
GeometryNode::~GeometryNode(){}

model GeometryNode::getGeometry() const{
	return model_;
}

void GeometryNode::setGeometry(model const& model){
	this -> model_ = model;
}