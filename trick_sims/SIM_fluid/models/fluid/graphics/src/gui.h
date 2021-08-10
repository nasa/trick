
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <GLFW/glfw3.h>


struct MatrixPointers { 
	const glm::mat4 *projection, *model, *view;
};

class GUI {
public:
	GUI(GLFWwindow*, int view_width = -1, int view_height = -1, int preview_height = -1);
	~GUI();

	void keyCallback(int key, int scancode, int action, int mods);
	void mousePosCallback(double mouse_x, double mouse_y);
	void mouseButtonCallback(int button, int action, int mods);
	void updateMatrices();
	MatrixPointers getMatrixPointers() const;

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MousePosCallback(GLFWwindow* window, double mouse_x, double mouse_y);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

private:
	GLFWwindow* window_;

	int window_width_, window_height_;
	int view_width_, view_height_;
	int preview_height_;

	bool drag_state_ = false;
	bool fps_mode_ = true;
	
	int current_button_ = -1;

	float last_x_ = 0.0f, last_y_ = 0.0f, current_x_ = 0.0f, current_y_ = 0.0f;
	float camera_distance_ = 300.0;
	float camera_depth_ = 0.f;

	float pan_speed_ = 5.f;
	float rotation_speed_ = 0.02f;
	float zoom_speed_ = 5.f;
	float aspect_;

	glm::vec3 eye_ = glm::vec3(0.0f, camera_depth_, camera_distance_);
	glm::vec3 up_ = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 look_ = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 tangent_ = glm::cross(look_, up_);
	glm::vec3 center_ = eye_ - camera_distance_ * look_;
	glm::mat3 orientation_ = glm::mat3(tangent_, up_, look_);
	glm::vec4 light_position_;

	glm::mat4 view_matrix_ = glm::lookAt(eye_, center_, up_);
	glm::mat4 projection_matrix_;
	glm::mat4 model_matrix_ = glm::mat4(1.0f);

	bool captureWASDUPDOWN(int key, int action);
};
