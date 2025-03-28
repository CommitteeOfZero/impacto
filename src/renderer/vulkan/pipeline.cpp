#include "pipeline.h"

#include <vector>

#include "../../log.h"
#include "../../util.h"
#include "renderer.h"

namespace Impacto {
namespace Vulkan {

static char const ShaderPath[] = "./shaders/vulkan";
static char const FragShaderExtension[] = "_frag.spv";
static char const VertShaderExtension[] = "_vert.spv";

Pipeline::Pipeline(VkDevice device, VkRenderPass renderPass) {
  Device = device;
  RenderPass = renderPass;

  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  RasterizerInfo = rasterizer;

  VkPipelineDepthStencilStateCreateInfo depthStencil{};
  depthStencil.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = VK_FALSE;
  depthStencil.depthWriteEnable = VK_FALSE;
  depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depthStencil.depthBoundsTestEnable = VK_FALSE;
  depthStencil.stencilTestEnable = VK_FALSE;
  DepthStencilInfo = depthStencil;
}

void Pipeline::SetRasterizerInfo(
    VkPipelineRasterizationStateCreateInfo rasterizerInfo) {
  RasterizerInfo = rasterizerInfo;
}

void Pipeline::SetDepthStencilInfo(
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo) {
  DepthStencilInfo = depthStencilInfo;
}

void Pipeline::SetPushConstants(VkPushConstantRange* pushConstantRange,
                                int count) {
  PushConstantRange = pushConstantRange;
  PushConstantRangeCount = count;
}

void Pipeline::CreateWithShader(
    char const* name, VkVertexInputBindingDescription bindingDescription,
    VkVertexInputAttributeDescription* attributeDescriptions,
    size_t attributeNum, VkDescriptorSetLayout setLayout, bool enableBlending) {
  ImpLog(LogLevel::Debug, LogChannel::Render,
         "Creating pipeline with shader \"{:s}\"\n", name);

  size_t vertShaderCodeSize;
  std::string vertexShaderPath = fmt::format(FMT_COMPILE("{}/{}{}"), ShaderPath,
                                             name, VertShaderExtension);
  char* vertShaderCode =
      (char*)SDL_LoadFile(vertexShaderPath.c_str(), &vertShaderCodeSize);
  if (!vertShaderCode) {
    ImpLog(LogLevel::Debug, LogChannel::Render,
           "Failed to read shader source file\n");
    Window->Shutdown();
  }

  size_t fragShaderCodeSize;
  std::string fragShaderPath = fmt::format(FMT_COMPILE("{}/{}{}"), ShaderPath,
                                           name, FragShaderExtension);
  char* fragShaderCode =
      (char*)SDL_LoadFile(fragShaderPath.c_str(), &fragShaderCodeSize);
  if (!fragShaderCode) {
    ImpLog(LogLevel::Debug, LogChannel::Render,
           "Failed to read shader source file\n");
    Window->Shutdown();
  }

  VkShaderModule vertShaderModule =
      CreateShaderModule(vertShaderCode, vertShaderCodeSize);
  VkShaderModule fragShaderModule =
      CreateShaderModule(fragShaderCode, fragShaderCodeSize);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
  fragShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                    fragShaderStageInfo};

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(attributeNum);
  vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
  vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.scissorCount = 1;

  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = (VkSampleCountFlagBits)Window->MsaaCount;

  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  if (enableBlending) {
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor =
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstAlphaBlendFactor =
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
  } else {
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
  }

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                               VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();

  VkDescriptorSetLayout texturedSetLayouts[] = {setLayout};

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 1;
  pipelineLayoutInfo.pSetLayouts = texturedSetLayouts;
  pipelineLayoutInfo.pPushConstantRanges = PushConstantRange;
  pipelineLayoutInfo.pushConstantRangeCount = PushConstantRangeCount;

  if (vkCreatePipelineLayout(Device, &pipelineLayoutInfo, nullptr,
                             &PipelineLayout) != VK_SUCCESS) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Failed to create pipeline layout!");
    Window->Shutdown();
  }

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &RasterizerInfo;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;
  pipelineInfo.pDepthStencilState = &DepthStencilInfo;
  pipelineInfo.layout = PipelineLayout;
  pipelineInfo.renderPass = RenderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

  if (vkCreateGraphicsPipelines(Device, VK_NULL_HANDLE, 1, &pipelineInfo,
                                nullptr, &GraphicsPipeline) != VK_SUCCESS) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Failed to create graphics pipeline!");
    Window->Shutdown();
  }

  vkDestroyShaderModule(Device, fragShaderModule, nullptr);
  vkDestroyShaderModule(Device, vertShaderModule, nullptr);
}

VkShaderModule Pipeline::CreateShaderModule(char const* code, size_t codeSize) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = codeSize;
  createInfo.pCode = reinterpret_cast<const uint32_t*>(code);

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(Device, &createInfo, nullptr, &shaderModule) !=
      VK_SUCCESS) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Failed to create shader module!");
    Window->Shutdown();
  }

  return shaderModule;
}

Pipeline::~Pipeline() {
  vkDestroyPipeline(Device, GraphicsPipeline, nullptr);
  vkDestroyPipelineLayout(Device, PipelineLayout, nullptr);
}

}  // namespace Vulkan
}  // namespace Impacto