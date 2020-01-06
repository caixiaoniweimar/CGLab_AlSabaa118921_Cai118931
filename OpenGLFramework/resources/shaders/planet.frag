#version 150

in vec3 pass_Normal;
in vec3 pass_Position;
in vec3 pass_Eye_Position;

out vec4 out_Color;

uniform vec3 PlanetColor;
uniform vec3 Ambient; // only a color, usually really dark
uniform vec3 LightColor;  // cal_lightColor() method already intensity*light_color

//Point light sources, put it at infinity one can simulate solar light
vec3 lightPosition=vec3(0, 0, 0);
uniform float Ka;
uniform float Kd;
uniform float Ks;

uniform float Shininess;

//CelShading: https://en.wikibooks.org/wiki/GLSL_Programming/Unity/Toon_Shading
uniform bool ifCelShading;

float UnlitOutlineThickness = 0.4;
float LitOutlineThickness = 0.1;

//Assignment3: BlinnPhong
void main() {

		vec3 N = normalize(pass_Normal); //normal
		vec3 L = normalize(lightPosition - pass_Position);  //lightsource direction
		vec3 V = normalize(pass_Eye_Position - pass_Position); // view direction

		vec3 H = normalize(L+V);
		//vec3 R = normalize( 2*dot(N,L)*N-L );

		//ambient
		vec3 ambient = Ambient*Ka;

		//diffuse reflection: Lambert Law, Lecture 3-Local Illumination models,page8
		float diffuseLight = max(dot(N,L), 0);
		vec3 diffuse = Kd*LightColor*diffuseLight;

		//specular
		float specularLight = pow(max(dot(H,N),0), Shininess);
		//float specularLight = pow(max(dot(R,V),0), Shininess);                    
		//if(diffuseLight <= 0) specularLight = 0;
		vec3 specular = Ks*LightColor*specularLight;

	if(ifCelShading){ //call 2, CelShading

		const int levels =3;
		const float weights = 1.0/levels;
		vec3 diffuseColor = Kd*LightColor*floor(diffuseLight*levels)*weights; // floor :round down

		vec3 specularColor;
		if( dot(L,N)>0.0 ){ // light source on the right side
			specularColor = Ks*LightColor*pow(max(0, dot(H,N)), Shininess);
		}
		
		float specLevel= 0; //limit the spec
		if( pow(dot(H,N),Shininess) > 0.5 ){ // more than half highlight intensity?
			specLevel = 1;
		}

		float outline = 0; // render dark outlines
		if( dot(V,N) > 0.2 ){
			outline = 1;
		}		

		out_Color = vec4( ((ambient + diffuseColor)*PlanetColor + specularColor*specLevel)*outline, 1.0); 

	}else{
		out_Color = vec4( (ambient + diffuse)*PlanetColor + specular, 1.0);
	}

//Outlines: in a specific color along the silhouettes of the model (usually black)
//determine whether a point is close enough to the silhouette. If it is, the fragment color is set to the color of the OutlineColor, dark, 0,0,0,1; 


}