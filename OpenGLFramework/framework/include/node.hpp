#ifndef NODE_HPP
#define NODE_HPP

#include <cmath>
#include <string>
#include <list>
#include <iostream>
using namespace std;
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <memory>

class Node{
public:

	Node();
	~Node();
	Node(shared_ptr<Node> const& parent, string const& name, string const& path, int const& depth, 
		   float const& size, float const& speed, float const& distance);
	Node(shared_ptr<Node> const& parent, string const& name, string const& path, float const& speed);

	shared_ptr<Node> getParent() const;
	void setParent(shared_ptr<Node> const& parent);

	Node getChildren(string const& name) const;
	list<shared_ptr<Node>> getChildrenList() const;
	
	string getName() const;
	string getPath() const;
 	int getDepth() const;
 	void setDapth(int const& depth);
 	glm::fmat4 getLocalTransform() const;
 	void setLocalTransform(glm::fmat4 const& local_transformation);
 	glm::fmat4 getWorldTransform() const;
 	void setWorldTransform(glm::fmat4 const& world_transformation);

 	float getSpeed() const;
 	void setSpeed(float const& speed);
 	
 	glm::fvec3 getDistance() const;
 	void setDistance(glm::fvec3 const& distance);

 	float getSize() const;
 	void setSize(float const& size);
 	
 	void addChildren(shared_ptr<Node>const& children);
 	Node removeChildren(string const& name);

 	virtual ostream& print(ostream& os) const;

private:
	shared_ptr<Node> parent_;
	list<shared_ptr<Node>> children_;
	string name_;
	string path_;
	int depth_;

	// extra parameters
	float speed_;
	glm::fvec3 distance_;
	float size_;

	glm::fmat4 local_transformation_{1.0f, 0.0f, 0.0f, 0.0f,
									 0.0f, 1.0f, 0.0f, 0.0f,
									 0.0f, 0.0f, 1.0f, 0.0f,
									 0.0f, 0.0f, 0.0f, 1.0f};
	glm::fmat4 world_transformation_{1.0f, 0.0f, 0.0f, 0.0f,
									 0.0f, 1.0f, 0.0f, 0.0f,
									 0.0f, 0.0f, 1.0f, 0.0f,
									 0.0f, 0.0f, 0.0f, 1.0f};
/* set distance_ as fvec3:
for instance, for method: glm::fmat4 translate(glm::fmat4 const& matrix, glm::fvec3 const& vector)
*/
};
ostream& operator<<(ostream& os, Node const& node);
#endif