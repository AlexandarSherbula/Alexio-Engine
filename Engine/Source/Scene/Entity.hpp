#include <entt/entt.hpp>
#include "Components.hpp"
#include "Scene.hpp"

namespace aio
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene)
			: mHandle(handle), mScene(scene) {}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			AIO_ASSERT(!HasComponent<T>(), "Entity already has component");
			return mScene->Reg().emplace<T>(mHandle, std::forward<Args>(args)...);
		}
	
		template<typename T>
		bool HasComponent()
		{
			return mScene->Reg().all_of<T>(mHandle);
		}
	
		template<typename T>
		T& GetComponent()
		{
			AIO_ASSERT(HasComponent<T>(), "Entity does not have component");
			return mScene->Reg().get<T>(mHandle);
		}
	
		template<typename T>
		void RemoveComponent()
		{
			mScene->Reg().remove<T>(mHandle);
		}
	
		operator bool() const { return mHandle != entt::null; }
		operator entt::entity() const { return mHandle; }
		operator uint32_t() const { return (uint32_t)mHandle; }
	
	private:
		entt::entity mHandle;
		Scene* mScene;
	};
}