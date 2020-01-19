#ifndef GEOMETRY_NODE_HPP
#define GEOMETRY_NODE_HPP
#include <iostream>
#include <string>
using namespace std;
#include "model.hpp"
#include "node.hpp"
#include "structs.hpp"
#include "pixel_data.hpp"
#include "texture_loader.hpp"
#include <glm/glm.hpp>

class GeometryNode : public Node{

public:
	GeometryNode();
	~GeometryNode();
	GeometryNode(shared_ptr<Node> const& parent, string const& name, string const& path, int const& depth, 
		   float const& size, float const& speed, float const& distance, glm::fvec3 const& color, string const& textureFilePath, bool const& ifNormalMapping);
	model getGeometry() const;
	void setGeometry(model const& model);

	glm::fvec3 getColor() const;
	void setColor(glm::fvec3 const& color);

	ostream& print(ostream& os) const override;

//Assignment 4
	string getTextureFilePath() const;
	void setTextureFilePath(string const& textureFilePath);

//struct texture_object in structs.hpp
	texture_object getTextureObject()  const;
	void setTextureObject(texture_object const& textureObject);

	pixel_data getPixelData() const;
	void setPixelData(string const& textureFilePath);

	bool getIfNormalMapping() const;
	void setIfNormalMapping(bool const& ifNormalMapping);
private:
	model model_;
	glm::fvec3 color_;

//Assignment 4
	string textureFilePath_;
// create a texture object from the pixel_data struct for each planet
	texture_object textureObject_;
	pixel_data pixelData_;
	bool ifNormalMapping_=false;

};
#endif