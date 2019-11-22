#include "camera_node.hpp"

CameraNode::CameraNode():
	Node(),
	isPerspective_{},
	isEnabled_{},
	projectionMatrix_{} {}

CameraNode::~CameraNode(){}

bool CameraNode::getPerspective() const{
	return isPerspective_;
}

bool CameraNode::getEnabled() const{
	return isEnabled_;
}

void CameraNode::setEnabled(bool const& isEnabled){
	this-> isEnabled_ = isEnabled;
}

glm::fmat4 CameraNode::getProjectionMatrix() const{
	return projectionMatrix_;
}

void CameraNode::setProjectionMatrix(glm::fmat4 const& matrix){
	this-> projectionMatrix_ = matrix;
}

void CameraNode::setPerspective(bool const& isPerspective) {
	this -> isPerspective_ = isPerspective;
}