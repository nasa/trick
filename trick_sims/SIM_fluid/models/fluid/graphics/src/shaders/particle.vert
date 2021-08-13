R"zzz(#version 330 core

in vec4 vertex_position;
in int idx;
uniform samplerBuffer sph_tbo_pos;
uniform vec4 light_position;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

out vec4 vs_light_direction;
out int vs_idx;

void main() {
	
	vs_idx = idx;
	vec3 vertex_pos = vertex_position.xyz;
	float x = texelFetch(sph_tbo_pos, 3 * idx).r;
	float y = texelFetch(sph_tbo_pos, 3 * idx + 1).r;
	float z = texelFetch(sph_tbo_pos, 3 * idx + 2).r;
	vec3 particle_pos = vec3(x, y, z);
	
	gl_Position = vec4(1 * vertex_pos + particle_pos, 1);
	vs_light_direction = light_position - gl_Position;

}
)zzz"
