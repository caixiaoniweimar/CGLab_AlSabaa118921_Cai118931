#ifndef CAMERA_NODE_HPP
#define CAMERA_NODE_HPP
#include <iostream>
#include <string>
using namespace std;
#include "node.hpp"
#include <glm/glm.hpp>

class CameraNode : public Node{

public:
	CameraNode();
	~CameraNode();
	bool getPerspective() const;
	bool getEnabled() const;
	void setPerspective(bool const& isPerspective);
	void setEnabled(bool const& isEnabled);
	glm::mat4 getProjectionMatrix() const;
	void setProjectionMatrix(glm::mat4 const& matrix);  

private:
	bool isPerspective_;
	bool isEnabled_;
	glm::mat4 projectionMatrix_;

};
#endif