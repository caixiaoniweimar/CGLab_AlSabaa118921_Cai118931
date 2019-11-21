#include "scene_graph.hpp"

SceneGraph* SceneGraph::getInstance() {
	if(!scene_instance_){
		scene_instance_ = new SceneGraph();
	}
	return scene_instance_;
}

// private constructure, no object will be created
SceneGraph::SceneGraph():
	name_{"default name"},
	root_{} {}

SceneGraph::~SceneGraph(){}

string SceneGraph::getName() const{
	return name_;
}

Node SceneGraph::getRoot() const{
	return root_;
}

void SceneGraph::setName(string const& name){
	this->name_ = name;
}

void SceneGraph::setRoot(Node const& node){
	this->root_ = node;
}