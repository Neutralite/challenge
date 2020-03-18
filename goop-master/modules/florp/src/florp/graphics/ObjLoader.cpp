#include "florp/graphics/ObjLoader.h"
#include "florp/graphics/MeshBuilder.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <regex>

#include "Logging.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/normal.hpp>
#include <unordered_map>

namespace florp {
	namespace graphics {

		struct VIndex {
			uint32_t Pos, Tex, Norm;
		};

		// Borrowed from https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
		#pragma region String Trimming

		// trim from start (in place)
		static inline void ltrim(std::string& s) {
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
				return !std::isspace(ch);
				}));
		}

		// trim from end (in place)
		static inline void rtrim(std::string& s) {
			s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
				return !std::isspace(ch);
				}).base(), s.end());
		}

		// trim from both ends (in place)
		static inline void trim(std::string& s) {
			ltrim(s);
			rtrim(s);
		}

		#pragma endregion 

		MeshData ObjLoader::LoadObj(const char* filename, glm::vec4 baseColor) {
			// Open our file in binary mode
			std::ifstream file;
			file.open(filename, std::ios::binary);

			// If our file fails to open, we will throw an error
			if (!file) {
				throw new std::runtime_error("Failed to open file");
			}

			LOG_TRACE("Loading mesh from '{}'", filename);

			// Declare vectors for our positions, normals, and face indices
			std::vector<glm::vec3>   positions;
			std::vector<glm::vec2>   texUvs;
			std::vector<glm::vec3>   normals;
			std::vector<glm::mat<3, 3, uint32_t>>  faces;
			// Stores our w value if it is provided
			float garb;
			// Stores the line that we are operating on
			std::string line;

			// A cache for mapping face vertex indices to a mesh vertex index
			std::unordered_map<uint64_t, int> vectorCache;

			// Iterate as long as there is content to read
			while (std::getline(file, line)) {
				// v is our position
				if (line.substr(0, 2) == "v ") {
					// Read in the position and append it to our positions list
					std::istringstream ss = std::istringstream(line.substr(2));
					glm::vec3 pos; ss >> pos.x; ss >> pos.y; ss >> pos.z; ss >> garb;
					positions.push_back(pos);
				}
				// vn is our normals
				else if (line.substr(0, 3) == "vn ") {
					// Read in all the normals and append to list
					std::istringstream ss = std::istringstream(line.substr(3));
					glm::vec3 norm; ss >> norm.x; ss >> norm.y; ss >> norm.z;
					normals.push_back(norm);
				}
				// vt is our UV's 
				else if (line.substr(0, 3) == "vt ") {
					// Read in the coordinates and append to list
					std::istringstream ss = std::istringstream(line.substr(3));
					glm::vec2 uv; ss >> uv.x; ss >> uv.y;
					texUvs.push_back(uv);
				}
				// f is our faces
				else if (line.substr(0, 2) == "f ") {
					// We will start parsing with a string stream
					std::istringstream ss = std::istringstream(line.substr(2));

					// Our face starts as all zeros
					glm::mat<3, 3, uint32_t> face = glm::mat<3, 3, uint32_t>(0.0f);

					// Remove the first 2 characters (the 'f ')
					line = line.substr(2);
					// Trim any leftover whitespace characters
					rtrim(line);

					// Track whether we are on a whitespace char, and get the length of the line
					bool ws = false;
					size_t len = line.length();

					// Set all the attributes to -1 by default
					face[0][0] = -1; face[0][1] = -1; face[0][2] = -1;
					face[1][0] = -1; face[1][1] = -1; face[1][2] = -1;
					face[2][0] = -1; face[2][1] = -1; face[2][2] = -1;

					// Iterate over line, track where we the beginning of our integer is
					for (size_t ix = 0, start = 0, attribIx = 0, vertexIx = 0; ix < len; ix++) {
						// If we have a digit, set our whitespace tracker to false
						if (std::isdigit(line[ix])) {
							ws = false;
						}
						// If we are at a divisor character, or at the end of the string, we need to extract
						if ((line[ix] == '/' || line[ix] == '\\') | (ix == (len - 1))) {
							// Extract the number
							int index = atoi(line.c_str() + start);
							// Update our start offset for the next number
							start = ix + 1;
							// Ensure that we are not past the end of our vertex
							LOG_ASSERT(attribIx < 3, "Too many attribute indices in OBJ vertex!");
							// Store the index (and adjust index to be 0-based)
							face[vertexIx][attribIx] = index - 1;
							// Advance to the next attribute
							attribIx++;
							ws = false;
						}
						// Handle whitespace characters, these indicate the start of a new face
						else if (line[ix] == ' ' || line[ix] == '\t') {
							// If the last character was not whitespace, we extract the last attribute
							if (!ws) {
								int index = atoi(line.c_str() + start);
								face[vertexIx][attribIx] = index - 1;
								// Advance to the next vertex, and reset the attrib index
								LOG_ASSERT(vertexIx < 3, "Too many vertices OBJ face!");
								vertexIx++;
								attribIx = 0;
							}
							// Update the start index for attributes
							start = ix + 1;
							ws = true;
						}
					}

					// Once we have face index data, add it to our list to be processed
					faces.push_back(face);
				}
			}

			LOG_TRACE("\tLoaded data, starting post-processing");

			// Allocate a new array for our vertices
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;

			// Iterate over all the positions we've read
			for (auto& face : faces) {
				for (int jx = 0; jx < 3; jx++) {
					auto& aSet = face[jx];
					// We will use our mask to only select the lowest 21 bits of each index
					uint64_t mask = 0b0'000000000000000000000'000000000000000000000'111111111111111111111;
					uint64_t key = 0;

					// We generate a key using our vertex's position, texture, and normal indices
					key = ((aSet[0] & mask) << 42) | ((aSet[1] & mask) << 21) | (aSet[2] & mask);

					// Search the cache for the key
					auto it = vectorCache.find(key);

					// If it exists, we push the index to our indices
					if (it != vectorCache.end())
						indices.push_back(it->second);
					// Otherwise, we need to create a new vertex
					else
					{
						// Load the vertex from the attributes
						Vertex vertex;
						vertex.Position =
							aSet[0] != (uint32_t)-1 ?
							positions[aSet[0]] :
							glm::vec3(0);
						vertex.Color = baseColor;
						vertex.UV =
							aSet[1] != (uint32_t)-1 ?
							texUvs[aSet[1]] :
							glm::vec2(0.0f);
						vertex.Normal =
							aSet[2] != (uint32_t)-1 ?
							normals[aSet[2]] :
							glm::triangleNormal(positions[face[0][0]], positions[face[1][0]], positions[face[2][0]]);
						// Add the index of the new vertex to the cache
						vectorCache[key] = vertices.size();
						// Add the index of the new vertex to our indices
						indices.push_back(vertices.size());
						// Add the vertex to the buffer
						vertices.push_back(vertex);

					}
				}
			}

			// Compute our TBN matrices for normal mapping
			LOG_TRACE("\tComputing TBN matrices...");
			MeshBuilder::ComputeTBN(vertices, indices);

			// Create and return a result as a meshBuilder mesh data object
			auto result = MeshBuilder::Begin();
			result.Vertices = vertices;
			result.Indices = indices;
			result.DebugName = filename;
			return result;
		}

	}
}
