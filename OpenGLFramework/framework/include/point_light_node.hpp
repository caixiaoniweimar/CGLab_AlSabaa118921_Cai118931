#ifndef POINT_LIGHT_NODE_HPP
#define POINT_LIGHT_NODE_HPP

#include <cmath>
#include <string>
#include <list>
#include <iostream>
using namespace std;
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include "node.hpp"

class PointLightNode : public Node{

public:
	PointLightNode();
	~PointLightNode();
	PointLightNode(shared_ptr<Node> const& parent, string const& name, string const& path, float const& lightIntensity, glm::vec3 const& lightColor);

	float getLightIntensity() const;
	glm::vec3 getLightColor() const;

	void setLightIntensity(float const& lightIntensity);
	void setLightColor(glm::vec3 const& lightColor);

	glm::vec3 cal_lightColor() const;

private:
	float lightIntensity_;
	glm::vec3 lightColor_;
};
#endif