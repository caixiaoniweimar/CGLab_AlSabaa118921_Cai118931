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
//const glm::vec3 ambient = glm::vec3{0.2, 0.2, 0.2};
const float Ks = 0.5f;
const float shininess = 16;

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
 ,planet_object{}
 ,star_object{}
 ,m_view_transform{glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 4.0f})}
 ,m_view_projection{utils::calculate_projection_matrix(initial_aspect_ratio)}{
  initializeGeometry();
  initializeShaderPrograms();
  //Assignment 1
  initializeAllPlanets();
  //Assignment 4
  initializeTextures();
  initializeSkybox();
  //Assignment 2
  initializeAllStars();
  initializeAllOrbits();

  //Assignment 5
  initializeFrameBuffer();
  initializeFullScreenQuad();

  /*for(auto const& p_holder: holder_nodes_pointers){
    p_holder -> setLocalTransform( glm::rotate(p_holder->getLocalTransform(), float(glfwGetTime())*p_holder->getSpeed(), glm::fvec3{0.0f, 1.0f, 0.0f}) );
    p_holder -> setWorldTransform( (scene_root -> getWorldTransform())*p_holder->getLocalTransform() );
  }*/
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

  //Assignment 5
  // Bind the Frame Buffer Object and render the scene to it and not to the Default one
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer.handle);
  // the bound Framebuffer is cleared
  // The framebuffer Attachments must be cleared every frame before drawing them
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Assignment 1
  drawPlanet();
  //sceneGraph->printGraph();

  //Assignment 2
  drawStar();
  drawOrbit();

  //Assignment 4
  drawSkyBox();

  //Assignment 5
  drawFullScreenQuad();

  //test methods, both work
  //auto neptune = (scene_root->getChildren("neptune_holder")).getChildren("neptune");
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

  glUseProgram(m_shaders.at("skybox").handle);
  glUniformMatrix4fv(m_shaders.at("skybox").u_locs.at("ViewMatrix"),
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

  glUseProgram(m_shaders.at("skybox").handle);
  glUniformMatrix4fv(m_shaders.at("skybox").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
}

void ApplicationSolar::switchAppearance(){
  glUseProgram(m_shaders.at("planet").handle);
  glUniform1i(m_shaders.at("planet").u_locs.at("modelType"), modelType);

  glUseProgram(m_shaders.at("fullscreenquad").handle);
  glUniform1i(m_shaders.at("fullscreenquad").u_locs.at("modelType"), modelType);
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
//Assignment 5
void ApplicationSolar::initializeFrameBuffer(unsigned width, unsigned height){
  // texture as Color Attachment
  glActiveTexture(GL_TEXTURE4);  // 0:planets 1:skybox1 2:skybox2 3:normal mapping
  glGenTextures(1, &texture_object_color_attachment.handle);
  glBindTexture(GL_TEXTURE_2D, texture_object_color_attachment.handle);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE ,NULL);

  //render buffer as Depth Attachment
  //generate Renderbuffer Object
  glGenRenderbuffers(1, &render_buffer_object_depth_attachment.handle);
  //bind RBO for formatting
  glBindRenderbuffer(GL_RENDERBUFFER, render_buffer_object_depth_attachment.handle);
  //specify RBO properties
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height); 

  //Framebuffer specification
  //1. define Framebuffer
  //generate Frame Buffer Object
  glGenFramebuffers(1, &frame_buffer.handle);
  //bind FBO for configuration
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer.handle);

  //2. define attachments
  //specify Texture Object attachments, to color attachment; + texture handle; 0: highest level of texture, no mipmap
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_object_color_attachment.handle, 0);
  //specify Renderbuffer Object attachments, to depth attachment; +render buffer handle
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, render_buffer_object_depth_attachment.handle);

  //3. define draw buffers
  //create array containing enums representing color attachments;   Framebuffer Object could hold n color attachment Textures and 1 depth attachment, Texture/Renderbuffer
  GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
  //set the color attachment to receive fragments
  glDrawBuffers(1, draw_buffers);

  //4. check validity
  //get the FBO status
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  //compare return value with the valid status value
  if(status != GL_FRAMEBUFFER_COMPLETE){
    cout<<"The Framebuffer can't be created successfully"<<endl;
  }
}

void ApplicationSolar::initializeFullScreenQuad() {
  model full_screen_quad_model = model_loader::obj(m_resource_path + "models/quad.obj", model::TEXCOORD);

  glGenVertexArrays(1, &full_screen_quad_object.vertex_AO);
  glBindVertexArray(full_screen_quad_object.vertex_AO);

  glGenBuffers(1, &full_screen_quad_object.vertex_BO);
  glBindBuffer(GL_ARRAY_BUFFER, full_screen_quad_object.vertex_BO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * full_screen_quad_model.data.size(), full_screen_quad_model.data.data(), GL_STATIC_DRAW);

  //in_position in vertex_shader
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, full_screen_quad_model.vertex_bytes, full_screen_quad_model.offsets[model::POSITION]);

  //in_TexCoord; in vertex_shader -> calculate only texture coordinates;
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, full_screen_quad_model.vertex_bytes, full_screen_quad_model.offsets[model::TEXCOORD]);

  //Offscreen Rendering, Screen Quad, slide 11
  //render them as a GL_TRIANGLE_STRIP;
  full_screen_quad_object.draw_mode = GL_TRIANGLE_STRIP;
  full_screen_quad_object.num_elements = GLsizei(full_screen_quad_model.indices.size());
}


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
  m_shaders.at("planet").u_locs["Ks"] = -1;
  m_shaders.at("planet").u_locs["Shininess"] = -1;
  m_shaders.at("planet").u_locs["LightColor"] = -1;
  m_shaders.at("planet").u_locs["modelType"] = 1;

  //Assignment 4
  m_shaders.at("planet").u_locs["colorTexture1"]=-1;
  m_shaders.at("planet").u_locs["ifNormalMapping"] = -1;

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

  // Assignment 4 SkyBox
  m_shaders.emplace("skybox", shader_program{{{ {GL_VERTEX_SHADER,m_resource_path + "shaders/skybox.vert"},
                                              {GL_FRAGMENT_SHADER, m_resource_path + "shaders/skybox.frag"}}}});
  m_shaders.at("skybox").u_locs["ViewMatrix"] = -1;
  m_shaders.at("skybox").u_locs["ProjectionMatrix"] = -1;
  m_shaders.at("skybox").u_locs["ModelMatrix"] = -1;
  m_shaders.at("skybox").u_locs["skybox"] = -1;

  // Assignment 5
  m_shaders.emplace("fullscreenquad", shader_program{{{ {GL_VERTEX_SHADER,m_resource_path + "shaders/fullscreenquad.vert"},
                                              {GL_FRAGMENT_SHADER, m_resource_path + "shaders/fullscreenquad.frag"}}}});
  m_shaders.at("fullscreenquad").u_locs["colorTexture"] = -1;
  m_shaders.at("fullscreenquad").u_locs["modelType"] = -1;
}

// load models
void ApplicationSolar::initializeGeometry() {
    model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL | model::TEXCOORD);
  
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

    //Assignment4 -> in vec2 in_TexCoord;
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::TEXCOORD]);
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
void ApplicationSolar::initializeAllPlanets(){

    for(int i=0; i<planets.size(); i++){
      planet planet = planets[i];
      if(planet.parent == "point_light"){

        //parent,name,path......
        GeometryNode sun{p_point_light, planet.name, planet.path, planet.depth, planet.size, planet.speed, planet.distance, planet.color, planet.texture_file_path, planet.ifNormalMapping};
        auto const p_sun = make_shared<GeometryNode>(sun);
        //Assignment 4
        p_sun -> setPixelData(planet.texture_file_path);

        scene_root -> addChildren(p_point_light);
        p_point_light -> addChildren(p_sun);

        planets_pointers.push_back(p_sun);
  
      }else if(planet.parent =="earth"){ // add moon

        auto p_earth = planets_pointers[3]; 
        //string parent_name = p_earth -> getParent() -> getName();
        //Node holder_node{p_earth->getParent(), planet.name+"_holder", "root/"+parent_name+"/"+planet.name+"_holder", planet.speed_relative_to_center};
        //auto const p_holder_node = make_shared<Node>(holder_node);
        //p_holder_node -> setDapth(2);
        //p_earth->getParent()->addChildren(p_holder_node);
        //holder_node.setLocalTransform( glm::rotate(holder_node.getLocalTransform(), float(glfwGetTime())*holder_node.getSpeed(), glm::fvec3{0.0f, 1.0f, 0.0f}) );
        //holder_node.setWorldTransform( (holder_node.getParent()-> getWorldTransform())*holder_node.getLocalTransform() );

        GeometryNode node{p_earth, planet.name, planet.path, planet.depth, planet.size, planet.speed, planet.distance, planet.color, planet.texture_file_path, planet.ifNormalMapping};
        auto const& p_node = make_shared<GeometryNode>(node);
        //Assignment 4
        p_node -> setPixelData(planet.texture_file_path);
        //p_holder_node -> addChildren(p_node);
        p_earth -> addChildren(p_node);
        planets_pointers.push_back(p_node);
      }else{ //other planets except sun and earth
        //create a holderNode for each planet, parent, name, path, speed
        //root add this holderNode as scene's child, set local/worldTransform for holder_node
        //create planet_node, add this planet_node as holderNode's child
        Node holder_node{scene_root, planet.name+"_holder", "root/"+planet.name+"_holder", planet.speed_relative_to_center};
        auto const p_holder_node = make_shared<Node>(holder_node);

        //holder_node.setLocalTransform( glm::rotate(holder_node.getLocalTransform(), float(glfwGetTime())*holder_node.getSpeed(), glm::fvec3{0.0f, 1.0f, 0.0f}) );
        //holder_node.setWorldTransform( (scene_root -> getWorldTransform())*holder_node.getLocalTransform() );
        scene_root -> addChildren(p_holder_node);
        holder_nodes_pointers.push_back(p_holder_node);

        GeometryNode node{p_holder_node, planet.name, planet.path, planet.depth, planet.size, planet.speed, planet.distance, planet.color, planet.texture_file_path,planet.ifNormalMapping};
        auto const& p_node = make_shared<GeometryNode>(node);
        //Assignment 4
        p_node -> setPixelData(planet.texture_file_path);

        p_holder_node -> addChildren(p_node);
        planets_pointers.push_back(p_node);
      }
    }
    //return geometry_nodes;
}

// Assignment 4
void ApplicationSolar::initializeTextures(){
  for(auto const& planet : planets_pointers){
      auto pixel_data = planet -> getPixelData();
      auto texture_object = planet -> getTextureObject();

      /*
      to prevent accidental modifications it's best to use the GL_TEXTURE0 unit only for formtting and to bind textures to another unit
      for the actual accessing

      void glActiveTexture(GLenum texture);
      texture: specifies which texture unit to make active. 'texture' must be one of GL_TEXTUREi, where 0<=i<GL_MAX_TEXTURE_UNITS
      the initial value is GL_TEXTURE0; The number of texture units is implementation dependent, must be at least 2.
      */

      glActiveTexture(GL_TEXTURE0); //activate Texture Unit to which to bind texture

      /*
      glGenTextures(GLsizei n, GLuint* textures)
      n: specifies the number of texture names to be generated, here only one
      textures: specifies an array in which the generated texture names are stored

      struct texture_object {
          // handle of texture object
          GLuint handle = 0;
          // binding point
          GLenum target = GL_NONE;
      };
      */
      glGenTextures(1, &texture_object.handle); //generate Texture Object;
      
      /*
      bind Texture Object to 2d texture binding point of unit
      glBindTexture(GLenum target, GLuint texture);
      target: Specifies the target to which the texture is bound.
      */
      glBindTexture(GL_TEXTURE_2D, texture_object.handle);

      //define interpolation type when fragment covers multiple texels(texture pixels)
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      //define interpolation type when fragment does not exactly cover one texel
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      /*
      glTexImage2D(target, level, internalformat, width, height, border, format, type, *data);
      target: GL_TEXTURE_2D, binding point for the texture
      level: 0, for no mipmaps
      internalformat(image_format): specifies the storage format containg number of channels(Specifies the number of color components in the texture),
      https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
      internalformat may be one of the base internal formats shown in Table 1, similar with pixel_data.channels 

      format, type, data -> pixel transfer parameters, describe how the image is represented in memory
      format: specifies the number of channels for color data, Specifies the format of the pixel data. The following symbolic values are accepted: GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA... 
      type: specifies the data type and size of the color data values per channel, 
            Specifies the data type of the pixel data. The following symbolic values are accepted: GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_HALF_FLOAT, GL_FLOAT,
      data: the location of the pixel data

      pixel_data constructure:
      pixel_data():pixels(),width{0},height{0},depth{0},channels{GL_NONE},channel_type{GL_NONE}{}
      */
      //Pixel Transfer opration
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pixel_data.width, pixel_data.height, 0, pixel_data.channels, pixel_data.channel_type ,pixel_data.ptr());
      planet -> setTextureObject(texture_object);
      //cout<<planet -> getIfNormalMapping()<<endl;
    //For NormalMapping
      if(planet -> getIfNormalMapping()){

          planet -> setPixelData(m_resource_path+"textures/mars_1k_normal.png");
          auto pixel_data2 = planet -> getPixelData();
          glActiveTexture(GL_TEXTURE3);
          glGenTextures(1, &mars_normal_mapping.handle);
          glBindTexture(GL_TEXTURE_2D, mars_normal_mapping.handle);

          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pixel_data2.width, pixel_data2.height, 0, pixel_data2.channels, pixel_data2.channel_type ,pixel_data2.ptr());
      }

  }
    //For SkyBox1
    glActiveTexture(GL_TEXTURE1);
    texture_object textureObject1 = skybox1.textureObject;
    glGenTextures(1, &textureObject1.handle);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureObject1.handle);

    //specify its wrapping and filtering methods
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    //GL_TEXTURE_WRAP_R, sets the wrapping method for the texture's R coordinate which corresponds to texture's 3rd dimension(z).
    //GL_CLAMP_TO_EDGE: always return their edge values whenever we sample between faces, since texture coordinates that are exactly between two faces might not hit an exact face

    pixel_data pixel_data_bk = texture_loader::file(skybox1.filePath_bk);
    pixel_data pixel_data_dn = texture_loader::file(skybox1.filePath_dn);
    pixel_data pixel_data_ft = texture_loader::file(skybox1.filePath_ft);
    pixel_data pixel_data_lf = texture_loader::file(skybox1.filePath_lf);
    pixel_data pixel_data_rt = texture_loader::file(skybox1.filePath_rt);
    pixel_data pixel_data_up = texture_loader::file(skybox1.filePath_up);

    //a cubemap consists of 6 textures, one for each face, have to call glTexImage2D 6 times with their parameters set to values
    //set the texture target paramenter to a specific face of the cubemap
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, pixel_data_bk.width, pixel_data_bk.height, 0, pixel_data_bk.channels, pixel_data_bk.channel_type, pixel_data_bk.ptr()); // Back, bk
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, pixel_data_dn.width, pixel_data_dn.height, 0, pixel_data_dn.channels, pixel_data_dn.channel_type, pixel_data_dn.ptr()); // Bottom, dn
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, pixel_data_ft.width, pixel_data_ft.height, 0, pixel_data_ft.channels, pixel_data_ft.channel_type, pixel_data_ft.ptr()); // Front, ft 
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, pixel_data_lf.width, pixel_data_lf.height, 0, pixel_data_lf.channels, pixel_data_lf.channel_type, pixel_data_lf.ptr()); // Left, lf
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, pixel_data_rt.width, pixel_data_rt.height, 0, pixel_data_rt.channels, pixel_data_rt.channel_type, pixel_data_rt.ptr()); // Right, rt
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, pixel_data_up.width, pixel_data_up.height, 0, pixel_data_up.channels, pixel_data_up.channel_type, pixel_data_up.ptr()); // Top, up
    skybox1.textureObject = textureObject1;

    //For SkyBox2
    glActiveTexture(GL_TEXTURE2);
    texture_object textureObject2 = skybox2.textureObject;
    glGenTextures(1, &textureObject2.handle);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureObject2.handle);

    //specify its wrapping and filtering methods
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    //GL_TEXTURE_WRAP_R, sets the wrapping method for the texture's R coordinate which corresponds to texture's 3rd dimension(z).
    //GL_CLAMP_TO_EDGE: always return their edge values whenever we sample between faces, since texture coordinates that are exactly between two faces might not hit an exact face

    pixel_data pixel_data_bk2 = texture_loader::file(skybox2.filePath_bk);
    pixel_data pixel_data_dn2 = texture_loader::file(skybox2.filePath_dn);
    pixel_data pixel_data_ft2 = texture_loader::file(skybox2.filePath_ft);
    pixel_data pixel_data_lf2 = texture_loader::file(skybox2.filePath_lf);
    pixel_data pixel_data_rt2 = texture_loader::file(skybox2.filePath_rt);
    pixel_data pixel_data_up2 = texture_loader::file(skybox2.filePath_up);

    //a cubemap consists of 6 textures, one for each face, have to call glTexImage2D 6 times with their parameters set to values
    //set the texture target paramenter to a specific face of the cubemap
    //cout<<"1 "<<pixel_data_up.channels<<endl;
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, pixel_data_bk2.width, pixel_data_bk2.height, 0, pixel_data_bk2.channels, pixel_data_bk2.channel_type, pixel_data_bk2.ptr()); // Back, bk
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, pixel_data_dn2.width, pixel_data_dn2.height, 0, pixel_data_dn2.channels, pixel_data_dn2.channel_type, pixel_data_dn2.ptr()); // Bottom, dn
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, pixel_data_ft2.width, pixel_data_ft2.height, 0, pixel_data_ft2.channels, pixel_data_ft2.channel_type, pixel_data_ft2.ptr()); // Front, ft 
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, pixel_data_lf2.width, pixel_data_lf2.height, 0, pixel_data_lf2.channels, pixel_data_lf2.channel_type, pixel_data_lf2.ptr()); // Left, lf
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, pixel_data_rt2.width, pixel_data_rt2.height, 0, pixel_data_rt2.channels, pixel_data_rt2.channel_type, pixel_data_rt2.ptr()); // Right, rt
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, pixel_data_up2.width, pixel_data_up2.height, 0, pixel_data_up2.channels, pixel_data_up2.channel_type, pixel_data_up2.ptr()); // Top, up
    skybox2.textureObject = textureObject2;
}

void ApplicationSolar::initializeSkybox(){
    model skybox_model = model_loader::obj(m_resource_path + "models/skybox.obj", model::NORMAL);

    glGenVertexArrays(1, &skybox_object.vertex_AO);
    glBindVertexArray(skybox_object.vertex_AO);

    glGenBuffers(1, &skybox_object.vertex_BO);
    glBindBuffer(GL_ARRAY_BUFFER, skybox_object.vertex_BO);
    //glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(sizeof(float) * 36), stars_container.data(), GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * skybox_model.data.size(), skybox_model.data.data(), GL_STATIC_DRAW);

    // activate first attribute on gpu
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride
    glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, skybox_model.vertex_bytes, skybox_model.offsets[model::POSITION]);

    glGenBuffers(1, &skybox_object.element_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skybox_object.element_BO);
    // configure currently bound array buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * skybox_model.indices.size(), skybox_model.indices.data(), GL_STATIC_DRAW);


    // store type of primitive to draw
    skybox_object.draw_mode = GL_TRIANGLES;
    // transfer number of indices to model object 
    skybox_object.num_elements = GLsizei(skybox_model.indices.size());
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
    glUniform1f(m_shaders.at("planet").u_locs.at("Ks"), Ks);
    glUniform1f(m_shaders.at("planet").u_locs.at("Shininess"), shininess);

    glUniform3f(m_shaders.at("planet").u_locs.at("LightColor"), p_point_light->cal_lightColor().x, p_point_light->cal_lightColor().y, p_point_light->cal_lightColor().z);

    //Assignment 4: Texture Usage
    /*
    1. in the shader, textures are accessed through sampler uniforms
    2. the sampler type defines which binding point is accessed -> GL_TEXTURE_2D
    3. the samplers holds an interger with the index of the Texture Unit that is should access as value
    4. the index must be uploaded to the sampler value with glUniform1i
    */
    auto texture_object = p_node -> getTextureObject();
    if(modelType==4){  // if call NormalMapping
      if(p_node -> getIfNormalMapping()){
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, mars_normal_mapping.handle);
      }else{
        glActiveTexture(GL_TEXTURE0); //active Texture Unit to which to bind texture
        glBindTexture(GL_TEXTURE_2D, texture_object.handle); //bind Texture OBject to 2D texture binding point of unit
      }
    }else{
      //Bind for Accessing
      glActiveTexture(GL_TEXTURE0); //active Texture Unit to which to bind texture
      glBindTexture(GL_TEXTURE_2D, texture_object.handle); //bind Texture OBject to 2D texture binding point of unit
    }

    //Upload Texture
    glUniform1i(m_shaders.at("planet").u_locs.at("colorTexture1"), 0); // 0-> the index of Texture Unit

    //NormalMapping
    bool ifNormalMapping = p_node -> getIfNormalMapping();
    glUniform1i(m_shaders.at("planet").u_locs.at("ifNormalMapping"), ifNormalMapping);

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
    
    glm::fmat4 local_transform = glm::fmat4{1.0f};
    float size = node->getSize();
    float speed = node->getSpeed();
    glm::fvec3 distance = node->getDistance();

    local_transform = glm::rotate(local_transform, float(glfwGetTime())*speed, glm::fvec3{0.0f, 1.0f, 0.0f});
    local_transform = glm::translate(local_transform, 1.0f*distance);

    local_transform = glm::scale(local_transform, glm::fvec3{size,size,size});

    if(node -> getParent() -> getName()=="point_light"){
      local_transform = glm::rotate(local_transform,float(glfwGetTime())* 0.8f, glm::fvec3{0.0f, 1.0f, 0.0f});  
    }else{
      local_transform = glm::rotate(local_transform,float(glfwGetTime())* node -> getParent() -> getSpeed(), glm::fvec3{0.0f, 1.0f, 0.0f});  
    }
    
    node -> setLocalTransform(local_transform);
    
/*
each node's world transformation: use as model matrix in a vertex shader
the world transformation for each node can be calculated by multiplying a node's local transformation matrix with the world 
transformation of its parent.
*/
    //cout<<glm::to_string(node->getParent()->getWorldTransform())<<endl;
    model_matrix = (node -> getParent() -> getWorldTransform()) * local_transform;

    node -> setWorldTransform(model_matrix);

  return model_matrix;
}
//Assignment 4
void ApplicationSolar::drawSkyBox() const{

    glDepthMask(GL_FALSE);
    glUseProgram(m_shaders.at("skybox").handle);

    if(modelType==3){
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_CUBE_MAP, skybox2.textureObject.handle);
    }else{
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_CUBE_MAP, skybox1.textureObject.handle);
    }

    glm::fmat4 model_matrix = glm::fmat4{1.0};
    model_matrix = glm::scale(model_matrix, glm::fvec3{60, 60, 60});
    glUniformMatrix4fv(m_shaders.at("skybox").u_locs.at("ModelMatrix"),1, GL_FALSE, glm::value_ptr(model_matrix));

    if(modelType==3)
      glUniform1i(m_shaders.at("skybox").u_locs.at("skybox"), 2);
    else
      glUniform1i(m_shaders.at("skybox").u_locs.at("skybox"), 1);

    glBindVertexArray(skybox_object.vertex_AO);
    glDrawElements(skybox_object.draw_mode, skybox_object.num_elements, model::INDEX.type, NULL);

    glDepthMask(GL_TRUE);

}

//Assignment 5
void ApplicationSolar::drawFullScreenQuad() const{

// in order to render to screen, bind again the default Framebuffer, 0
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glUseProgram(m_shaders.at("fullscreenquad").handle);
  
  glActiveTexture(GL_TEXTURE4); //-> color attachment
  glBindTexture(GL_TEXTURE_2D, texture_object_color_attachment.handle);

  glUniform1i(m_shaders.at("fullscreenquad").u_locs.at("colorTexture"), 4);

  glBindVertexArray(full_screen_quad_object.vertex_AO);
  glDrawArrays(full_screen_quad_object.draw_mode, 0, full_screen_quad_object.num_elements);
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

void ApplicationSolar::drawOrbit() const{

  for(auto const& p_node: planets_pointers){
    if( (p_node->getName()!="sun") && (p_node -> getName()!= "moon") ){
  
      glm::fmat4 orbit_transform_matrix = glm::fmat4{};

      auto parent = p_node-> getParent();

      float radius = p_node->getDistanceX();
      
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
      glm::fmat4 orbit_transform_matrix = glm::fmat4{1.0f};
      float radius=p_node-> getDistanceX();

      orbit_transform_matrix = glm::scale(p_node -> getParent() -> getWorldTransform(), glm::fvec3{radius,radius,radius});

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
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.3f});
        uploadView();
    }
    //zoom out: C
    else if (key == GLFW_KEY_C  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.3f});
        uploadView();
    }
    //move left: A
    else if(key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{-0.2f, 0.0f, 0.0f});
        uploadView();
    }
    //move right: D
    else if(key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.2f, 0.0f, 0.0f});
        uploadView();
    }
    //move up: W
    else if(key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.2f, 0.0f});
        uploadView();
    }
    //move down: S
    else if(key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, -0.2f, 0.0f});
        uploadView();
    }

    //Assignment 3
    //default Blinn-Phong local illumination
    else if(key == GLFW_KEY_1 && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        modelType = 1;
        switchAppearance();
    }
    else if(key == GLFW_KEY_2 && (action == GLFW_PRESS || action == GLFW_REPEAT )){
        modelType = 2;
        switchAppearance();
    }

    //Assignment 4
    else if(key == GLFW_KEY_3 && (action == GLFW_PRESS || action == GLFW_REPEAT )){  //skybox2
        modelType = 3;
        switchAppearance();
    }

    else if(key == GLFW_KEY_4 && (action == GLFW_PRESS || action == GLFW_REPEAT )){ // Normal Mapping
        modelType = 4;
        switchAppearance();
    }

    else if(key == GLFW_KEY_7 && (action == GLFW_PRESS || action == GLFW_REPEAT )){ // Grayscale
        modelType = 7;
        switchAppearance();
    }
    else if(key == GLFW_KEY_8 && (action == GLFW_PRESS || action == GLFW_REPEAT )){ // horizontal mirroring
        modelType = 8;
        switchAppearance();
    }
    else if(key == GLFW_KEY_9 && (action == GLFW_PRESS || action == GLFW_REPEAT )){ // vertical mirroing
        modelType = 9;
        switchAppearance();
    }
    else if(key == GLFW_KEY_0 && (action == GLFW_PRESS || action == GLFW_REPEAT )){ // blur with 3*3 Gaussian Kernel
        modelType = 0;
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
  initializeFrameBuffer(width, height);
  // upload new projection matrix
  uploadProjection();

}


// exe entry point
int main(int argc, char* argv[]) {
  Application::run<ApplicationSolar>(argc, argv, 3, 2);
}