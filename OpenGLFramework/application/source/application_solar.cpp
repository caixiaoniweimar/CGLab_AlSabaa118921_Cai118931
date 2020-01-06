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
#include <memory>
#include <glm/glm.hpp>

const int star_num = 5000;
const int orbit_point_num = 360000;
// Assignment 3:
// Later every planet can have their own parameter Ka;
const glm::vec3 ambient = glm::vec3{0.2, 0.2, 0.2};
const float Ka = 1.0f; // how much of ambient light is reflected by the object, 0~1
const float Kd = 1.0f;
const float Ks = 0.7f;
const float shininess = 24;

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
 ,planet_object{}
 ,star_object{}
 ,m_view_transform{glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 4.0f})}
 ,m_view_projection{utils::calculate_projection_matrix(initial_aspect_ratio)}{
  initializeGeometry();
  initializeShaderPrograms();
  //Assignment 1
  planets_pointers=initializeAllPlanets();
  //Assignment 2
  initializeAllStars();
  initializeAllOrbits();
}

ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);

  // Assignment2 delete/release two buffer object(VBO,IBO) and array object(VAO)
  glDeleteBuffers(1, &star_object.vertex_BO);
  glDeleteVertexArrays(1, &star_object.vertex_AO);

  glDeleteBuffers(1, &orbit_object.vertex_BO);
  glDeleteVertexArrays(1, &orbit_object.vertex_AO);
}

void ApplicationSolar::render() const{

  //Assignment 1
  drawPlanet();
  //sceneGraph->printGraph();

  //Assignment 2
  drawStar();
  drawOrbit();

  //test methods, both work
  //scene_root->getChildren("sun").getChildren("mars");
  //scene_root->getChildren("sun").removeChildren("mars");
}

void ApplicationSolar::uploadView() {
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);
  // upload matrix to gpu
  glUseProgram(m_shaders.at("planet").handle);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("star").handle);
  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("orbit").handle);
  glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));

}

void ApplicationSolar::uploadProjection() {
  // upload matrix to gpu
  //use glUseProgramm to bind shader programm in each function
  glUseProgram(m_shaders.at("planet").handle);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("star").handle);
  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("orbit").handle);
  glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
}

void ApplicationSolar::switchAppearance(){
  cout<<ifCelShading<<endl;
  glUseProgram(m_shaders.at("planet").handle);
  glUniform1f(m_shaders.at("planet").u_locs.at("ifCelShading"), ifCelShading);
}

// update uniform locations
void ApplicationSolar::uploadUniforms() { 
  // bind shader to which to upload unforms
  // upload uniform values to new locations
  uploadView();
  uploadProjection();
  switchAppearance();
}

///////////////////////////// intialisation functions /////////////////////////
// load shader sources

//Assignment 3: use own planet.vert, planet.frag
void ApplicationSolar::initializeShaderPrograms() {
  // store shader program objects in container
  m_shaders.emplace("planet", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/planet.vert"},
                                           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/planet.frag"}}});
  // request uniform locations for shader program
  m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;

  m_shaders.at("planet").u_locs["PlanetColor"] = -1;
  m_shaders.at("planet").u_locs["Ambient"] = -1;
  m_shaders.at("planet").u_locs["Ka"] = -1;
  m_shaders.at("planet").u_locs["Kd"] = -1;
  m_shaders.at("planet").u_locs["Ks"] = -1;
  m_shaders.at("planet").u_locs["Shininess"] = -1;
  m_shaders.at("planet").u_locs["LightColor"] = -1;
  m_shaders.at("planet").u_locs["ifCelShading"] = false;

  // Assignment 2
  // use predefined vertex shader and fragment shader
  m_shaders.emplace("star", shader_program{{{ {GL_VERTEX_SHADER,m_resource_path + "shaders/star.vert"},
                                              {GL_FRAGMENT_SHADER, m_resource_path + "shaders/star.frag"}}}});

  m_shaders.at("star").u_locs["ViewMatrix"] = -1;
  m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;

  m_shaders.emplace("orbit", shader_program{{{ {GL_VERTEX_SHADER,m_resource_path + "shaders/orbit.vert"},
                                              {GL_FRAGMENT_SHADER, m_resource_path + "shaders/orbit.frag"}}}});
  m_shaders.at("orbit").u_locs["OrbitTransformMatrix"] = -1;
  m_shaders.at("orbit").u_locs["ViewMatrix"] = -1;
  m_shaders.at("orbit").u_locs["ProjectionMatrix"] = -1;

}

// load models
void ApplicationSolar::initializeGeometry() {
    model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);
  
    // Initialise Vertex Array Object

    // generate vertex array object
    glGenVertexArrays(1, &planet_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(planet_object.vertex_AO);

    // Initialise Vertex Buffer Object and load data

    // generate generic buffer
    glGenBuffers(1, &planet_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
    // configure currently bound array buffer
    // creates and initializes a buffer object's data store: size, data, usage
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(), planet_model.data.data(), GL_STATIC_DRAW);

    // Specify(activate, connect and set format) the Attributes

    // activate first attribute on gpu
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride
    glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::POSITION]);
    // activate second attribute on gpu
    glEnableVertexAttribArray(1);
    // second attribute is 3 floats with no offset & stride
    glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::NORMAL]);

    // Define Vertex Indices(optional)

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
/*
draw all Planets
load model, 
for each planet, set model and then update the model_matrix and normal_matrix;
in render() apply drawPlanet() method
*/
void ApplicationSolar::drawPlanet() const{

  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

  for(auto const& p_node: planets_pointers){
    p_node -> setGeometry(planet_model);
    glm::fmat4 model_matrix = update_planet_transform(p_node);

    //extra matrix for normal transformation to keep them orthogonal to surface
    glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform)* model_matrix);

    //bind shader to upload uniforms
    glUseProgram(m_shaders.at("planet").handle);
    //give matrices to shaders
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                 1, GL_FALSE, glm::value_ptr(model_matrix));

    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                 1, GL_FALSE, glm::value_ptr(normal_matrix));

    //Assignment3
    //give planetColor to vertex shaders
    //glUniform3f(GLint location, GLfloatv0, v1, v2)
    glUniform3f(m_shaders.at("planet").u_locs.at("PlanetColor"), (p_node -> getColor()).x, (p_node -> getColor()).y, (p_node -> getColor()).z);
    glUniform3f(m_shaders.at("planet").u_locs.at("Ambient"), ambient.x, ambient.y, ambient.z);
    glUniform1f(m_shaders.at("planet").u_locs.at("Ka"), Ka);
    glUniform1f(m_shaders.at("planet").u_locs.at("Kd"), Kd);
    glUniform1f(m_shaders.at("planet").u_locs.at("Ks"), Ks);
    glUniform1f(m_shaders.at("planet").u_locs.at("Shininess"), shininess);

    glUniform3f(m_shaders.at("planet").u_locs.at("LightColor"), p_point_light->cal_lightColor().x, p_point_light->cal_lightColor().y, p_point_light->cal_lightColor().z);


    // Drawing:

    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);

    // draw bound vertex array using bound shader
    // Using indices and parameters
    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);

  }

}

// Assignment 1
/*
initialize each planet in planets
three kinds of situations:
(1) planet.parent == root,
this node is sun, add sun as child of scene_root 
(2) planet.parent == sun,
there are 8 nodes that their parent is sun.  add them as children of sun
(3) planet.parent ==earth,
only moon's parent is earth, add moon as child of earth, the depth of moon is 3. deepest
return vector<shared_ptr<GeometryNode>>
*/
vector<shared_ptr<GeometryNode>> ApplicationSolar::initializeAllPlanets() const{
   vector<shared_ptr<GeometryNode>> geometry_nodes;

    for(int i=0; i<planets.size(); i++){
      planet planet = planets[i];
      if(planet.parent == "point_light"){

        //parent,name,path......
        GeometryNode sun{p_point_light, planet.name, planet.path, planet.depth, planet.size, planet.speed, planet.distance, planet.color};
        auto const p_sun = make_shared<GeometryNode>(sun);

        scene_root -> addChildren(p_point_light);
        p_point_light -> addChildren(p_sun);

        geometry_nodes.push_back(p_sun);
  
      }else if(planet.parent =="earth"){ // add moon

        auto p_earth = geometry_nodes[3]; 
        //string parent_name = p_earth -> getParent() -> getName();
        //Node holder_node{p_earth->getParent(), planet.name+"_holder", "root/"+parent_name+"/"+planet.name+"_holder", planet.speed_relative_to_center};
        //auto const p_holder_node = make_shared<Node>(holder_node);
        //p_holder_node -> setDapth(2);
        //p_earth->getParent()->addChildren(p_holder_node);
        //holder_node.setLocalTransform( glm::rotate(holder_node.getLocalTransform(), float(glfwGetTime())*holder_node.getSpeed(), glm::fvec3{0.0f, 1.0f, 0.0f}) );
        //holder_node.setWorldTransform( (holder_node.getParent()-> getWorldTransform())*holder_node.getLocalTransform() );

        GeometryNode node{p_earth, planet.name, planet.path, planet.depth, planet.size, planet.speed, planet.distance, planet.color};
        auto const& p_node = make_shared<GeometryNode>(node);
        //p_holder_node -> addChildren(p_node);
        p_earth -> addChildren(p_node);
        geometry_nodes.push_back(p_node);
      }else{ //other planets except sun and earth
        //create a holderNode for each planet, parent, name, path, speed
        //root add this holderNode as scene's child, set local/worldTransform for holder_node
        //create planet_node, add this planet_node as holderNode's child
        Node holder_node{scene_root, planet.name+"_holder", "root/"+planet.name+"_holder", planet.speed_relative_to_center};
        auto const p_holder_node = make_shared<Node>(holder_node);
        scene_root -> addChildren(p_holder_node);
        holder_node.setLocalTransform( glm::rotate(holder_node.getLocalTransform(), float(glfwGetTime())*holder_node.getSpeed(), glm::fvec3{0.0f, 1.0f, 0.0f}) );
        holder_node.setWorldTransform( (scene_root -> getWorldTransform())*holder_node.getLocalTransform() );

        GeometryNode node{p_holder_node, planet.name, planet.path, planet.depth, planet.size, planet.speed, planet.distance, planet.color};
        auto const& p_node = make_shared<GeometryNode>(node);
        p_holder_node -> addChildren(p_node);
        geometry_nodes.push_back(p_node);
      }
    }
    return geometry_nodes;
}

// Assignment 1
/*
For each planet, it's important to update their local transform and world transfrom
The local coordinate system is only relative to a single model, 
and the world coordinates are used to determine the position of the local coordinate system of each model.

In SceneGraph: Every Node inherit the coordinate system of their ancestor.
Then, every node defines its own local coordinate system, the root defines the world coordinate system.

local transform:
represent its position and orientation in relation to its parent node, can contain translation, rotation, and scaling information.

local_transform = translationMatrix * rotationMatrix*scaleMatrix;

world transform:
need each node’s world transformation, to use as the model matrix in a vertex shader.
we must go through the whole graph, starting from the root, 
the world transformation for each node can be calculated by multiplying a node’s local transformation matrix 
with the world transformation of its parent. 

We store both local and world transformations for each planet, 
so that we don't need to go through the tree every time. -> avoid a lot of unnecessary multiplications
*/
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

// Assignment 2

void ApplicationSolar::initializeAllStars(){
    //predefined star_num
    //generate stars at random positions x,y,z and random colors r,g,b

    /* [low,up]
    low+(up-low)*rand()/(RAND_MAX);
    */
    float position_low=-100.0;
    float position_up = 100.0;

    float color_low = 0.8;
    float color_up = 1.0;

    //generate position x,y,z between [50,100] 
    for(int i=0; i<star_num; ++i){
      GLfloat x = position_low+(position_up-position_low)*(rand()/(float)RAND_MAX);
      stars_container.push_back(x);
      GLfloat y = position_low+(position_up-position_low)*(rand()/(float)RAND_MAX);
      stars_container.push_back(y);
      GLfloat z = position_low+(position_up-position_low)*(rand()/(float)RAND_MAX);
      stars_container.push_back(z);

    //generate color between [0.5,1.0], bright
      GLfloat r = color_low+(color_up-color_low)*(rand()/(float)RAND_MAX);
      stars_container.push_back(r);
      GLfloat g = color_low+(color_up-color_low)*(rand()/(float)RAND_MAX);
      stars_container.push_back(g);
      GLfloat b = color_low+(color_up-color_low)*(rand()/(float)RAND_MAX);
      stars_container.push_back(b);
    }

    // Initialise Vertex Array Object

    // generate vertex array object
    glGenVertexArrays(1, &star_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(star_object.vertex_AO);

    // Initialise Vertex Buffer Object and load data

    // generate generic buffer
    glGenBuffers(1, &star_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, star_object.vertex_BO);
    // configure currently bound array buffer
    // creates and initializes a buffer object's data store: size, data, usage
    //GLsizeiptr size; const void * data, Glenum usage
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(sizeof(float) * stars_container.size()), stars_container.data(), GL_STATIC_DRAW);

    // Specify(activate, connect and set format) the Attributes

    // activate first attribute on gpu
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride
    // glVertexAttribPointer(index, size, dtype, norm, str, ptr)

    // size: int(1-4), the number of components per generic vertex attribute
    // dtype: type of the attributes: GL_FLOAT, GL_BYTE...;
    // normalized: whether the value should be normalised, bool;
    // stride: byte distance/offset between values of consecutive vertices, GLsizei, here 6 since each vertex consists of 6 float value
    // pointer/offset: offset in bytes of first component of the first attrbiute, the initial value is 0.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, GLsizei(sizeof(float)*6), (GLvoid*)(0));
    // activate second attribute on gpu
    glEnableVertexAttribArray(1);
    // second attribute is 3 floats with no offset & stride
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, GLsizei(sizeof(float)*6), (GLvoid*)(sizeof(float)*3));

    // store type of primitive to draw
    star_object.draw_mode = GL_POINTS;
    // transfer number of indices to model object 

    star_object.num_elements = GLsizei(star_num); 

}

void ApplicationSolar::drawStar() const{
    // Drawing:

    //bind shader to upload uniforms
    glUseProgram(m_shaders.at("star").handle);
    // bind the VAO to draw
    glBindVertexArray(star_object.vertex_AO);

    // draw bound vertex array using bound shader
    //Using parameters but NO indices
    //glDrawArrays(GLenum mode, first, count)
    //first: statring index in the enabled arryas
    //count, the number of indices to be rendered
    glDrawArrays(star_object.draw_mode, 0, star_object.num_elements);
    
    // twinkle star effect
    /*
    GL_PROGRAM_POINT_SIZE: specify the diameter of rasterized points, the initial value is 1
    
    If enalbed and a vertex shader is active, then the derived point size is taken from the shader 
    builtin gl_PointSize and clamped to the implementation-dependent size rang
    website: khronos.org/registry/OpenGL-Refpages/gl4/html/glPointSize.xhtml

    glPointSize(GLfloat size):
    size: the diameter of rasterized points
    */    
    glEnable(GL_PROGRAM_POINT_SIZE);
    // random diameter of points zwischen [2-3]
    glPointSize( (rand()/(float)(RAND_MAX))*1+2);
}

void ApplicationSolar::initializeAllOrbits(){
    for(int i=0; i<orbit_point_num; ++i){
      GLfloat x = cos(2*M_PI/orbit_point_num*i);
      GLfloat y = 0; // orbit -> rotata (0,1,0)
      GLfloat z = sin(2*M_PI/orbit_point_num*i);
      orbits_container.push_back(x);
      orbits_container.push_back(y);
      orbits_container.push_back(z);
    }

    glGenVertexArrays(1, &orbit_object.vertex_AO);
    glBindVertexArray(orbit_object.vertex_AO);

    glGenBuffers(1, &orbit_object.vertex_BO);
    glBindBuffer(GL_ARRAY_BUFFER, orbit_object.vertex_BO);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(sizeof(float)*orbits_container.size()), orbits_container.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, GLsizei(sizeof(float)*3), (GLvoid*)(0));

    orbit_object.draw_mode = GL_LINE_LOOP;
    orbit_object.num_elements = GLsizei(orbit_point_num);
}

void ApplicationSolar::drawOrbit() const{

  int i=0;
  for(auto const& p_node: planets_pointers){
    if( (p_node->getName()!="sun") && (p_node -> getName()!= "moon") ){
      i=i+1;
      /*
      the distance between planet and sun, 18, 16, 14, 12, 10, 8.....
      the size of planet relative to sun, 0.9, 0.8, 0.7, 0.6.....
      find the multiple relationship in oder to calculate the radius of the rabit
      */
  
      glm::fmat4 orbit_transform_matrix = glm::fmat4{1.0f};
      float radius = p_node->getDistanceX() - 2*i*(p_node -> getSize());
      orbit_transform_matrix = glm::scale(orbit_transform_matrix, glm::fvec3{radius,radius,radius});

      //bind shader to upload uniforms
      glUseProgram(m_shaders.at("orbit").handle);
      //give matrices to shaders
      glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("OrbitTransformMatrix"),
                   1, GL_FALSE, glm::value_ptr(orbit_transform_matrix));

      // Drawing:
      // bind the VAO to draw
      glBindVertexArray(orbit_object.vertex_AO);
      glDrawArrays(orbit_object.draw_mode, 0, orbit_object.num_elements);
    }else if(p_node -> getName()=="moon"){
      i=i+1;
      glm::fmat4 orbit_transform_matrix = glm::fmat4{1.0f};
      float radius=(p_node -> getParent() -> getDistanceX() - p_node-> getDistanceX())+2*i*p_node->getSize();
      orbit_transform_matrix = glm::scale(p_node -> getParent() -> getLocalTransform(), glm::fvec3{radius,radius,radius});

            //bind shader to upload uniforms
      glUseProgram(m_shaders.at("orbit").handle);
      //give matrices to shaders
      glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("OrbitTransformMatrix"),
                   1, GL_FALSE, glm::value_ptr(orbit_transform_matrix));
      glBindVertexArray(orbit_object.vertex_AO);
      glDrawArrays(orbit_object.draw_mode, 0, orbit_object.num_elements);
    }

  }

}

// Assignment 1
///////////////////////////// callback functions for window events ////////////
// handle key input
void ApplicationSolar::keyCallback(int key, int action, int mods) {
    //zoom in: X
    if (key == GLFW_KEY_X  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.5f});
        uploadView();
    }
    //zoom out: C
    else if (key == GLFW_KEY_C  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.5f});
        uploadView();
    }
    //move left: A
    else if(key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.2f, 0.0f, 0.0f});
        uploadView();
    }
    //move right: D
    else if(key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{-0.2f, 0.0f, 0.0f});
        uploadView();
    }
    //move up: W
    else if(key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, -0.2f, 0.0f});
        uploadView();
    }
    //move down: S
    else if(key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.2f, 0.0f});
        uploadView();
    }

    //Assignment 3
    //default Blinn-Phong local illumination
    else if(key == GLFW_KEY_1 && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        ifCelShading = false;
        switchAppearance();
    }
    else if(key == GLFW_KEY_2 && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        ifCelShading = true;
        switchAppearance();
    }
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
  // mouse handling

    float angle_x = (float)pos_x/10;
    float angle_y = (float)pos_y/10;
    
    m_view_transform = glm::rotate(m_view_transform, glm::radians(angle_x), glm::fvec3{0.0f, 1.0f, 0.0f});
    m_view_transform = glm::rotate(m_view_transform, glm::radians(angle_y), glm::fvec3{1.0f, 0.0f, 0.0f});
    uploadView();
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
  Application::run<ApplicationSolar>(argc, argv, 3, 2);
}