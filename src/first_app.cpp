#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "lve_buffer.hpp"
#include "lve_camera.hpp"
#include "systems/point_light_system.hpp"
#include "systems/simple_render_system.hpp"

#include "dataloader/pmx_loader.h"

#include "systems/directional_light_shadow.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


// std
#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>


namespace lve {

    VkSampler getOrCreateNearestSampler(VkPhysicalDevice physical_device, VkDevice device)
    {
        VkSampler m_nearest_sampler;
        {
            VkPhysicalDeviceProperties physical_device_properties{};
            vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);

            VkSamplerCreateInfo samplerInfo{};

            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = VK_FILTER_NEAREST;
            samplerInfo.minFilter = VK_FILTER_NEAREST;
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            samplerInfo.mipLodBias = 0.0f;
            samplerInfo.anisotropyEnable = VK_FALSE;
            samplerInfo.maxAnisotropy = physical_device_properties.limits.maxSamplerAnisotropy; // close :1.0f
            samplerInfo.compareEnable = VK_FALSE;
            samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
            samplerInfo.minLod = 0.0f;
            samplerInfo.maxLod = 8.0f; // todo: m_irradiance_texture_miplevels
            samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            samplerInfo.unnormalizedCoordinates = VK_FALSE;

            if (vkCreateSampler(device, &samplerInfo, nullptr, &m_nearest_sampler) != VK_SUCCESS)
            {
                throw std::runtime_error("vk create sampler");
            }
        }

        return m_nearest_sampler;
    }

FirstApp::FirstApp() {
  globalPool =
      LveDescriptorPool::Builder(lveDevice)
          .setMaxSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT)
          .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
          .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, LveSwapChain::MAX_FRAMES_IN_FLIGHT)
          .build();
  loadGameObjects();
}

FirstApp::~FirstApp() {}

void FirstApp::run() {
  std::vector<std::unique_ptr<LveBuffer>> uboBuffers(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < uboBuffers.size(); i++) {
    uboBuffers[i] = std::make_unique<LveBuffer>(
        lveDevice,
        sizeof(GlobalUbo),
        1,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    uboBuffers[i]->map();
  }

  //create descriptor set layout
  auto globalSetLayout =
      LveDescriptorSetLayout::Builder(lveDevice)
          .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
          .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_ALL_GRAPHICS)
          .build();


  //setup descriptor set
  std::vector<VkDescriptorImageInfo> directional_light_shadow_texture_image_info(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < directional_light_shadow_texture_image_info.size(); i++) {
      directional_light_shadow_texture_image_info[i].sampler =
          getOrCreateNearestSampler(lveDevice.getPhysicalDevice(), lveDevice.device());
      directional_light_shadow_texture_image_info[i].imageView = lveRenderer.getShadowColorImages()[i];
      directional_light_shadow_texture_image_info[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  }
  

  std::vector<VkDescriptorSet> globalDescriptorSets(LveSwapChain::MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < globalDescriptorSets.size(); i++) {
    auto bufferInfo = uboBuffers[i]->descriptorInfo();
    LveDescriptorWriter(*globalSetLayout, *globalPool)
        .writeBuffer(0, &bufferInfo)
        .writeImage(1, &directional_light_shadow_texture_image_info[i])
        .build(globalDescriptorSets[i]);
  }


  SimpleRenderSystem simpleRenderSystem{
      lveDevice,
      lveRenderer.getSwapChainRenderPass(),
      globalSetLayout->getDescriptorSetLayout()};
  PointLightSystem pointLightSystem{
      lveDevice,
      lveRenderer.getSwapChainRenderPass(),
      globalSetLayout->getDescriptorSetLayout()};

  // added directional light shadow system
  DirectionalLightShadowSystem directioanlLightShadowSystem{
      lveDevice,
      lveRenderer.getShadowRenderPass(),
      globalSetLayout->getDescriptorSetLayout()};


  LveCamera camera{};

  auto viewerObject = LveGameObject::createGameObject();
  viewerObject.transform.translation.z = -2.5f;
  KeyboardMovementController cameraController{};

  auto currentTime = std::chrono::high_resolution_clock::now();
  while (!lveWindow.shouldClose()) {
    glfwPollEvents();

    auto newTime = std::chrono::high_resolution_clock::now();
    float frameTime =
        std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
    currentTime = newTime;

    cameraController.moveInPlaneXZ(lveWindow.getGLFWwindow(), frameTime, viewerObject);
    camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

    float aspect = lveRenderer.getAspectRatio();
    camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

    if (auto commandBuffer = lveRenderer.beginFrame()) {
      int frameIndex = lveRenderer.getFrameIndex();
      FrameInfo frameInfo{
          frameIndex,
          frameTime,
          commandBuffer,
          camera,
          globalDescriptorSets[frameIndex],
          gameObjects};

      // update
      GlobalUbo ubo{};
      ubo.projection = camera.getProjection();
      ubo.view = camera.getView();
      ubo.inverseView = camera.getInverseView();
      pointLightSystem.update(frameInfo, ubo);
      uboBuffers[frameIndex]->writeToBuffer(&ubo);
      uboBuffers[frameIndex]->flush();

      // shadow map
      lveRenderer.beginShadowRenderPass(commandBuffer);
      directioanlLightShadowSystem.render(frameInfo);
      lveRenderer.endSwapChainRenderPass(commandBuffer);

      //render scene
      lveRenderer.beginSwapChainRenderPass(commandBuffer);
      // order here matters
      simpleRenderSystem.renderGameObjects(frameInfo);
      pointLightSystem.render(frameInfo);
      lveRenderer.endSwapChainRenderPass(commandBuffer);

      lveRenderer.endFrame();
    }
  }

  vkDeviceWaitIdle(lveDevice.device());
}

void FirstApp::loadGameObjects() {
    
    /*
    dataloader::PMX_only_obj mmdModel = dataloader::PMX_only_obj("D:/GitHub/WA2000_GFL.pmx");

    std::shared_ptr<LveModel> lveModel = LveModel::createModelFromPMX(lveDevice, mmdModel);
    auto wa2k = LveGameObject::createGameObject();
    wa2k.model = lveModel;
    wa2k.transform.translation = { 0.f, .5f, 0.f };
    wa2k.transform.scale = { .1f, -.1f, .1f };
    gameObjects.emplace(wa2k.getId(), std::move(wa2k));
    */
    
    
    /*
    std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "models/bunny1.obj");
    auto bunny = LveGameObject::createGameObject();
    bunny.model = lveModel;
    bunny.transform.translation = { 0.f, .5f, -.0f };
    bunny.transform.scale = { .5f, -.5f, -.5f };
    gameObjects.emplace(bunny.getId(), std::move(bunny));
    */

    std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(lveDevice, "models/flat_vase.obj");
    auto flatVase = LveGameObject::createGameObject();
    flatVase.model = lveModel;
    flatVase.transform.translation = {-.5f, .5f, 0.f};
    flatVase.transform.scale = {1.f, .5f, 1.f};
    gameObjects.emplace(flatVase.getId(), std::move(flatVase));

  lveModel = LveModel::createModelFromFile(lveDevice, "models/smooth_vase.obj");
  auto smoothVase = LveGameObject::createGameObject();
  smoothVase.model = lveModel;
  smoothVase.transform.translation = {.5f, .5f, 0.f};
  smoothVase.transform.scale = {3.f, 1.5f, 3.f};
  gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

  lveModel = LveModel::createModelFromFile(lveDevice, "models/quad.obj");
  auto floor = LveGameObject::createGameObject();
  floor.model = lveModel;
  floor.transform.translation = {0.f, .5f, 0.f};
  floor.transform.scale = {3.f, 1.f, 3.f};
  gameObjects.emplace(floor.getId(), std::move(floor));

  std::vector<glm::vec3> lightColors{
      {1.f, .1f, .1f},
      {.1f, .1f, 1.f},
      {.1f, 1.f, .1f},
      {1.f, 1.f, .1f},
      {.1f, 1.f, 1.f},
      {1.f, 1.f, 1.f}  //
  };

  for (int i = 0; i < lightColors.size(); i++) {
    auto pointLight = LveGameObject::makePointLight(0.2f);
    pointLight.color = lightColors[i];
    auto rotateLight = glm::rotate(
        glm::mat4(1.f),
        (i * glm::two_pi<float>()) / lightColors.size(),
        {0.f, -1.f, 0.f});
    pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
    gameObjects.emplace(pointLight.getId(), std::move(pointLight));
  }
}

}  // namespace lve
