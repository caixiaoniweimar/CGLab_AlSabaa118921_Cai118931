#version 150

in vec3 pass_Normal;
in vec3 pass_Position;
in vec3 pass_Eye_Position;

out vec4 out_Color;

uniform vec3 PlanetColor;
uniform vec3 Ambient; // only a color, usually really dark
uniform vec3 LightColor;  // cal_lightColor() method already intensity*light_color

//Point light sources, put it at sun's position 
vec3 lightPosition=vec3(0, 0, 0);
uniform float Ka;
uniform float Kd;
uniform float Ks;

uniform float Shininess;

float reflectivity=5.0;

//CelShading: https://en.wikibooks.org/wiki/GLSL_Programming/Unity/Toon_Shading
uniform bool ifCelShading;

float UnlitOutlineThickness = 0.4;
float LitOutlineThickness = 0.1;

//Assignment3: BlinnPhong
void main() {

		vec3 N = normalize(pass_Normal); //normal
		vec3 L = lightPosition - pass_Position;
		vec3 V = pass_Eye_Position - pass_Position;

		vec3 H = normalize(L+V);

		L = normalize(L);  //lightsource direction
		V = normalize(V); // view direction

		//ambient
		vec3 ambient = Ambient*Ka;
        
        //diffuse reflection:
		float distance = length(lightPosition - pass_Position);

		vec3 light = LightColor/( (4*3.14159265359)* pow(distance,2) ); //intensity of the light that reaches the processed fragment

		float diffuseLight = max(dot(N,L), 0);
		vec3 diffuse = Kd*light*diffuseLight*(reflectivity/3.14159265359); //03-Shaders slide 8

		//specular
		float specularLight = pow(max(dot(H,N),0), Shininess);                   
		if(diffuseLight <= 0) specularLight = 0;

		vec3 specular = Ks*light*specularLight;

	if(ifCelShading){ //call 2, CelShading

		const int levels =3;
		const float weights = 1.0/levels;
		vec3 diffuseColor = Kd*light*floor(diffuseLight*levels)*weights*(reflectivity/3.14159265359); // floor :round down

		vec3 specularColor;
		if( dot(L,N)>0.0 ){ // light source on the right side
			specularColor = Ks*light*pow(max(0, dot(H,N)), 4*Shininess);
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