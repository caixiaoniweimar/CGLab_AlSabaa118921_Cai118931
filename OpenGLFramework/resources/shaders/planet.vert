#version 150
#extension GL_ARB_explicit_attrib_location : require

//Vertex Attributes, varying input, 1st is position in Model Space
//2nd is normal in Model Space 
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
//Assignment 4
//add another input attribute in_Texcord, is directly assigned to an output variable vec2 pass_TexCoord
layout(location = 2) in vec2 in_TexCoord;

//Matrix Uniforms uploaded with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;

out vec3 pass_Normal;
out vec3 pass_Position;
out vec3 pass_Eye_Position;
//Assignment 4:
out vec2 pass_TexCoord;

void main(){

	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	pass_Normal = (NormalMatrix * vec4(in_Normal, 0.0)).xyz;
	
	pass_Position = (ModelMatrix*vec4(in_Position, 1.0)).xyz; //world_position, object -> World space  

// gl_Position is transformed from Model Space to Porjection-Space
// normal is transformed from Model Space to View Space

	pass_Eye_Position = (inverse(ViewMatrix)*vec4(0,0,0,1)).xyz;  //*vec4(0,0,0,1) can get the 4th column

//Assignment 4:
	pass_TexCoord = in_TexCoord;
}

//helpful link: https://www.3dgep.com/understanding-the-view-matrix/
// compute camera position(eye) from View Matrix
// we have already known fmat4 m_view_transform M(camera transform matrix, world transform)
// V is the view matrix, the matrix V is the inverse of M, in uploadView(): view_matrix = glm::inverse(m_view_transform);

// In shaders, we only have access to the view matrix, but we need to find out the position of the camera in world space:
// M = V^-1; eye_world = M0,3; M1,3; M2,3;
// => that is to say, take the 4th column of the inverted view matrix 