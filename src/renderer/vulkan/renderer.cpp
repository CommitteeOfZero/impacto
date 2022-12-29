#include "renderer.h"

#include <SDL_Vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <set>

#include "../../profile/game.h"
#include "../../game.h"
#include "yuvframe.h"

namespace Impacto {
namespace Vulkan {

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
              void* pUserData) {
  Impacto::ImpLog(LL_Debug, LC_Render, "validation layer: %s\n",
                  pCallbackData->pMessage);
  return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

void Renderer::CreateInstance() {
  unsigned int extensionCount = 0;
  SDL_Vulkan_GetInstanceExtensions(Window->SDLWindow, &extensionCount, nullptr);
  std::vector<const char*> extensionNames(extensionCount);
  SDL_Vulkan_GetInstanceExtensions(Window->SDLWindow, &extensionCount,
                                   extensionNames.data());

  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = Profile::WindowName;
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "impacto";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_1;

  VkInstanceCreateInfo instanceCreateInfo = {};
  instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceCreateInfo.pApplicationInfo = &appInfo;
#ifdef __APPLE__
  extensionNames.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
  instanceCreateInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
  if (EnableValidationLayers) {
    extensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
  instanceCreateInfo.enabledExtensionCount = extensionNames.size();
  instanceCreateInfo.ppEnabledExtensionNames = extensionNames.data();

  VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &Instance);

  if (result != VK_SUCCESS) {
    ImpLog(LL_Error, LC_Render, "Failed to create Vulkan instance! %s",
           string_VkResult(result));
    Window->Shutdown();
  }
}

void Renderer::SetupDebug() {
  if (!EnableValidationLayers) return;

  VkDebugUtilsMessengerCreateInfoEXT createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
  createInfo.pUserData = nullptr;
  if (CreateDebugUtilsMessengerEXT(Instance, &createInfo, nullptr,
                                   &DebugMessenger) != VK_SUCCESS) {
    ImpLog(LL_Error, LC_Render, "Failed to create Vulkan debug!");
    Window->Shutdown();
  }
}

void Renderer::PickPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(Instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    ImpLog(LL_Error, LC_Render, "No suitable video adapter found!");
    Window->Shutdown();
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(Instance, &deviceCount, devices.data());

  // TODO: Implement proper device selection
  PhysicalDevice = devices[0];
}

void Renderer::FindQueues() {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueFamilyCount,
                                           nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueFamilyCount,
                                           queueFamilies.data());

  QueueIndices.GraphicsQueueIdx = 0xFFFFFFFF;
  QueueIndices.PresentQueueIdx = 0xFFFFFFFF;
  int i = 0;
  for (const auto& queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      QueueIndices.GraphicsQueueIdx = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, i, Surface,
                                         &presentSupport);
    if (presentSupport) QueueIndices.PresentQueueIdx = i;

    i++;
  }
}

void Renderer::CreateLogicalDevice() {
  FindQueues();

  if (QueueIndices.GraphicsQueueIdx == 0xFFFFFFFF) {
    ImpLog(LL_Error, LC_Render, "No graphics queue found!");
    Window->Shutdown();
  }

  const std::vector<const char*> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME};

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {QueueIndices.GraphicsQueueIdx,
                                            QueueIndices.PresentQueueIdx};

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures{};
  deviceFeatures.samplerAnisotropy = VK_TRUE;

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.enabledExtensionCount = deviceExtensions.size();
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();
  if (EnableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateDevice(PhysicalDevice, &createInfo, nullptr, &Device) !=
      VK_SUCCESS) {
    ImpLog(LL_Error, LC_Render, "Failed to create logical device!");
    Window->Shutdown();
  }

  vkGetDeviceQueue(Device, QueueIndices.GraphicsQueueIdx, 0, &GraphicsQueue);
  vkGetDeviceQueue(Device, QueueIndices.PresentQueueIdx, 0, &PresentQueue);

  CreateAllocator(PhysicalDevice, Device, Instance);

  vkCmdPushDescriptorSetKHR =
      (PFN_vkCmdPushDescriptorSetKHR)vkGetDeviceProcAddr(
          Device, "vkCmdPushDescriptorSetKHR");

  MainUploadContext.Device = Device;
  MainUploadContext.GraphicsQueue = GraphicsQueue;
}

void Renderer::CreateSurface() {
  if (SDL_Vulkan_CreateSurface(Window->SDLWindow, Instance, &Surface) !=
      SDL_TRUE) {
    ImpLog(LL_Error, LC_Render, "Failed to create surface!");
    Window->Shutdown();
  }
}

void Renderer::CreateSwapChain() {
  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface,
                                            &surfaceCapabilities);

  uint32_t surfaceFormatsCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface,
                                       &surfaceFormatsCount, nullptr);
  std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatsCount);
  vkGetPhysicalDeviceSurfaceFormatsKHR(
      PhysicalDevice, Surface, &surfaceFormatsCount, surfaceFormats.data());

  int width, height = 0;
  SDL_Vulkan_GetDrawableSize(Window->SDLWindow, &width, &height);

  SwapChainExtent.width = width;
  SwapChainExtent.height = height;
  SwapChainImageFormat = surfaceFormats[0].format;

  uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
  if (surfaceCapabilities.maxImageCount > 0 &&
      imageCount > surfaceCapabilities.maxImageCount) {
    imageCount = surfaceCapabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = Surface;
  createInfo.minImageCount = surfaceCapabilities.minImageCount;
  createInfo.imageFormat = SwapChainImageFormat;
  createInfo.imageColorSpace = surfaceFormats[0].colorSpace;
  createInfo.imageExtent = SwapChainExtent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  uint32_t queueFamilyIndices[] = {QueueIndices.GraphicsQueueIdx,
                                   QueueIndices.PresentQueueIdx};
  if (QueueIndices.GraphicsQueueIdx != QueueIndices.PresentQueueIdx) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  createInfo.preTransform = surfaceCapabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;  // Vsync, kind of
  createInfo.clipped = VK_TRUE;

  vkCreateSwapchainKHR(Device, &createInfo, nullptr, &SwapChain);

  uint32_t swapChainImageCount = 0;
  vkGetSwapchainImagesKHR(Device, SwapChain, &swapChainImageCount, nullptr);
  SwapChainImages.resize(swapChainImageCount);
  vkGetSwapchainImagesKHR(Device, SwapChain, &swapChainImageCount,
                          SwapChainImages.data());
}

void Renderer::CreateImageViews() {
  SwapChainImageViews.resize(SwapChainImages.size());
  for (size_t i = 0; i < SwapChainImages.size(); i++) {
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = SwapChainImages[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = SwapChainImageFormat;
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    if (vkCreateImageView(Device, &createInfo, nullptr,
                          &SwapChainImageViews[i]) != VK_SUCCESS) {
      ImpLog(LL_Error, LC_Render, "Failed to create image view!");
      Window->Shutdown();
    }
  }
}

void Renderer::CreateRenderPass() {
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = SwapChainImageFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;

  if (vkCreateRenderPass(Device, &renderPassInfo, nullptr, &RenderPass) !=
      VK_SUCCESS) {
    ImpLog(LL_Debug, LC_Render, "Failed to read create render pass!\n");
    Window->Shutdown();
  }
}

void Renderer::CreateFramebuffers() {
  SwapChainFramebuffers.resize(SwapChainImageViews.size());
  for (size_t i = 0; i < SwapChainImageViews.size(); i++) {
    VkImageView attachments[] = {SwapChainImageViews[i]};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = RenderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = SwapChainExtent.width;
    framebufferInfo.height = SwapChainExtent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(Device, &framebufferInfo, nullptr,
                            &SwapChainFramebuffers[i]) != VK_SUCCESS) {
      ImpLog(LL_Debug, LC_Render, "Failed to create framebuffer!\n");
      Window->Shutdown();
    }
  }
}

void Renderer::CreateCommandPool() {
  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = QueueIndices.GraphicsQueueIdx;
  if ((vkCreateCommandPool(Device, &poolInfo, nullptr, &CommandPool) !=
       VK_SUCCESS) ||
      (vkCreateCommandPool(Device, &poolInfo, nullptr,
                           &MainUploadContext.CommandPool) != VK_SUCCESS)) {
    ImpLog(LL_Debug, LC_Render, "Failed to create command pool!\n");
    Window->Shutdown();
  }
}

void Renderer::CreateCommandBuffer() {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = CommandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

  if ((vkAllocateCommandBuffers(Device, &allocInfo, CommandBuffers) !=
       VK_SUCCESS) ||
      (vkAllocateCommandBuffers(Device, &allocInfo,
                                &MainUploadContext.CommandBuffer) !=
       VK_SUCCESS)) {
    ImpLog(LL_Debug, LC_Render, "Failed to allocate command buffers!\n");
    Window->Shutdown();
  }
}

void Renderer::CreateSyncObjects() {
  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(Device, &semaphoreInfo, nullptr,
                          &ImageAvailableSemaphores[i]) != VK_SUCCESS ||
        vkCreateSemaphore(Device, &semaphoreInfo, nullptr,
                          &RenderFinishedSemaphores[i]) != VK_SUCCESS ||
        vkCreateFence(Device, &fenceInfo, nullptr, &InFlightFences[i]) !=
            VK_SUCCESS ||
        vkCreateFence(Device, &fenceInfo, nullptr,
                      &MainUploadContext.UploadFence) != VK_SUCCESS) {
      ImpLog(LL_Debug, LC_Render,
             "Failed to create synchronization objects for a frame!\n");
      Window->Shutdown();
    }
  }
}

void Renderer::CreateVertexBuffer() {
  VertexBufferDevice = CreateBuffer(
      VertexBufferSize,
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VMA_MEMORY_USAGE_GPU_ONLY);
}

void Renderer::CreateIndexBuffer() {
  IndexBufferDevice = CreateBuffer(
      IndexBufferCount * sizeof(uint16_t),
      VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
      VMA_MEMORY_USAGE_GPU_ONLY);
}

void Renderer::PushVertices() {
  auto stagingBuffer =
      CreateBuffer(VertexBufferFill, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VMA_MEMORY_USAGE_CPU_ONLY);

  void* data;
  vmaMapMemory(Allocator, stagingBuffer.Allocation, &data);
  memcpy(data, VertexBuffer, VertexBufferFill);
  vmaUnmapMemory(Allocator, stagingBuffer.Allocation);

  ImmediateSubmit([=](VkCommandBuffer cmd) {
    VkBufferCopy copy;
    copy.dstOffset = VertexBufferOffset;
    copy.srcOffset = 0;
    copy.size = VertexBufferFill;
    vkCmdCopyBuffer(cmd, stagingBuffer.Buffer, VertexBufferDevice.Buffer, 1,
                    &copy);
  });

  vmaDestroyBuffer(Allocator, stagingBuffer.Buffer, stagingBuffer.Allocation);
}

void Renderer::PushIndices() {
  const size_t bufferSize = IndexBufferFill * sizeof(uint16_t);
  auto stagingBuffer = CreateBuffer(
      bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

  void* data;
  vmaMapMemory(Allocator, stagingBuffer.Allocation, &data);
  memcpy(data, IndexBuffer, (size_t)IndexBufferFill * sizeof(uint16_t));
  vmaUnmapMemory(Allocator, stagingBuffer.Allocation);

  ImmediateSubmit([=](VkCommandBuffer cmd) {
    VkBufferCopy copy;
    copy.dstOffset = IndexBufferOffset;
    copy.srcOffset = 0;
    copy.size = bufferSize;
    vkCmdCopyBuffer(cmd, stagingBuffer.Buffer, IndexBufferDevice.Buffer, 1,
                    &copy);
  });

  vmaDestroyBuffer(Allocator, stagingBuffer.Buffer, stagingBuffer.Allocation);
}

void Renderer::CreateDescriptors() {
  std::vector<VkDescriptorPoolSize> sizes = {
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10},
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100}};

  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.flags = 0;
  poolInfo.maxSets = 100;
  poolInfo.poolSizeCount = (uint32_t)sizes.size();
  poolInfo.pPoolSizes = sizes.data();
  vkCreateDescriptorPool(Device, &poolInfo, nullptr, &DescriptorPool);

  VkDescriptorSetLayoutBinding textureBind = {};
  textureBind.binding = 0;
  textureBind.descriptorCount = 1;
  textureBind.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  textureBind.pImmutableSamplers = nullptr;
  textureBind.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutCreateInfo textureBindInfo = {};
  textureBindInfo.bindingCount = 1;
  textureBindInfo.flags =
      VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;
  textureBindInfo.pNext = nullptr;
  textureBindInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  textureBindInfo.pBindings = &textureBind;

  vkCreateDescriptorSetLayout(Device, &textureBindInfo, nullptr,
                              &SingleTextureSetLayout);

  VkDescriptorSetLayoutBinding textureBinds;
  textureBinds.binding = 0;
  textureBinds.descriptorCount = 2;
  textureBinds.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  textureBinds.pImmutableSamplers = nullptr;
  textureBinds.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutCreateInfo texturesBindInfo = {};
  texturesBindInfo.bindingCount = 1;
  texturesBindInfo.flags =
      VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR;
  texturesBindInfo.pNext = nullptr;
  texturesBindInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  texturesBindInfo.pBindings = &textureBinds;

  vkCreateDescriptorSetLayout(Device, &texturesBindInfo, nullptr,
                              &DoubleTextureSetLayout);

  textureBinds.descriptorCount = 3;
  vkCreateDescriptorSetLayout(Device, &texturesBindInfo, nullptr,
                              &TripleTextureSetLayout);
}

void Renderer::InitImpl() {
  if (IsInit) return;
  ImpLog(LL_Info, LC_Render, "Initializing Renderer2D Vulkan system\n");
  IsInit = true;
  NuklearSupported = false;

  VkWindow = new VulkanWindow();
  VkWindow->Init();
  Window = (BaseWindow*)VkWindow;

  CreateInstance();
  SetupDebug();
  PickPhysicalDevice();
  CreateSurface();
  CreateLogicalDevice();

  CreateSwapChain();
  CreateImageViews();
  CreateRenderPass();
  CreateFramebuffers();
  CreateCommandPool();
  CreateVertexBuffer();
  CreateIndexBuffer();
  CreateCommandBuffer();
  CreateSyncObjects();
  CreateDescriptors();

  auto attributeDescriptions = VertexBufferSprites::getAttributeDescriptions();
  auto bindingDescription = VertexBufferSprites::getBindingDescription();

  PipelineSprite = new Pipeline(Device, RenderPass);
  PipelineSprite->CreateWithShader(
      "Sprite", bindingDescription, attributeDescriptions.data(),
      attributeDescriptions.size(), SingleTextureSetLayout);

  PipelineSpriteInverted = new Pipeline(Device, RenderPass);
  PipelineSpriteInverted->CreateWithShader(
      "Sprite_inverted", bindingDescription, attributeDescriptions.data(),
      attributeDescriptions.size(), SingleTextureSetLayout);

  PipelineMaskedSprite = new Pipeline(Device, RenderPass);
  PipelineMaskedSprite->CreateWithShader(
      "MaskedSprite", bindingDescription, attributeDescriptions.data(),
      attributeDescriptions.size(), DoubleTextureSetLayout);

  PipelineYUVFrame = new Pipeline(Device, RenderPass);
  PipelineYUVFrame->CreateWithShader(
      "YUVFrame", bindingDescription, attributeDescriptions.data(),
      attributeDescriptions.size(), TripleTextureSetLayout);

  CurrentPipeline = PipelineSprite;

  if (Profile::GameFeatures & GameFeature::Scene3D) {
    // Scene = new Scene3D(OpenGLWindow, Shaders);
  }

  // Fill index buffer with quads
  int index = 0;
  int vertex = 0;
  while (index + 6 <= IndexBufferCount) {
    // bottom-left -> top-left -> top-right
    IndexBuffer[index] = vertex + 0;
    IndexBuffer[index + 1] = vertex + 1;
    IndexBuffer[index + 2] = vertex + 2;
    // bottom-left -> top-right -> bottom-right
    IndexBuffer[index + 3] = vertex + 0;
    IndexBuffer[index + 4] = vertex + 2;
    IndexBuffer[index + 5] = vertex + 3;
    index += 6;
    vertex += 4;
  }

  // Make 1x1 white pixel for colored rectangles
  Texture rectTexture;
  rectTexture.Load1x1(0xFF, 0xFF, 0xFF, 0xFF);
  SpriteSheet rectSheet(1.0f, 1.0f);
  rectSheet.Texture = rectTexture.Submit();
  RectSprite = Sprite(rectSheet, 0.0f, 0.0f, 1.0f, 1.0f);

  VkSamplerCreateInfo samplerInfo = {};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.pNext = nullptr;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.anisotropyEnable = VK_TRUE;
  samplerInfo.maxAnisotropy = 16;
  vkCreateSampler(Device, &samplerInfo, nullptr, &Sampler);
}

void Renderer::ShutdownImpl() {
  if (!IsInit) return;
  //  if (RectSprite.Sheet.Texture) glDeleteTextures(1,
  //  &RectSprite.Sheet.Texture);
  IsInit = false;

  if (Profile::GameFeatures & GameFeature::Scene3D) {
    Scene->Shutdown();
  }

  vkDestroyCommandPool(Device, CommandPool, nullptr);
  for (auto framebuffer : SwapChainFramebuffers) {
    vkDestroyFramebuffer(Device, framebuffer, nullptr);
  }
  vkDestroyRenderPass(Device, RenderPass, nullptr);
  for (auto imageView : SwapChainImageViews) {
    vkDestroyImageView(Device, imageView, nullptr);
  }
  vkDestroySwapchainKHR(Device, SwapChain, nullptr);
  vkDestroySurfaceKHR(Instance, Surface, nullptr);
  vkDestroyDevice(Device, nullptr);
  if (EnableValidationLayers) {
    DestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, nullptr);
  }
  vkDestroyInstance(Instance, nullptr);
}

void Renderer::NuklearInitImpl() {}

void Renderer::NuklearShutdownImpl() {}

int Renderer::NuklearHandleEventImpl(SDL_Event* ev) { return 0; }

void Renderer::BeginFrameImpl() {
  if (Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->BeginFrame() called before EndFrame()\n");
    return;
  }

  Drawing = true;

  CurrentTexture = 0;

  vkWaitForFences(Device, 1, &InFlightFences[CurrentFrameIndex], VK_TRUE,
                  UINT64_MAX);
  vkResetFences(Device, 1, &InFlightFences[CurrentFrameIndex]);

  vkAcquireNextImageKHR(Device, SwapChain, UINT64_MAX,
                        ImageAvailableSemaphores[CurrentFrameIndex],
                        VK_NULL_HANDLE, &CurrentImageIndex);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

  vkResetCommandBuffer(CommandBuffers[CurrentFrameIndex], 0);
  if (vkBeginCommandBuffer(CommandBuffers[CurrentFrameIndex], &beginInfo) !=
      VK_SUCCESS) {
    ImpLog(LL_Debug, LC_Render, "Failed to begin recording command buffer!\n");
    Window->Shutdown();
  }

  VkRenderPassBeginInfo renderPassInfo{};
  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = RenderPass;
  renderPassInfo.framebuffer = SwapChainFramebuffers[CurrentImageIndex];
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = SwapChainExtent;
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(CommandBuffers[CurrentFrameIndex], &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(CommandBuffers[CurrentFrameIndex],
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    CurrentPipeline->GraphicsPipeline);
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(SwapChainExtent.width);
  viewport.height = static_cast<float>(SwapChainExtent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(CommandBuffers[CurrentFrameIndex], 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = SwapChainExtent;
  vkCmdSetScissor(CommandBuffers[CurrentFrameIndex], 0, 1, &scissor);

  VertexBufferOffset = 0;
  IndexBufferOffset = 0;
}

void Renderer::EndFrameImpl() {
  if (!Drawing) return;
  Flush();

  vkCmdEndRenderPass(CommandBuffers[CurrentFrameIndex]);
  if (vkEndCommandBuffer(CommandBuffers[CurrentFrameIndex]) != VK_SUCCESS) {
    ImpLog(LL_Debug, LC_Render, "Failed to record command buffer!\n");
    Window->Shutdown();
  }

  VkSubmitInfo submitInfo{};
  VkSemaphore waitSemaphores[] = {ImageAvailableSemaphores[CurrentFrameIndex]};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  VkSemaphore signalSemaphores[] = {
      RenderFinishedSemaphores[CurrentFrameIndex]};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &CommandBuffers[CurrentFrameIndex];
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;
  VkResult res = vkQueueSubmit(GraphicsQueue, 1, &submitInfo,
                               InFlightFences[CurrentFrameIndex]);
  if (res != VK_SUCCESS) {
    ImpLog(LL_Debug, LC_Render, "Failed to submit draw command buffer! %s\n",
           string_VkResult(res));
    Window->Shutdown();
  }

  VkPresentInfoKHR presentInfo{};
  VkSwapchainKHR swapChains[] = {SwapChain};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &CurrentImageIndex;
  vkQueuePresentKHR(PresentQueue, &presentInfo);

  CurrentFrameIndex = (CurrentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;

  Drawing = false;
}

uint32_t Renderer::SubmitTextureImpl(TexFmt format, uint8_t* buffer, int width,
                                     int height) {
  VkDeviceSize imageSize = 0;
  VkFormat imageFormat = VK_FORMAT_R8G8B8A8_SRGB;

  switch (format) {
    case TexFmt_RGBA:
      imageSize = width * height * 4;
      imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
      break;
    case TexFmt_RGB:
      imageSize = width * height * 3;
      imageFormat = VK_FORMAT_R8G8B8_UNORM;
      break;
    case TexFmt_U8:
      imageSize = width * height;
      imageFormat = VK_FORMAT_R8_UNORM;
  }

  AllocatedBuffer stagingBuffer = CreateBuffer(
      imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
  void* data;
  vmaMapMemory(Allocator, stagingBuffer.Allocation, &data);
  memcpy(data, buffer, static_cast<size_t>(imageSize));
  vmaUnmapMemory(Allocator, stagingBuffer.Allocation);

  VkExtent3D imageExtent;
  imageExtent.width = static_cast<uint32_t>(width);
  imageExtent.height = static_cast<uint32_t>(height);
  imageExtent.depth = 1;

  VkImageCreateInfo dimgInfo = GetImageCreateInfo(imageFormat, imageExtent);
  AllocatedImage newImage{};
  VmaAllocationCreateInfo dimgAllocinfo = {};
  dimgAllocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  vmaCreateImage(Allocator, &dimgInfo, &dimgAllocinfo, &newImage.Image,
                 &newImage.Allocation, nullptr);

  ImmediateSubmit([&](VkCommandBuffer cmd) {
    VkImageSubresourceRange range;
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel = 0;
    range.levelCount = 1;
    range.baseArrayLayer = 0;
    range.layerCount = 1;

    VkImageMemoryBarrier imageBarrierToTransfer = {};
    imageBarrierToTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageBarrierToTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageBarrierToTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageBarrierToTransfer.image = newImage.Image;
    imageBarrierToTransfer.subresourceRange = range;
    imageBarrierToTransfer.srcAccessMask = 0;
    imageBarrierToTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                         nullptr, 1, &imageBarrierToTransfer);

    VkBufferImageCopy copyRegion = {};
    copyRegion.bufferOffset = 0;
    copyRegion.bufferRowLength = 0;
    copyRegion.bufferImageHeight = 0;
    copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyRegion.imageSubresource.mipLevel = 0;
    copyRegion.imageSubresource.baseArrayLayer = 0;
    copyRegion.imageSubresource.layerCount = 1;
    copyRegion.imageExtent = imageExtent;
    vkCmdCopyBufferToImage(cmd, stagingBuffer.Buffer, newImage.Image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                           &copyRegion);

    VkImageMemoryBarrier imageBarrierToReadable = imageBarrierToTransfer;
    imageBarrierToReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageBarrierToReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageBarrierToReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageBarrierToReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
                         0, nullptr, 1, &imageBarrierToReadable);
  });

  vmaDestroyBuffer(Allocator, stagingBuffer.Buffer, stagingBuffer.Allocation);

  VkTexture outTexture{};
  outTexture.Image = newImage;
  VkImageViewCreateInfo imageInfo = {};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  imageInfo.pNext = nullptr;
  imageInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageInfo.image = outTexture.Image.Image;
  imageInfo.format = imageFormat;
  imageInfo.subresourceRange.baseMipLevel = 0;
  imageInfo.subresourceRange.levelCount = 1;
  imageInfo.subresourceRange.baseArrayLayer = 0;
  imageInfo.subresourceRange.layerCount = 1;
  imageInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  vkCreateImageView(Device, &imageInfo, nullptr, &outTexture.ImageView);

  auto id = NextTextureId;
  NextTextureId += 1;
  Textures[id] = outTexture;

  return id;
}

void Renderer::FreeTextureImpl(uint32_t id) {}

YUVFrame* Renderer::CreateYUVFrameImpl(int width, int height) {
  VideoFrameInternal = new VkYUVFrame();
  VideoFrameInternal->Init(width, height);
  return (YUVFrame*)VideoFrameInternal;
}

void Renderer::DrawRectImpl(RectF const& dest, glm::vec4 color, float angle) {
  DrawSprite(RectSprite, dest, color, angle);
}

void Renderer::DrawSprite3DRotatedImpl(Sprite const& sprite, RectF const& dest,
                                       float depth, glm::vec2 vanishingPoint,
                                       bool stayInScreen, glm::quat rot,
                                       glm::vec4 tint, bool inverted) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->DrawSprite3DRotated() called before BeginFrame()\n");
    return;
  }

  // Are we in sprite mode?
  if (inverted)
    EnsureMode(PipelineSpriteInverted);
  else
    EnsureMode(PipelineSprite);

  // Do we have the texture assigned?
  EnsureTextureBound(sprite.Sheet.Texture);

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  QuadSetUV(sprite.Bounds, sprite.Sheet.DesignWidth, sprite.Sheet.DesignHeight,
            (uintptr_t)&vertices[0].UV, sizeof(VertexBufferSprites));
  QuadSetPosition3DRotated(dest, depth, vanishingPoint, stayInScreen, rot,
                           (uintptr_t)&vertices[0].Position,
                           sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void Renderer::DrawRect3DRotatedImpl(RectF const& dest, float depth,
                                     glm::vec2 vanishingPoint,
                                     bool stayInScreen, glm::quat rot,
                                     glm::vec4 color) {
  DrawSprite3DRotated(RectSprite, dest, depth, vanishingPoint, stayInScreen,
                      rot, color);
}

void Renderer::DrawCharacterMvlImpl(Sprite const& sprite, glm::vec2 topLeft,
                                    int verticesCount, float* mvlVertices,
                                    int indicesCount, uint16_t* mvlIndices,
                                    bool inverted, glm::vec4 tint) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->DrawCharacterMvl() called before BeginFrame()\n");
    return;
  }
}

void Renderer::DrawSpriteImpl(Sprite const& sprite, RectF const& dest,
                              glm::vec4 tint, float angle, bool inverted,
                              bool isScreencap) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->DrawSprite() called before BeginFrame()\n");
    return;
  }

  if (!sprite.Sheet.Texture) return;

  // Are we in sprite mode?
  if (inverted)
    EnsureMode(PipelineSpriteInverted);
  else
    EnsureMode(PipelineSprite);

  // if (isScreencap) {
  //   Flush();
  // }

  // Do we have the texture assigned?
  EnsureTextureBound(sprite.Sheet.Texture);

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  QuadSetUV(sprite.Bounds, sprite.Sheet.DesignWidth, sprite.Sheet.DesignHeight,
            (uintptr_t)&vertices[0].UV, sizeof(VertexBufferSprites));
  QuadSetPosition(dest, angle, (uintptr_t)&vertices[0].Position,
                  sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void Renderer::DrawMaskedSpriteImpl(Sprite const& sprite, Sprite const& mask,
                                    RectF const& dest, glm::vec4 tint,
                                    int alpha, int fadeRange, bool isScreencap,
                                    bool isInverted, bool isSameTexture) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->DrawMaskedSprite() called before BeginFrame()\n");
    return;
  }

  if (alpha < 0) alpha = 0;
  if (alpha > fadeRange + 256) alpha = fadeRange + 256;

  float alphaRange = 256.0f / fadeRange;
  float constAlpha = ((255.0f - alpha) * alphaRange) / 255.0f;

  Flush();
  EnsureMode(PipelineMaskedSprite, false);

  VkDescriptorImageInfo imageBufferInfo[2];
  imageBufferInfo[0].sampler = Sampler;
  imageBufferInfo[0].imageView = Textures[sprite.Sheet.Texture].ImageView;
  imageBufferInfo[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  imageBufferInfo[1].sampler = Sampler;
  imageBufferInfo[1].imageView = Textures[mask.Sheet.Texture].ImageView;
  imageBufferInfo[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  VkWriteDescriptorSet writeDescriptorSet{};
  writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writeDescriptorSet.dstSet = 0;
  writeDescriptorSet.dstBinding = 0;
  writeDescriptorSet.descriptorCount = 2;
  writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  writeDescriptorSet.pImageInfo = imageBufferInfo;

  vkCmdPushDescriptorSetKHR(
      CommandBuffers[CurrentFrameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS,
      PipelineMaskedSprite->PipelineLayout, 0, 1, &writeDescriptorSet);

  SpritePushConstants constants = {};
  constants.Alpha = glm::vec2(alphaRange, constAlpha);
  constants.IsInverted = isInverted;
  constants.IsSameTexture = isSameTexture;
  vkCmdPushConstants(
      CommandBuffers[CurrentFrameIndex], CurrentPipeline->PipelineLayout,
      VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SpritePushConstants), &constants);

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  QuadSetUV(sprite.Bounds, sprite.Sheet.DesignWidth, sprite.Sheet.DesignHeight,
            (uintptr_t)&vertices[0].UV, sizeof(VertexBufferSprites));
  QuadSetUV(sprite.Bounds, sprite.Bounds.Width, sprite.Bounds.Height,
            (uintptr_t)&vertices[0].MaskUV, sizeof(VertexBufferSprites));

  QuadSetPosition(dest, 0.0f, (uintptr_t)&vertices[0].Position,
                  sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void Renderer::DrawCCMessageBoxImpl(Sprite const& sprite, Sprite const& mask,
                                    RectF const& dest, glm::vec4 tint,
                                    int alpha, int fadeRange, float effectCt,
                                    bool isScreencap) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->DrawCCMessageBox() called before BeginFrame()\n");
    return;
  }
}

inline void Renderer::QuadSetUV(RectF const& spriteBounds, float designWidth,
                                float designHeight, uintptr_t uvs, int stride) {
  float topUV = (spriteBounds.Y / designHeight);
  float leftUV = (spriteBounds.X / designWidth);
  float bottomUV = ((spriteBounds.Y + spriteBounds.Height) / designHeight);
  float rightUV = ((spriteBounds.X + spriteBounds.Width) / designWidth);

  // top-left
  *(glm::vec2*)(uvs + 0 * stride) = glm::vec2(leftUV, topUV);
  // top-right
  *(glm::vec2*)(uvs + 1 * stride) = glm::vec2(rightUV, topUV);
  // bottom-right
  *(glm::vec2*)(uvs + 2 * stride) = glm::vec2(rightUV, bottomUV);
  // bottom-left
  *(glm::vec2*)(uvs + 3 * stride) = glm::vec2(leftUV, bottomUV);
}

inline void Renderer::QuadSetPosition(RectF const& transformedQuad, float angle,
                                      uintptr_t positions, int stride) {
  glm::vec2 bottomLeft =
      glm::vec2(transformedQuad.X, transformedQuad.Y + transformedQuad.Height);
  glm::vec2 topLeft = glm::vec2(transformedQuad.X, transformedQuad.Y);
  glm::vec2 topRight =
      glm::vec2(transformedQuad.X + transformedQuad.Width, transformedQuad.Y);
  glm::vec2 bottomRight = glm::vec2(transformedQuad.X + transformedQuad.Width,
                                    transformedQuad.Y + transformedQuad.Height);

  if (angle != 0.0f) {
    glm::vec2 center = transformedQuad.Center();
    glm::mat2 rot = Rotate2D(angle);

    bottomLeft = rot * (bottomLeft - center) + center;
    topLeft = rot * (topLeft - center) + center;
    topRight = rot * (topRight - center) + center;
    bottomRight = rot * (bottomRight - center) + center;
  }

  // top-left
  *(glm::vec2*)(positions + 0 * stride) = DesignToNDCNonFlipped(topLeft);
  // top-right
  *(glm::vec2*)(positions + 1 * stride) = DesignToNDCNonFlipped(topRight);
  // bottom-right
  *(glm::vec2*)(positions + 2 * stride) = DesignToNDCNonFlipped(bottomRight);
  // bottom-left
  *(glm::vec2*)(positions + 3 * stride) = DesignToNDCNonFlipped(bottomLeft);
}

void Renderer::QuadSetPosition3DRotated(RectF const& transformedQuad,
                                        float depth, glm::vec2 vanishingPoint,
                                        bool stayInScreen, glm::quat rot,
                                        uintptr_t positions, int stride) {
  float widthNormalized = transformedQuad.Width / (Profile::DesignWidth * 0.5f);
  float heightNormalized =
      transformedQuad.Height / (Profile::DesignHeight * 0.5f);

  glm::vec4 corners[4]{
      // top-left
      {glm::vec2(-widthNormalized / 2.0f, heightNormalized / 2.0f), 0, 1},
      // top-right
      {glm::vec2(widthNormalized / 2.0f, heightNormalized / 2.0f), 0, 1},
      // bottom-right
      {glm::vec2(widthNormalized / 2.0f, -heightNormalized / 2.0f), 0, 1},
      // bottom-left
      {glm::vec2(-widthNormalized / 2.0f, -heightNormalized / 2.0f), 0, 1}};

  glm::mat4 transform =
      glm::translate(
          glm::mat4(1.0f),
          glm::vec3(DesignToNDCNonFlipped(transformedQuad.Center()), 0)) *
      glm::mat4_cast(rot);

  glm::vec4 vanishingPointNDC(DesignToNDCNonFlipped(vanishingPoint), 0, 0);

  for (int i = 0; i < 4; i++) {
    corners[i] = transform * corners[i];
  }

  if (stayInScreen) {
    float maxZ = 0.0f;
    for (int i = 0; i < 4; i++) {
      if (corners[i].z > maxZ) maxZ = corners[i].z;
    }
    for (int i = 0; i < 4; i++) {
      corners[i].z -= maxZ;
    }
  }

  for (int i = 0; i < 4; i++) {
    // perspective
    corners[i] -= vanishingPointNDC;
    corners[i].x *= (depth / (depth - corners[i].z));
    corners[i].y *= (depth / (depth - corners[i].z));
    corners[i] += vanishingPointNDC;

    *(glm::vec2*)(positions + i * stride) = corners[i];
  }
}

void Renderer::EnsureTextureBound(unsigned int texture) {
  if (CurrentTexture != texture) {
    ImpLogSlow(LL_Trace, LC_Render,
               "Renderer->EnsureTextureBound flushing because texture %d is "
               "not %d\n",
               CurrentTexture, texture);
    Flush();

    VkDescriptorImageInfo imageBufferInfo;
    imageBufferInfo.sampler = Sampler;
    imageBufferInfo.imageView = Textures[texture].ImageView;
    imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = 0;
    writeDescriptorSet.dstBinding = 0;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.descriptorType =
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSet.pImageInfo = &imageBufferInfo;

    vkCmdPushDescriptorSetKHR(
        CommandBuffers[CurrentFrameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS,
        CurrentPipeline->PipelineLayout, 0, 1, &writeDescriptorSet);

    CurrentTexture = texture;
  }
}

void Renderer::EnsureMode(Pipeline* pipeline, bool flush) {
  if (CurrentPipeline != pipeline) {
    ImpLogSlow(LL_Trace, LC_Render, "Renderer2D changing mode\n");
    if (flush) Flush();
    vkCmdBindPipeline(CommandBuffers[CurrentFrameIndex],
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      pipeline->GraphicsPipeline);
    CurrentPipeline = pipeline;
  }
}

void Renderer::Flush() {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->Flush() called before BeginFrame()\n");
    return;
  }

  if (VertexBufferFill > 0 && IndexBufferFill > 0) {
    PushVertices();
    PushIndices();

    // I don't think there's any reason to rebind this every frame, but I'll
    // leave that for later
    VkBuffer vertexBuffers[] = {VertexBufferDevice.Buffer};
    VkDeviceSize offsets[] = {(VkDeviceSize)VertexBufferOffset};
    vkCmdBindVertexBuffers(CommandBuffers[CurrentFrameIndex], 0, 1,
                           vertexBuffers, offsets);
    vkCmdBindIndexBuffer(CommandBuffers[CurrentFrameIndex],
                         IndexBufferDevice.Buffer, IndexBufferOffset,
                         VK_INDEX_TYPE_UINT16);

    vkCmdDrawIndexed(CommandBuffers[CurrentFrameIndex], IndexBufferFill, 1, 0,
                     0, 0);
  }
  IndexBufferOffset += IndexBufferFill * sizeof(uint16_t);
  IndexBufferFill = 0;
  VertexBufferOffset += VertexBufferFill;
  VertexBufferFill = 0;
}

void Renderer::DrawVideoTextureImpl(YUVFrame* tex, RectF const& dest,
                                    glm::vec4 tint, float angle,
                                    bool alphaVideo) {
  if (!Drawing) {
    ImpLog(LL_Error, LC_Render,
           "Renderer->DrawVideoTexture() called before BeginFrame()\n");
    return;
  }

  EnsureMode(PipelineYUVFrame);

  VkDescriptorImageInfo imageBufferInfo[3];
  imageBufferInfo[0].sampler = Sampler;
  imageBufferInfo[0].imageView = VideoFrameInternal->LumaImage.ImageView;
  imageBufferInfo[0].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
  imageBufferInfo[1].sampler = Sampler;
  imageBufferInfo[1].imageView = VideoFrameInternal->CbImage.ImageView;
  imageBufferInfo[1].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
  imageBufferInfo[2].sampler = Sampler;
  imageBufferInfo[2].imageView = VideoFrameInternal->CrImage.ImageView;
  imageBufferInfo[2].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

  VkWriteDescriptorSet writeDescriptorSet{};
  writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writeDescriptorSet.dstSet = 0;
  writeDescriptorSet.dstBinding = 0;
  writeDescriptorSet.descriptorCount = 3;
  writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  writeDescriptorSet.pImageInfo = imageBufferInfo;

  vkCmdPushDescriptorSetKHR(
      CommandBuffers[CurrentFrameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS,
      PipelineYUVFrame->PipelineLayout, 0, 1, &writeDescriptorSet);

  // OK, all good, make quad

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  IndexBufferFill += 6;

  QuadSetUV(RectF(0.0f, 0.0f, tex->Width, tex->Height), tex->Width, tex->Height,
            (uintptr_t)&vertices[0].UV, sizeof(VertexBufferSprites));
  QuadSetPosition(dest, angle, (uintptr_t)&vertices[0].Position,
                  sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void Renderer::CaptureScreencapImpl(Sprite const& sprite) {}

void Renderer::EnableScissorImpl() {}

void Renderer::SetScissorRectImpl(RectF const& rect) {}

void Renderer::DisableScissorImpl() {}

}  // namespace Vulkan
}  // namespace Impacto
