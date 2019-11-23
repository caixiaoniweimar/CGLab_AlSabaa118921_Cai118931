#ifndef GEOMETRY_NODE_HPP
#define GEOMETRY_NODE_HPP
#include <iostream>
#include <string>
using namespace std;
#include "model.hpp"
#include "node.hpp"

class GeometryNode : public Node{

public:
	GeometryNode();
	~GeometryNode();
	GeometryNode(shared_ptr<Node> const& parent, string const& name, string const& path, int const& depth, 
		   float const& size, float const& speed, float const& distance);
	model getGeometry() const;
	void setGeometry(model const& model);

	ostream& print(ostream& os) const override;

private:
	model model_;

};
#endif