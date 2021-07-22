#include <GL/glew.h>

#include "gui.h"
#include "sph.h"
#include "grid_cell.h"
#include "marching_cubes.h"

#include <fstream>
#include <iostream>
#include <cstring>
#include <vector>

#include "glm/gtx/io.hpp"


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

extern int selectedKeyframe;

enum {kVertexBuffer, kParticleIndex, kIndexBuffer, kNumVbos};
GLuint mesh_buffer_objects[kNumVbos];





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



int main()
{

	char* obj_file = "../100_sphere.obj";

	std::cout << obj_file << std::endl;
	GLFWwindow *window = init_glefw();
	GUI gui(window, window_width, window_height, preview_height);

	initSPH();
	std::vector<float> particlePositions = getParticlePositions();

	
	const int MC_GRID_DIM = 16;
	const int RADIUS = 4 * BOUND / MC_GRID_DIM;
	// number of particles within MC GridCell vertex;
	double PARTICLES_WITHIN_VERTEX = 5;

	std::vector<GridCell> gridCells;
	initializeGridCells(gridCells, BOUND, MC_GRID_DIM);
	printf("Grid cells size: %d\n", gridCells.size());
	updateIsoValues(gridCells, particlePositions, RADIUS);
	std::vector<glm::vec4> mesh_vertices;
	std::vector<glm::uvec3> mesh_faces;

	for (int i = 0; i < gridCells.size(); i++) {
		generateCellMesh(gridCells[i], PARTICLES_WITHIN_VERTEX, mesh_faces, mesh_vertices);
	}
	
	printf("size of faces: %d\n", mesh_faces.size());

	/*
	GridCell testCell;

	int vertIdx = 0;
	for (int i = 0; i <= 1; i++) {
		for (int j = 0; j <= 1; j++) {
			for (int k = 0; k <= 1; k++) {
				testCell.vertices[vertIdx] = 10.f * glm::vec4(i, j, k, 0);
				vertIdx++;
			}
		}
	}*/

	printf("size of vertices: %d\n", mesh_vertices.size());
	

	glm::vec4 light_position = glm::vec4(0.0f, 0.0f, BOUND + 10.0f, 1.0f);
	MatrixPointers mats; 

	/* Initialize mesh OpenGL Program */
	GLuint meshVAO;
	// Setup VAO for mesh program
	glGenVertexArrays(1, &meshVAO);
	// Bind VAO for mesh program
	glBindVertexArray(meshVAO);
	
	// Generate VBOs for mesh program
	glGenBuffers(kNumVbos, &mesh_buffer_objects[0]);

	// Setup particle vertex data in VBO
	glBindBuffer(GL_ARRAY_BUFFER, mesh_buffer_objects[kVertexBuffer]);
	// Describe vertex data to OpenGL
	glBufferData(GL_ARRAY_BUFFER, 
				sizeof(float) * mesh_vertices.size() * 4, mesh_vertices.data(),
				GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);


	// Setup element array buffer. 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_buffer_objects[kIndexBuffer]);
	// Describe elemnt array buffer to OpenGL
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
					sizeof(uint32_t) * mesh_faces.size() * 3,
					mesh_faces.data(), GL_DYNAMIC_DRAW);

	// Setup vertex shader.
	GLuint vertex_shader_id = 0;
	const char* vertex_source_pointer = mesh_vertex_shader;
	vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader_id, 1, &vertex_source_pointer, nullptr);
	glCompileShader(vertex_shader_id);


	// Setup geometry shader.
	GLuint geometry_shader_id = 0;
	const char* geometry_source_pointer = mesh_geometry_shader;
	geometry_shader_id = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry_shader_id, 1, &geometry_source_pointer, nullptr);
	glCompileShader(geometry_shader_id);


	// Setup fragment shader.
	GLuint fragment_shader_id = 0;
	const char* fragment_source_pointer = mesh_fragment_shader;
	fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader_id, 1, &fragment_source_pointer, nullptr);
	glCompileShader(fragment_shader_id);

	// Setup program for the particles, and get its locations.
	GLuint mesh_program_id = 0;
	mesh_program_id = glCreateProgram();
	glAttachShader(mesh_program_id, vertex_shader_id);
	glAttachShader(mesh_program_id, geometry_shader_id);
	glAttachShader(mesh_program_id, fragment_shader_id);

	// Bind attributes.
	glBindAttribLocation(mesh_program_id, 0, "vertex_position");
	glBindFragDataLocation(mesh_program_id, 0, "fragment_color");
	glLinkProgram(mesh_program_id);


	// Get the uniform locations.
	GLint projection_matrix_location = 0;
	projection_matrix_location =
		glGetUniformLocation(mesh_program_id, "projection");

	GLint view_matrix_location = 0;
	view_matrix_location = 
		glGetUniformLocation(mesh_program_id, "view");

	GLint model_matrix_location = 0;
	model_matrix_location = 
		glGetUniformLocation(mesh_program_id, "model");

	GLint light_position_location = 0;
	light_position_location = 
		glGetUniformLocation(mesh_program_id, "light_position");


	float aspect = 0.0f;
	bool draw_floor = false;
	double time = 0.0f;
	int timeStep = 0;
	int oldMeshFaces = mesh_faces.size();
	while (!glfwWindowShouldClose(window)) {
	//while (true) {
		// Setup some basic window stuff.
		
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
		if (!paused) {



			time+= DT;
			printf("time: %f\n", time);
			//updateSPH(0, NUM_PARTICLES);
			//particlePositions = getParticlePositions();
			for (int i = 0; i < particlePositions.size() / 3; i++) {
				particlePositions[3 * i + 1] -= 1;
			}
			printf("%f\n", particlePositions[1]);
			if (timeStep % 1 == 0) {
				//updateIsoValues(gridCells, particlePositions, RADIUS);

				 
				printf("%d\n", mesh_vertices.size()); 
				for (int i = 0; i < gridCells.size(); i++) {
					generateCellMesh(gridCells[i], PARTICLES_WITHIN_VERTEX, mesh_faces, mesh_vertices);
				}
				printf("%d\n", mesh_vertices.size());
			}
			timeStep++;
			PARTICLES_WITHIN_VERTEX+=1;

						/* Update mesh face and vertex buffers data after updating isoValues */
			glBindVertexArray(meshVAO);
			glBindBuffer(GL_ARRAY_BUFFER, mesh_buffer_objects[kVertexBuffer]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh_vertices.size(), mesh_vertices.data(), GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_buffer_objects[kIndexBuffer]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
								sizeof(uint32_t) * mesh_faces.size() * 3,
								mesh_faces.data(), GL_DYNAMIC_DRAW);

		}

		
		//glBindBUffer(GL_FACE)


		// Switch VAO
		glBindVertexArray(meshVAO);

		// Switch Program
		glUseProgram(mesh_program_id);

		// Pass uniforms
		glUniformMatrix4fv(projection_matrix_location, 1, GL_FALSE,
			&((*mats.projection)[0][0]));

		glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE,
			&((*mats.view)[0][0]));
		
		glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE,
			&((*mats.model)[0][0]));
		
		glUniform4fv(light_position_location, 1, &light_position[0]);
		
		
		glDrawElements(GL_TRIANGLES, 3 * mesh_faces.size(), GL_UNSIGNED_INT, 0);


	
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
