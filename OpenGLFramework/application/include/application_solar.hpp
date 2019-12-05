#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"
#include "node.hpp"
#include "scene_graph.hpp"
#include "geometry_node.hpp"
#include "camera_node.hpp"
#include <list>
#include <vector>
using namespace std;

/*
planet{
  holdernode: 
  geometrynode:
}
*/

// implement planet struct which holds values for the needed properties
struct planet {
  string parent;
  string name;
  string path;
  int depth;
  float size;
  float speed;
  float distance;
  float speed_relative_to_center;
};

// gpu representation of model
class ApplicationSolar : public Application {
 public:
  // allocate and initialize objects
  ApplicationSolar(std::string const& resource_path);
  // free allocated objects
  ~ApplicationSolar();

  // react to key input
  void keyCallback(int key, int action, int mods);
  //handle delta mouse movement input
  void mouseCallback(double pos_x, double pos_y);
  //handle resizing
  void resizeCallback(unsigned width, unsigned height);

  // draw all objects
  void render() const;

  // Assignment1 :
  /*
  planets: sphere objects, difference between planets: size, rotation speed, distance to origin
  implement planet struct which holds values for the needed properties,
  store planets in a container as member of the applicationSolar class
  */
  vector<shared_ptr<GeometryNode>> initializeAllPlanets() const;
  void drawPlanet() const;
  glm::fmat4 update_planet_transform(shared_ptr<Node> node) const;

  SceneGraph* sceneGraph = SceneGraph::getInstance();
  shared_ptr<Node> scene_root = sceneGraph -> getRoot();

  vector<planet> planets{   // name, parent, depth, size, speed, distance, speed_relative_to_center
    {"sun", "root", "root/sun", 1, 1.0f, 0.0f, 0.0f,  0.0f},
    {"mercury", "mercury_holder", "root/mercury_holder/mercury", 2, 0.90f, 0.7f, 5.0f, 0.10f},
    {"venus", "venus_holder", "root/venus_holder/venus", 2, 0.80f, 0.6f, 8.0f, 0.2f  },
    {"earth","earth_holder", "root/earth_holder/earth", 2, 0.70f, 0.5f, 11.0f, 0.3f},
    {"moon", "earth", "root/earth_holder/moon_holder/moon", 3, 0.60f, 0.8f, 5.5f, 0.4f },
    {"mars", "mars_holder", "root/mars_holder/mars",2, 0.5f, 0.65f, 15.0f, 0.5f  },
    {"jupiter","jupiter_holder", "root/jupiter_holder/jupiter", 2, 0.4f, 0.7f, 14.0f, 0.6f},
    {"saturn", "saturn_holder","root/saturn_holder/saturn", 2, 0.3f, 0.75f, 17.0f, 0.7f },
    {"uranus","uranus_holder","root/uranus_holder/uranus",2, 0.2f, 0.8f, 21.0f, 0.8f},
    {"neptune","neptune_holder","root/neptune_holder/neptune",2, 0.1f, 0.5f, 25.0f, 0.9f}
  };
  vector<shared_ptr<GeometryNode>> planets_pointers;

 protected:
  void initializeShaderPrograms();
  void initializeGeometry();
  // update uniform values
  void uploadUniforms();
  // upload projection matrix
  void uploadProjection();
  // upload view matrix
  void uploadView();

  // cpu representation of model
  model_object planet_object;
  
  // camera transform matrix
  glm::fmat4 m_view_transform;
  // camera projection matrix
  glm::fmat4 m_view_projection;
};

#endif