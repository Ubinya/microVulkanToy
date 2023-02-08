#pragma once

#include "lve_camera.hpp"
#include "lve_device.hpp"
#include "lve_frame_info.hpp"
#include "lve_game_object.hpp"
#include "lve_pipeline.hpp"

#include "lve_descriptors.hpp"

// std
#include <memory>
#include <vector>



namespace lve {

    struct ShadowMapTextureResource {
        VkImage       _shadowmap_texture_image{ VK_NULL_HANDLE };
        VkImageView   _shadowmap_texture_image_view{ VK_NULL_HANDLE };
        VkSampler     _shadowmap_texture_sampler{ VK_NULL_HANDLE };
    };

    struct DirectionalLight {
        glm::vec4 dir{};  // ignore w
        glm::vec4 color{};     // w is intensity
    };

    struct DirectionalLightUbo {
        glm::mat4 projection{ 1.f };
        glm::mat4 view{ 1.f };
        glm::mat4 inverseView{ 1.f };
        glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };  // w is intensity
        DirectionalLight pointLights[MAX_LIGHTS];
        int numLights;
    };

    class DirectionalLightShadowSystem {
    public:

        DirectionalLightShadowSystem(
            LveDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~DirectionalLightShadowSystem();

        DirectionalLightShadowSystem(const DirectionalLightShadowSystem&) = delete;
        DirectionalLightShadowSystem& operator=(const DirectionalLightShadowSystem&) = delete;

        void update(FrameInfo& frameInfo, GlobalUbo& ubo);
        void render(FrameInfo& frameInfo);


    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        LveDevice& lveDevice;

        std::unique_ptr<LvePipeline> lvePipeline;
        VkPipelineLayout pipelineLayout;

    };
}  // namespace lve
