#ifndef NODE_HPP
#define NODE_HPP

#include <list>
#include <iostream>
#include <string>
using namespace std;
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

class Node{
public:

	Node();
	~Node();
	Node(Node* const& parent, string const& name, string const& path, int const& depth);

	Node* getParent() const;
	void setParent(Node* const& parent);

	Node getChildren(string const& name) const;
	list<Node*> getChildrenList() const;
	
	string getName() const;
	string getPath() const;
 	int getDepth() const;
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
 	
 	void addChildren(Node* const& children);
 	Node removeChildren(string const& name);

private:
	Node* parent_;
	list<Node*> children_;
	string name_;
	string path_;
	int depth_;

	// extra parameters
	float speed_;
	glm::fvec3 distance_;
	float size_;
	
	glm::fmat4 local_transformation_{1,0,0,0,
									 0,1,0,0,
									 0,0,1,0,
									 0,0,0,1};
	glm::fmat4 world_transformation_{1,0,0,0,
									 0,1,0,0,
									 0,0,1,0,
									 0,0,0,1};
/* set distance_ as fvec3:
for instance, for method: glm::fmat4 translate(glm::fmat4 const& matrix, glm::fvec3 const& vector)
*/
};
#endif