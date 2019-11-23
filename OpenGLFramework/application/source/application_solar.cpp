#include "application_solar.hpp"
#include "window_handler.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
 ,planet_object{}
 ,m_view_transform{glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 4.0f})}
 ,m_view_projection{utils::calculate_projection_matrix(initial_aspect_ratio)}
{
  initializeGeometry();
  initializeShaderPrograms();
}

ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);
}

void ApplicationSolar::render() const {
  // bind shader to upload uniforms
  glUseProgram(m_shaders.at("planet").handle);

  //initializeAllPlanets();
  drawPlanet();
  sceneGraph->printGraph();

  /*glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime()), glm::fvec3{0.0f, 1.0f, 0.0f});
  model_matrix = glm::translate(model_matrix, glm::fvec3{0.0f, 0.0f, -1.0f});
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                     1, GL_FALSE, glm::value_ptr(model_matrix));

  // extra matrix for normal transformation to keep them orthogonal to surface
  glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                     1, GL_FALSE, glm::value_ptr(normal_matrix));

  // bind the VAO to draw
  glBindVertexArray(planet_object.vertex_AO);

  // draw bound vertex array using bound shader
  glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);*/
}

void ApplicationSolar::uploadView() {
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::uploadProjection() {
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() { 
  // bind shader to which to upload unforms
  glUseProgram(m_shaders.at("planet").handle);
  // upload uniform values to new locations
  uploadView();
  uploadProjection();
}

///////////////////////////// intialisation functions /////////////////////////
// load shader sources
void ApplicationSolar::initializeShaderPrograms() {
  // store shader program objects in container
  m_shaders.emplace("planet", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/simple.vert"},
                                           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/simple.frag"}}});
  // request uniform locations for shader program
  m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
}

// load models
void ApplicationSolar::initializeGeometry() {
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

  // generate vertex array object
  glGenVertexArrays(1, &planet_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(planet_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &planet_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(), planet_model.data.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::POSITION]);
  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::NORMAL]);

   // generate generic buffer
  glGenBuffers(1, &planet_object.element_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
  // configure currently bound array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * planet_model.indices.size(), planet_model.indices.data(), GL_STATIC_DRAW);

  // store type of primitive to draw
  planet_object.draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object 
  planet_object.num_elements = GLsizei(planet_model.indices.size());
}

// Assignment 1
void ApplicationSolar::drawPlanet() const{

  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

  vector<shared_ptr<GeometryNode>> geometry_nodes = initializeAllPlanets();
  for(int i=0; i<9; i++){
    auto p_node = geometry_nodes[i];
    p_node -> setGeometry(planet_model);
    glm::fmat4 model_matrix = update_planet_transform(p_node);

    //extra matrix for normal transformation to keep them orthogonal to surface
    glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform)* model_matrix);

    //give matrices to shaders
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                 1, GL_FALSE, glm::value_ptr(model_matrix));

    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                 1, GL_FALSE, glm::value_ptr(normal_matrix));

    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);

    // draw bound vertex array using bound shader
    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

  }

}

vector<shared_ptr<GeometryNode>> ApplicationSolar::initializeAllPlanets() const{
   vector<shared_ptr<GeometryNode>> geometry_nodes;
    for(int i=0; i<10; i++){
      planet planet = planets[i];
      if(planet.parent == "root"){

        GeometryNode sun{scene_root, planet.name, planet.path, planet.depth, planet.size, planet.speed, planet.distance};
        auto const p_sun = make_shared<GeometryNode>(sun);
        scene_root -> addChildren(p_sun);
        geometry_nodes.push_back(p_sun);
  
      }else if(planet.parent=="sun"){

        auto p_sun = geometry_nodes[0];
        GeometryNode node{p_sun, planet.name, planet.path, planet.depth, planet.size, planet.speed, planet.distance};
        auto p_node = make_shared<GeometryNode>(node);
        p_sun -> addChildren(p_node);
        geometry_nodes.push_back(p_node);

      }else if(planet.parent =="earth"){
        auto p_earth = geometry_nodes[3];
        GeometryNode node{p_earth, planet.name, planet.path, planet.depth, planet.size, planet.speed, planet.distance};
        auto p_node = make_shared<GeometryNode>(node);
        p_earth -> addChildren(p_node);
        geometry_nodes.push_back(p_node);
      }
    }
    return geometry_nodes;
}

glm::fmat4 ApplicationSolar::update_planet_transform(shared_ptr<Node> node) const{
  
  glm::fmat4 model_matrix = glm::fmat4{1.0f};

  if(node->getParent() != nullptr){  // if node is not root; if root, use default local_transform and world_transform
    
    glm::fmat4 local_transform = glm::fmat4{1.0f};
    float size = node->getSize();
    float speed = node->getSpeed();
    glm::fvec3 distance = node->getDistance();

    local_transform = glm::scale(local_transform, glm::fvec3{size,size,size});
    local_transform = glm::rotate(local_transform, float(glfwGetTime())*speed, glm::fvec3{0.0f, 1.0f, 0.0f});
    local_transform = glm::translate(local_transform, distance);

    node -> setLocalTransform(local_transform);
    
/*
each node's world transformation: use as model matrix in a vertex shader
the world transformation for each node can be calculated by multiplying a node's local transformation matrix with the world 
transformation of its parent.
*/
    model_matrix = (node -> getParent() -> getWorldTransform()) * local_transform;
    node -> setWorldTransform(model_matrix);

  }
  return model_matrix;
}



///////////////////////////// callback functions for window events ////////////
// handle key input
void ApplicationSolar::keyCallback(int key, int action, int mods) {
  if (key == GLFW_KEY_W  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.1f});
    uploadView();
  }
  else if (key == GLFW_KEY_S  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.1f});
    uploadView();
  }
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
  // mouse handling
}

//handle resizing
void ApplicationSolar::resizeCallback(unsigned width, unsigned height) {
  // recalculate projection matrix for new aspect ration
  m_view_projection = utils::calculate_projection_matrix(float(width) / float(height));
  // upload new projection matrix
  uploadProjection();
}


// exe entry point
int main(int argc, char* argv[]) {
   cout<<"Test_______________"<<endl;
   list<int> test;
   test.push_back(1);
   //cout<<test.count(test.begin(),test.end())<<"dwwwwew"<<endl;
   //cout<<test.length()<<"dwdweq defwef"<<endl; 
  Application::run<ApplicationSolar>(argc, argv, 3, 2);
}