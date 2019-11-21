#ifndef SCENE_GRAPH_HPP
#define SCENE_GRAPH_HPP
#include <iostream>
#include <string>
using namespace std;
#include "node.hpp"
class SceneGraph{

public :

	SceneGraph* getInstance();  // STATIC
	string getName() const;
	Node getRoot() const;
	string printGraph();

private:

	SceneGraph();
	~SceneGraph();
	void setName(string const& name);
	void setRoot(Node const& node);

private:
	static SceneGraph* scene_instance_;
	string name_;
	Node root_;
};
// call: SceneGraph* scene = SceneGraph::getInstance()
#endif