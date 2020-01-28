#version 150

in vec2 pass_TexCoord;

out vec4 out_Color;

//upload the Texture Unit of the color textures as uniform
//do a texture lookup in the fragment shader
uniform sampler2D colorTexture;

uniform int modelType;

//7: GrayScale
//8: Horizontal Mirroring
//9: Vertical Mirroring
//0: Blur with 3*3 Gaussian Kernel

vec4 gaussian_kernel_blur(vec2 pass_TexCoord){
    
    vec2 pixel_size = pass_TexCoord / gl_FragCoord.xy; // pass_TexCoord = pixel_size * gl_FragCoord
    vec4 sum = vec4(0.0);

    // 3x3 matrix
    // |(-x, +y)/16| |(+0, +y)/8| |(+x, +y)/16|
    // |(-x, +0)/8 | |(+0, +0)/4| |(+x, +0)/8 |
    // |(-x, -y)/16| |(+0, -y)/8| |(+x, -y)/16|

    sum = sum+texture(colorTexture, (vec2(pass_TexCoord.x - pixel_size.x, pass_TexCoord.y + pixel_size.y))) * 0.0625;
    sum = sum+texture(colorTexture, (vec2(pass_TexCoord.x, pass_TexCoord.y + pixel_size.y))) * 0.125;
    sum = sum+texture(colorTexture, (vec2(pass_TexCoord.x + pixel_size.x, pass_TexCoord.y + pixel_size.y))) * 0.0625;
    sum = sum+texture(colorTexture, (vec2(pass_TexCoord.x - pixel_size.x, pass_TexCoord.y))) * 0.125;
    sum = sum+texture(colorTexture, (vec2(pass_TexCoord.x, pass_TexCoord.y))) * 0.25;
    sum = sum+texture(colorTexture, (vec2(pass_TexCoord.x + pixel_size.x, pass_TexCoord.y))) * 0.125;
    sum = sum+texture(colorTexture, (vec2(pass_TexCoord.x - pixel_size.x, pass_TexCoord.y - pixel_size.y))) * 0.0625;
    sum = sum+texture(colorTexture, (vec2(pass_TexCoord.x , pass_TexCoord.y - pixel_size.y))) * 0.125;
    sum = sum+texture(colorTexture, (vec2(pass_TexCoord.x + pixel_size.x, pass_TexCoord.y - pixel_size.y))) * 0.0625;

    //for(int i = 0; i <= 8; i++){
    //    sum += pixel_color[i];
    //}

    return sum;
}



void main(){
	
	vec2 pass_TexCoord_in = pass_TexCoord;
	float luminance;

	if(modelType==8){ //horizontal mirroring , invert the texture coordinates around the desired axes
		pass_TexCoord_in.y = 1.0 - pass_TexCoord_in.y;

	}
	if(modelType==9){ //vertical mirroring
		pass_TexCoord_in.x = 1.0 - pass_TexCoord_in.x;
	}

	out_Color = texture(colorTexture, pass_TexCoord_in);


	if(modelType==7){ // grayscale
		luminance = (0.2126*out_Color.r + 0.7152*out_Color.g+0.0722*out_Color.b);
		out_Color = vec4( vec3(luminance), 1.0);
	}
	if(modelType==0){ // blur
		out_Color = gaussian_kernel_blur(pass_TexCoord_in);
	}
			
}