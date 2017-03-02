#include "Camera3D.h"


Camera3D::Camera3D()
{
}


Camera3D::~Camera3D()
{
}

void Camera3D::init(glm::vec3 positionInWorldSpace, glm::vec3 direction, float fov, float aspectRatio, float near, float far, float moveSpeed, float rotateSpeed)
{
	_position = positionInWorldSpace;
	_direction = direction;
	_up = glm::vec3(0, 1, 0);
	_cameraRight = glm::normalize(glm::cross(_up, _direction));
	_cameraUp = glm::cross(_direction, _cameraRight);

	_fov = fov;
	_aspectRatio = aspectRatio;
	_near = near;
	_far = far;

	_moveSpeed = moveSpeed;
	_rotateSpeed = rotateSpeed;

	pitch = 0.0f;
	yaw = 90.0f;

	updateMatricies();
}

glm::mat4 Camera3D::CameraMatrix()
{
	return _projection * _view;
}

void Camera3D::updateMatricies()
{
	if (pitch > 89.0) pitch = 89.0;
	if (pitch < -89.0) pitch = -89.0;


	_direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	_direction.y = sin(glm::radians(pitch));
	_direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

	_view = glm::lookAt(_position, _position + _direction, _cameraUp);

	_projection = glm::perspective(_fov, _aspectRatio, _near, _far);
}

void Camera3D::Forward()
{
	_position += _moveSpeed * _direction;
	updateMatricies();
}

void Camera3D::Backward()
{
	_position -= _moveSpeed * _direction;
	updateMatricies();
}

void Camera3D::Up()
{
	_position += _moveSpeed * _up;
	updateMatricies();
}

void Camera3D::Down()
{
	_position -= _moveSpeed * _up;
	updateMatricies();
}

void Camera3D::RotateLeft()
{
	yaw -= _rotateSpeed;
	updateMatricies();
}

void Camera3D::RotateRight()
{
	yaw += _rotateSpeed;
	updateMatricies();
}

void Camera3D::LookUp()
{
	pitch += _rotateSpeed;
	updateMatricies();
}

void Camera3D::LookDown()
{
	pitch -= _rotateSpeed;
	updateMatricies();
}

void Camera3D::ForwardZ()
{
	_position += _moveSpeed * glm::vec3(0, 0, 1);
	updateMatricies();
}

void Camera3D::BackwardZ()
{
	_position += _moveSpeed * glm::vec3(0, 0, -1);
	updateMatricies();
}

void Camera3D::StrafeLeft()
{
	_position += _moveSpeed * glm::vec3(1, 0, 0);
	updateMatricies();
}

void Camera3D::StrafeRight()
{
	_position += _moveSpeed * glm::vec3(-1, 0, 0);
	updateMatricies();
}
