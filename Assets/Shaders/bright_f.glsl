#version 420

layout(binding = 0) uniform sampler2D u_scene; // original scene image

uniform float u_bloomThreshold;

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
	// IMPLEMENT BRIGHT PASS HERE
	// - See pseudo code in lab document
	////////////////////////////////////////////////////////////////////////// 
	vec4 c = texture(u_scene, vIn.texCoord.xy);

	vec4 bright = vec4((c-vec4(u_bloomThreshold))/ (1-(u_bloomThreshold)));
	FragColor = clamp(bright, 0.0,1.0);
}