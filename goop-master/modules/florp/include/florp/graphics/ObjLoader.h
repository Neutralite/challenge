#pragma once
#include "MeshData.h"

namespace florp { namespace graphics {
	class ObjLoader {
	public:
		static MeshData LoadObj(const char* filename, glm::vec4 baseColor);
	};
}}