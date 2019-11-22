#include "node.hpp"


Node::Node():
	parent_{nullptr},
	children_{},
	name_{"default name"},
	path_{},
	depth_{} {}

Node::~Node(){}

Node::Node(Node* const& parent, string const& name, string const& path, int const& depth):
	parent_{parent},
	children_{},
	name_{name},
	path_{path},
	depth_{depth} {}

Node* Node::getParent() const{
	return parent_;
}

void Node::setParent(Node* const& parent){
	this -> parent_ = parent;
}

Node Node::getChildren(string const& name) const{

	for(auto iter=children_.begin(); iter!=children_.end(); iter++){  
		if( (*iter)->getName().compare(name)==0 ){          // *iter = Node*
			cout<< "Planet "+name+" will be returned."<<endl;
			return *(*iter);
		}
	}
	cout<<"Planet "+name+" can not be found."<<endl;
	return Node();  // not NULL, return empty constructor
}

list<Node*> Node::getChildrenList() const{
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

glm::fmat4 Node::getLocalTransform() const{
	return local_transformation_;
}

void Node::setLocalTransform(glm::fmat4 const& local_transformation){
	this-> local_transformation_ = local_transformation;
}

glm::fmat4 Node::getWorldTransform() const{
	return world_transformation_;
}

void Node::setWorldTransform(glm::fmat4 const& world_transformation){
	this-> world_transformation_ = world_transformation;
} 

void Node::addChildren(Node* const& children){
	children_.push_back(children);
}

Node Node::removeChildren(string const& name){
	
	list<Node*>::iterator iter;
	for(iter=children_.begin(); iter!=children_.end(); iter++){
		if( (*iter)->getName().compare(name)==0){
			auto node = *(*iter);    // (*iter) -> Node*; *(*iter) -> Node; 
			children_.erase(iter); // delete specific Node* in the list<Node*>
			// list.erase(iterator)
			cout<<"Planet "+name+" will be deleted."<<endl;
			return node;    // return deleted Node
		}
	}
	cout<<"Planet "+name+" can not be found."<<endl;	

	return Node();
}

