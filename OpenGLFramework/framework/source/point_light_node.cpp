#include "point_light_node.hpp"

PointLightNode::PointLightNode():
	lightIntensity_{0.0f},
	lightColor_{glm::fvec3{0.0f, 0.0f, 0.0f}} {}

PointLightNode::~PointLightNode(){}

PointLightNode::PointLightNode(shared_ptr<Node> const& parent,string const& name, string const& path, float const& lightIntensity, glm::vec3 const& lightColor):
	Node{parent,name,path},
	lightIntensity_{lightIntensity},
	lightColor_{lightColor} {}

float PointLightNode::getLightIntensity() const{
	return lightIntensity_;
}

glm::fvec3 PointLightNode::getLightColor() const{
	return lightColor_;
}

void PointLightNode::setLightIntensity(float const& lightIntensity){
	this -> lightIntensity_ = lightIntensity;
}

void PointLightNode::setLightColor(glm::vec3 const& lightColor){
	this -> lightColor_ = lightColor;
}

glm::vec3 PointLightNode::cal_lightColor() const{
	return lightColor_*lightIntensity_;
}