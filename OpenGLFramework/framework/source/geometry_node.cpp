#include "geometry_node.hpp"

GeometryNode::GeometryNode():
	Node{},
	model_{},
	color_{glm::fvec3{1.0f, 1.0f, 1.0f}} {} //White
GeometryNode::~GeometryNode(){}

GeometryNode::GeometryNode(shared_ptr<Node> const& parent, string const& name, string const& path, int const& depth, 
		   float const& size, float const& speed, float const& distance, glm::fvec3 const& color, string const& textureFilePath):
	Node{parent, name, path, depth, size, speed, distance},
	model_{},
	color_{color},
	textureFilePath_{textureFilePath} {}

model GeometryNode::getGeometry() const{
	return model_;
}

void GeometryNode::setGeometry(model const& model){
	this -> model_ = model;
}

//Assignment3
glm::fvec3 GeometryNode::getColor() const{
	return color_;
}

void GeometryNode::setColor(glm::fvec3 const& color){
	this -> color_ = color;
}

ostream& GeometryNode::print(ostream& os) const{
	Node::printPlanet(os);
	os<< " Color: "<<color_.x<<", "<<color_.y<<", "<<color_.z<<endl;
	return os;
}

//Assignment 4
string GeometryNode::getTextureFilePath() const{
	return textureFilePath_;
}

void GeometryNode::setTextureFilePath(string const& textureFilePath){
	this -> textureFilePath_ = textureFilePath;
}

texture_object GeometryNode::getTextureObject() const{
	return textureObject_;
}

void GeometryNode::setTextureObject(texture_object const& textureObject){
	this -> textureObject_ = textureObject;
}

pixel_data GeometryNode::getPixelData() const{
	return pixelData_;
}
/*
load png files with texture_loader::file() function, the pixel_data structure is returned, and it contains
all variables necessary for a pixel transfer
*/
void GeometryNode::setPixelData(string const& textureFilePath){
	this -> pixelData_ = texture_loader::file(textureFilePath);
}