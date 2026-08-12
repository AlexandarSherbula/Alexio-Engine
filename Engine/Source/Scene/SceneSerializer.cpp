#include "aio_pch.hpp"
#include "SceneSerializer.hpp"
#include "Scene.hpp"
#include "Entity.hpp"
#include "Components.hpp"

#include <yaml-cpp/yaml.h>

namespace aio
{
    SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
    {
        mScene = scene;
    }

    void SceneSerializer::Serialize(const std::filesystem::path& filepath)
    {
        YAML::Emitter out;
        
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled";
        
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
        
        for (auto entityHandle : mScene->mRegistry.storage<entt::entity>())
        {
            Entity entity = { entityHandle, mScene.get() };
        
            out << YAML::BeginMap;
            out << YAML::Key << "Entity" << YAML::Value << 13212654686584;
        
            if (entity.HasComponent<TagComponent>())
            {
                auto& tagComponent = entity.GetComponent<TagComponent>();
                out << YAML::Key << "TagComponent" << YAML::Value << YAML::BeginMap;
                out << YAML::Key << "Tag" << YAML::Value << tagComponent.Tag;
                out << YAML::EndMap;
            }

            if (entity.HasComponent<TransformComponent>())
            {
                auto& tc = entity.GetComponent<TransformComponent>();

                out << YAML::Key << "TransformComponent" << YAML::Value << YAML::BeginMap;

                out << YAML::Key << "Position" << YAML::Value
                    << YAML::Flow
                    << YAML::BeginSeq 
                    << tc.Position.x
                    << tc.Position.y
                    << tc.Position.z
                    << YAML::EndSeq;

                out << YAML::Key << "Rotation" << YAML::Value
                    << YAML::Flow
                    << YAML::BeginSeq
                    << tc.Rotation.x
                    << tc.Rotation.y
                    << tc.Rotation.z
                    << YAML::EndSeq;

                out << YAML::Key << "Scale" << YAML::Value
                    << YAML::Flow
                    << YAML::BeginSeq
                    << tc.Scale.x
                    << tc.Scale.y
                    << tc.Scale.z
                    << YAML::EndSeq;

                out << YAML::EndMap;
            }

            if (entity.HasComponent<SpriteComponent>())
            {
                auto& sc = entity.GetComponent<SpriteComponent>();

                out << YAML::Key << "SpriteComponent" << YAML::Value << YAML::BeginMap;

                out << YAML::Key << "Color" << YAML::Value
                    << YAML::Flow 
                    << YAML::BeginSeq 
                    << sc.Color.r
                    << sc.Color.g 
                    << sc.Color.b 
                    << sc.Color.a 
                    << YAML::EndSeq;

                out << YAML::EndMap;
            }

            if (entity.HasComponent<CameraComponent>())
            {
                auto& cc = entity.GetComponent<CameraComponent>();
                out << YAML::Key << "CameraComponent" << YAML::Value << YAML::BeginMap;
                out << YAML::Key << "Primary" << YAML::Value << cc.Primary;
                out << YAML::Key << "Size" << YAML::Value << cc.OrthographicSize;
                out << YAML::EndMap;
            }

            out << YAML::EndMap;
        }
        out << YAML::EndSeq; // Entities
        out << YAML::EndMap; // Scene
        
        std::ofstream fout(filepath.string());
        fout << out.c_str();
        AIO_LOG_INFO("Saved Scene Sucessfully!");
    }

    void SceneSerializer::Deserialize(const std::filesystem::path& filepath)
    {
        YAML::Node data = YAML::LoadFile(filepath.string());
        AIO_ASSERT(data["Scene"], "Failed to load the Scene!");

        auto NodeOfEntities = data["Entities"];

        if (NodeOfEntities)
        {
            for (auto entityNode : NodeOfEntities)
            {
                std::string name = "Unnamed";

                if (entityNode["TagComponent"])
                    name = entityNode["TagComponent"]["Tag"].as<std::string>();

                Entity entity = mScene->CreateEntity(name);

                if (entityNode["TransformComponent"])
                {
                    auto& tc = entity.GetComponent<TransformComponent>();
                    auto pos = entityNode["TransformComponent"]["Position"];
                    auto rot = entityNode["TransformComponent"]["Rotation"];
                    auto scale = entityNode["TransformComponent"]["Scale"];

                    tc.Position = { pos[0].as<float>(),   pos[1].as<float>(),    pos[2].as<float>() };
                    tc.Rotation = { rot[0].as<float>(),   rot[1].as<float>(),    rot[2].as<float>() };
                    tc.Scale = { scale[0].as<float>(), scale[1].as<float>(),  scale[2].as<float>() };
                }

                if (entityNode["SpriteComponent"])
                {
                    auto color = entityNode["SpriteComponent"]["Color"];
                    entity.AddComponent<SpriteComponent>(Vector4(color[0].as<float>(), color[1].as<float>(), color[2].as<float>(), color[3].as<float>()));
                }

                if (entityNode["CameraComponent"])
                {
                    entity.AddComponent<CameraComponent>();
                    auto& cc = entity.GetComponent<CameraComponent>();
                    cc.Primary = entityNode["CameraComponent"]["Primary"].as<bool>();

                    cc.Camera.SetOrthographicSize(entityNode["CameraComponent"]["Size"].as<float>());
                    cc.OrthographicSize = entityNode["CameraComponent"]["Size"].as<float>();
                }

                AIO_LOG_INFO("Loaded entity with an ID: {0} and tag <{1}>", (uint32_t)entity, entity.GetComponent<TagComponent>().Tag);
            }
        }

        
    }
}


