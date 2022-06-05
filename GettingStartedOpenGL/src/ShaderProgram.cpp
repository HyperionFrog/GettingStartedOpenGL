#include "include/ShaderProgram.h"
#include "include/Shader.h"

// Internal Structures
struct ShaderVariable
{
	std::string name;
	GLint var_location;
	uint32 shaderProgramId;

	bool operator==(const ShaderVariable& other) const
	{
		return other.shaderProgramId == shaderProgramId && other.name == name;
	}
};

struct HashShaderVar
{
	std::size_t operator()(const ShaderVariable& key) const
	{
		// This code was adapted from https://stackoverflow.com/questions/35985960/c-why-is-boosthash-combine-the-best-way-to-combine-hash-values
		std::size_t seed = std::hash<std::string>()(key.name);
		seed ^= std::hash<uint32>()(key.shaderProgramId) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	}
};

// Internal Variables
static auto allShaderVariableLocations = robin_hood::unordered_set<ShaderVariable, HashShaderVar>();

// Forward Declarations
static GLint getVariableLocation(const ShaderProgram& shader, const char* varName);

bool ShaderProgram::CompileAndLink(const char* vertexShaderFile, const char* fragmentShaderFile)
{
	// Create the shader program
	GLuint program = glCreateProgram();

	// Delete the shader if compilation fails( no sense to keep it )
	Shader vertexShader;
	if (!vertexShader.compile(ShaderType::Vertex, vertexShaderFile))
	{
		vertexShader.destroy();
		std::cerr << ("Failed to compile vertex shader.");
		return false;
	}

	Shader fragmentShader;
	if (!fragmentShader.compile(ShaderType::Fragment, fragmentShaderFile))
	{
		fragmentShader.destroy();
		std::cerr << ("Failed to compile fragment shader.");
		return false;
	}

	// Attach the vertex/fragment shaders and try to link them together
	glAttachShader(program, vertexShader.shaderId);
	glAttachShader(program, fragmentShader.shaderId);

	// Try to link our program
	glLinkProgram(program);

	// Log errors if the linking failed
	GLint isLinked = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		// We don't need the program anymore if linking failed
		glDeleteProgram(program);
		vertexShader.destroy();
		fragmentShader.destroy();

		printf("Shader linking failed:\n%s", infoLog.data());
		programId = UINT32_MAX;
		return false;
	}

	// Always detach shaders after a successful link and destroy them since we don't need them anymore
	glDetachShader(program, vertexShader.shaderId);
	glDetachShader(program, fragmentShader.shaderId);
	vertexShader.destroy();
	fragmentShader.destroy();

	// If linking succeeded, get all the active uniforms and store them in our map of uniform variable locations
	int numUniforms;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);

	int maxCharLength;
	glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxCharLength);
	if (numUniforms > 0 && maxCharLength > 0)
	{
		char* charBuffer = new char;

		for (int i = 0; i < numUniforms; i++)
		{
			int length, size;
			GLenum dataType;
			glGetActiveUniform(program, i, maxCharLength, &length, &size, &dataType, charBuffer);
			GLint varLocation = glGetUniformLocation(program, charBuffer);
			ShaderVariable shaderVar;
			shaderVar.name = std::string(charBuffer, length);
			shaderVar.var_location = varLocation;
			shaderVar.shaderProgramId = program;
			allShaderVariableLocations.emplace(shaderVar);
		}

	}

	programId = program;
	printf("Shader compilation and linking succeeded <Vertex:%s>:<Fragment:%s>", vertexShaderFile, fragmentShaderFile);
	return true;
}

void ShaderProgram::Destroy()
{
	if (programId != UINT32_MAX)
	{
		glDeleteProgram(programId);
		programId = UINT32_MAX;
	}
}

void ShaderProgram::Bind() const
{
	glUseProgram(programId);
}

void ShaderProgram::Unbind() const
{
	glUseProgram(0);
}

void ShaderProgram::UploadVec4(const char* varName, const glm::vec4& vec4) const
{
	int var_location = getVariableLocation(*this, varName);
	glUniform4f(var_location, vec4.x, vec4.y, vec4.z, vec4.w);
}

void ShaderProgram::UploadVec3(const char* varName, const glm::vec3& vec3) const
{
	int var_location = getVariableLocation(*this, varName);
	glUniform3f(var_location, vec3.x, vec3.y, vec3.z);
}

void ShaderProgram::UploadVec2(const char* varName, const glm::vec2& vec2) const
{
	int var_location = getVariableLocation(*this, varName);
	glUniform2f(var_location, vec2.x, vec2.y);
}

void ShaderProgram::UploadIVec4(const char* varName, const glm::ivec4& vec4) const
{
	int var_location = getVariableLocation(*this, varName);
	glUniform4i(var_location, vec4.x, vec4.y, vec4.z, vec4.w);
}

void ShaderProgram::UploadIVec3(const char* varName, const glm::ivec3& vec3) const
{
	int var_location = getVariableLocation(*this, varName);
	glUniform3i(var_location, vec3.x, vec3.y, vec3.z);
}

void ShaderProgram::UploadIVec2(const char* varName, const glm::ivec2& vec2) const
{
	int var_location = getVariableLocation(*this, varName);
	glUniform2i(var_location, vec2.x, vec2.y);
}

void ShaderProgram::UploadFloat(const char* varName, float value) const
{
	int var_location = getVariableLocation(*this, varName);
	glUniform1f(var_location, value);
}

void ShaderProgram::UploadInt(const char* varName, int value) const
{
	int var_location = getVariableLocation(*this, varName);
	glUniform1i(var_location, value);
}

void ShaderProgram::UploadUInt(const char* varName, uint32 value) const
{
	int var_location = getVariableLocation(*this, varName);
	glUniform1ui(var_location, value);
}

void ShaderProgram::UploadMat4(const char* varName, const glm::mat4& mat4) const
{
	int var_location = getVariableLocation(*this, varName);
	glUniformMatrix4fv(var_location, 1, GL_FALSE, glm::value_ptr(mat4));
}

void ShaderProgram::UploadMat3(const char* varName, const glm::mat3& mat3) const
{
	int var_location = getVariableLocation(*this, varName);
	glUniformMatrix3fv(var_location, 1, GL_FALSE, glm::value_ptr(mat3));
}

void ShaderProgram::UploadIntArray(const char* varName, int length, const int* array) const
{
	int var_location = getVariableLocation(*this, varName);
	glUniform1iv(var_location, length, array);
}

void ShaderProgram::UploadBool(const char* varName, bool value) const
{
	int var_location = getVariableLocation(*this, varName);
	glUniform1i(var_location, value ? 1 : 0);
}

void ShaderProgram::clearAllShaderVariables()
{
	allShaderVariableLocations.clear();
}

// Private functions
static GLint getVariableLocation(const ShaderProgram& shader, const char* varName)
{
	ShaderVariable match = {
		varName,
		0,
		shader.programId
	};
	auto iter = allShaderVariableLocations.find(match);
	if (iter != allShaderVariableLocations.end())
	{
		return iter->var_location;
	}

	return -1;
}
