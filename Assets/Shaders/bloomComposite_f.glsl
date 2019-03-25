#version 420

// Remember to bind the two textures to different texture units!
layout(binding = 0) uniform sampler2D u_bright; // bright pass image
layout(binding = 1) uniform sampler2D u_scene; // original scene image

// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 posEye;
} vIn;

layout(location = 0) out vec4 FragColor;

void main()
{
	//////////////////////////////////////////////////////////////////////////
	// COMPOSITE BLOOM HERE
	// - Sample from the two textures and add the colors together
	////////////////////////////////////////////////////////////////////////// 

	vec3 bright = texture(u_bright, vIn.texCoord.xy).rgb;
	vec3 scene = texture(u_scene, vIn.texCoord.xy).rgb;

	FragColor = vec4(bright + scene,1.0);
}