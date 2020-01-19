#ifndef MODEL_LOADER_HPP
#define MODEL_LOADER_HPP

#include "model.hpp"

#include "tiny_obj_loader.h"

namespace model_loader {

/*
Assignment 4:
request the model loader to load the texture coordinates
*/
model obj(std::string const& path, model::attrib_flag_t import_attribs = model::NORMAL | model::TEXCOORD);

}

#endif