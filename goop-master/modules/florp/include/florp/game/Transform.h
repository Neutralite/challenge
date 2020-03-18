#pragma once
#include <memory> // for shared_ptr
#include <GLM/glm.hpp> // for all the GLM types
#include <GLM/gtc/quaternion.hpp> // for the GLM quaternion stuff
#include "entt.hpp" // For the entt parenting stuff

namespace florp {
	namespace game {

		struct Transform {

			typedef std::shared_ptr<Transform> Sptr;

			Transform() :
				myWorldTransform(glm::mat4(1.0f)),
				isDirty(false), isLocalDirty(false),
				myLocalTransform(glm::mat4(1.0f)),
				myLocalPosition(glm::vec3()),
				myScale(glm::vec3(1.0f)),
				myLocalRotation(glm::quat(1,0,0,0)),
				myParent(entt::null) { }
			~Transform();

			/*
			 * Set the parent transformation for this transform. All transformations within this component
			 * will be relative to the parent. To break this relationship, call SetParent with entt::null
			 * @param parent The parent entity for this transformation
			 */
			void SetParent(const entt::entity& parent);
			/*
			 * Returns the parent entity of this transform (the next entity up in the hierarchy), or entt::null
			 * if this entity does not have a parent assigned
			 */
			const entt::entity& GetParent() const { return myParent; }

			/*
			 * Sets the local scale for this transformation, along each axis. By default, this is 1.0f
			 * @param scale The scaling to use for this transformation
			 */
			void SetScale(const glm::vec3& scale = glm::vec3(1.0f));
			/*
			 * Set the local scale for this transformation, as a uniform scale. By default this is 1.0f
			 * @param scale The uniform scale to use for all axes
			 */
			void SetScale(float scale = 1.0f) { SetScale(glm::vec3(scale)); }
			/*
			 * Gets the local scale of this transformation
			 */
			const glm::vec3& GetScale() const { return myScale; }

			/*
			 * Gets the forward vector (corresponding to the z axis) of this transformation
			 */
			glm::vec3 GetForward() const;
			/*
			 * Gets the up vector (corresponding to the y axis) for this transformation
			 */
			glm::vec3 GetUp() const;
			/*
			 * Gets the right vector (corresponding to the x axis) for this transformation
			 */
			glm::vec3 GetRight() const;

			/*
			 * Orients this transform to look at the given point in local space. This does not
			 * change the transform's position in the world
			 * @param localPos The point in local space to look at
			 * @param localUp  The vector in local space indicating which direction is up (usually +Y or +Z)
			 */
			void LookAt(const glm::vec3& localPos, const glm::vec3& localUp);

			/*
			 * Sets the position of this transformation, either in local space or world space
			 * @param pos The position to move this transform to
			 * @param local True if pos is in local space, false for world space (default is local)
			 */
			void SetPosition(const glm::vec3& pos, bool local = true);
			/*
			 * Gets the local position of this transformation component
			 */
			const glm::vec3& GetLocalPosition() const { return myLocalPosition; }
			/*
			 * Gets the position of this transform component in world space
			 */
			glm::vec3 GetWorldPosition() const { return GetWorldTransform() * glm::vec4(myLocalPosition, 1.0f); }

			/*
			 * Sets the Euler rotation for this transform, in degrees
			 * @param euler The rotation for this transformation, in degrees
			 * @param local True if the given rotation is in local space, false for world space
			 */
			void SetEulerAngles(const glm::vec3& euler, bool local = true);
			/*
			 * Sets our quaternion rotation for this transform
			 * @param rot The rotation to set the transform to
			 * @param local True if the rotation is local to this transform, false if it is in world space
			 */
			void SetRotation(const glm::quat& rot, bool local = true);
			/*
			 * Gets the local rotation of this transformation in degrees
			 */
			glm::vec3 GetLocalEulerAngles() const { return glm::degrees(glm::eulerAngles(myLocalRotation)); }

			/*
			 * Rotates this transformation by the given quaternion
			 * @param rot The quaternion to rotate by
			 */
			void Rotate(const glm::quat& rot);
			/*
			 * Rotates this transformation by the given euler angles, in degrees
			 * @param rot The quaternion to rotate by
			 */
			void Rotate(const glm::vec3& euler);

			/*
			 * Gets this transform's local transformation matrix
			 */
			const glm::mat4& GetLocalTransform() const;
			/*
			 * Gets this transform's world transform matrix
			 */
			const glm::mat4& GetWorldTransform() const;
			/*
			 * Gets this transform's parent transformation matrix, or the identity matrix if this
			 * transform is not a child of another transform
			 */
			glm::mat4 GetParentTransform() const;

		protected:
			mutable glm::mat4           myWorldTransform;
			mutable bool                isDirty;
			mutable bool                isLocalDirty;
			mutable glm::mat4           myLocalTransform;

			glm::vec3 myLocalPosition;
			glm::vec3 myScale;
			glm::quat myLocalRotation;

			bool __IsChainDirty() const;

			entt::entity                myParent;
		};
	}
}