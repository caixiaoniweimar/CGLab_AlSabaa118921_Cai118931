#ifndef SCENE_GRAPH_HPP
#define SCENE_GRAPH_HPP
#include <iostream>
#include <string>
using namespace std;
#include "node.hpp"
#include <memory>

class SceneGraph{

	
public :
	static SceneGraph* getInstance();  // STATIC
	string getName() const;
	shared_ptr<Node> getRoot() const;
	void printGraph();


private:

	SceneGraph();
	~SceneGraph();
	void setName(string const& name);
	void setRoot(shared_ptr<Node> const& node);
	void printNode(shared_ptr<Node> const& node, int const& depth);

private:
	string name_;
	shared_ptr<Node> root_;
	static SceneGraph* scene_instance_;
};
// call: SceneGraph* scene = SceneGraph::getInstance()
#endif