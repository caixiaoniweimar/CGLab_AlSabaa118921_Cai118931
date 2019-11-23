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
	glm::fmat4 getProjectionMatrix() const;
	void setProjectionMatrix(glm::fmat4 const& matrix);  

	ostream& print(ostream& os) const override;

private:
	bool isPerspective_;
	bool isEnabled_;
	glm::fmat4 projectionMatrix_;

};
#endif