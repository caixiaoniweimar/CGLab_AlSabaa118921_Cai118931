#version 150
in vec3 pass_Normal;
in vec3 pass_Position;
in vec3 pass_Eye_Position;

//Assignment 4: add input vec2 pass_TexCoord
in vec2 pass_TexCoord;  //then sample the texture with pass_TexCoord and use it as diffuse and ambient color in the shading computation

out vec4 out_Color;

uniform vec3 PlanetColor;
uniform vec3 LightColor;  // cal_lightColor() method already intensity*light_color
uniform float Ks;
uniform float Shininess;

//Assignment 4
uniform int modelType; //modelType=2, call CelShading: https://en.wikibooks.org/wiki/GLSL_Programming/Unity/Toon_Shading;  modelType=3, call ifNormalMapping
uniform bool ifNormalMapping;
uniform sampler2D colorTexture1;           //declare sampler variable
uniform sampler2D normalMap;
vec4 color1 = texture(colorTexture1, pass_TexCoord);    //read data from sampler, use it as diffuse and ambient color in the shading computation

vec3 Kd = vec3(color1.x, color1.y, color1.z);
vec3 Ambient = vec3(color1.x, color1.y, color1.z);
float reflectivity=1.0;
vec3 lightPosition=vec3(0, 0, 0); //Point light sources, put it at sun's position 
float normalScale = 0.5;

//Assignment3: BlinnPhong
void main() {

		vec3 N = normalize(pass_Normal); //normal

		if(modelType==4){   // call Normal Mapping, rewrite N;
			if(ifNormalMapping){
// http://hacksoflife.blogspot.com/2009/11/per-pixel-tangent-space-normal-mapping.html
// goal: convert normal map from tangent space to eye sapce, need to know S, T and N;
// S: horizontal axis of texture, T: vertical axis of texture; N -> normal vector
// derivation: measure the variation in the values between one element and its neighbours
// Tangent Space -> Model Space -> World Space -> View Space -> Clip Space

	            vec3 q0 = dFdx(pass_Position.xyz); //partial derivative of one pixel horizontally
	            vec3 q1 = dFdy(pass_Position.xyz); //partial derivative of one pixel vertically

//q0,q1 -> the change of eye space position; st0, st1 -> the change of the S/T coordinate of the UV map, flat representation of the surface of a 3D model, U-> horizontal axes, V->vertical 
	            vec2 st0 = dFdx(pass_TexCoord.st); 
	            vec2 st1 = dFdy(pass_TexCoord.st);
//transform the coordinates of the Map into model Space of the object

//convert normal map from tangent to eye space -> need unit vectors
	            vec3 S = normalize(q0 * st1.t - q1 * st0.t); //horizontal
	            vec3 T = normalize(-q0 * st1.s + q1 * st0.s); //vertical
	            vec3 N2 = normalize(pass_Normal); //normal

//the data in the normal map has to be stored in the range[0, -1], need to rescale it back to its original [-1, 1]
	            vec3 mapN = texture2D(normalMap, pass_TexCoord).xyz * 2.0 -1.0;
	            mapN.xy = normalScale * mapN.xy;
	            mat3 tsn = mat3 (S, T, N2); 

//re-define normal
	            N = normalize (tsn * mapN);

/*Pros: GET much simpler and lighter geomerty objects;*/
			}
		}
		vec3 L = lightPosition - pass_Position;
		vec3 V = pass_Eye_Position - pass_Position;

		vec3 H = normalize(L+V);

		L = normalize(L);  //lightsource direction
		V = normalize(V); // view direction

		//ambient
		vec3 ambient = Ambient;
        
        //diffuse reflection:
		float distance = length(lightPosition - pass_Position);

		vec3 light = LightColor/( (4*3.14159265359)* pow(distance,2) ); //intensity of the light that reaches the processed fragment

		float diffuseLight = max(dot(N,L), 0);
		vec3 diffuse = Kd*light*diffuseLight*(reflectivity/3.14159265359); //03-Shaders slide 8

		//specular
		float specularLight = pow(max(dot(H,N),0), Shininess);                   
		if(diffuseLight <= 0) specularLight = 0;

		vec3 specular = Ks*light*specularLight;

		if(modelType==2){ //call 2, CelShading

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

			out_Color = vec4( ((ambient + diffuseColor)*PlanetColor + specularColor*specLevel)*outline, 1.0 ); 

		}else{
			out_Color = vec4( (ambient + diffuse)*PlanetColor + specular, 1.0) ;
		}

//Outlines: in a specific color along the silhouettes of the model (usually black)
//determine whether a point is close enough to the silhouette. If it is, the fragment color is set to the color of the OutlineColor, dark, 0,0,0,1; 


}