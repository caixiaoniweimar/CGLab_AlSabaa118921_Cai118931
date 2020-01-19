#version 150
in vec3 pass_TexCoord;

out vec4 out_Color;

uniform samplerCube skybox;

void main()
{
	out_Color = texture(skybox, pass_TexCoord);
}

/*
in vertex shader, set the incoming position vectors as the outcoming texture coordinates for the fragment shader,
the fragment shader then takes these as input to sample a samplerCube
*/