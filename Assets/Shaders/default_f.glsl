#version 420

uniform vec4 u_lightPos;
uniform vec4 u_colour;

// Note: Uniform bindings
// This lets you specify the texture unit directly in the shader!
layout(binding = 0) uniform sampler2D u_rgb; // rgb texture

// Fragment Shader Inputs
in VertexData
{
	vec3 normal;
	vec3 texCoord;
	vec4 colour;
	vec3 posEye;
} vIn;

// Multiple render targets!
// Notice that we now have two outputs
// This means this fragment shader can write to 2 different textures!
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 FragNormal;

vec3 diffuse()
{
	vec3 L = normalize(u_lightPos.xyz - vIn.posEye);
	vec3 N = normalize(vIn.normal);

	float ndotl = max(0.0, dot(N, L));
	float diffuseIntensity = 1.0;
	vec3 diffuseColor = texture(u_rgb, vIn.texCoord.xy).rgb;

	if (length(diffuseColor) == 0)
		diffuseColor = vec3(0.5);

	vec3 diffuse = diffuseIntensity * ndotl * diffuseColor;

	return diffuse;
}

void main()
{
	// Write to color texture (FBO attachment 0)
	FragColor = vec4(diffuse() + u_colour.rgb, 1.0);

	// Write to normal texture (FBO attachment 1)
	vec3 N = normalize(vIn.normal);

	// Normals can be in the range of (-1, 1) but our textures
	// can only store data between the range (0, 1)
	// So we need to shift the normal from the range (-1, 1) to (0, 1)
	N = N * 0.5 + 0.5;

	FragNormal = vec4(N, 1.0);
}