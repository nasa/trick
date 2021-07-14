#include <GL/glew.h>

#include "sph.h"
#include "gui.h"
#include "custom_client.h"

#include <fstream>
#include <iostream>
#include <cstring>
#include <vector>

#include "glm/gtx/io.hpp"

const char* sph_vertex_shader =
#include "shaders/sph.vert"
;

const char* sph_geometry_shader =
#include "shaders/sph.geom"
;

const char* sph_fragment_shader =
#include "shaders/sph.frag"
;

int window_width = 960;
int window_height = 720;
int main_view_width = 960;
int main_view_height = 720;
int preview_width = window_width - main_view_width; 
int preview_height = preview_width / 4 * 3;
int preview_bar_width = preview_width;
int preview_bar_height = main_view_height;
const std::string window_title = "Animation";



bool paused = true;

extern int selectedKeyframe;

enum {kVertexBuffer, kParticleIndex, kIndexBuffer, kNumVbos};
GLuint sph_buffer_objects[kNumVbos];


void ErrorCallback(int error, const char* description) {
	std::cerr << "GLFW Error: " << description << "\n";
}

GLFWwindow* init_glefw()
{
	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwSetErrorCallback(ErrorCallback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // Disable resizing, for simplicity
	glfwWindowHint(GLFW_SAMPLES, 4);
	auto ret = glfwCreateWindow(window_width, window_height, window_title.data(), nullptr, nullptr);

	glfwMakeContextCurrent(ret);
	glewExperimental = GL_TRUE;

	glewInit();
	glGetError();  // clear GLEW's error for it
	glfwSwapInterval(1);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	std::cout << "Renderer: " << renderer << "\n";
	std::cout << "OpenGL version supported:" << version << "\n";

	return ret;
}

void
loadObj(const std::string& file, std::vector<glm::vec4>& vertices,
	std::vector<glm::uvec3>& indices, int cur_vertex)
{
	std::ifstream obj_file(file);
	std::string line;

	while (getline(obj_file, line)) {
		if (line.at(0) == 'v') {
			const char* buf = line.c_str();
			char* ptr = (char*)malloc(strlen(buf));
			strcpy(ptr, buf);
			char* token = strtok(ptr, " ");

			std::vector<float> line_vertices;
			int i = 0;
			while (token != NULL) {
				if (i > 0) {
					line_vertices.push_back(atof(token));
				}
				token = strtok(NULL, " ");
				i++;
			}


			vertices.push_back(glm::vec4(line_vertices[0], line_vertices[1], line_vertices[2], 1.0f));

		}
		else if (line.at(0) == 'f') {
			const char* buf = line.c_str();
			char* ptr = (char*)malloc(strlen(buf));
			strcpy(ptr, buf);
			char* token = strtok(ptr, " ");

			std::vector<int> line_indicies;
			int i = 0;
			while (token != NULL) {
				if (i > 0) {
					line_indicies.push_back(atoi(token));
				}
				token = strtok(NULL, " ");
				i++;
			}


			indices.push_back(glm::uvec3(cur_vertex + line_indicies[0] - 1, cur_vertex + line_indicies[1] - 1, cur_vertex + line_indicies[2] - 1));

		}
	}
}



int main(int argc, char* argv[])
{

	if (argc < 2) {
		std::cerr << "Input model file is missing" << std::endl;
		std::cerr << "Usage: " << argv[0] << " <OBJ file>" << std::endl;
		return -1;
	}

	std::cout << argv[1] << std::endl;
	GLFWwindow *window = init_glefw();
	GUI gui(window, window_width, window_height, preview_height);
	
	int sockfd = setupComm();


	/* Load particle models from .obj file */
	std::vector<glm::vec4> sph_vertices;
	std::vector<glm::uvec3> sph_faces;

	loadObj(argv[1], sph_vertices, sph_faces, 0);

	int num_vert = sph_vertices.size();
	for (int i = 1; i < NUM_PARTICLES; i++) {
		loadObj(argv[1], sph_vertices, sph_faces, i * num_vert);
	}

	/* Assign index to each particle model */
	std::vector<int> particle_idx;

	for (int i = 0; i < NUM_PARTICLES; i++) {
		for (int p = 0; p < num_vert; p++) {
			particle_idx.push_back(i);
		}
	}

	/* Initialize SPH simulation state */
	//initSPH();
	std::vector<float> particle_positions;
	

	glm::vec4 light_position = glm::vec4(0.0f, 0.0f, BOUND + 10.0f, 1.0f);
	MatrixPointers mats; 

	/* Initialize SPH OpenGL Program */
	GLuint sphVAO;
	// Setup VAO for SPH program
	glGenVertexArrays(1, &sphVAO);
	// Bind VAO for SPH program
	glBindVertexArray(sphVAO);
	
	// Generate VBOs for SPH program
	glGenBuffers(kNumVbos, &sph_buffer_objects[0]);

	// Setup particle vertex data in VBO
	glBindBuffer(GL_ARRAY_BUFFER, sph_buffer_objects[kVertexBuffer]);
	// Describe vertex data to OpenGL
	glBufferData(GL_ARRAY_BUFFER, 
				sizeof(float) * sph_vertices.size() * 4, sph_vertices.data(),
				GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// Setup particle index data in VBO
	glBindBuffer(GL_ARRAY_BUFFER, sph_buffer_objects[kParticleIndex]);
	// Describe particle index data to OpenGL
	glBufferData(GL_ARRAY_BUFFER,
					sizeof(int) * particle_idx.size(), particle_idx.data(),
					GL_STATIC_DRAW);
	glVertexAttribIPointer(1, 1, GL_INT, 0, 0);
	glEnableVertexAttribArray(1);

	// Setup element array buffer. 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sph_buffer_objects[kIndexBuffer]);
	// Describe elemnt array buffer to OpenGL
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
					sizeof(uint32_t) * sph_faces.size() * 3,
					sph_faces.data(), GL_STATIC_DRAW);

	// Setup vertex shader.
	GLuint vertex_shader_id = 0;
	const char* vertex_source_pointer = sph_vertex_shader;
	vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader_id, 1, &vertex_source_pointer, nullptr);
	glCompileShader(vertex_shader_id);


	// Setup geometry shader.
	GLuint geometry_shader_id = 0;
	const char* geometry_source_pointer = sph_geometry_shader;
	geometry_shader_id = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry_shader_id, 1, &geometry_source_pointer, nullptr);
	glCompileShader(geometry_shader_id);


	// Setup fragment shader.
	GLuint fragment_shader_id = 0;
	const char* fragment_source_pointer = sph_fragment_shader;
	fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader_id, 1, &fragment_source_pointer, nullptr);
	glCompileShader(fragment_shader_id);

	// Setup program for the particles, and get its locations.
	GLuint sph_program_id = 0;
	sph_program_id = glCreateProgram();
	glAttachShader(sph_program_id, vertex_shader_id);
	glAttachShader(sph_program_id, geometry_shader_id);
	glAttachShader(sph_program_id, fragment_shader_id);

	// Bind attributes.
	glBindAttribLocation(sph_program_id, 0, "vertex_position");
	glBindAttribLocation(sph_program_id, 1, "idx");
	glBindFragDataLocation(sph_program_id, 0, "fragment_color");
	glLinkProgram(sph_program_id);


	// Get the uniform locations.
	GLint projection_matrix_location = 0;
	projection_matrix_location =
		glGetUniformLocation(sph_program_id, "projection");

	GLint view_matrix_location = 0;
	view_matrix_location = 
		glGetUniformLocation(sph_program_id, "view");

	GLint model_matrix_location = 0;
	model_matrix_location = 
		glGetUniformLocation(sph_program_id, "model");

	GLint light_position_location = 0;
	light_position_location = 
		glGetUniformLocation(sph_program_id, "light_position");

	/* SPH TBO */
	GLuint sphTBO;
	GLuint sph_tbo_texture;
	glGenBuffers(1, &sphTBO);
	glBindBuffer(GL_TEXTURE_BUFFER, sphTBO);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(float)* particle_positions.size(), particle_positions.data(), GL_DYNAMIC_DRAW);
	
	glGenTextures(1, &sph_tbo_texture);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	GLint sph_tbo_pos = glGetUniformLocation(sph_program_id, "sph_tbo_pos");

	float aspect = 0.0f;
	bool draw_floor = false;
	double time = 0.0f;
	
	while (!glfwWindowShouldClose(window)) {
		// Setup some basic window stuff.
		
		glfwGetFramebufferSize(window, &window_width, &window_height);
		glViewport(0, 0, window_width, window_height);
		
		
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthFunc(GL_LESS);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glCullFace(GL_BACK);
		
		gui.updateMatrices();
		mats = gui.getMatrixPointers();

		std::stringstream title;

		title << window_title;
		if (!paused) {
			title << " Playing: "
			      << std::setprecision(2)
			      << std::setfill('0') << std::setw(6)
			      << time << " sec";
			
		}
		else
		{
			title << " Paused: "
				<< std::setprecision(2)
				<< std::setfill('0') << std::setw(6)
				<< time << " sec";
		}

		glfwSetWindowTitle(window, title.str().data());

		/*
		if (!paused) {
			updateSPH(0, EDGE_NUM_PARTICLES * EDGE_NUM_PARTICLES * PARTICLE_DEPTH);
			time += DT;
			printf("%f\n", time);
		}*/
		
		particle_positions = receiveParticlePositions(sockfd);

		// Setup TBO
		glBindBuffer(GL_TEXTURE_BUFFER, sphTBO);
		glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * particle_positions.size(), particle_positions.data(), GL_DYNAMIC_DRAW);

		// Switch VAO
		glBindVertexArray(sphVAO);

		// Switch Program
		glUseProgram(sph_program_id);

		// Pass uniforms
		glUniformMatrix4fv(projection_matrix_location, 1, GL_FALSE,
			&((*mats.projection)[0][0]));

		glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE,
			&((*mats.view)[0][0]));
		
		glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE,
			&((*mats.model)[0][0]));
		
		glUniform4fv(light_position_location, 1, &light_position[0]);
		
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_BUFFER, sph_tbo_texture);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, sphTBO);
		
		glUniform1i(sph_tbo_pos, 0);
		
		glDrawElements(GL_TRIANGLES, 3 * sph_faces.size(), GL_UNSIGNED_INT, 0);
	

	
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
