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

vec4 outline_color = vec4(0,0,0,1);
float UnlitOutlineThickness = 0.4;
float LitOutlineThickness = 0.1;
float DiffuseThreshold = 0.1;

//Assignment3: BlinnPhong
void main() {

		vec3 N = normalize(pass_Normal); //normal
		vec3 L = normalize(lightPosition - pass_Position);  //lightsource direction
		vec3 V = normalize(pass_Eye_Position - pass_Position); // view direction

		//vec3 H = normalize(L+V);
		vec3 R = normalize( 2*dot(N,L)*N-L );

		//ambient
		vec3 ambient = Ambient*Ka;

		//diffuse reflection: Lambert Law, Lecture 3-Local Illumination models,page8
		float diffuseLight = max(dot(N,L), 0);
		vec3 diffuse = Kd*LightColor*diffuseLight;

		//specular
		//float specularLight = pow(max(dot(H,N),0), Shininess);
		float specularLight = pow(max(dot(R,V),0), Shininess); //lecture slides;
		//if(diffuseLight <= 0) specularLight = 0;
		vec3 specular = Ks*LightColor*specularLight;

	if(ifCelShading){ //call 2, CelShading

		out_Color = vec4( (ambient + diffuse)*PlanetColor + specular, 1.0);

		//low priority: diffuse illumination, not suitable;
		//if the diffuse reflection term of the Phong reaches a certain threshold, use full diffuse reflection color,switch to the lighter color
		// otherwise a second color.
		//if( max(0.0, dot(N,L)) >= DiffuseThreshold ){ // >= 0.1
		//	out_Color = vec4(LightColor*vec3(1,1,1), 1.0);
		//}

		//higher priority: outline
		if( dot(V,N) < mix(UnlitOutlineThickness, LitOutlineThickness, max(0.0, dot(N, L)) )){
			out_Color = vec4(LightColor*vec3(outline_color), 1.0);
		}

		//highest priority: highlights:
		// dot(N,L)>0, light source on the right side
		// specularLight could = pow( max(0.0, dot(reflect(-L,N),V)), Shininess ) > 0.5 

		// the specular highlights are plainly white without any addition of other colors, sharp boundary
		// compute the specular reflection term of Phong, then set the out_Color to the specular reflection color times the light source
		// if the specular reflection term is greater than a certain threshold, e.g: 0.5
		if( dot(N,L)>0 && specularLight> 0.5 ){ // more than half highlight intensity?
			out_Color = vec4( vec3(1,1,1)*LightColor, 1.0 );
			//out_Color = vec4(1,1,1,1);
		}

	}else{
		out_Color = vec4( (ambient + diffuse)*PlanetColor + specular, 1.0);
	}

//Outlines: in a specific color along the silhouettes of the model (usually black)

//Here we use another technique based on Section “Silhouette Enhancement”: if a fragment is determined to be close enough to a silhouette, it is set to the color of the outline. //This works only for smooth surfaces.
//the user could specify two overall outline thicknesses: one for fully lit areas and one for unlit areas (according to the diffuse reflection term of the Phong reflection //model). In between these extremes, the thickness parameter could be interpolated (again according to the diffuse reflection term). This, however, makes the outlines dependent //on a specific light source; therefore, the shader below renders outlines and diffuse illumination only for the first light source, which should usually be the most important //one. All other light sources only render specular highlights.

//The following implementation uses the mix instruction to interpolate between the _UnlitOutlineThickness (if the dot product of the diffuse reflection term is less or equal 0) //and _LitOutlineThickness (if the dot product is 1). For a linear interpolation from a value a to another value b with a parameter x between 0 and 1, GLSL offers the built-in //function mix(a, b, x). This interpolated value is then used as a threshold to determine whether a point is close enough to the silhouette. If it is, the fragment color is set //to the color of the outline _OutlineColor: 


}