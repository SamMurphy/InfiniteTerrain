#pragma once
#include <GL/glew.h>
#include <GL/gl.h>
#include <string>
#include <glm/glm.hpp>

namespace GLSLShader 
{
	enum GLSLShaderType 
	{
		VERTEX,
		TESS_CONTROL,
		TESS_EVALUATION,
		GEOMETRY,
		FRAGMENT
	};
};

// Pipeline
// Vertex --> TessControl --> TessEval --> Geometry --> Fragment
class GLSLProgram
{
private:
	int  id;
	bool is_linked;
	std::string logString;
	std::string log();
	std::string log(std::string log_string);

	int  getUniformLocation(const char * name);
	bool fileExists(const std::string & fileName);

public:
	GLSLProgram();

	bool compileShaderFromFile(const char * fileName, GLSLShader::GLSLShaderType type);
	bool compileShaderFromString(const std::string & source, GLSLShader::GLSLShaderType type);
	bool link();
	bool validate();
	void use();

	int ID();
	bool isLinked();

	void bindAttribLocation(GLuint location, const char * name);
	void bindFragDataLocation(GLuint location, const char * name);

	void setUniform(const char *name, float x, float y, float z);
	void setUniform(const char *name, const glm::vec2 & v);
	void setUniform(const char *name, const glm::vec3 & v);
	void setUniform(const char *name, const glm::vec4 & v);
	void setUniform(const char *name, const glm::mat4 & m);
	void setUniform(const char *name, const glm::mat3 & m);
	void setUniform(const char *name, float val);
	void setUniform(const char *name, int val);
	void setUniform(const char *name, GLuint val);
	void setUniform(const char *name, bool val);

	void printUniforms();
	void printAttributes();
};
