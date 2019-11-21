#ifndef NODE_HPP
#define NODE_HPP

#include <list>
#include <iostream>
#include <string>
using namespace std;
#include <glm/glm.hpp>

class Node{
public:

	Node();
	~Node();
	//Node(Node const& parent, list<Node> const& children, string const& name, string const& path, int const& depth)

	Node* getParent() const;
	void setParent(Node* const& parent);
	Node getChildren(string const& name) const;
	list<Node> getChildrenList() const;
	string getName() const;
	string getPath() const;
 	int getDepth() const;
 	glm::mat4 getLocalTransform() const;
 	void setLocalTransform(glm::mat4 const& local_transformation);
 	glm::mat4 getWorldTransform() const;
 	void setWorldTransform(glm::mat4 const& world_transformation);
 	
 	void addChildren(Node const& children);
 	Node removeChildren(string const& name);

private:
	Node* parent_;
	list<Node> children_;
	string name_;
	string path_;
	int depth_;
	glm::mat4 local_transformation_;
	glm::mat4 world_transformation_{1,0,0,0,
								   0,1,0,0,
								   0,0,1,0,
								   0,0,0,1};

};
#endif