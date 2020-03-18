#include "florp/graphics/Mesh.h"

namespace florp {
	namespace graphics {
		
		Mesh::Mesh(void* vertices, size_t numVerts, const BufferLayout& layout, uint32_t* indices, size_t numIndices) {
			myIndexCount = numIndices;
			myVertexCount = numVerts;

			// Cache the layout
			myLayout = layout;

			// Create and bind our vertex array
			glCreateVertexArrays(1, &myRendererID);
			glBindVertexArray(myRendererID);

			// Create 2 buffers, 1 for vertices and the other for indices
			glCreateBuffers(2, myBuffers);

			// Bind and buffer our vertex data
			glBindBuffer(GL_ARRAY_BUFFER, myBuffers[0]);
			glBufferData(GL_ARRAY_BUFFER, numVerts * layout.GetStride(), vertices, GL_STATIC_DRAW);

			// Bind and buffer our index data
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myBuffers[1]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(uint32_t), indices, GL_STATIC_DRAW);

			// Our elements will be sequential in the shaders (so attrib 0, 1, 2 ...)
			uint32_t index = 0;
			// Iterate over all elements
			for (const BufferElement& element : layout) {
				// Enable the attribute
				glEnableVertexAttribArray(index);
				// Set up the vertex attribute
				glVertexAttribPointer(index,
					element.GetComponentCount(), // Number of components in the attribute
					ToGLElementType(GetShaderDataTypeCode(element.Type)), // The data type of the attribute
					element.IsNormalized, // Whether or not the element is normalized
					layout.GetStride(),
					(const void*)element.Offset);
				index++;
			}

			// Unbind our VAO
			glBindVertexArray(0);
		}

		void* Mesh::ExtractVertices(size_t& outSize) const {
			outSize = myLayout.GetStride() * myVertexCount;
			void* result = malloc(outSize);
			glGetNamedBufferSubData(myBuffers[0], 0, outSize, result);
			return result;
		}

		uint32_t* Mesh::ExtractIndices(size_t& outSize) const {
			outSize = sizeof(uint32_t) * myIndexCount;
			void* data = malloc(outSize);
			glGetNamedBufferSubData(myBuffers[1], 0, outSize, data);
			return (uint32_t*)data;
		}

		Mesh::~Mesh() {
			LOG_INFO("Deleting mesh with ID: {}", myRendererID);
			// Clean up our buffers
			glDeleteBuffers(2, myBuffers);
			// Clean up our VAO
			glDeleteVertexArrays(1, &myRendererID);
		}

		void Mesh::Draw() {
			// Bind the mesh
			glBindVertexArray(myRendererID);
			if (myIndexCount > 0)
				// Draw all of our vertices as triangles, our indexes are unsigned ints (uint32_t)
				glDrawElements(GL_TRIANGLES, myIndexCount, GL_UNSIGNED_INT, nullptr);
			else
				glDrawArrays(GL_TRIANGLES, 0, myVertexCount);
		}
	}
}