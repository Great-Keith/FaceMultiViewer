#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction 
// to stay away from window-system specific input methods
enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 50.f;
const float SENSITIVITY = 0.01f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, 
// Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// Camera Attributes
	glm::vec3 pos;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	// Euler Angles
	float yaw;
	float pitch;

	// Camera options
	float movementSpeed;
	float mouseSensitivity;
	float Zoom;

	// Constructor with vectors
	Camera(
		glm::vec3 position = glm::vec3(0.0f, 0.0f, 800.0f), 
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
		float yaw = YAW, 
		float pitch = PITCH) : 
		front(glm::vec3(0.0f, 0.0f, -1.0f)), 
		movementSpeed(SPEED), 
		mouseSensitivity(SENSITIVITY), 
		Zoom(ZOOM)
	{
		pos = position;
		worldUp = up;
		yaw = yaw;
		pitch = pitch;
		updateCameraVectors();
	}


	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : 
		front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		pos = glm::vec3(posX, posY, posZ);
		worldUp = glm::vec3(upX, upY, upZ);
		yaw = yaw;
		pitch = pitch;
		updateCameraVectors();
	}


	Camera(const Camera& obj)
	{
		pos = obj.pos;
		front = obj.front;
		up = obj.up;
		right = obj.right;
		worldUp = obj.worldUp;
		yaw = obj.yaw;
		pitch = obj.pitch;
		movementSpeed = obj.movementSpeed;
		mouseSensitivity = obj.mouseSensitivity;
		Zoom = obj.Zoom;
	}

	glm::vec3 getPosition() { return pos; }

	void resetPosition(glm::vec3 position)
	{
		pos = position;
		worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		yaw = YAW;
		pitch = PITCH;
		updateCameraVectors();
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(pos, pos + front, up);
	}

	// Processes input received from any keyboard-like input system. 
	// Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(CameraMovement direction, float deltaTime)
	{
		float velocity = movementSpeed * deltaTime;
		if (direction == FORWARD)
			pos += front * velocity;
		if (direction == BACKWARD)
			pos -= front * velocity;
		if (direction == LEFT)
			pos -= right * velocity;
		if (direction == RIGHT)
			pos += right * velocity;
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= mouseSensitivity;
		yoffset *= mouseSensitivity;

		yaw += xoffset;
		pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		if (Zoom >= 1.0f && Zoom <= 45.0f)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 45.0f)
			Zoom = 45.0f;
	}

private:
	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(front);
		// Also re-calculate the Right and Up vector
		right = glm::normalize(glm::cross(front, worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		up = glm::normalize(glm::cross(right, front));
	}
};
#endif