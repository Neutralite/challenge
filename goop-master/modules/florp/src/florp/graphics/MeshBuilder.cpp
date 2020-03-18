#include "florp/graphics/MeshBuilder.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <unordered_map>
#include "Logging.h"

namespace florp {
	namespace graphics {
		
		int AddMiddlePoint(uint32_t offset, glm::vec3 scale, glm::vec3 center, int a, int b, std::vector<Vertex>& vertices, std::unordered_map<uint64_t, uint32_t>& midpointCache)
		{
			uint64_t key = 0;
			if (a < b) {
				key = (static_cast<uint64_t>(a) << 32ul) | static_cast<uint64_t>(b);
			}
			else {
				key = (static_cast<uint64_t>(b) << 32ul) | static_cast<uint64_t>(a);
			}
			auto it = midpointCache.find(key);
			if (it != midpointCache.end())
				return it->second;
			else {
				Vertex p1 = vertices[a];
				Vertex p2 = vertices[b];
				Vertex interpolated;
				glm::vec3 pos = glm::normalize((p1.Normal + p2.Normal) / 2.0f);
				float u = atan2f(pos.y, pos.x) / (2.0f * M_PI);
				float v = (asinf(pos.z) / M_PI) + 0.5f;
				interpolated.Position = center + (pos * scale);
				interpolated.Color = (p1.Color + p2.Color) / 2.0f;
				interpolated.Normal = pos;
				interpolated.UV = glm::vec2(u, v);

				int ix = offset + vertices.size();
				midpointCache[key] = ix;
				vertices.push_back(interpolated);
				return ix;
			}
		}

		void CorrectUVSeams(std::vector<Vertex>& verts, std::vector<uint32_t>& indices, size_t offset) {
			// lambda closure to easily add a vertex with unique texture coordinate to our mesh
			auto addVertex = [&](size_t ix, const glm::vec2& uv) {
				const uint32_t index = indices[ix];
				indices[ix] = (uint32_t)verts.size();
				Vertex vert = verts[index];
				vert.UV = uv;
				verts.push_back(vert);
			};

			// fix texture seams (this is where the magic happens) TODO: fix seams on tops and bottoms
			size_t numTriangles = (indices.size() - offset) / 3;
			for (size_t i = offset; i < offset + numTriangles; ++i) {
				const glm::vec2& uv0 = verts[indices[i * 3 + 0]].UV;
				const glm::vec2& uv1 = verts[indices[i * 3 + 1]].UV;
				const glm::vec2& uv2 = verts[indices[i * 3 + 2]].UV;

				const float d1 = uv1.x - uv0.x;
				const float d2 = uv2.x - uv0.x;

				if (abs(d1) > 0.5f && abs(d2) > 0.5f)
					addVertex(i * 3 + 0, uv0 + glm::vec2((d1 > 0.0f) ? 1.0f : -1.0f, 0.0f));
				else if (abs(d1) > 0.5f)
					addVertex(i * 3 + 1, uv1 + glm::vec2((d1 < 0.0f) ? 1.0f : -1.0f, 0.0f));
				else if (abs(d2) > 0.5f)
					addVertex(i * 3 + 2, uv2 + glm::vec2((d2 < 0.0f) ? 1.0f : -1.0f, 0.0f));
			}
		}

		Vertex CalculateSphereVert(glm::vec3 norm, glm::vec3 scale, glm::vec3 center) {
			Vertex vert;
			glm::vec3 pos = glm::normalize(norm);
			float u = atan2f(pos.y, pos.x) / (2.0f * M_PI);
			float v = (asinf(pos.z) / M_PI) + 0.5f;
			vert.Position = center + (pos * scale);
			vert.Normal = pos;
			vert.UV = glm::vec2(u, v);
			return vert;
		}

		void MeshBuilder::ComputeTBN(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, size_t begin, size_t end) {
			for (size_t i = begin; (i < indices.size()) & (i < end); i += 3) {
				glm::vec3 pos[3];
				pos[0] = vertices[indices[i + 0u]].Position;
				pos[1] = vertices[indices[i + 1u]].Position;
				pos[2] = vertices[indices[i + 2u]].Position;

				glm::vec2 uvs[3];
				uvs[0] = vertices[indices[i + 0u]].UV;
				uvs[1] = vertices[indices[i + 1u]].UV;
				uvs[2] = vertices[indices[i + 2u]].UV;

				glm::vec3 deltaP1 = pos[1] - pos[0];
				glm::vec3 deltaP2 = pos[2] - pos[0];

				glm::vec2 deltaT1 = uvs[1] - uvs[0];
				glm::vec2 deltaT2 = uvs[2] - uvs[0];

				float r = 1.0f / (deltaT1.x * deltaT2.y - deltaT1.y * deltaT2.x);
				glm::vec3 tangent = (deltaP1 * deltaT2.y - deltaP2 * deltaT1.y) * r;
				glm::vec3 bitangent = (deltaP2 * deltaT1.x - deltaP1 * deltaT2.x) * r;

				vertices[indices[i + 0u]].Tangent = tangent;
				vertices[indices[i + 1u]].Tangent = tangent;
				vertices[indices[i + 2u]].Tangent = tangent;

				vertices[indices[i + 0u]].BiTangent = bitangent;
				vertices[indices[i + 1u]].BiTangent = bitangent;
				vertices[indices[i + 2u]].BiTangent = bitangent;
			}
		}

		void MeshBuilder::AddIcoSphere(MeshData& data, const glm::vec3& center, float radius, int tessellation) {
			AddIcoSphere(data, center, glm::vec3(radius), tessellation);
		}

		void MeshBuilder::AddIcoSphere(MeshData& data, const glm::vec3& center, const glm::vec3& radii, int tessellation) {
			LOG_ASSERT(tessellation >= 0, "Tessellation must be greater than zero!");

			uint32_t indexOffset = data.Vertices.size();
			uint32_t initialIndex = data.Indices.size();
			std::vector<glm::ivec3> faces;

			uint32_t estimatedFaces = 12 + pow(tessellation + 1, 3);

			float t = (1.0f + sqrtf(5.0f)) / 2.0f;

			data.Vertices.emplace_back(CalculateSphereVert(glm::vec3(-1, t, 0), radii, center));
			data.Vertices.emplace_back(CalculateSphereVert(glm::vec3(1, t, 0), radii, center));
			data.Vertices.emplace_back(CalculateSphereVert(glm::vec3(-1, -t, 0), radii, center));
			data.Vertices.emplace_back(CalculateSphereVert(glm::vec3(1, -t, 0), radii, center));

			data.Vertices.emplace_back(CalculateSphereVert(glm::vec3(0, -1, t), radii, center));
			data.Vertices.emplace_back(CalculateSphereVert(glm::vec3(0, 1, t), radii, center));
			data.Vertices.emplace_back(CalculateSphereVert(glm::vec3(0, -1, -t), radii, center));
			data.Vertices.emplace_back(CalculateSphereVert(glm::vec3(0, 1, -t), radii, center));

			data.Vertices.emplace_back(CalculateSphereVert(glm::vec3(t, 0, -1), radii, center));
			data.Vertices.emplace_back(CalculateSphereVert(glm::vec3(t, 0, 1), radii, center));
			data.Vertices.emplace_back(CalculateSphereVert(glm::vec3(-t, 0, -1), radii, center));
			data.Vertices.emplace_back(CalculateSphereVert(glm::vec3(-t, 0, 1), radii, center));

			glm::ivec3 iOff = glm::ivec3(indexOffset);

			// 5 faces around point 0
			faces.emplace_back(iOff + glm::ivec3(0, 11, 5));
			faces.emplace_back(iOff + glm::ivec3(0, 5, 1));
			faces.emplace_back(iOff + glm::ivec3(0, 1, 7));
			faces.emplace_back(iOff + glm::ivec3(0, 7, 10));
			faces.emplace_back(iOff + glm::ivec3(0, 10, 11));

			// 5 adjacent faces
			faces.emplace_back(iOff + glm::ivec3(1, 5, 9));
			faces.emplace_back(iOff + glm::ivec3(5, 11, 4));
			faces.emplace_back(iOff + glm::ivec3(11, 10, 2));
			faces.emplace_back(iOff + glm::ivec3(10, 7, 6));
			faces.emplace_back(iOff + glm::ivec3(7, 1, 8));

			// 5 faces around point 3
			faces.emplace_back(iOff + glm::ivec3(3, 9, 4));
			faces.emplace_back(iOff + glm::ivec3(3, 4, 2));
			faces.emplace_back(iOff + glm::ivec3(3, 2, 6));
			faces.emplace_back(iOff + glm::ivec3(3, 6, 8));
			faces.emplace_back(iOff + glm::ivec3(3, 8, 9));

			// 5 adjacent faces
			faces.emplace_back(iOff + glm::ivec3(4, 9, 5));
			faces.emplace_back(iOff + glm::ivec3(2, 4, 11));
			faces.emplace_back(iOff + glm::ivec3(6, 2, 10));
			faces.emplace_back(iOff + glm::ivec3(8, 6, 7));
			faces.emplace_back(iOff + glm::ivec3(9, 8, 1));

			// Cache used to index our midpoints
			std::unordered_map<uint64_t, uint32_t> midPointCache;

			for (int ix = 0; ix < tessellation; ix++)
			{
				std::vector<glm::ivec3> tempFaces;
				for (auto& indices : faces)
				{
					uint32_t a = AddMiddlePoint(indexOffset, radii, center, indices[0], indices[1], data.Vertices, midPointCache);
					uint32_t b = AddMiddlePoint(indexOffset, radii, center, indices[1], indices[2], data.Vertices, midPointCache);
					uint32_t c = AddMiddlePoint(indexOffset, radii, center, indices[2], indices[0], data.Vertices, midPointCache);

					tempFaces.emplace_back(glm::ivec3(indices[0], a, c));
					tempFaces.emplace_back(glm::ivec3(indices[1], b, a));
					tempFaces.emplace_back(glm::ivec3(indices[2], c, b));
					tempFaces.emplace_back(glm::ivec3(a, b, c));
				}
				faces = tempFaces;
			}

			for (auto& face : faces) {
				data.Indices.push_back(face[0]);
				data.Indices.push_back(face[1]);
				data.Indices.push_back(face[2]);
			}

			CorrectUVSeams(data.Vertices, data.Indices, initialIndex);

		}

		void MeshBuilder::AddUvSphere(MeshData& data, const glm::vec3& center, float radius, int tessellation) {
			AddUvSphere(data, center, glm::vec3(radius), tessellation);
		}

		void MeshBuilder::AddUvSphere(MeshData& data, const glm::vec3& center, const glm::vec3& radii, int tessellation) {
			LOG_ASSERT(tessellation >= 0, "Tessellation must be greater than zero!");
			int slices = 1 + pow(2, tessellation + 1);
			int stacks = (slices / 2) + 1;


			int numverts = (slices + 1) * (slices - 1) + 2;
			std::vector<Vertex>& verts = data.Vertices;

			uint32_t offset = verts.size();
			uint32_t initialIndex = data.Indices.size();
			verts.reserve(verts.size() + numverts);

			float stackAngle, sliceAngle;
			float x, y, z, xy;

			float dLong = (M_PI * 2) / slices;
			float dLat = M_PI / stacks;

			for (int i = 0; i <= stacks; ++i) {
				stackAngle = M_PI / 2.0f - i * dLat;
				xy = cosf(stackAngle);
				z = sinf(stackAngle);

				for (int j = 0; j <= slices; ++j) {
					sliceAngle = j * dLong;

					x = xy * cosf(sliceAngle);
					y = xy * sinf(sliceAngle);

					Vertex vert;
					vert.Normal.x = x;
					vert.Normal.y = y;
					vert.Normal.z = z;
					vert.Position = center + (vert.Normal * radii);
					float u = (float)j / slices;
					float v = 1.0f - (float)i / stacks;
					vert.UV = { u, v };
					verts.push_back(vert);
				}
			}
			verts[offset].UV = { 0.5f, 1.0f };
			verts[verts.size() - 1].UV = { 0.5f, 0.0f };

			int numIndices = (slices - 1) * slices * 6;
			data.Indices.reserve(data.Indices.size() + numIndices);

			// Body loop
			int k1, k2;
			for (int i = 0; i < stacks; ++i)
			{
				k1 = i * (slices + 1);
				k2 = k1 + slices + 1;
				for (int j = 0; j < slices; ++j, ++k1, ++k2)
				{
					// Our top loop
					if (i != 0) {
						data.Indices.push_back(offset + k1);
						data.Indices.push_back(offset + k2);
						data.Indices.push_back(offset + k1 + 1);
					}

					// Everything but our bottom loop
					if (i != (stacks - 1)) {
						data.Indices.push_back(offset + k1 + 1);
						data.Indices.push_back(offset + k2);
						data.Indices.push_back(offset + k2 + 1);
					}
				}
			}
		}

		void MeshBuilder::AddAlignedCube(MeshData& data, const glm::vec3& center, const glm::vec3& halfBounds) {
			glm::vec3 ftl = center + halfBounds * glm::vec3(1, 1, 1);
			glm::vec3 btl = center + halfBounds * glm::vec3(-1, 1, 1);
			glm::vec3 fbl = center + halfBounds * glm::vec3(1, 1, -1);
			glm::vec3 bbl = center + halfBounds * glm::vec3(-1, 1, -1);

			glm::vec3 ftr = center + halfBounds * glm::vec3(1, -1, 1);
			glm::vec3 btr = center + halfBounds * glm::vec3(-1, -1, 1);
			glm::vec3 fbr = center + halfBounds * glm::vec3(1, -1, -1);
			glm::vec3 bbr = center + halfBounds * glm::vec3(-1, -1, -1);

			glm::vec2 tl = glm::vec2(1, 1);
			glm::vec2 tr = glm::vec2(0, 1);
			glm::vec2 bl = glm::vec2(1, 0);
			glm::vec2 br = glm::vec2(0, 0);

			uint32_t offset = data.Vertices.size();
			data.Vertices.reserve(data.Vertices.size() + 24);

			data.Vertices.push_back({ btl, {  0,  1,  0}, tl });
			data.Vertices.push_back({ ftl, {  0,  1,  0}, tr });
			data.Vertices.push_back({ bbl, {  0,  1,  0}, bl });
			data.Vertices.push_back({ fbl, {  0,  1,  0}, br });

			data.Vertices.push_back({ ftr, {  0, -1,  0}, tl });
			data.Vertices.push_back({ btr, {  0, -1,  0}, tr });
			data.Vertices.push_back({ fbr, {  0, -1,  0}, bl });
			data.Vertices.push_back({ bbr, {  0, -1,  0}, br });

			data.Vertices.push_back({ ftl, {  1,  0,  0}, tl });
			data.Vertices.push_back({ ftr, {  1,  0,  0}, tr });
			data.Vertices.push_back({ fbl, {  1,  0,  0}, bl });
			data.Vertices.push_back({ fbr, {  1,  0,  0}, br });

			data.Vertices.push_back({ btr, { -1,  0,  0}, tl });
			data.Vertices.push_back({ btl, { -1,  0,  0}, tr });
			data.Vertices.push_back({ bbr, { -1,  0,  0}, bl });
			data.Vertices.push_back({ bbl, { -1,  0,  0}, br });

			data.Vertices.push_back({ btr, {  0,  0,  1}, tl });
			data.Vertices.push_back({ ftr, {  0,  0,  1}, tr });
			data.Vertices.push_back({ btl, {  0,  0,  1}, bl });
			data.Vertices.push_back({ ftl, {  0,  0,  1}, br });

			data.Vertices.push_back({ bbl, {  0,  0, -1}, tl });
			data.Vertices.push_back({ fbl, {  0,  0, -1}, tr });
			data.Vertices.push_back({ bbr, {  0,  0, -1}, bl });
			data.Vertices.push_back({ fbr, {  0,  0, -1}, br });

			data.Indices.reserve(data.Indices.size() + 36);

			for (int ix = 0; ix < 6; ix++) {
				data.Indices.push_back(offset + (ix * 4) + 0);
				data.Indices.push_back(offset + (ix * 4) + 1);
				data.Indices.push_back(offset + (ix * 4) + 2);
				data.Indices.push_back(offset + (ix * 4) + 2);
				data.Indices.push_back(offset + (ix * 4) + 1);
				data.Indices.push_back(offset + (ix * 4) + 3);
			}
		}

		void MeshBuilder::AddOrientedCube(MeshData& data, const glm::vec3& center, const glm::vec3& halfBounds, const glm::mat4& transform) {
			uint32_t offset = data.Vertices.size();
			AddAlignedCube(data, center, halfBounds);

			glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(transform)));
			for (int ix = offset; ix < offset + 24; ix++) {
				data.Vertices[ix].Position = glm::vec4(data.Vertices[ix].Position, 1.0f) * transform;
				data.Vertices[ix].Normal = data.Vertices[ix].Normal * normalMatrix;
			}
		}

		MeshData MeshBuilder::Begin(const std::string& debugName) {
			MeshData result = MeshData();
			result.DebugName = debugName;
			return result;
		}

		Mesh::Sptr MeshBuilder::Bake(MeshData& data) {
			static const BufferLayout layout = {
				{ "inPosition",  ShaderDataType::Float3 },
				{ "inColor",     ShaderDataType::Float4 },
				{ "inNormal",    ShaderDataType::Float3 },
				{ "inTangent",   ShaderDataType::Float3 },
				{ "inBiTangent", ShaderDataType::Float3 },
				{ "inUV",        ShaderDataType::Float2 }
			};
			ComputeTBN(data.Vertices, data.Indices);

			const auto result = std::make_shared<Mesh>(
				data.Vertices.data(), data.Vertices.size(),
				layout,
				data.Indices.data(), data.Indices.size());
			result->Name = data.DebugName;
			return result;
		}
	}
}
