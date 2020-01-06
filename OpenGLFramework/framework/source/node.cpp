#include "node.hpp"


Node::Node():
	parent_{nullptr},
	children_{},
	name_{"root"},
	path_{"root"},
	depth_{0}, 
	speed_{0.0f},
	distance_{glm::fvec3{0.0f,0.0f,0.0f}},
	size_{0.0f} {}

Node::~Node(){}

Node::Node(shared_ptr<Node> const& parent, string const& name, string const& path, int const& depth, 
		   float const& size, float const& speed, float const& distance):
	parent_{parent},
	children_{},
	name_{name},
	path_{path},
	depth_{depth},
	speed_{speed},
	distance_{glm::fvec3{distance, 0.0f, 0.0f}},
	size_{size} {}

//constructor for holder_node, parent=root
Node::Node(shared_ptr<Node> const& parent, string const& name, string const& path,float const& speed):
	parent_{parent},
	children_{},
	name_{name},
	path_{path},
	depth_{1},
	speed_{0.0f},
	distance_{glm::fvec3{0.0f,0.0f,0.0f}},
	size_{0.0f} {}

//constructor for point_light
Node::Node(shared_ptr<Node> const& parent, string const& name, string const& path):
	parent_{parent},
	children_{},
	name_{name},
	path_{path},
	depth_{1}, 
	speed_{0.0f},
	distance_{glm::fvec3{0.0f,0.0f,0.0f}},
	size_{0.0f}{}


shared_ptr<Node> Node::getParent() const{
	return parent_;
}

void Node::setParent(shared_ptr<Node> const& parent){
	this -> parent_ = parent;
}

Node Node::getChildren(string const& name) const{

	for(auto iter=children_.begin(); iter!=children_.end(); iter++){  
		if( (*iter)->getName()==name ){          // *iter = Node*
			cout<< "Planet "<< name <<" will be returned."<<endl;
			return *(*iter);
		}
	}
	cout<<"Planet "<<name<<" can not be found."<<endl;
	return Node();  // not NULL, return empty constructor
}

list<shared_ptr<Node>> Node::getChildrenList() const{
	return children_;
}

string Node::getName() const{
	return name_;
}

string Node::getPath() const{
	return path_;
} 

void Node::setDapth(int const& depth){
	this -> depth_ = depth;
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

float Node::getSpeed() const{
	return speed_;
}

void Node::setSpeed(float const& speed){
	this -> speed_ = speed;
}

glm::fvec3 Node::getDistance() const{
	return distance_;
}

float Node::getDistanceX() const{
	return distance_.x;
}

void Node::setDistance(glm::fvec3 const& distance){
	this -> distance_ = distance;
}

float Node::getSize() const{
	return size_;
}

void Node::setSize(float const& size){
	this -> size_ = size;
}

void Node::addChildren(shared_ptr<Node> const& children){
	children_.push_back(children);
	cout<<"Planet: "<< children->getName() <<" will be added."<<endl;
}

Node Node::removeChildren(string const& name){
	
	list<shared_ptr<Node>>::iterator iter;
	for(iter=children_.begin(); iter!=children_.end(); iter++){
		if( (*iter)->getName().compare(name)==0){
			auto node = *(*iter);    // (*iter) -> Node*; *(*iter) -> Node; 
			children_.erase(iter); // delete specific Node* in the list<Node*>
			// list.erase(iterator)
			cout<<"Planet "<<name<<" will be deleted."<<endl;
			return node;    // return deleted Node
		}
	}
	cout<<"Planet "<<name<<" can not be found."<<endl;	

	return Node();
}

ostream& Node::print(ostream& os) const{
	os<< "Name: "<<name_<< " Path: "<<path_<< " Depth: "<<depth_<<" Speed: "<<speed_<< " Size: "<<size_<<endl;
	return os;
}
ostream& Node::printPlanet(ostream& os) const{
	os<< "Name: "<<name_<< " Path: "<<path_<< " Depth: "<<depth_;//<<" Speed: "<<speed_<< " Size: "<<size_<<endl;
	return os;	
}

ostream& operator<<(ostream& os, Node const& node){
	ostream::sentry const ostream_sentry(os);
	return ostream_sentry ? node.print(os) : os;
}
