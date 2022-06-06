#include "Animation.h"

Animation::Animation()
{
	this->m_model_mat = glm::mat4(1.0f);
}

Animation::~Animation()
{
}

void Animation::init()
{
	reset("");
}

void Animation::update(float delta_time)
{
}

void Animation::reset(std::string model)
{
	m_model_mat = glm::mat4(1.0f);
	m_model_mat = glm::translate(m_model_mat, glm::vec3(0.0f, 0.0f, 0.0f));
	if (model == "./objs/cyborg.obj") {
		m_model_mat= glm::translate(m_model_mat, glm::vec3(0.0f, -2.0f, 0.0f));
	}
	else if (model == "./objs/rock.obj") {
		m_model_mat = glm::translate(m_model_mat, glm::vec3(0.0f, -0.1f, 0.0f));
	}
}
