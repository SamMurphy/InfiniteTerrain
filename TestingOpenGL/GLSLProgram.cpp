#include "GLSLProgram.h"

#include <fstream>
#include <SDL/SDL.h>
#include <GL/glew.h>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <sys/stat.h>

GLSLProgram::GLSLProgram() : id(0), is_linked(false) { }

// Takes a file, checks if it exists
// Passes the text to compileShaderFromString
bool GLSLProgram::compileShaderFromFile(const char * fileName, GLSLShader::GLSLShaderType type)
{
	if (!fileExists(fileName))
	{
		log("File not found.");
		return false;
	}

	if (id <= 0) 
	{
		id = glCreateProgram();
		if (id == 0) {
			log("Unable to create shader program.");
			return false;
		}
	}

	std::ifstream inFile(fileName, std::ios::in);
	if (!inFile) 
	{
		return false;
	}

	std::ostringstream code;
	while (inFile.good()) 
	{
		int c = inFile.get();
		if (!inFile.eof())
			code << (char)c;
	}
	inFile.close();

	return compileShaderFromString(code.str(), type);
}

// Accepts string source, and type of shader
// Creates program, compiles and attaches shader
bool GLSLProgram::compileShaderFromString(const std::string & source, GLSLShader::GLSLShaderType type)
{
	// If the program hasn't been created yet, then create it now.
	if (id <= 0) 
	{
		id = glCreateProgram();
		if (id == 0) 
		{
			log("Unable to create shader program.");
			return false;
		}
	}

	GLuint shaderID = 0;

	// Checks the type of the shader & create correct type
	switch (type) 
	{
		case GLSLShader::VERTEX:
			shaderID = glCreateShader(GL_VERTEX_SHADER);
			break;
		case GLSLShader::FRAGMENT:
			shaderID = glCreateShader(GL_FRAGMENT_SHADER);
			break;
		case GLSLShader::GEOMETRY:
			shaderID = glCreateShader(GL_GEOMETRY_SHADER);
			break;
		case GLSLShader::TESS_CONTROL:
			shaderID = glCreateShader(GL_TESS_CONTROL_SHADER);
			break;
		case GLSLShader::TESS_EVALUATION:
			shaderID = glCreateShader(GL_TESS_EVALUATION_SHADER);
			break;
		default:
			return false;
	}

	// Set the source
	const char * c_code = source.c_str();
	glShaderSource(shaderID, 1, &c_code, NULL);

	// Compile the shader
	glCompileShader(shaderID);

	// Check for errors
	int result;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result) 
	{
		// Compile failed, store log and return false
		int length = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
		if (length > 0) 
		{
			char * c_log = new char[length];
			int written = 0;
			glGetShaderInfoLog(shaderID, length, &written, c_log);
			log(c_log);
			delete[] c_log;
		}

		return false;
	}
	else 
	{
		// Compile succeeded, attach shader and return true
		glAttachShader(id, shaderID);
		return true;
	}
}

// Link shader
bool GLSLProgram::link()
{
	if (is_linked) return true;
	if (id <= 0) return false;

	glLinkProgram(id);

	int status = 0;
	glGetProgramiv(id, GL_LINK_STATUS, &status);
	if (GL_FALSE == status) 
	{
		// Store log and return false
		int length = 0;

		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);

		if (length > 0) 
		{
			char * c_log = new char[length];
			int written = 0;
			glGetProgramInfoLog(id, length, &written, c_log);
			log(c_log);
			delete[] c_log;
		}
		return false;
	}
	else 
	{
		is_linked = true;
		return is_linked;
	}
}

// Use this shader
void GLSLProgram::use()
{
	if (id <= 0 || (!is_linked)) return;
	glUseProgram(id);
}

// Returns the most recent log message
std::string GLSLProgram::log()
{
	return logString;
}

// Sets the most recent log message
// Prints the log message
std::string GLSLProgram::log(std::string log_string)
{
	logString = log_string;
	printf(logString.c_str());
	return logString;
}

// Returns the shaders ID
int GLSLProgram::ID()
{
	return id;
}

bool GLSLProgram::isLinked()
{
	return is_linked;
}

void GLSLProgram::bindAttribLocation(GLuint location, const char * name)
{
	glBindAttribLocation(id, location, name);
}

void GLSLProgram::bindFragDataLocation(GLuint location, const char * name)
{
	glBindFragDataLocation(id, location, name);
}

void GLSLProgram::setUniform(const char *name, float x, float y, float z)
{
	int loc = getUniformLocation(name);
	if (loc >= 0) 
	{
		glUniform3f(loc, x, y, z);
	}
	else 
	{
		printf("Uniform: %s not found.\n", name);
	}
}

void GLSLProgram::setUniform(const char *name, const glm::vec3 & v)
{
	this->setUniform(name, v.x, v.y, v.z);
}

void GLSLProgram::setUniform(const char *name, const glm::vec4 & v)
{
	int loc = getUniformLocation(name);
	if (loc >= 0) 
	{
		glUniform4f(loc, v.x, v.y, v.z, v.w);
	}
	else 
	{
		printf("Uniform: %s not found.\n", name);
	}
}

void GLSLProgram::setUniform(const char *name, const glm::vec2 & v)
{
	int loc = getUniformLocation(name);
	if (loc >= 0) 
	{
		glUniform2f(loc, v.x, v.y);
	}
	else 
	{
		printf("Uniform: %s not found.\n", name);
	}
}

void GLSLProgram::setUniform(const char *name, const glm::mat4 & m)
{
	int loc = getUniformLocation(name);
	if (loc >= 0)
	{
		glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
	}
	else 
	{
		printf("Uniform: %s not found.\n", name);
	}
}

void GLSLProgram::setUniform(const char *name, const glm::mat3 & m)
{
	int loc = getUniformLocation(name);
	if (loc >= 0)
	{
		glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
	}
	else 
	{
		printf("Uniform: %s not found.\n", name);
	}
}

void GLSLProgram::setUniform(const char *name, float val)
{
	int loc = getUniformLocation(name);
	if (loc >= 0)
	{
		glUniform1f(loc, val);
	}
	else 
	{
		printf("Uniform: %s not found.\n", name);
	}
}

void GLSLProgram::setUniform(const char *name, int val)
{
	int loc = getUniformLocation(name);
	if (loc >= 0)
	{
		glUniform1i(loc, val);
	}
	else 
	{
		printf("Uniform: %s not found.\n", name);
	}
}

void GLSLProgram::setUniform(const char *name, GLuint val)
{
	int loc = getUniformLocation(name);
	if (loc >= 0)
	{
		glUniform1i(loc, val);
	}
	else
	{
		printf("Uniform: %s not found.\n", name);
	}
}

void GLSLProgram::setUniform(const char *name, bool val)
{
	int loc = getUniformLocation(name);
	if (loc >= 0)
	{
		glUniform1i(loc, val);
	}
	else 
	{
		printf("Uniform: %s not found.\n", name);
	}
}

void GLSLProgram::printUniforms()
{
	GLint nUniforms, size, location, maxLen;
	GLchar * name;
	GLsizei written;
	GLenum type;

	glGetProgramiv(id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen);
	glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &nUniforms);

	name = (GLchar *)malloc(maxLen);

	printf("Shader Uniforms (Index / Name)\n");
	for (int i = 0; i < nUniforms; ++i)
	{
		glGetActiveUniform(id, i, maxLen, &written, &size, &type, name);
		location = glGetUniformLocation(id, name);
		if (location < 10)
			printf("%d  | %s\n", location, name);
		else
			printf("%d | %s\n", location, name);
	}

	free(name);
}

void GLSLProgram::printAttributes()
{
	GLint written, size, location, maxLength, nAttribs;
	GLenum type;
	GLchar * name;

	glGetProgramiv(id, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);
	glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &nAttribs);

	name = (GLchar *)malloc(maxLength);

	printf("Shader Attributes (Index / Name)\n");
	for (int i = 0; i < nAttribs; i++)
	{
		glGetActiveAttrib(id, i, maxLength, &written, &size, &type, name);
		location = glGetAttribLocation(id, name);
		if (location < 10)
			printf("%d  | %s\n", location, name);
		else
			printf("%d | %s\n", location, name);
	}

	free(name);
}

// Checks that the shader program has been compiled, attached and linked
bool GLSLProgram::validate()
{
	if (!isLinked()) return false;

	GLint status;
	glValidateProgram(id);
	glGetProgramiv(id, GL_VALIDATE_STATUS, &status);

	if (GL_FALSE == status)
	{
		// Store log and return false
		int length = 0;

		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);

		if (length > 0)
		{
			char * c_log = new char[length];
			int written = 0;
			glGetProgramInfoLog(id, length, &written, c_log);
			log(c_log);
			delete[] c_log;
		}

		return false;
	}
	else
	{
		return true;
	}
}

// Returns the location id of uniform shader variable
int GLSLProgram::getUniformLocation(const char * name)
{
	return glGetUniformLocation(id, name);
}

// Fastest check for existance of file
// Taken from http://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
bool GLSLProgram::fileExists(const std::string & fileName)
{
	struct stat info;
	int ret = -1;

	ret = stat(fileName.c_str(), &info);
	return 0 == ret;
}
