#include "florp/graphics/Shader.h"
#include "glad/glad.h"
#include "Logging.h"
#include "florp/utils/FileUtils.h"
#include <GLM/gtc/type_ptr.hpp>

namespace florp
{
	namespace graphics
	{
		
		constexpr uint32_t ToGlEnum(ShaderStageType type) {
			switch (type) {
				case ShaderStageType::VertexShader: return GL_VERTEX_SHADER;
				case ShaderStageType::FragmentShader: return GL_FRAGMENT_SHADER;
				case ShaderStageType::TessControl: return GL_TESS_CONTROL_SHADER;
				case ShaderStageType::TessEval: return GL_TESS_EVALUATION_SHADER;
				case ShaderStageType::Geometry: return GL_GEOMETRY_SHADER;
				default: LOG_ASSERT(false, "Invalid shader type"); return GL_NONE;
			}
		}


		Shader::Shader() {
			myRendererID = glCreateProgram();
			for(int ix = 0; ix < 5; ix++) myStages[ix] = 0;
			isLinked = false;
		}

		Shader::~Shader() {
			glDeleteProgram(myRendererID);
		}

		void Shader::CompilePart(ShaderStageType type, const std::string& source) {
			// Fail if we try to modify the shader after linkage
			LOG_ASSERT(!isLinked, "Cannot modify shader after linking");
			
			// If there is already a part in that slot, destroy it
			if (myStages[*type] != 0) {
				LOG_WARN("Replacing {} stage in shader", type);
				glDetachShader(myRendererID, myStages[*type]);
				glDeleteShader(myStages[*type]);
				myStages[*type] = 0;
			}
			
			GLuint result = glCreateShader(ToGlEnum(type));

			const char* sourceTxt = source.c_str();
			// Load in our shader source and compile it
			glShaderSource(result, 1, &sourceTxt, NULL);
			glCompileShader(result);

			__CheckCompileStatus(result);

			// Attach the shader, and store the handle
			glAttachShader(myRendererID, result);
			myStages[*type] = result;
		}

		void Shader::LoadPart(ShaderStageType type, const std::string& filePath) {
			const char* source = utils::ReadFile(filePath.c_str());
			CompilePart(type, source);
			delete[] source;
		}

		void Shader::LoadSpirv(ShaderStageType type, const char* compiledSource, size_t length, const std::string& entryPoint) {
			// Fail if we try to modify the shader after linkage
			LOG_ASSERT(!isLinked, "Cannot modify shader after linking");
			
			// If there is already a part in that slot, destroy it
			if (myStages[*type] != 0) {
				LOG_WARN("Replacing {} stage in shader", type);
				glDetachShader(myRendererID, myStages[*type]);
				glDeleteShader(myStages[*type]);
				myStages[*type] = 0;
			}

			// Create the shader, load the binary, and specialize it
			GLuint result = glCreateShader(ToGlEnum(type));
			glShaderBinary(1, &result, GL_SHADER_BINARY_FORMAT_SPIR_V, compiledSource, length);
			glSpecializeShader(result, entryPoint.c_str(), 0, nullptr, nullptr);

			__CheckCompileStatus(result);

			// Attach the shader, and store the handle
			glAttachShader(myRendererID, result);
			myStages[*type] = result;
		}

		void Shader::Link() {
			// Link the programs together
			glLinkProgram(myRendererID);

			// Remove shader parts to save space
			for(int ix = 0; ix < 5; ix++) {
				if (myStages[ix] != 0) {
					glDetachShader(myRendererID, myStages[ix]);
					glDeleteShader(myStages[ix]);
				}
			}

			// Get whether the link was successful
			GLint success = 0;
			glGetProgramiv(myRendererID, GL_LINK_STATUS, &success);

			// If not, we need to grab the log and throw an exception
			if (success == GL_FALSE) {
				// Get the length of the log
				GLint length = 0;
				glGetProgramiv(myRendererID, GL_INFO_LOG_LENGTH, &length);

				if (length > 0) {
					// Read the log from openGL
					char* log = new char[length];
					glGetProgramInfoLog(myRendererID, length, &length, log);
					LOG_ERROR("Shader failed to link:\n{}", log);
					delete[] log;
				}
				else {
					LOG_ERROR("Shader failed to link for an unknown reason!");
				}

				// Delete the partial program
				glDeleteProgram(myRendererID);

				// Throw a runtime exception
				throw new std::runtime_error("Failed to link shader program!");
			}
			else {
				LOG_TRACE("Shader has been linked");
				isLinked = true;
				__Introspect();
			}
		}

		#pragma region Uniform Introspection

		bool Shader::HasUniform(const std::string& name) const {
			return myUniforms.find(name) != myUniforms.end();
		}

		ShaderDataType Shader::GetUniformType(const std::string& name) const {
			auto it = myUniforms.find(name);
			if (it != myUniforms.end())
				return it->second.Type;
			else
				return ShaderDataType::None;
		}

		int Shader::GetUniformArraySize(const std::string& name) const {
			auto it = myUniforms.find(name);
			if (it != myUniforms.end())
				return it->second.ArraySize;
			else
				return 0;
		}

		int Shader::GetUniformLocation(const std::string& name) const {
			auto it = myUniforms.find(name);
			if (it != myUniforms.end())
				return it->second.Location;
			else
				return -1;
		}

		#pragma endregion

		#pragma region Set Uniforms

		void Shader::SetUniform(int loc, const glm::mat4& value) {
			glProgramUniformMatrix4fv(myRendererID, loc, 1, false, glm::value_ptr(value));
		}
		void Shader::SetUniform(int loc, const glm::mat3& value) {
			glProgramUniformMatrix3fv(myRendererID, loc, 1, false, glm::value_ptr(value));
		}
		void Shader::SetUniform(int loc, const glm::vec2& value) {
			glProgramUniform2fv(myRendererID, loc, 1, glm::value_ptr(value));
		}

		void Shader::SetUniform(int loc, const glm::ivec2& value) {
			glProgramUniform2iv(myRendererID, loc, 1, glm::value_ptr(value));
		}

		void Shader::SetUniform(int loc, const glm::vec3& value) {
			glProgramUniform3fv(myRendererID, loc, 1, glm::value_ptr(value));
		}
		void Shader::SetUniform(int loc, const glm::vec4& value) {
			glProgramUniform4fv(myRendererID, loc, 1, glm::value_ptr(value));
		}
		void Shader::SetUniform(int loc, const float& value) {
			glProgramUniform1fv(myRendererID, loc, 1, &value);
		}
		void Shader::SetUniform(int loc, const int& value) {
			glProgramUniform1iv(myRendererID, loc, 1, &value);
		}
		void Shader::SetUniform(int loc, const uint64_t& value) {
			glProgramUniformHandleui64ARB(myRendererID, loc, value);
		}
		
		void Shader::SetUniforms(int loc, int count, const glm::mat4* value) {
			glProgramUniformMatrix4fv(myRendererID, loc, count, false, glm::value_ptr(value[0]));
		}
		void Shader::SetUniforms(int loc, int count, const glm::mat3* value) {
			glProgramUniformMatrix3fv(myRendererID, loc, count, false, glm::value_ptr(value[0]));
		}
		void Shader::SetUniforms(int loc, int count, const glm::vec2* values) {
			glProgramUniform2fv(myRendererID, loc, count, glm::value_ptr(values[0]));
		}
		void Shader::SetUniforms(int loc, int count, const glm::vec3* values) {
			glProgramUniform3fv(myRendererID, loc, count, glm::value_ptr(values[0]));
		}
		void Shader::SetUniforms(int loc, int count, const glm::vec4* values) {
			glProgramUniform4fv(myRendererID, loc, count, glm::value_ptr(values[0]));
		}
		void Shader::SetUniforms(int loc, int count, const float* values) {
			glProgramUniform1fv(myRendererID, loc, count, &values[0]);
		}
		void Shader::SetUniforms(int loc, int count, const int* values) {
			glProgramUniform1iv(myRendererID, loc, count, &values[0]);
		}
		void Shader::SetUniforms(int loc, int count, const uint64_t* values) {
			glProgramUniformHandleui64vARB(myRendererID, loc, count, &values[0]);
		}
		#pragma endregion

		void Shader::Use() const {
			LOG_ASSERT(isLinked, "Shader must be linked before usage");
			glUseProgram(myRendererID);
		}

		bool Shader::__CheckCompileStatus(uint32_t shaderHandle) {
			// Check our compile status
			GLint compileStatus = 0;
			glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileStatus);

			// If we failed to compile
			if (compileStatus == GL_FALSE) {
				// Get the size of the error log
				GLint logSize = 0;
				glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logSize);

				// Create a new character buffer for the log
				char* log = new char[logSize];

				// Get the log
				glGetShaderInfoLog(shaderHandle, logSize, &logSize, log);

				// Dump error log
				LOG_ERROR("Failed to compile shader part:\n{}", log);

				// Clean up our log memory
				delete[] log;

				// Delete the broken shader result
				glDeleteShader(shaderHandle);

				// Throw a runtime exception
				throw new std::runtime_error("Failed to compile shader part!");
				return false;
			}
			else {
				LOG_TRACE("Shader part has been compiled!");
				return true;
			}
		}

		void Shader::__Introspect() {
			LOG_INFO("\tIntrospecting Uniforms");
			__IntrospectUniforms();
			LOG_INFO("\tIntrospecting Uniform Blocks");
			__IntrospectUniformBlocks();
		}

		void Shader::__IntrospectUniforms() {
			int numInputs = 0;
			glGetProgramInterfaceiv(myRendererID, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numInputs);

			for (int ix = 0; ix < numInputs; ix++) {
				static GLenum pNames[] = {
					GL_NAME_LENGTH,
					GL_TYPE,
					GL_ARRAY_SIZE,
					GL_LOCATION
				};
				int numProps = 0;
				int props[4];
				glGetProgramResourceiv(myRendererID, GL_UNIFORM, ix, 4, pNames, 4, &numProps, props);

				if (props[3] == -1)
					continue;

				UniformInfo e;
				std::string name;
				name.resize(props[0] - 1);
				int length = 0;
				glGetProgramResourceName(myRendererID, GL_UNIFORM, ix, props[0], &length, &name[0]);
				e.Name = name;
				e.Type = FromGLShaderDataType(props[1]);
				e.Location = props[3];
				e.ArraySize = props[2];
				if (e.ArraySize > 1) {
					e.Name = e.Name.substr(0, e.Name.find('['));
				}
				LOG_TRACE("\t\tDetected a new uniform: {}[{}]({}) -> {}", e.Name, e.ArraySize, e.Location, e.Type);
				myUniforms[e.Name] = e;
			}
		}

		void Shader::__IntrospectUniformBlocks() {
			int numBlocks = 0;
			glGetProgramInterfaceiv(myRendererID, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numBlocks);

			for (int ix = 0; ix < numBlocks; ix++) {
				static GLenum pNamesBlockProperties[] = {
					GL_NUM_ACTIVE_VARIABLES,
					GL_BUFFER_BINDING,
					GL_BUFFER_DATA_SIZE,
					GL_NAME_LENGTH
				};
				int results[4];
				glGetProgramResourceiv(myRendererID, GL_UNIFORM_BLOCK, ix, 4, pNamesBlockProperties, 4, NULL, results);

				if (!results[0])
					continue;

				static GLenum pNamesActiveVars[] = {
					GL_ACTIVE_VARIABLES
				};
				std::vector<int> activeVars(results[0]);
				glGetProgramResourceiv(myRendererID, GL_UNIFORM_BLOCK, ix, 1, pNamesActiveVars, results[0], NULL, &activeVars[0]);

				UniformBlockInfo block;
				block.Binding = results[1];
				block.SizeInBytes = results[2];
				block.NumVariables = results[0];
				std::string name;
				name.resize(results[3] - 1);
				glGetProgramResourceName(myRendererID, GL_UNIFORM_BLOCK, ix, results[3], NULL, &name[0]);
				block.Binding = glGetProgramResourceIndex(myRendererID, GL_UNIFORM_BLOCK, name.c_str());
				block.Name = name;
				block.SubUniforms.reserve(results[0]);
				block.BlockIndex = glGetUniformBlockIndex(myRendererID, name.c_str());

				LOG_TRACE("\t\tDetected a new uniform block \"{}\" with {} variables bound at {} ", block.Name, block.NumVariables, block.Binding);

				for (int v = 0; v < results[0]; v++) {
					static GLenum pNames[] = {
						GL_NAME_LENGTH,
						GL_TYPE,
						GL_ARRAY_SIZE,
						GL_LOCATION
					};
					int props[4];
					glGetProgramResourceiv(myRendererID, GL_UNIFORM, activeVars[v], 4, pNames, 4, NULL, props);
					UniformInfo var;
					std::string name;
					name.resize(props[0] - 1);
					glGetProgramResourceName(myRendererID, GL_UNIFORM, activeVars[v], props[0], NULL, &name[0]);
					var.Name = name;
					var.Type = FromGLShaderDataType(props[1]);
					var.Location = props[3];
					var.ArraySize = props[2];
					if (var.ArraySize > 1) {
						var.Name = var.Name.substr(0, var.Name.find('['));
					}
					LOG_TRACE("\t\tDetected a new uniform: {}[{}] -> {}", var.Name, var.ArraySize, var.Type);
					block.SubUniforms.push_back(var);
				}

				myUniformBlocks[block.Name] = block;
			}
		}

	}
}