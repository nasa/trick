#include <GL/glew.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include <vector>
#include <unistd.h>

#include "../../include/Fluid.hh"
#include "gui.h"
#include "grid_cell.h"
#include "marching_cubes.h"
#include "glm/gtx/io.hpp"

const char* particle_vertex_shader =
#include "shaders/particle.vert"
;

const char* particle_geometry_shader =
#include "shaders/particle.geom"
;

const char* particle_fragment_shader = 
#include "shaders/particle.frag"
;

const char* mesh_vertex_shader =
#include "shaders/mesh.vert"
;

const char* mesh_geometry_shader =
#include "shaders/mesh.geom"
;

const char* mesh_fragment_shader =
#include "shaders/mesh.frag"
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
bool meshMode = false;


enum {kVertexBufferMesh, kIndexBufferMesh, kNumVbosMesh};
enum {kVertexBufferParticle, kParticleIndex, kIndexBufferParticle, kNumVbosParticle};


GLuint mesh_buffer_objects[kNumVbosMesh];
GLuint particle_buffer_objects[kNumVbosParticle];


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



int openGLMain(Fluid* fluid)
{

	char* obj_file = "100_sphere.obj";

	std::cout << obj_file << std::endl;


	GLFWwindow *window = init_glefw();
	GUI gui(window, window_width, window_height, preview_height);

	std::vector<glm::vec4> particle_vertices;
	std::vector<glm::uvec3> particle_faces;


	loadObj(obj_file, particle_vertices, particle_faces, 0);
	int num_vert = particle_vertices.size();

	for (int i = 1; i < fluid->NUM_PARTICLES; i++) {
		loadObj(obj_file, particle_vertices, particle_faces, i * num_vert);
	}

	/* Assign index to each particle model */
	std::vector<int> particle_idx;

	for(int i = 0; i < fluid->NUM_PARTICLES; i++) {
		for (int p = 0; p < num_vert; p++) {
			particle_idx.push_back(i);
		}
	}


	std::vector<float> particlePositions = fluid->getParticlePositions();

	
	const int MC_GRID_DIM = 16;
	const int RADIUS = 2 * fluid->BOUND / MC_GRID_DIM;
	// number of particles within MC GridCell vertex;
	int PARTICLES_WITHIN_VERTEX = 1;

	std::vector<GridCell> gridCells;
	initializeGridCells(gridCells, fluid->BOUND, MC_GRID_DIM);
	
	updateIsoValues(gridCells, particlePositions, RADIUS);
	std::vector<glm::vec4> mesh_vertices;
	std::vector<glm::uvec3> mesh_faces;

	for (int i = 0; i < gridCells.size(); i++) {
		generateCellMesh(gridCells[i], PARTICLES_WITHIN_VERTEX, mesh_faces, mesh_vertices);
	}
	
	

	glm::vec4 light_position = glm::vec4(0.0f, 0.0f, fluid->BOUND + 10.0f, 1.0f);
	MatrixPointers mats; 

	/* Initialize mesh OpenGL Program */
	GLuint meshVAO;
	// Setup VAO for mesh program
	glGenVertexArrays(1, &meshVAO);
	// Bind VAO for mesh program
	glBindVertexArray(meshVAO);
	
	// Generate VBOs for mesh program
	glGenBuffers(kNumVbosMesh, &mesh_buffer_objects[0]);

	// Setup particle vertex data in VBO
	glBindBuffer(GL_ARRAY_BUFFER, mesh_buffer_objects[kVertexBufferMesh]);
	// Describe vertex data to OpenGL
	glBufferData(GL_ARRAY_BUFFER, 
				sizeof(float) * mesh_vertices.size() * 4, mesh_vertices.data(),
				GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);


	// Setup element array buffer. 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_buffer_objects[kIndexBufferMesh]);
	// Describe elemnt array buffer to OpenGL
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
					sizeof(uint32_t) * mesh_faces.size() * 3,
					mesh_faces.data(), GL_DYNAMIC_DRAW);

	// Setup mesh vertex shader.
	GLuint mesh_vertex_shader_id = 0;
	mesh_vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(mesh_vertex_shader_id, 1, &mesh_vertex_shader, nullptr);
	glCompileShader(mesh_vertex_shader_id);


	// Setup mesh geometry shader.
	GLuint mesh_geometry_shader_id = 0;
	mesh_geometry_shader_id = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(mesh_geometry_shader_id, 1, &mesh_geometry_shader, nullptr);
	glCompileShader(mesh_geometry_shader_id);


	// Setup mesh fragment shader.
	GLuint mesh_fragment_shader_id = 0;
	mesh_fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(mesh_fragment_shader_id, 1, &mesh_fragment_shader, nullptr);
	glCompileShader(mesh_fragment_shader_id);

	// Setup program for the particles, and get its locations.
	GLuint mesh_program_id = 0;
	mesh_program_id = glCreateProgram();
	glAttachShader(mesh_program_id, mesh_vertex_shader_id);
	glAttachShader(mesh_program_id, mesh_geometry_shader_id);
	glAttachShader(mesh_program_id, mesh_fragment_shader_id);

	// Bind attributes.
	glBindAttribLocation(mesh_program_id, 0, "vertex_position");
	glBindFragDataLocation(mesh_program_id, 0, "fragment_color");
	glLinkProgram(mesh_program_id);


	// Get the mesh uniform locations.
	GLint mesh_projection_matrix_location = 0;
	mesh_projection_matrix_location =
		glGetUniformLocation(mesh_program_id, "projection");

	GLint mesh_view_matrix_location = 0;
	mesh_view_matrix_location = 
		glGetUniformLocation(mesh_program_id, "view");

	GLint mesh_model_matrix_location = 0;
	mesh_model_matrix_location = 
		glGetUniformLocation(mesh_program_id, "model");

	GLint mesh_light_position_location = 0;
	mesh_light_position_location = 
		glGetUniformLocation(mesh_program_id, "light_position");

	/* Initialize particle OpenGL Program */
	GLuint particleVAO;
	// Setup VAO for particle program
	glGenVertexArrays(1, &particleVAO);
	// Bind VAO for particle program
	glBindVertexArray(particleVAO);
	
	// Generate VBOs for particle program
	glGenBuffers(kNumVbosParticle, &particle_buffer_objects[0]);

	// Setup particle vertex data in VBO
	glBindBuffer(GL_ARRAY_BUFFER, particle_buffer_objects[kVertexBufferParticle]);
	// Describe vertex data to OpenGL
	glBufferData(GL_ARRAY_BUFFER, 
				sizeof(float) * particle_vertices.size() * 4, particle_vertices.data(),
				GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// Setup particle index datas in VBO
	glBindBuffer(GL_ARRAY_BUFFER, particle_buffer_objects[kParticleIndex]);
	// Describe particle index data to OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(int) * particle_idx.size(), particle_idx.data(), GL_STATIC_DRAW);
	glVertexAttribIPointer(1, 1, GL_INT, 0, 0);
	glEnableVertexAttribArray(1);


	// Setup element array buffer. 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, particle_buffer_objects[kIndexBufferParticle]);
	// Describe element array buffer to OpenGL
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
					sizeof(uint32_t) * particle_faces.size() * 3,
					particle_faces.data(), GL_DYNAMIC_DRAW);

	// Setup particle vertex shader.
	GLuint particle_vertex_shader_id = 0;
	particle_vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(particle_vertex_shader_id, 1, &particle_vertex_shader, nullptr);
	glCompileShader(particle_vertex_shader_id);


	// Setup particle geometry shader.
	GLuint particle_geometry_shader_id = 0;
	particle_geometry_shader_id = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(particle_geometry_shader_id, 1, &particle_geometry_shader, nullptr);
	glCompileShader(particle_geometry_shader_id);


	// Setup particle fragment shader.
	GLuint particle_fragment_shader_id = 0;
	particle_fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(particle_fragment_shader_id, 1, &particle_fragment_shader, nullptr);
	glCompileShader(particle_fragment_shader_id);

	// Setup program for the particles, and get its locations.
	GLuint particle_program_id = 0;
	particle_program_id = glCreateProgram();
	glAttachShader(particle_program_id, particle_vertex_shader_id);
	glAttachShader(particle_program_id, particle_geometry_shader_id);
	glAttachShader(particle_program_id, particle_fragment_shader_id);

	// Bind attributes.
	glBindAttribLocation(particle_program_id, 0, "vertex_position");
	glBindAttribLocation(particle_program_id, 1, "idx");
	glBindFragDataLocation(particle_program_id, 0, "fragment_color");
	glLinkProgram(particle_program_id);


	// Get the particle uniform locations.
	GLint particle_projection_matrix_location = 0;
	particle_projection_matrix_location =
		glGetUniformLocation(particle_program_id, "projection");

	GLint particle_view_matrix_location = 0;
	particle_view_matrix_location = 
		glGetUniformLocation(particle_program_id, "view");

	GLint particle_model_matrix_location = 0;
	particle_model_matrix_location = 
		glGetUniformLocation(particle_program_id, "model");

	GLint particle_light_position_location = 0;
	particle_light_position_location = 
		glGetUniformLocation(particle_program_id, "light_position");

	/* Setup TBO for particle position data*/
	GLuint sphTBO;
	GLuint sph_tbo_texture;
	glGenBuffers(1, &sphTBO);
	glBindBuffer(GL_TEXTURE_BUFFER, sphTBO);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * particlePositions.size(), particlePositions.data(), GL_DYNAMIC_DRAW);


	glGenTextures(1, &sph_tbo_texture);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	GLint sph_tbo_pos = glGetUniformLocation(particle_program_id, "sph_tbo_pos");

	float aspect = 0.0f;
	bool draw_floor = false;
	double time = 0.0f;
	int timeStep = 0;
	int oldMeshFaces = mesh_faces.size();
	while (!glfwWindowShouldClose(window)) {
		
		glfwGetFramebufferSize(window, &window_width, &window_height);
		glViewport(0, 0, window_width, window_height);
		
		
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_BLEND);
		//glEnable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthFunc(GL_LESS);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glCullFace(GL_BACK);
		
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
		
		if (meshMode) {
			particlePositions = fluid->getParticlePositions();

			if (fluid->timeSteps % 10 == 0) {
				updateIsoValues(gridCells, particlePositions, RADIUS);
				
				mesh_vertices.clear();
				mesh_faces.clear();
				gridCells.clear();
				initializeGridCells(gridCells, fluid->BOUND, MC_GRID_DIM);
				
				updateIsoValues(gridCells, particlePositions, RADIUS);
				for (int i = 0; i < gridCells.size(); i++) {
					generateCellMesh(gridCells[i], PARTICLES_WITHIN_VERTEX, mesh_faces, mesh_vertices);
				}
				
				glBindVertexArray(meshVAO);
				glBindBuffer(GL_ARRAY_BUFFER, mesh_buffer_objects[kVertexBufferMesh]);
				glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float) * mesh_vertices.size(), mesh_vertices.data(), GL_DYNAMIC_DRAW);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_buffer_objects[kIndexBufferMesh]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
									sizeof(uint32_t) * mesh_faces.size() * 3,
									mesh_faces.data(), GL_DYNAMIC_DRAW);
			}


			// Switch VAO
			glBindVertexArray(meshVAO);

			// Switch Program
			glUseProgram(mesh_program_id);

			// Pass uniforms
			glUniformMatrix4fv(mesh_projection_matrix_location, 1, GL_FALSE,
				&((*mats.projection)[0][0]));

			glUniformMatrix4fv(mesh_view_matrix_location, 1, GL_FALSE,
				&((*mats.view)[0][0]));
			
			glUniformMatrix4fv(mesh_model_matrix_location, 1, GL_FALSE,
				&((*mats.model)[0][0]));
			
			glUniform4fv(mesh_light_position_location, 1, &light_position[0]);
			
			
			glDrawElements(GL_TRIANGLES, 3 * mesh_faces.size(), GL_UNSIGNED_INT, 0);
		} else {
			particlePositions = fluid->getParticlePositions();

			// Switch VAO
			glBindVertexArray(particleVAO);

			// Switch Program
			glUseProgram(particle_program_id);

			// Setup TBO
			glBindBuffer(GL_TEXTURE_BUFFER, sphTBO);
			glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * particlePositions.size(), particlePositions.data(), GL_DYNAMIC_DRAW);

			// Pass uniforms
			glUniformMatrix4fv(particle_projection_matrix_location, 1, GL_FALSE,
				&((*mats.projection)[0][0]));

			glUniformMatrix4fv(particle_view_matrix_location, 1, GL_FALSE,
				&((*mats.view)[0][0]));
			
			glUniformMatrix4fv(particle_model_matrix_location, 1, GL_FALSE,
				&((*mats.model)[0][0]));
			
			glUniform4fv(particle_light_position_location, 1, &light_position[0]);
			
			
			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_BUFFER, sph_tbo_texture);
			glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, sphTBO);

			glUniform1i(sph_tbo_pos, 0);

			glDrawElements(GL_TRIANGLES, 3 * particle_faces.size(), GL_UNSIGNED_INT, 0);
		}
	
		glfwPollEvents();
		glfwSwapBuffers(window);
		
		double refreshRate = 60;
		usleep(1000000 / refreshRate);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
