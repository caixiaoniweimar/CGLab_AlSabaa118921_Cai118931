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


// implement planet struct which holds values for the needed properties
struct planet {
  string name;
  string parent;
  string path;
  int depth;
  float size;
  float speed;
  float distance;
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
  vector<shared_ptr<GeometryNode>> initializeAllPlanets() const;
  void drawPlanet() const;
  glm::fmat4 update_planet_transform(shared_ptr<Node> node) const;

  SceneGraph* sceneGraph = SceneGraph::getInstance();
  shared_ptr<Node> scene_root = sceneGraph -> getRoot();

  vector<planet> planets{   // name, parent, size, speed, distance
    {"sun", "root", "root/sun", 1, 1.0f, 0.0f, 0.0f },
    {"mercury", "sun", "root/sun/mercury", 2, 0.90f, 0.7f, 5.0f  },
    {"venus", "sun", "root/sun/venus", 2, 0.80f, 0.6f, 8.0f  },
    {"earth", "sun", "root/sun/earth", 2, 0.70f, 0.5f, 11.0f},
    {"moon", "earth", "root/sun/earth/moon", 3, 0.60f, 0.8f, 5.5f },
    {"mars", "sun", "root/sun/mars",2, 0.5f, 0.65f, 15.0f  },
    {"jupiter", "sun", "root/sun/jupiter", 2, 0.4f, 0.7f, 14.0f},
    {"saturn", "sun", "root/sun/saturn", 2, 0.3f, 0.75f, 17.0f },
    {"uranus", "sun", "root/sun/uranus",2, 0.2f, 0.8f, 21.0f},
    {"neptune", "sun", "root/sun/neptune",2, 0.1f, 0.5f, 25.0f}
  };

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