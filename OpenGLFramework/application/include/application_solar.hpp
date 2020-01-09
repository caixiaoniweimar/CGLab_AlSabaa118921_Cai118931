#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"
#include "node.hpp"
#include "scene_graph.hpp"
#include "geometry_node.hpp"
#include "camera_node.hpp"
#include "point_light_node.hpp"
#include <list>
#include <vector>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
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
  float speed_relative_to_center; 
  glm::fvec3 color;
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
  void drawStar() const;
  void drawOrbit() const;
  void initializeAllStars();
  void initializeAllOrbits();
  void switchAppearance();

  glm::fmat4 update_planet_transform(shared_ptr<Node> node) const;

  SceneGraph* sceneGraph = SceneGraph::getInstance();
  shared_ptr<Node> scene_root = sceneGraph -> getRoot();

//Assignment 3, initialize point light for sun
  const float lightIntensity = 500.0; //light brightness
  const glm::vec3 lightColor = glm::vec3{1, 1, 1};      //point_light{parent, name, path,......}
  PointLightNode point_light{scene_root, "point_light", "root/point_light", lightIntensity, lightColor};
  shared_ptr<PointLightNode> p_point_light = make_shared<PointLightNode>(point_light);

  vector<planet> planets{   // name, parent, depth, size, speed, distance, speed_relative_to_center
    {"sun", "point_light", "root/point_light/sun", 2, 1.0f, 0.0f, 0.0f,  0.0f, glm::fvec3{1.0f, 0.0f, 0.0f} },//Red
    {"mercury", "mercury_holder", "root/mercury_holder/mercury", 2, 0.90f, 0.7f, 18.0f, 0.10f, glm::fvec3{0.0f, 0.0f, 1.0f} }, //Blue
    {"venus", "venus_holder", "root/venus_holder/venus", 2, 0.80f, 0.6f, 16.0f, 0.2f, glm::fvec3{0.0f, 0.8f, 0.0f}  }, //Green
    {"earth","earth_holder", "root/earth_holder/earth", 2, 0.70f, 0.5f, 14.0f, 0.3f, glm::fvec3{0.3f, 0.0f, 1.0f} }, //Purple
    {"moon", "earth", "root/earth_holder/earth/moon", 3, 0.20f, 0.8f, 13.0f, 0.0f, glm::fvec3{0.9f, 0.5f, 0.0f} },//Orange
    {"mars", "mars_holder", "root/mars_holder/mars",2, 0.5f, 0.65f, 10.0f, 0.5f, glm::fvec3{0.9f, 0.5f, 0.9f} },//Pink
    {"jupiter","jupiter_holder", "root/jupiter_holder/jupiter", 2, 0.4f, 0.7f, 8.0f, 0.6f, glm::fvec3{0.9f, 1.0f, 0.3f} },//light Green
    {"saturn", "saturn_holder","root/saturn_holder/saturn", 2, 0.3f, 0.75f, 6.0f, 0.7f, glm::fvec3{1.0f, 1.0f, 0.1f}  },//Yellow
    {"uranus","uranus_holder","root/uranus_holder/uranus",2, 0.2f, 0.8f, 4.0f, 0.8f, glm::fvec3{1.0f, 0.5f, 0.5f} }, //light Pink
    {"neptune","neptune_holder","root/neptune_holder/neptune",2, 0.1f, 0.5f, 2.0f, 0.9f, glm::fvec3{0.9f, 0.6f, 0.1f} }//dark Yellow
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

  // Assignment 2
  model_object star_object;
  model_object orbit_object;
  vector<GLfloat> stars_container;
  vector<GLfloat> orbits_container;

  // Assignment 3
  bool ifCelShading=false;

  // camera transform matrix
  glm::fmat4 m_view_transform;
  // camera projection matrix
  glm::fmat4 m_view_projection;
};

#endif