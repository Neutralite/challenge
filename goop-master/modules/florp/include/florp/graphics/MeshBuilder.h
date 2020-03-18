#pragma once
#include "MeshData.h"
#include "GLM/glm.hpp"
#include "Mesh.h"

namespace florp {
	namespace graphics {
		
		class MeshBuilder
		{
		public:

			static void AddIcoSphere(MeshData& data, const glm::vec3& center, float radius, int tessellation = 0);
			static void AddIcoSphere(MeshData& data, const glm::vec3& center, const glm::vec3& radii, int tessellation = 0);
			static void AddUvSphere(MeshData& data, const glm::vec3& center, float radius, int tessellation = 0);
			static void AddUvSphere(MeshData& data, const glm::vec3& center, const glm::vec3& radii, int tessellation = 0);

			/*
			 * Computes the tangent, bitangent, and normals for a given set of vertices and indices
			 * @param vertices The vertices to modify
			 * @param indices The indices that form triangles from the vertices
			 * @param begin The offset into the indices list to start calculating from
			 * @param end The offset into the indices list to stop calculations at
			 */
			static void ComputeTBN(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, size_t begin = 0, size_t end = 0xFFFFFFFFFFFFFFFFu);

			static void AddAlignedCube(MeshData& data, const glm::vec3& center, const glm::vec3& halfBounds);
			static void AddOrientedCube(MeshData& data, const glm::vec3& center, const glm::vec3& halfBounds, const glm::mat4& transform);

			/*
			 * Begins building a new mesh, with the given debug name
			 * @param debugName The debug name of the mesh
			 * @returns An empty mesh data
			 */
			static MeshData Begin(const std::string& debugName = "");
			/*
			 * Turns a mesh data into a mesh on the GPU
			 * @param data The data to turn into a mesh
			 * @returns A mesh created from the data that was passed in
			 */
			static graphics::Mesh::Sptr Bake(MeshData& data);
		};
	}
}