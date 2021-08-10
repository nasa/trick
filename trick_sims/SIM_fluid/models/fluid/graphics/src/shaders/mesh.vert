R"zzz(#version 330 core

in vec4 vertex_position;
uniform vec4 light_position;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

out vec4 vs_light_direction;
out int vs_idx;

void main() {
	vec3 vertex_pos = vertex_position.xyz;

	
	gl_Position = vec4(vertex_pos, 1);
	vs_light_direction = light_position - gl_Position;

}
)zzz"
