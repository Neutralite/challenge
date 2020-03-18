#pragma once
#include "florp/graphics/Mesh.h"
#include "Material.h"

namespace florp {
	namespace game {
		
		/*
		 * Represents a component that we can attach to a game object that is used to render a mesh on the screen
		 */
		struct RenderableComponent {
			// The mesh that will be rendered
			graphics::Mesh::Sptr Mesh;
			// The material to render the mesh with
			Material::Sptr       Material;
		};
		
	}
}
