#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Animation
{
public:

	Animation();
	~Animation();

	void init();
	void update(float delta_time);

	void reset(std::string);
	glm::mat4 get_model_mat() { return m_model_mat; };

	void rotatex(int a) {
		/*
			float transa[16] = { 1,0,0,0,
				0,cos(a),-sin(a),0,
				0,sin(a),cos(a),0,
				0,0,0,1
			};
			glm::mat4 trans;

			memcpy(glm::value_ptr(trans),transa, sizeof(transa));

			main_object_model_mat = main_object_model_mat*trans;
			*/
	m_model_mat = glm::rotate(m_model_mat, (glm::mediump_float)a, glm::vec3(1.0f, 0.0f, 0.0f));
		
		
	}
	void rotatey(float b) {
		

		/*
		float transa[16] = { cos(b),0,sin(b),0,
			0,1,0,0,
			-sin(b),0,cos(90),0,
			0,0,0,1
		};
		glm::mat4 trans;

		memcpy(glm::value_ptr(trans),transa, sizeof(transa));

		main_object_model_mat = main_object_model_mat*trans;
	   */
		glm::mat4 rot_mat = glm::mat4(1.0f);
		rot_mat = glm::rotate(rot_mat, glm::radians(b), glm::vec3(0.0f, 1.0f, 0.0f));
		m_model_mat = rot_mat * m_model_mat;
		//rotate around y

		//translate back
		
	}

private:
	glm::mat4 m_model_mat;
};

