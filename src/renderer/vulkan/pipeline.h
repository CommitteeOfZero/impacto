#pragma once

#include <vulkan/vulkan.h>

namespace Impacto {
namespace Vulkan {

class Pipeline {
 public:
  Pipeline(VkDevice device, VkRenderPass renderPass);
  ~Pipeline();
  void SetRasterizerInfo(VkPipelineRasterizationStateCreateInfo rasterizerInfo);
  void SetDepthStencilInfo(
      VkPipelineDepthStencilStateCreateInfo depthStencilInfo);
  void SetPushConstants(VkPushConstantRange* pushConstantRange, int count);
  void CreateWithShader(
      char const* name, VkVertexInputBindingDescription bindingDescription,
      VkVertexInputAttributeDescription* attributeDescriptions,
      size_t attributeNum, VkDescriptorSetLayout setLayout,
      bool enableBlending = true);

  VkPipeline GraphicsPipeline;
  VkPipelineLayout PipelineLayout;

 private:
  void CreateRenderPass(VkFormat swapChainImageFormat);
  VkShaderModule CreateShaderModule(char const* code, size_t codeSize);

  VkDevice Device;
  VkRenderPass RenderPass;
  VkPipelineRasterizationStateCreateInfo RasterizerInfo;
  VkPipelineDepthStencilStateCreateInfo DepthStencilInfo;
  VkPushConstantRange* PushConstantRange = 0;
  int PushConstantRangeCount = 0;
};

}  // namespace Vulkan
}  // namespace Impacto