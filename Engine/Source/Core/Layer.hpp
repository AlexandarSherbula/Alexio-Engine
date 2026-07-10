#pragma once
#include "Events/Events.hpp"

#include <vector>

namespace aio {

	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return mDebugName; }
	protected:
		std::string mDebugName;
	};


	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		inline std::vector<Layer*>::iterator begin() { return mLayers.begin(); }
		inline std::vector<Layer*>::iterator end() { return mLayers.end(); }
		inline std::vector<Layer*>::reverse_iterator rbegin() { return mLayers.rbegin(); }
		inline std::vector<Layer*>::reverse_iterator rend() { return mLayers.rend(); }

		inline std::vector<Layer*>::const_iterator begin() const { return mLayers.begin(); }
		inline std::vector<Layer*>::const_iterator end()	const { return mLayers.end(); }
		inline std::vector<Layer*>::const_reverse_iterator rbegin() const { return mLayers.rbegin(); }
		inline std::vector<Layer*>::const_reverse_iterator rend() const { return mLayers.rend(); }
	private:
		std::vector<Layer*> mLayers;
		unsigned int mLayerInsertIndex = 0;
	};
}