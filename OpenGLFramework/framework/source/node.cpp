#include "node.hpp"


Node::Node():
	parent_{},
	children_{},
	name_{},
	path_{},
	depth_{},
	local_transformation_{},
	world_transformation_{} {}

Node::~Node(){}

Node* Node::getParent() const{
	return parent_;
}

void Node::setParent(Node* const& parent){
	this -> parent_ = parent;
}

Node Node::getChildren(string const& name) const{

	//list<Node>::iterator iter;
	//auto iter;
	for(auto iter=children_.begin(); iter!=children_.end(); iter++){
		if( (*iter).getName().compare(name)==0 ){
			return *iter;
		}
	}
	return Node();  // not NULL, return empty constructor
}

list<Node> Node::getChildrenList() const{
	return children_;
}

string Node::getName() const{
	return name_;
}

string Node::getPath() const{
	return path_;
} 

int Node::getDepth() const{
	return depth_;
}

glm::mat4 Node::getLocalTransform() const{
	return local_transformation_;
}

void Node::setLocalTransform(glm::mat4 const& local_transformation){
	this-> local_transformation_ = local_transformation;
}

glm::mat4 Node::getWorldTransform() const{
	return world_transformation_;
}

void Node::setWorldTransform(glm::mat4 const& world_transformation){
	this-> world_transformation_ = world_transformation;
} 

void Node::addChildren(Node const& children){
	children_.push_back(children);
}

Node Node::removeChildren(string const& name){
	
	list<Node>::iterator iter;
	for(iter=children_.begin(); iter!=children_.end(); iter++){
		if((*iter).getName().compare(name)==0){
			Node node =*iter;    // QUESTION
			children_.erase(iter);
			return node;
		}
	}	

	return Node();
}


//get the Node according to the index
/**
	list<Node>::iterator iterator = children_.begin();
	for(int i=0; i<index; i++){
		++iterator;
	}
	return *iterator;
*/