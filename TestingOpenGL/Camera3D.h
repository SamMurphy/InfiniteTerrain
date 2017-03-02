#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

class Camera3D
{
public:
	Camera3D();
	~Camera3D();

	void init(glm::vec3 positionInWorldSpace, glm::vec3 direction, float fov, float aspectRatio, float near, float far, float moveSpeed = 1.0f, float rotateSpeed = 0.03f);

	glm::vec3 Position() { return _position; }
	void SetPosition(glm::vec3 position) { _position = position; updateMatricies(); }

	glm::vec3 Direction() { return _direction; }
	void SetDirection(glm::vec3 direction) { _direction = direction; updateMatricies(); }

	void setAspectRatio(float aspectRatio){ _aspectRatio = aspectRatio; updateMatricies(); }

	glm::mat4 CameraMatrix();

	glm::mat4 View() { return _view; }
	glm::mat4 Projection() { return _projection; }

	float MoveSpeed() { return _moveSpeed; }
	void SetMoveSpeed(float moveSpeed) { _moveSpeed = moveSpeed; }

	float RotateSpeed() { return _rotateSpeed; }
	void SetRotateSpeed(float rotateSpeed) { _rotateSpeed = rotateSpeed; }

	// Camera Movement
	void Forward();
	void Backward();

	void Up();
	void Down();

	void RotateLeft();
	void RotateRight();

	void LookUp();
	void LookDown();

	void ForwardZ();
	void BackwardZ();
	void StrafeLeft();
	void StrafeRight();

	glm::vec3 _position;

	float pitch;
	float yaw;

	void updateMatricies();
private:
	
	glm::vec3 _up;
	glm::vec3 _direction;
	glm::vec3 _cameraRight;
	glm::vec3 _cameraUp;
	

	glm::mat4 _projection;
	glm::mat4 _view;

	float _fov;
	float _aspectRatio;
	float _near;
	float _far;

	float _moveSpeed;
	float _rotateSpeed;

	
	void updateProjection(float fov, float aspectRatio, float near, float far);
};

