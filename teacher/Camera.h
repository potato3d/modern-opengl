#pragma once
#include <ShaderData.h>
#include <AABB.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cmath>
#include <GL/glew.h>

class Camera
{
public:
	enum MouseButton
	{
		Button_None,
		Button_Left,
		Button_Right,
		Button_Middle
	};

	enum MouseWheel
	{
		Wheel_Up,
		Wheel_Down
	};

	Camera(int width, int height)
	{
		_dirty = true;
		_width = width;
		_height = height;
		_fovy = glm::radians(60.0f);
		_eye = glm::vec3(0,10,0);
		_center = glm::vec3(0,0,0);
		_up = glm::vec3(0,0,1);
		_currMouseButton = Button_None;
		_dragging = false;
		_rotationSpeed = 5e-3f;
		_translationSpeed = 1e-2f;
		_ubo = 0;
	}

	bool initialize(const AABB& bbox)
	{
		setFocus(bbox);

		// create UBO to store camera data inside GPU
		glCreateBuffers(1, &_ubo);
		glNamedBufferStorage(_ubo, sizeof(CameraData), &_data, GL_DYNAMIC_STORAGE_BIT); // will update buffer contents later

		// associate camera UBO to shader binding point
		glBindBufferBase(GL_UNIFORM_BUFFER, UB_CAMERA, _ubo);

		return true;
	}

	void resize(int width, int height)
	{
		_width = width;
		_height = height;
		_dirty = true;
	}

	void setFocus(const AABB& bbox)
	{
		AABB box = bbox;

		if(!box.valid())
		{
			box.min = glm::vec3(-1.0f);
			box.max = glm::vec3(1.0f);
		}

		// look to box center
		_center = (box.min + box.max) * 0.5f;

		// align up direction
		_up = glm::vec3(0.0f, 0.0f, 1.0f);

		// compute distance from box center
		float radius = glm::length(box.max - box.min) * 0.5f;
		float finalDistance = 1.1f * radius / std::tan(_fovy * 0.5f);

		// compute view direction
		//glm::vec3 dir = glm::normalize(_eye - _center); // maintain current direction
		glm::vec3 dir(0,1,0); // reset to default direction

		// final camera position
		_eye = _center + dir * finalDistance;

		_dirty = true;
	}

	void mousePressed(MouseButton button, int x, int y)
	{
		_currMouseButton = button;
		_prevMousePos = glm::vec2(x, y);
		_dragging = true;
	}

	void mouseReleased(MouseButton button, int x, int y)
	{
		(void)button;
		(void)x;
		(void)y;
		_currMouseButton = Button_None;
		_dragging = false;
	}

	void mouseWheel(MouseWheel wheel)
	{
		glm::vec3 dir = _center - _eye; // do not normalize to move faster over larger distances
		dir = dir * (wheel == Wheel_Up? 1.0f : -1.0f);
		glm::vec3 newEye = _eye + dir * _translationSpeed * 10.0f;
		if(glm::length(_center - newEye) > 1.0f)
		{
			_eye = newEye;
			_dirty = true;
		}
	}

	void mouseMoved(int x, int y)
	{
		if(!_dragging)
		{
			return;
		}

		glm::vec2 delta = glm::vec2(x, y) - _prevMousePos;

		switch(_currMouseButton)
		{
		case Button_Left: // rotate around center
		{
			// apply rotation speed
			delta *= _rotationSpeed;
			// rotation axis
			glm::vec3 right = glm::normalize(glm::cross((_center - _eye), _up));
			// place eye at center position to perform rotation around center and then translate back
			glm::vec3 newEye = (glm::rotate(glm::quat(), -delta.y, right) * glm::rotate(glm::quat(), -delta.x, _up)) * (_eye - _center) + _center;
			_eye = newEye;
			_dirty = true;
			break;
		}
		case Button_Middle: // move parallel to screen plane (pan)
		{
			// apply translation speed
			delta *= _translationSpeed;
			// need horizontal camera axis
			glm::vec3 right = glm::normalize(glm::cross((_center - _eye), _up));
			// translation
			glm::vec3 translation = right * -delta.x +  _up * delta.y;
			translation *= glm::length(_center - _eye) * 0.1f;
			_eye += translation;
			_center += translation;
			_dirty = true;
			break;
		}
		case Button_Right: // move forward/backward
		{
			// apply translation speed
			delta *= _translationSpeed;
			// translation
			glm::vec3 translation = glm::normalize(_center - _eye) * delta.y;
			translation *= glm::length(_center - _eye) * 0.1f;
			_eye += translation;
			_center += translation;
			_dirty = true;
			break;
		}
		default:
			return;
		}

		_prevMousePos = glm::vec2(x, y);
	}

	void update()
	{
		if(_dirty)
		{
			_data.viewMatrix = glm::lookAt(_eye, _center, _up);
			_data.viewProjMatrix = glm::perspective(_fovy, ((float)_width/_height), 0.1f, 1000.0f) * _data.viewMatrix;
			glNamedBufferSubData(_ubo, 0, sizeof(CameraData), &_data); // offset = 0
			_dirty = false;
		}
	}

	const CameraData& getData() const
	{
		return _data;
	}

private:
	bool _dirty;

	float _fovy;
	int _width;
	int _height;
	glm::vec3 _eye;
	glm::vec3 _center;
	glm::vec3 _up;
	CameraData _data;

	MouseButton _currMouseButton;
	glm::vec2 _prevMousePos;
	bool _dragging;

	float _rotationSpeed;
	float _translationSpeed;

	unsigned int _ubo;
};
