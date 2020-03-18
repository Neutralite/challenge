#pragma once
#include <string>
#include <vector>
#include <GLM/glm.hpp>
#include "BufferLayout.h"

namespace florp {
	namespace graphics {
		
		struct Vertex {
			glm::vec3 Position;
			glm::vec4 Color;
			glm::vec3 Normal;
			glm::vec3 Tangent;
			glm::vec3 BiTangent;
			glm::vec2 UV;

			Vertex(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 norm = glm::vec3(0.0f), glm::vec2 uv = glm::vec2(0.0f)) :
				Position(pos),
				Color(glm::vec4(1.0f)),
				Normal(norm),
				Tangent(glm::vec3(0.0f)),
				BiTangent(glm::vec3(0.0f)),
				UV(uv) { }
		};

		const BufferLayout VertexLayout = {
			{ "Position",  ShaderDataType::Float3, VertexUsage::Position },
			{ "Color",     ShaderDataType::Float4, VertexUsage::Color },
			{ "Normal",    ShaderDataType::Float3, VertexUsage::Normal },
			{ "Tangent",   ShaderDataType::Float3, VertexUsage::Tangent },
			{ "BiTangent", ShaderDataType::Float3, VertexUsage::Bitangent },
			{ "UV",        ShaderDataType::Float2, VertexUsage::Texture }
		};

		struct MeshData {
			std::vector<Vertex>   Vertices;
			std::vector<uint32_t> Indices;
			std::string           DebugName;
		};
		
	}
}
