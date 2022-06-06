#define STB_IMAGE_IMPLEMENTATION
#include "Renderer.h"
#include "stb_image.h"

float rotationxangle = 0;
float rotationyangle = 0;
float rotatezangle = 0;


Camera* Renderer::m_camera = new Camera();

Lighting* Renderer::m_lightings = new Lighting();

Animation* Renderer::m_animation = new Animation();

nanogui::Screen* Renderer::m_nanogui_screen = nullptr;


bool Renderer::keys[1024];
float A;
bool enabled = true;
enum test_enum {
	CCW = 0,
	CW
};
test_enum culling=CW;
enum render_enum {
	Solid = 0,
	WireFrame,
	Points
};
render_enum rtype = Solid;

std::string model="./objs/cube.obj";
Object main_object(model);
float direction_light_x=0;
float direction_light_y=-1;
float direction_light_z=-1;

bool direction_light_on = false;
nanogui::Color col_val(0.0f, 0.0f, 0.0f, 0.0f);

nanogui::Color col_val2(0.0f, 0.0f, 0.0f, 0.0f);
nanogui::Color col_val3(0.0f, 0.0f, 0.0f, 0.0f);

bool point_light_on = false;
nanogui::Color col_val4(0.0f, 0.0f, 0.0f, 0.0f);
nanogui::Color col_val5(0.0f, 0.0f, 0.0f, 0.0f);
nanogui::Color col_val6(0.0f, 0.0f, 0.0f, 0.0f);
bool light_rotate_x=false;
bool light_rotate_y;
bool light_rotate_z;
int currangle=0;
enum shader_enum {
	Flat=0,
	Smooth
};
shader_enum shadetype = Flat;
enum depth_enum {
	Less = 0,
	Always
};
depth_enum depthtest= Less;
bool diffuse_text_on;
bool normal_text_on;

Renderer::Renderer()
{
}


Renderer::~Renderer()
{	
}

void Renderer::nanogui_init(GLFWwindow* window)
{
	m_nanogui_screen = new nanogui::Screen();
	m_nanogui_screen->initialize(window, true);

	glViewport(0, 0, m_camera->width, m_camera->height);

	//glfwSwapInterval(0);
	//glfwSwapBuffers(window);

	// Create nanogui gui
	nanogui::FormHelper *gui_1 = new nanogui::FormHelper(m_nanogui_screen);
	nanogui::ref<nanogui::Window> nanoguiWindow_1 = gui_1->addWindow(Eigen::Vector2i(0, 0), "Nanogui control bar_1");

	//screen->setPosition(Eigen::Vector2i(-width/2 + 200, -height/2 + 300));

	gui_1->addVariable("Model Name", model);

	gui_1->addGroup("Camera Position");
	static auto camera_x_widget = gui_1->addVariable("X", m_camera->position[0]);
	static auto camera_y_widget = gui_1->addVariable("Y", m_camera->position[1]);
	static auto camera_z_widget = gui_1->addVariable("Z", m_camera->position[2]);
	

	gui_1->addButton("Change Model", [&]() {
		model_reset();
		});

	gui_1->addButton("Reset Camera", []() {
		m_camera->reset();
		camera_x_widget->setValue(m_camera->position[0]);
		camera_y_widget->setValue(m_camera->position[1]);
		camera_z_widget->setValue(m_camera->position[2]);
	});


	gui_1->addGroup("Camera Rotation");
	gui_1->addVariable("a", A);

	gui_1->addButton("Rotate X+", []() {
		m_camera->rotate_x(A);//insert X rotation  function here
		});
	gui_1->addButton("Rotate X-", []() {
		m_camera->rotate_x(-A);// insert y function here
		});
	gui_1->addButton("Rotate Y+", []() {
		m_camera->rotate_y(A);//insert X rotation  function here
		});
	gui_1->addButton("Rotate Y-", []() {
		m_camera->rotate_y(-A);// insert y function here
		});
	gui_1->addButton("Rotate Z+", []() {
		m_camera->rotate_z(A);// insert y function here
		});
	gui_1->addButton("Rotate Z-", []() {
		m_camera->rotate_z(-A);// insert y function here
		});
	gui_1->addVariable("culling type", culling, enabled)->setItems({ "CCW","CW" });
	gui_1->addVariable("Rendering Type", rtype, enabled)->setItems({ "Solid","WireFrame","Points" });
	gui_1->addVariable("Near clip plane", m_camera->near);
	gui_1->addVariable("Near clip plane", m_camera->far);
	gui_1->addVariable("Directional Light Status", direction_light_on);
	gui_1->addVariable("Directional Ambient Color", col_val);
	gui_1->addVariable("Directional Diffuse Color", col_val2);
	gui_1->addVariable("Directional Specular Color", col_val3);
	gui_1->addVariable("Point Light Status", point_light_on);
	gui_1->addVariable("Point Ambient Color", col_val4);
	gui_1->addVariable("Point Diffuse Color", col_val5);
	gui_1->addVariable("Point Specular Color", col_val6);
	gui_1->addVariable("Point Light Rotate Around X", light_rotate_x);
	gui_1->addVariable("Point Light Rotate Around Y", light_rotate_y);
	gui_1->addVariable("Point Light Rotate Around Z", light_rotate_z);
	gui_1->addVariable("Shading type",shadetype , enabled)->setItems({ "Flat","Smooth" });
	gui_1->addVariable("Shading type", depthtest, enabled)->setItems({ "Less","Always" });
	gui_1->addButton("Reset Light", []() {
		m_lightings->point_light.position = m_camera->position;
		rotationxangle = 0;
		rotationyangle = 0;
		rotatezangle=0;
		});


	nanogui::FormHelper* gui_2 = new nanogui::FormHelper(m_nanogui_screen);
	nanogui::ref<nanogui::Window> nanoguiWindow_2 = gui_2->addWindow(Eigen::Vector2i(0, 0), "Nanogui control bar_2");
	gui_2->addVariable("Direction Light X direction:", direction_light_x);
	gui_2->addVariable("Direction Light Y direction:", direction_light_y);
	gui_2->addVariable("Direction Light Y direction:", direction_light_z);
	gui_2->addVariable("diffuse texture:", diffuse_text_on);
	gui_2->addVariable("normal texture:", normal_text_on);

	m_nanogui_screen->setVisible(true);
	m_nanogui_screen->performLayout();

	glfwSetCursorPosCallback(window,
		[](GLFWwindow *window, double x, double y) {
		m_nanogui_screen->cursorPosCallbackEvent(x, y);
	}
	);

	glfwSetMouseButtonCallback(window,
		[](GLFWwindow *, int button, int action, int modifiers) {
		m_nanogui_screen->mouseButtonCallbackEvent(button, action, modifiers);
	}
	);

	glfwSetKeyCallback(window,
		[](GLFWwindow *window, int key, int scancode, int action, int mods) {
		//screen->keyCallbackEvent(key, scancode, action, mods);

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		if (key >= 0 && key < 1024)
		{
			if (action == GLFW_PRESS)
				keys[key] = true;
			else if (action == GLFW_RELEASE)
				keys[key] = false;
		}
	}
	);

	glfwSetCharCallback(window,
		[](GLFWwindow *, unsigned int codepoint) {
		m_nanogui_screen->charCallbackEvent(codepoint);
	}
	);

	glfwSetDropCallback(window,
		[](GLFWwindow *, int count, const char **filenames) {
		m_nanogui_screen->dropCallbackEvent(count, filenames);
	}
	);

	glfwSetScrollCallback(window,
		[](GLFWwindow *, double x, double y) {
		m_nanogui_screen->scrollCallbackEvent(x, y);
		//m_camera->ProcessMouseScroll(y);
	}
	);

	glfwSetFramebufferSizeCallback(window,
		[](GLFWwindow *, int width, int height) {
		m_nanogui_screen->resizeCallbackEvent(width, height);
	}
	);
}

void Renderer::init()
{
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	m_camera->init();

	m_lightings->init();
	
	m_animation->init();

	// Create a GLFWwindow object that we can use for GLFW's functions
	this->m_window = glfwCreateWindow(m_camera->width, m_camera->height, "Assignment 1", nullptr, nullptr);
	glfwMakeContextCurrent(this->m_window);
	glfwSetWindowAspectRatio(this->m_window, 16, 9);

	glewExperimental = GL_TRUE;
	glewInit();

	nanogui_init(this->m_window);
}

void Renderer::display(GLFWwindow* window,Shader m_shader)
{


	// Main frame while loop
	while (!glfwWindowShouldClose(window))
	{
		unsigned int texture1=0;
		unsigned int texture2=0;

		m_lightings->direction_light.direction = { direction_light_x,direction_light_y,direction_light_z };

		if (diffuse_text_on) {
			

			glGenTextures(1, &texture1);
			glBindTexture(GL_TEXTURE_2D, texture1);
		
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			int width, height, nrChannels;
			stbi_set_flip_vertically_on_load(true); 
			unsigned char* data = stbi_load("cube_diffuse.png", &width, &height, &nrChannels, 0);;
			if (model == "./objs/cyborg.obj") {

				data = stbi_load("cyborg_diffuse.png", &width, &height, &nrChannels, 0);
				if (data)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					glGenerateMipmap(GL_TEXTURE_2D);
				}
				else
				{
					std::cout << "Failed to load texture" << std::endl;
				}
			}
			else if (model == "./objs/cube.obj") {

				data = stbi_load("cube_diffuse.png", &width, &height, &nrChannels, 0);
				if (data)
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
					glGenerateMipmap(GL_TEXTURE_2D);
				}
				else
				{
					std::cout << "Failed to load texture" << std::endl;
				}
			}


			stbi_image_free(data);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture1);
		}
		if (normal_text_on) {
			

			glGenTextures(1, &texture1);
			glBindTexture(GL_TEXTURE_2D, texture1);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			int width, height, nrChannels;
			stbi_set_flip_vertically_on_load(true); 
			unsigned char* data = stbi_load("cube_normal.png", &width, &height, &nrChannels, 0);;
			if (model == "./objs/cyborg.obj") {

				data = stbi_load("cyborg_normal.png", &width, &height, &nrChannels, 0);
			}
			else if (model == "./objs/cube.obj") {

				data = stbi_load("cube_normal.png", &width, &height, &nrChannels, 0);
			}
			if (data)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}
			stbi_image_free(data);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture1);
		}
		if (normal_text_on && diffuse_text_on && model=="./objs/cyborg.obj") {
			glGenTextures(1, &texture1);
			glBindTexture(GL_TEXTURE_2D, texture1);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			int width, height, nrChannels;
			stbi_set_flip_vertically_on_load(true); 
			

			unsigned char* data = stbi_load("cyborg_diffuse.png", &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}
			stbi_image_free(data);



			glGenTextures(1, &texture2);
			glBindTexture(GL_TEXTURE_2D, texture2);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_set_flip_vertically_on_load(true);

			data = stbi_load("cyborg_normal.png", &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}

			stbi_image_free(data);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture2);

			

		}

		if (normal_text_on && diffuse_text_on && model == "./objs/cube.obj") {
			glGenTextures(1, &texture1);
			glBindTexture(GL_TEXTURE_2D, texture1);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			int width, height, nrChannels;
			stbi_set_flip_vertically_on_load(true); 

			unsigned char* data = stbi_load("cube_diffuse.png", &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}
			stbi_image_free(data);



			glGenTextures(1, &texture2);
			glBindTexture(GL_TEXTURE_2D, texture2);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_set_flip_vertically_on_load(true);

			data = stbi_load("cube_normal.png", &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}

			stbi_image_free(data);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture2);
			glUniform1i(glGetUniformLocation(m_shader.program, "texture1"), 0);

		}

		glfwPollEvents();

		if (is_scean_reset) {
			scean_reset();
			is_scean_reset = false;
		}


		camera_move();
		if (light_rotate_x) {
			float maximumx = 4;
			float minimumX =-4;
			float medianX = (maximumx - minimumX);
			m_lightings->point_light.position[0] = m_camera->position[0] + (cosf(rotationxangle) * medianX);
			rotationxangle += .1;
			if (rotationxangle == 6.28319) {
				rotationxangle = 0;
				m_lightings->point_light.position[0] = minimumX;
			}

		}
		if (light_rotate_y) {
			float maximumy = 4;
			float minimumy = -4;
			float mediany = (maximumy - minimumy);
			m_lightings->point_light.position[1] = m_camera->position[1] + (cosf(rotationxangle) * mediany);
			rotationxangle += .1;
			if (rotationxangle == 6.28319) {
				rotationxangle = 0;
				m_lightings->point_light.position[1] = minimumy;
			}

		}
		if (light_rotate_z) {
			float maximumz = 4;
			float minimumz = -4;
			float medianz = (maximumz - minimumz);
			m_lightings->point_light.position[2] = m_camera->position[2] + (cosf(rotationxangle) * medianz);
			rotationxangle += .1;
			if (rotationxangle == 6.28319) {
				rotationxangle = 0;
				m_lightings->point_light.position[2] = minimumz;
			}

		}

		m_shader.use();
			
		setup_uniform_values(m_shader);

		draw_scene(m_shader);

		m_nanogui_screen->drawWidgets();

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return;
}

void Renderer::run()
{
	init();
	Shader m_shader = Shader("./shader/basicflat.vert", "./shader/basicflat.frag");
	if (shadetype == Smooth) {
		Shader m_shader = Shader("./shader/basicsmooth.vert", "./shader/basicsmooth.frag");
	}
	display(this->m_window,m_shader);
}

void Renderer::load_models()
{
	if (culling == 0) {
		glFrontFace(GL_CCW);
		
	}
	else {
		glFrontFace(GL_CW);

	}

	obj_list.clear();
	Object main_object(model);
	main_object.obj_color = glm::vec4(0.7f, 0.7f, 0.7f, 0.7f);
	 
	m_lightings->direction_light.ambient = glm::vec4(col_val(0), col_val(1), col_val(2),col_val(3));
	m_lightings->direction_light.diffuse = glm::vec4(col_val2(0), col_val2(1), col_val2(2), col_val2(3));
	m_lightings->direction_light.specular = glm::vec4(col_val3(0), col_val3(1), col_val3(2), col_val3(3));
	m_lightings->point_light.ambient = glm::vec4(col_val4(0), col_val4(1), col_val4(2), col_val4(3));
	m_lightings->point_light.diffuse = glm::vec4(col_val5(0), col_val5(1), col_val5(2), col_val5(3));
	m_lightings->point_light.specular = glm::vec4(col_val6(0), col_val6(1), col_val6(2), col_val6(3));
	main_object.obj_name = "main_object";
	Object light_object("./objs/cube.obj");
	light_object.obj_color = glm::vec4(0.7f,0.7f,0.7f,0.7f);
	light_object.obj_name = "light_object";
	if (light_rotate_x) {


	}

	bind_vaovbo(main_object);

	// Here we only load one model
	obj_list.push_back(main_object);


}

void Renderer::draw_scene(Shader& shader)
{
	// Set up some basic parameters
	glClearColor(background_color[0], background_color[1], background_color[2], background_color[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	if (depthtest == 0) {
		glDepthFunc(GL_LESS);
	}
	else if (depthtest== 1) {
		glDepthFunc(GL_ALWAYS);
	}
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	if (culling == 0) {
		glFrontFace(GL_CCW);
	}
	else {
		glFrontFace(GL_CW);
	}
	m_lightings->direction_light.status = direction_light_on;
	m_lightings->point_light.status = point_light_on;
	for (size_t i = 0; i < obj_list.size(); i++)
	{

		if (obj_list[i].obj_name == "main_object")
		{
			// Before draw the model, change its model mat
			glm::mat4 main_object_model_mat =  glm::mat4(1.0f);
			
			m_animation->update(delta_time);

			main_object_model_mat = m_animation->get_model_mat();

			glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(main_object_model_mat));
			glUniformMatrix4fv(glGetUniformLocation(shader.program, "view"), 1, GL_FALSE, glm::value_ptr(m_camera->get_view_mat()));
			if (rtype == WireFrame) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else if (rtype == Points) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			if (shadetype == 0) {
				shader = Shader("./shader/basicflat.vert", "./shader/basicflat.frag");
			}
			else if (shadetype == 1) {
				shader = Shader("./shader/basicsmooth.vert", "./shader/basicsmooth.frag");
			}

			draw_object(shader,obj_list[i]);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		if (obj_list[i].obj_name == "light_object")
		{
			// Before draw the model, change its model mat
			glm::mat4 light_object_model_mat = glm::mat4(1.0f);
			light_object_model_mat=glm::translate(light_object_model_mat, glm::vec3(0.0f, 4.0f,0.0f));
			glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(light_object_model_mat));
			glUniformMatrix4fv(glGetUniformLocation(shader.program, "view"), 1, GL_FALSE, glm::value_ptr(m_camera->get_view_mat()));


			draw_object(shader, obj_list[i]);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}


	}
}

void Renderer::camera_move()
{
	GLfloat current_frame = glfwGetTime();
	delta_time = current_frame - last_frame;
	last_frame = current_frame;
	// Camera controls
	if (keys[GLFW_KEY_W])
		m_camera->process_keyboard(FORWARD, delta_time);
	if (keys[GLFW_KEY_S])
		m_camera->process_keyboard(BACKWARD, delta_time);
	if (keys[GLFW_KEY_A])
		m_camera->process_keyboard(LEFT, delta_time);
	if (keys[GLFW_KEY_D])
		m_camera->process_keyboard(RIGHT, delta_time);
	if (keys[GLFW_KEY_Q])
		m_camera->process_keyboard(UP, delta_time);
	if (keys[GLFW_KEY_E])
		m_camera->process_keyboard(DOWN, delta_time);
	if (keys[GLFW_KEY_I])
		m_camera->process_keyboard(ROTATE_X_UP, delta_time);
	if (keys[GLFW_KEY_K])
		m_camera->process_keyboard(ROTATE_X_DOWN, delta_time);
	if (keys[GLFW_KEY_J])
		m_camera->process_keyboard(ROTATE_Y_UP, delta_time);
	if (keys[GLFW_KEY_L])
		m_camera->process_keyboard(ROTATE_Y_DOWN, delta_time);
	if (keys[GLFW_KEY_U])
		m_camera->process_keyboard(ROTATE_Z_UP, delta_time);
	if (keys[GLFW_KEY_O])
		m_camera->process_keyboard(ROTATE_Z_DOWN, delta_time);
}

void Renderer::draw_object(Shader& shader, Object& object)
{
	glBindVertexArray(object.vao);

	glUniform3f(glGetUniformLocation(shader.program, "m_object.object_color"), object.obj_color[0], object.obj_color[1], object.obj_color[2]);
	glUniform1f(glGetUniformLocation(shader.program, "m_object.shininess"), object.shininess);
	glDrawArrays(GL_TRIANGLES, 0, object.vao_vertices.size());
	glBindVertexArray(0);
}

void Renderer::bind_vaovbo(Object &cur_obj)
{
	glGenVertexArrays(1, &cur_obj.vao);
	glGenBuffers(1, &cur_obj.vbo);

	glBindVertexArray(cur_obj.vao);

	glBindBuffer(GL_ARRAY_BUFFER, cur_obj.vbo);
	glBufferData(GL_ARRAY_BUFFER, cur_obj.vao_vertices.size() * sizeof(Object::Vertex), &cur_obj.vao_vertices[0], GL_STATIC_DRAW);

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Object::Vertex), (GLvoid*)0);
	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Object::Vertex), (GLvoid*)offsetof(Object::Vertex, Normal));
	// Vertex Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Object::Vertex), (GLvoid*)offsetof(Object::Vertex, TexCoords));

	glBindVertexArray(0);
}

void Renderer::setup_uniform_values(Shader& shader)
{
	// Camera uniform values
	glUniform3f(glGetUniformLocation(shader.program, "camera_pos"), m_camera->position.x, m_camera->position.y, m_camera->position.z);

	glUniformMatrix4fv(glGetUniformLocation(shader.program, "projection"), 1, GL_FALSE, glm::value_ptr(m_camera->get_projection_mat()));

	// Light uniform values
	glUniform1i(glGetUniformLocation(shader.program, "dir_light.status"), m_lightings->direction_light.status);
	glUniform3f(glGetUniformLocation(shader.program, "dir_light.direction"), m_lightings->direction_light.direction[0], m_lightings->direction_light.direction[1], m_lightings->direction_light.direction[2]);
	glUniform3f(glGetUniformLocation(shader.program, "dir_light.ambient"), m_lightings->direction_light.ambient[0], m_lightings->direction_light.ambient[1], m_lightings->direction_light.ambient[2]);
	glUniform3f(glGetUniformLocation(shader.program, "dir_light.diffuse"), m_lightings->direction_light.diffuse[0], m_lightings->direction_light.diffuse[1], m_lightings->direction_light.diffuse[2]);
	glUniform3f(glGetUniformLocation(shader.program, "dir_light.specular"), m_lightings->direction_light.specular[0], m_lightings->direction_light.specular[1], m_lightings->direction_light.specular[2]);

	// Set current point light as camera's position
	//m_lightings->point_light.position = m_camera->position;
	glUniform1i(glGetUniformLocation(shader.program, "point_light.status"), m_lightings->point_light.status);
	glUniform3f(glGetUniformLocation(shader.program, "point_light.position"), m_lightings->point_light.position[0], m_lightings->point_light.position[1], m_lightings->point_light.position[2]);
	glUniform3f(glGetUniformLocation(shader.program, "point_light.ambient"), m_lightings->point_light.ambient[0], m_lightings->point_light.ambient[1], m_lightings->point_light.ambient[2]);
	glUniform3f(glGetUniformLocation(shader.program, "point_light.diffuse"), m_lightings->point_light.diffuse[0], m_lightings->point_light.diffuse[1], m_lightings->point_light.diffuse[2]);
	glUniform3f(glGetUniformLocation(shader.program, "point_light.specular"), m_lightings->point_light.specular[0], m_lightings->point_light.specular[1], m_lightings->point_light.specular[2]);
	glUniform1f(glGetUniformLocation(shader.program, "point_light.constant"), m_lightings->point_light.constant);
	glUniform1f(glGetUniformLocation(shader.program, "point_light.linear"), m_lightings->point_light.linear);
	glUniform1f(glGetUniformLocation(shader.program, "point_light.quadratic"), m_lightings->point_light.quadratic);

}

void Renderer::scean_reset()
{
	load_models();
	m_camera->reset();
}
void Renderer::model_reset()
{
	load_models();
	m_animation->reset(model);
}
