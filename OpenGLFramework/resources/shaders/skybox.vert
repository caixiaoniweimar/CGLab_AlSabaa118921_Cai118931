#version 150
#extension GL_ARB_explicit_attrib_location : require
layout(location =0) in vec3 in_Position;

out vec3 pass_TexCoord;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

void main(){
	mat4 view = mat4(mat3(ViewMatrix));
/*
The original ViewMatrix transforms all the skybox's positions, so if user moves, the cubemap moves as well
we want to only remove the translation part of the view matrix, so the movement doesn't affect the skybox's position vectors
*/
	pass_TexCoord = in_Position;
	gl_Position = (ProjectionMatrix  * view *ModelMatrix) * vec4(in_Position, 1.0);
}