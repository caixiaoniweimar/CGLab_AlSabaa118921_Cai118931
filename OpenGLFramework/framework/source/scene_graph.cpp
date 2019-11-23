#include "scene_graph.hpp"
  
SceneGraph* SceneGraph::scene_instance_=nullptr;
// private constructure, no object will be created
SceneGraph::SceneGraph():
	name_{"default scene"},
	root_{new Node()} {}

SceneGraph::~SceneGraph(){}


SceneGraph* SceneGraph::getInstance() {
	if(scene_instance_==nullptr){
		scene_instance_  = new SceneGraph();
	//scene_instance_->setName("default scene");
	//Scene_instance_->setRoot(new Node());
	}
	return scene_instance_;
}

string SceneGraph::getName() const{
	return name_;
}

shared_ptr<Node> SceneGraph::getRoot() const{
	return root_;
}

void SceneGraph::setName(string const& name){
	this->name_ = name;
}

void SceneGraph::setRoot(shared_ptr<Node>  const& node){
	this->root_ = node;
}

void SceneGraph::printGraph(){
	cout<<"check"<<endl;
	printNode(root_, root_->getDepth());
}

void SceneGraph::printNode(shared_ptr<Node> const& node, int const& depth){
	for(int i=0; i<depth; ++i){
		cout<<" ";
	}
	node -> print(cout);

	list<shared_ptr<Node>> children_list = node->getChildrenList();
	for(auto const& child: children_list){
		printNode(child, depth+1);
	}
}

