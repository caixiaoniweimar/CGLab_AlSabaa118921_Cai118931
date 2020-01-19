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

#include <glm/gtx/string_cast.hpp>
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
  string texture_file_path;
  bool ifNormalMapping;
};

struct skyBox {
  string name;
  string filePath_bk;
  string filePath_dn;
  string filePath_ft;
  string filePath_lf;
  string filePath_rt;
  string filePath_up;
  texture_object textureObject;
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
  void initializeAllPlanets();
  void initializeTextures();
  void initializeAllStars();
  void initializeAllOrbits();
  void switchAppearance();
  void initializeSkybox();

  void drawPlanet() const;
  void drawStar() const;
  void drawOrbit() const;
  void drawSkyBox() const;

  glm::fmat4 update_planet_transform(shared_ptr<Node> node) const;

  SceneGraph* sceneGraph = SceneGraph::getInstance();
  shared_ptr<Node> scene_root = sceneGraph -> getRoot();

//Assignment 3, initialize point light for sun
  const float lightIntensity = 300.0; //light brightness
  const glm::vec3 lightColor = glm::vec3{1, 1, 1};      //point_light{parent, name, path,......}
  PointLightNode point_light{scene_root, "point_light", "root/point_light", lightIntensity, lightColor};
  shared_ptr<PointLightNode> p_point_light = make_shared<PointLightNode>(point_light);

  vector<planet> planets{   // name, parent, depth, size, speed, distance, speed_relative_to_center
    {"sun", "point_light", "root/point_light/sun", 2, 1.0f, 0.0f, 0.0f,  0.7f, glm::fvec3{0.98f, 0.97f, 0.66f}, m_resource_path+"textures/sunmapthumb.png", false },//light yellow
    {"mercury", "mercury_holder", "root/mercury_holder/mercury", 2, 0.90f, 0.7f, 10.0f, 0.70f, glm::fvec3{0.86f, 0.73f, 0.55f}, m_resource_path+"textures/mercurymapthumb.png", false  }, //light brown
    {"venus", "venus_holder", "root/venus_holder/venus", 2, 0.80f, 0.6f, 9.0f, 0.5f, glm::fvec3{0.81f, 0.57f, 0.18f}, m_resource_path+"textures/venusmapthumb.png", false  }, //dark brown
    {"earth","earth_holder", "root/earth_holder/earth", 2, 0.70f, 0.5f, 8.0f, 0.7f, glm::fvec3{0.27f, 0.46f, 0.63f}, m_resource_path+"textures/earthmapthumb.png", false }, //blue
    {"moon", "earth", "root/earth_holder/earth/moon", 3, 0.30f, 0.3f, 2.0f, 0.3f, glm::fvec3{0.82f, 0.83f, 0.84f}, m_resource_path+"textures/moonmapthumb.png",false },//gray
    {"mars", "mars_holder", "root/mars_holder/mars",2, 0.5f, 0.65f, 6.0f, 0.5f, glm::fvec3{0.83f, 0.53f, 0.34f}, m_resource_path+"textures/marsmapthumb.png",true },//red brown
    {"jupiter","jupiter_holder", "root/jupiter_holder/jupiter", 2, 0.4f, 0.7f, 5.0f, 0.6f, glm::fvec3{0.69f, 0.62f, 0.55f}, m_resource_path+"textures/jupitermapthumb.png",false },//? 
    {"saturn", "saturn_holder","root/saturn_holder/saturn", 2, 0.3f, 0.75f, 4.0f, 0.7f, glm::fvec3{0.86f, 0.71f, 0.57f}, m_resource_path+"textures/saturnmapthumb.png", false  },//?
    {"uranus","uranus_holder","root/uranus_holder/uranus",2, 0.2f, 0.2f, 3.0f, 1.5f, glm::fvec3{0.58f, 0.68f, 0.72f}, m_resource_path+"textures/uranusmapthumb.png", false }, //light Blue
    {"neptune","neptune_holder","root/neptune_holder/neptune",2, 0.2f, 0.2f, 2.0f, 0.9f, glm::fvec3{0.49f, 0.59f, 0.88f}, m_resource_path+"textures/neptunemapthumb.png", false }//some Blue
  };
  vector<shared_ptr<GeometryNode>> planets_pointers;
  vector<shared_ptr<Node>> holder_nodes_pointers;

  //Assignment 4 SkyBox:
  skyBox skybox1{"criminalImpact", m_resource_path+"textures/skybox1/criminal-impact_bk.png", m_resource_path+"textures/skybox1/criminal-impact_dn.png", m_resource_path+"textures/skybox1/criminal-impact_ft.png",
                  m_resource_path+"textures/skybox1/criminal-impact_lf.png", m_resource_path+"textures/skybox1/criminal-impact_rt.png", m_resource_path+"textures/skybox1/criminal-impact_up.png", texture_object{}  };
  skyBox skybox2{"mnight", m_resource_path+"textures/skybox2/mnight_bk.png", m_resource_path+"textures/skybox2/mnight_dn.png", m_resource_path+"textures/skybox2/mnight_ft.png",
                  m_resource_path+"textures/skybox2/mnight_lf.png", m_resource_path+"textures/skybox2/mnight_rt.png", m_resource_path+"textures/skybox2/mnight_up.png", texture_object{} };

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

  //Assignment 4
  model_object skybox_object;

  // Assignment 3
  int modelType=1;

  // camera transform matrix
  glm::fmat4 m_view_transform;
  // camera projection matrix
  glm::fmat4 m_view_projection;

  texture_object mars_normal_mapping;
};

#endif