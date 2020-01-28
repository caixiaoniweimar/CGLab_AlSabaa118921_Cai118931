#version 150
#extension GL_ARB_explicit_attrib_location : require
// glVertexAttribPointer mapped positions to first
layout(location = 0) in vec2 in_Position;
// glVertexAttribPointer mapped color  to second attribute 
layout(location = 1) in vec2 in_TexCoord;

out vec2 pass_TexCoord;

//without any transformations in the vertex shader
//calculate only texture coordinates;
void main(){
	gl_Position = vec4(in_Position, 0.0, 1.0);
	pass_TexCoord = in_TexCoord;
}