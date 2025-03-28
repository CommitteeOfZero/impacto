#include "renderer.h"

#include <SDL_vulkan.h>
#include <set>

#include "../../profile/game.h"
#include "../../game.h"
#include "3d/scene.h"
#include "yuvframe.h"

#ifndef IMPACTO_DISABLE_IMGUI
#include <imgui_impl_vulkan.h>
#endif

namespace Impacto {
namespace Vulkan {

VkVertexInputBindingDescription Renderer::GetBindingDescription() {
  VkVertexInputBindingDescription bindingDescription{};
  bindingDescription.binding = 0;
  bindingDescription.stride = sizeof(Renderer::VertexBufferSprites);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 4>
Renderer::GetAttributeDescriptions() {
  std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

  attributeDescriptions[0].binding = 0;
  attributeDescriptions[0].location = 0;
  attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  attributeDescriptions[0].offset = offsetof(VertexBufferSprites, Position);

  attributeDescriptions[1].binding = 0;
  attributeDescriptions[1].location = 1;
  attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
  attributeDescriptions[1].offset = offsetof(VertexBufferSprites, UV);

  attributeDescriptions[2].binding = 0;
  attributeDescriptions[2].location = 2;
  attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
  attributeDescriptions[2].offset = offsetof(VertexBufferSprites, Tint);

  attributeDescriptions[3].binding = 0;
  attributeDescriptions[3].location = 3;
  attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
  attributeDescriptions[3].offset = offsetof(VertexBufferSprites, MaskUV);

  return attributeDescriptions;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
              void* pUserData) {
  Impacto::ImpLog(LogLevel::Debug, LogChannel::Render,
                  "validation layer: {:s}\n", pCallbackData->pMessage);
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
  instanceCreateInfo.enabledExtensionCount = (uint32_t)extensionNames.size();
  instanceCreateInfo.ppEnabledExtensionNames = extensionNames.data();

  VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &Instance);

  if (result != VK_SUCCESS) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Failed to create Vulkan instance! 0x{:04x}\n",
           to_underlying(result));
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
  createInfo.pfnUserCallback = DebugCallback;
  createInfo.pUserData = nullptr;
  if (CreateDebugUtilsMessengerEXT(Instance, &createInfo, nullptr,
                                   &DebugMessenger) != VK_SUCCESS) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Failed to create Vulkan debug!");
    Window->Shutdown();
  }
}

void Renderer::PickPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(Instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "No suitable video adapter found!");
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

    if (QueueIndices.GraphicsQueueIdx != 0xFFFFFFFF &&
        QueueIndices.PresentQueueIdx != 0xFFFFFFFF)
      return;

    i++;
  }
}

void Renderer::CreateLogicalDevice() {
  FindQueues();

  if (QueueIndices.GraphicsQueueIdx == 0xFFFFFFFF) {
    ImpLog(LogLevel::Error, LogChannel::Render, "No graphics queue found!");
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
  createInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  if (vkCreateDevice(PhysicalDevice, &createInfo, nullptr, &Device) !=
      VK_SUCCESS) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Failed to create logical device!");
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
    ImpLog(LogLevel::Error, LogChannel::Render, "Failed to create surface!");
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
      ImpLog(LogLevel::Error, LogChannel::Render,
             "Failed to create image view!");
      Window->Shutdown();
    }
  }
}

void Renderer::CreateRenderPass() {
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = SwapChainImageFormat;
  colorAttachment.samples = (VkSampleCountFlagBits)Window->MsaaCount;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment{};
  depthAttachment.format = VK_FORMAT_D32_SFLOAT;
  depthAttachment.samples = (VkSampleCountFlagBits)Window->MsaaCount;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef{};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription colorAttachmentResolve{};
  colorAttachmentResolve.format = SwapChainImageFormat;
  colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentResolveRef{};
  colorAttachmentResolveRef.attachment = 2;
  colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;
  if (Window->MsaaCount > 1)
    subpass.pResolveAttachments = &colorAttachmentResolveRef;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                             VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  VkAttachmentDescription attachments[3] = {colorAttachment, depthAttachment,
                                            colorAttachmentResolve};
  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = Window->MsaaCount > 1 ? 3 : 2;
  renderPassInfo.pAttachments = attachments;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  if (vkCreateRenderPass(Device, &renderPassInfo, nullptr, &RenderPass) !=
      VK_SUCCESS) {
    ImpLog(LogLevel::Debug, LogChannel::Render,
           "Failed to read create render pass!\n");
    Window->Shutdown();
  }
}

void Renderer::CreateFramebuffers() {
  SwapChainFramebuffers.resize(SwapChainImageViews.size());
  for (size_t i = 0; i < SwapChainImageViews.size(); i++) {
    VkImageView attachments[] = {ColorImageView, DepthImageView,
                                 SwapChainImageViews[i]};
    if (Window->MsaaCount == 1) attachments[0] = attachments[2];

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = RenderPass;
    framebufferInfo.attachmentCount = Window->MsaaCount > 1 ? 3 : 2;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = SwapChainExtent.width;
    framebufferInfo.height = SwapChainExtent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(Device, &framebufferInfo, nullptr,
                            &SwapChainFramebuffers[i]) != VK_SUCCESS) {
      ImpLog(LogLevel::Debug, LogChannel::Render,
             "Failed to create framebuffer!\n");
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
    ImpLog(LogLevel::Debug, LogChannel::Render,
           "Failed to create command pool!\n");
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
    ImpLog(LogLevel::Debug, LogChannel::Render,
           "Failed to allocate command buffers!\n");
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
      ImpLog(LogLevel::Debug, LogChannel::Render,
             "Failed to create synchronization objects for a frame!\n");
      Window->Shutdown();
    }
  }
}

void Renderer::CreateVertexBuffer() {
  VertexBufferAlloc =
      CreateBuffer(VertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                   VMA_MEMORY_USAGE_CPU_ONLY);

  void* data;
  vmaMapMemory(Allocator, VertexBufferAlloc.Allocation, &data);
  VertexBuffer = (uint8_t*)data;
}

void Renderer::CreateIndexBuffer() {
  IndexBufferAlloc =
      CreateBuffer(IndexBufferCount, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                   VMA_MEMORY_USAGE_CPU_ONLY);

  void* data;
  vmaMapMemory(Allocator, IndexBufferAlloc.Allocation, &data);
  IndexBuffer = (uint16_t*)data;
}

void Renderer::CreateDescriptors() {
  std::vector<VkDescriptorPoolSize> sizes = {
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10},
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100}};

  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
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

void Renderer::CreateColorAndDepthImage() {
  VkExtent3D imageExtent;
  imageExtent.width = SwapChainExtent.width;
  imageExtent.height = SwapChainExtent.height;
  imageExtent.depth = 1;

  // Color image
  VkImageCreateInfo cimgInfo =
      GetImageCreateInfo(SwapChainImageFormat, imageExtent,
                         static_cast<VkSampleCountFlagBits>(Window->MsaaCount));
  cimgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  cimgInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
                   VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  VmaAllocationCreateInfo cimgAllocinfo = {};
  cimgAllocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  vmaCreateImage(Allocator, &cimgInfo, &cimgAllocinfo, &ColorImage.Image,
                 &ColorImage.Allocation, nullptr);

  VkImageViewCreateInfo colorImageInfo = {};
  colorImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  colorImageInfo.pNext = nullptr;
  colorImageInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  colorImageInfo.image = ColorImage.Image;
  colorImageInfo.format = SwapChainImageFormat;
  colorImageInfo.subresourceRange.baseMipLevel = 0;
  colorImageInfo.subresourceRange.levelCount = 1;
  colorImageInfo.subresourceRange.baseArrayLayer = 0;
  colorImageInfo.subresourceRange.layerCount = 1;
  colorImageInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  vkCreateImageView(Device, &colorImageInfo, nullptr, &ColorImageView);

  // Depth image
  VkImageCreateInfo dimgInfo =
      GetImageCreateInfo(VK_FORMAT_D32_SFLOAT, imageExtent,
                         static_cast<VkSampleCountFlagBits>(Window->MsaaCount));
  dimgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  dimgInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
  VmaAllocationCreateInfo dimgAllocinfo = {};
  dimgAllocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
  vmaCreateImage(Allocator, &dimgInfo, &dimgAllocinfo, &DepthImage.Image,
                 &DepthImage.Allocation, nullptr);

  VkImageViewCreateInfo imageInfo = {};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  imageInfo.pNext = nullptr;
  imageInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageInfo.image = DepthImage.Image;
  imageInfo.format = VK_FORMAT_D32_SFLOAT;
  imageInfo.subresourceRange.baseMipLevel = 0;
  imageInfo.subresourceRange.levelCount = 1;
  imageInfo.subresourceRange.baseArrayLayer = 0;
  imageInfo.subresourceRange.layerCount = 1;
  imageInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  vkCreateImageView(Device, &imageInfo, nullptr, &DepthImageView);
}

void Renderer::CleanupSwapChain() {
  for (size_t i = 0; i < SwapChainFramebuffers.size(); i++) {
    vkDestroyFramebuffer(Device, SwapChainFramebuffers[i], nullptr);
  }

  for (size_t i = 0; i < SwapChainImageViews.size(); i++) {
    vkDestroyImageView(Device, SwapChainImageViews[i], nullptr);
  }

  vkDestroySwapchainKHR(Device, SwapChain, nullptr);
}

void Renderer::RecreateSwapChain() {
  vkDeviceWaitIdle(Device);

  CleanupSwapChain();

  CreateSwapChain();
  CreateImageViews();
  CreateFramebuffers();
}

void Renderer::Init() {
  if (IsInit) return;
  ImpLog(LogLevel::Info, LogChannel::Render,
         "Initializing Renderer2D Vulkan system\n");
  IsInit = true;

  CurrentFrameIndex = 0;
  CurrentImageIndex = 0;

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
  CreateColorAndDepthImage();
  CreateRenderPass();
  CreateFramebuffers();
  CreateCommandPool();
  CreateVertexBuffer();
  CreateIndexBuffer();
  CreateCommandBuffer();
  CreateSyncObjects();
  CreateDescriptors();

  auto attributeDescriptions = GetAttributeDescriptions();
  auto bindingDescription = GetBindingDescription();

  PipelineSprite = new Pipeline(Device, RenderPass);
  PipelineSprite->CreateWithShader(
      "Sprite", bindingDescription, attributeDescriptions.data(),
      attributeDescriptions.size(), SingleTextureSetLayout);

  PipelineSpriteNoBlending = new Pipeline(Device, RenderPass);
  PipelineSpriteNoBlending->CreateWithShader(
      "Sprite", bindingDescription, attributeDescriptions.data(),
      attributeDescriptions.size(), SingleTextureSetLayout, false);

  PipelineSpriteInverted = new Pipeline(Device, RenderPass);
  PipelineSpriteInverted->CreateWithShader(
      "Sprite_inverted", bindingDescription, attributeDescriptions.data(),
      attributeDescriptions.size(), SingleTextureSetLayout);

  VkPushConstantRange maskedSpritePushConstant;
  maskedSpritePushConstant.offset = 0;
  maskedSpritePushConstant.size = sizeof(SpritePushConstants);
  maskedSpritePushConstant.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  PipelineMaskedSprite = new Pipeline(Device, RenderPass);
  PipelineMaskedSprite->SetPushConstants(&maskedSpritePushConstant, 1);
  PipelineMaskedSprite->CreateWithShader(
      "MaskedSprite", bindingDescription, attributeDescriptions.data(),
      attributeDescriptions.size(), DoubleTextureSetLayout);

  VkPushConstantRange maskedSpriteNoAlphaPushConstant;
  maskedSpriteNoAlphaPushConstant.offset = 0;
  maskedSpriteNoAlphaPushConstant.size = sizeof(MaskedNoAlphaPushConstants);
  maskedSpriteNoAlphaPushConstant.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  PipelineMaskedSpriteNoAlpha = new Pipeline(Device, RenderPass);
  PipelineMaskedSpriteNoAlpha->SetPushConstants(
      &maskedSpriteNoAlphaPushConstant, 1);
  PipelineMaskedSpriteNoAlpha->CreateWithShader(
      "MaskedSpriteNoAlpha", bindingDescription, attributeDescriptions.data(),
      attributeDescriptions.size(), DoubleTextureSetLayout);

  VkPushConstantRange yuvFramePushConstants;
  yuvFramePushConstants.offset = 0;
  yuvFramePushConstants.size = sizeof(YUVFramePushConstants);
  yuvFramePushConstants.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  PipelineYUVFrame = new Pipeline(Device, RenderPass);
  PipelineYUVFrame->SetPushConstants(&yuvFramePushConstants, 1);
  PipelineYUVFrame->CreateWithShader(
      "YUVFrame", bindingDescription, attributeDescriptions.data(),
      attributeDescriptions.size(), TripleTextureSetLayout);

  VkPushConstantRange ccBoxPushConstant;
  ccBoxPushConstant.offset = 0;
  ccBoxPushConstant.size = sizeof(CCBoxPushConstants);
  ccBoxPushConstant.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
  PipelineCCMessageBox = new Pipeline(Device, RenderPass);
  PipelineCCMessageBox->SetPushConstants(&ccBoxPushConstant, 1);
  PipelineCCMessageBox->CreateWithShader(
      "CCMessageBoxSprite", bindingDescription, attributeDescriptions.data(),
      attributeDescriptions.size(), DoubleTextureSetLayout);

  PipelineCHLCCMenuBackground = new Pipeline(Device, RenderPass);
  PipelineCHLCCMenuBackground->SetPushConstants(&ccBoxPushConstant, 1);
  PipelineCHLCCMenuBackground->CreateWithShader(
      "CHLCCMenuBackground", bindingDescription, attributeDescriptions.data(),
      attributeDescriptions.size(), DoubleTextureSetLayout);

  CurrentPipeline = PipelineSprite;

  if (Profile::GameFeatures & GameFeature::Scene3D) {
    Scene = new Scene3D(VkWindow, Device, RenderPass, CommandBuffers);
    Scene->Init();
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

#ifndef IMPACTO_DISABLE_IMGUI
  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForVulkan(VkWindow->SDLWindow);

  ImGui_ImplVulkan_InitInfo imguiInfo = {};
  imguiInfo.Instance = Instance;
  imguiInfo.PhysicalDevice = PhysicalDevice;
  imguiInfo.Device = Device;
  imguiInfo.QueueFamily = QueueIndices.PresentQueueIdx;
  imguiInfo.Queue = PresentQueue;
  imguiInfo.DescriptorPool = DescriptorPool;
  imguiInfo.RenderPass = RenderPass;
  imguiInfo.Subpass = 0;
  imguiInfo.MinImageCount = 2;
  imguiInfo.ImageCount = 2;
  imguiInfo.MSAASamples = (VkSampleCountFlagBits)Window->MsaaCount;
  ImGui_ImplVulkan_Init(&imguiInfo);
#endif

  MainRendererInstance = this;
}

void Renderer::Shutdown() {
  if (!IsInit) return;
  IsInit = false;

  if (Profile::GameFeatures & GameFeature::Scene3D) {
    Scene->Shutdown();
  }

  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkWaitForFences(Device, 1, &InFlightFences[i], VK_TRUE, UINT64_MAX);
  }

  // Textures are invalidated when freeing, so store keys first
  std::vector<uint32_t> textureIds;
  for (const auto& texture : Textures) {
    textureIds.push_back(texture.first);
  }
  for (uint32_t id : textureIds) {
    FreeTexture(id);
  }
  for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(Device, RenderFinishedSemaphores[i], nullptr);
    vkDestroySemaphore(Device, ImageAvailableSemaphores[i], nullptr);
    vkDestroyFence(Device, InFlightFences[i], nullptr);
  }
  vkDestroyCommandPool(Device, CommandPool, nullptr);
  vkDestroyRenderPass(Device, RenderPass, nullptr);
  vmaUnmapMemory(Allocator, VertexBufferAlloc.Allocation);
  vmaDestroyBuffer(Allocator, VertexBufferAlloc.Buffer,
                   VertexBufferAlloc.Allocation);
  vmaUnmapMemory(Allocator, IndexBufferAlloc.Allocation);
  vmaDestroyBuffer(Allocator, IndexBufferAlloc.Buffer,
                   IndexBufferAlloc.Allocation);
  vkDestroyImageView(Device, DepthImageView, nullptr);
  vmaDestroyImage(Allocator, DepthImage.Image, DepthImage.Allocation);
  vkDestroyImageView(Device, ColorImageView, nullptr);
  vmaDestroyImage(Allocator, ColorImage.Image, ColorImage.Allocation);
  CleanupSwapChain();
  vkDestroySurfaceKHR(Instance, Surface, nullptr);
  vkDestroyDevice(Device, nullptr);
  if (EnableValidationLayers) {
    DestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, nullptr);
  }
  vkDestroyInstance(Instance, nullptr);
}

#ifndef IMPACTO_DISABLE_IMGUI
void Renderer::ImGuiBeginFrame() {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
}
#endif

void Renderer::BeginFrame() {
  if (Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->BeginFrame() called before EndFrame()\n");
    return;
  }

  Drawing = true;

  CurrentTexture = 0;

  vkWaitForFences(Device, 1, &InFlightFences[CurrentFrameIndex], VK_TRUE,
                  UINT64_MAX);

  vkAcquireNextImageKHR(Device, SwapChain, UINT64_MAX,
                        ImageAvailableSemaphores[CurrentFrameIndex],
                        VK_NULL_HANDLE, &CurrentImageIndex);

  vkResetFences(Device, 1, &InFlightFences[CurrentFrameIndex]);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

  vkResetCommandBuffer(CommandBuffers[CurrentFrameIndex], 0);
  if (vkBeginCommandBuffer(CommandBuffers[CurrentFrameIndex], &beginInfo) !=
      VK_SUCCESS) {
    ImpLog(LogLevel::Debug, LogChannel::Render,
           "Failed to begin recording command buffer!\n");
    Window->Shutdown();
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = RenderPass;
  renderPassInfo.framebuffer = SwapChainFramebuffers[CurrentImageIndex];
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = SwapChainExtent;
  renderPassInfo.clearValueCount = 0;

  vkCmdBeginRenderPass(CommandBuffers[CurrentFrameIndex], &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  VkClearAttachment clearAttachments[2] = {};
  clearAttachments[0].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  clearAttachments[0].clearValue = clearColor;
  clearAttachments[0].colorAttachment = 0;
  VkClearValue clearDepth = {};
  clearDepth.depthStencil = {1.0f, 0};
  clearAttachments[1].aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  clearAttachments[1].clearValue = clearDepth;
  VkClearRect clearRect = {};
  clearRect.layerCount = 1;
  clearRect.rect.offset = {0, 0};
  clearRect.rect.extent = SwapChainExtent;

  vkCmdClearAttachments(CommandBuffers[CurrentFrameIndex], 2, clearAttachments,
                        1, &clearRect);
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
  PreviousScissorRect = RectF(0.0f, 0.0f, (float)SwapChainExtent.width,
                              (float)SwapChainExtent.height);

  VertexBufferOffset = 0;
  IndexBufferOffset = 0;
}

void Renderer::BeginFrame2D() {}

void Renderer::EndFrame() {
  if (!Drawing) return;
  Flush();

#ifndef IMPACTO_DISABLE_IMGUI
  ImGui::Render();
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),
                                  CommandBuffers[CurrentFrameIndex]);
#endif

  vkCmdEndRenderPass(CommandBuffers[CurrentFrameIndex]);
  if (vkEndCommandBuffer(CommandBuffers[CurrentFrameIndex]) != VK_SUCCESS) {
    ImpLog(LogLevel::Debug, LogChannel::Render,
           "Failed to record command buffer!\n");
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
    ImpLog(LogLevel::Debug, LogChannel::Render,
           "Failed to submit draw command buffer! 0x{:04x}\n",
           to_underlying(res));
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

uint32_t Renderer::SubmitTexture(TexFmt format, uint8_t* buffer, int width,
                                 int height) {
  VkDeviceSize imageSize = 0;
  VkFormat imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
  uint8_t* newBuffer;

  switch (format) {
    case TexFmt_RGBA:
      imageSize = width * height * 4;
      imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
      break;
    case TexFmt_RGB: {
      imageSize = width * height * 4;
      imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
      newBuffer = (uint8_t*)malloc(imageSize);
      int num = width * height;
      for (int i = 0; i < num; i++) {
        newBuffer[4 * i] = buffer[3 * i];
        newBuffer[4 * i + 1] = buffer[3 * i + 1];
        newBuffer[4 * i + 2] = buffer[3 * i + 2];
        newBuffer[4 * i + 3] = 0xFF;
      }
    } break;
    case TexFmt_U8:
      imageSize = width * height;
      imageFormat = VK_FORMAT_R8_UNORM;
  }

  AllocatedBuffer stagingBuffer = CreateBuffer(
      imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
  void* data;
  vmaMapMemory(Allocator, stagingBuffer.Allocation, &data);
  if (format == TexFmt_RGB) {
    memcpy(data, newBuffer, static_cast<size_t>(imageSize));
    free(newBuffer);
  } else {
    memcpy(data, buffer, static_cast<size_t>(imageSize));
  }
  vmaUnmapMemory(Allocator, stagingBuffer.Allocation);

  VkExtent3D imageExtent;
  imageExtent.width = static_cast<uint32_t>(width);
  imageExtent.height = static_cast<uint32_t>(height);
  imageExtent.depth = 1;

  VkImageCreateInfo dimgInfo = GetImageCreateInfo(
      imageFormat, imageExtent, VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT);
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

void Renderer::FreeTexture(uint32_t id) {
  // TODO: I need to figure this out... images are getting destroyed but are
  // still used in draw somehow
  // UPDATE: This... seems to just work now? Keep an eye on it I guess

  vmaDestroyImage(Allocator, Textures[id].Image.Image,
                  Textures[id].Image.Allocation);
  vkDestroyImageView(Device, Textures[id].ImageView, nullptr);
  Textures.erase(id);
}

YUVFrame* Renderer::CreateYUVFrame(float width, float height) {
  VideoFrameInternal = new VkYUVFrame();
  VideoFrameInternal->Init(width, height);
  return (YUVFrame*)VideoFrameInternal;
}

void Renderer::DrawRect(RectF const& dest, glm::vec4 color, float angle) {
  BaseRenderer::DrawSprite(RectSprite, dest, color, angle);
}

void Renderer::DrawSprite3DRotated(Sprite const& sprite, RectF const& dest,
                                   float depth, glm::vec2 vanishingPoint,
                                   bool stayInScreen, glm::quat rot,
                                   glm::vec4 tint, bool inverted) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawSprite3DRotated() called before BeginFrame()\n");
    return;
  }

  if (Textures.count(sprite.Sheet.Texture) == 0) return;

  // Are we in sprite mode?
  if (inverted)
    EnsureMode(PipelineSpriteInverted);
  else
    EnsureMode(PipelineSprite);

  // Do we have the texture assigned?
  EnsureTextureBound(sprite.Sheet.Texture);

  // OK, all good, make quad
  MakeQuad();

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferOffset +
                             VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  QuadSetUV(sprite.Bounds, sprite.Sheet.DesignWidth, sprite.Sheet.DesignHeight,
            (uintptr_t)&vertices[0].UV, sizeof(VertexBufferSprites));
  QuadSetPosition3DRotated(dest, depth, vanishingPoint, stayInScreen, rot,
                           (uintptr_t)&vertices[0].Position,
                           sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void Renderer::DrawRect3DRotated(RectF const& dest, float depth,
                                 glm::vec2 vanishingPoint, bool stayInScreen,
                                 glm::quat rot, glm::vec4 color) {
  DrawSprite3DRotated(RectSprite, dest, depth, vanishingPoint, stayInScreen,
                      rot, color);
}

void Renderer::DrawCharacterMvl(Sprite const& sprite, glm::vec2 topLeft,
                                int verticesCount, float* mvlVertices,
                                int indicesCount, uint16_t* mvlIndices,
                                bool inverted, glm::vec4 tint,
                                glm::vec2 scale) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawCharacterMvl() called before BeginFrame()\n");
    return;
  }

  if (Textures.count(sprite.Sheet.Texture) == 0) return;

  // Draw just the character with this since we need to rebind the index buffer
  // anyway...
  Flush();

  // Are we in sprite mode?
  if (inverted)
    EnsureMode(PipelineSpriteInverted);
  else
    EnsureMode(PipelineSprite);

  // Do we have the texture assigned?
  EnsureTextureBound(sprite.Sheet.Texture);

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferOffset +
                             VertexBufferFill);
  VertexBufferFill += verticesCount * sizeof(VertexBufferSprites);

  IndexBufferFill += indicesCount;

  int indexBufferOffset = IndexBufferOffset / sizeof(uint16_t);
  memcpy(IndexBuffer + indexBufferOffset, mvlIndices,
         indicesCount * sizeof(mvlIndices[0]));

  for (int i = 0; i < verticesCount; i += 4) {
    glm::vec2 pos = glm::vec2(mvlVertices[i * 5], mvlVertices[i * 5 + 1]);
    (void)pos;  // Currently unused
    glm::vec2 bottomLeftV =
        glm::vec2(mvlVertices[i * 5], mvlVertices[i * 5 + 1]);
    glm::vec2 topLeftV =
        glm::vec2(mvlVertices[(i + 1) * 5], mvlVertices[(i + 1) * 5 + 1]);
    glm::vec2 topRightV =
        glm::vec2(mvlVertices[(i + 2) * 5], mvlVertices[(i + 2) * 5 + 1]);
    glm::vec2 bottomRightV =
        glm::vec2(mvlVertices[(i + 3) * 5], mvlVertices[(i + 3) * 5 + 1]);

    glm::vec2 bottomLeftUV =
        glm::vec2(mvlVertices[i * 5 + 3], mvlVertices[i * 5 + 4]);
    glm::vec2 topLeftUV =
        glm::vec2(mvlVertices[(i + 1) * 5 + 3], mvlVertices[(i + 1) * 5 + 4]);
    glm::vec2 topRightUV =
        glm::vec2(mvlVertices[(i + 2) * 5 + 3], mvlVertices[(i + 2) * 5 + 4]);
    glm::vec2 bottomRightUV =
        glm::vec2(mvlVertices[(i + 3) * 5 + 3], mvlVertices[(i + 3) * 5 + 4]);

    bottomLeftV *= scale;
    bottomLeftV += topLeft;
    topLeftV *= scale;
    topLeftV += topLeft;
    topRightV *= scale;
    topRightV += topLeft;
    bottomRightV *= scale;
    bottomRightV += topLeft;

    // top-left
    vertices[i].Position = DesignToNDCNonFlipped(topLeftV);
    vertices[i].UV = topLeftUV;
    vertices[i].Tint = tint;
    // bottom-left
    vertices[i + 1].Position = DesignToNDCNonFlipped(bottomLeftV);
    vertices[i + 1].UV = bottomLeftUV;
    vertices[i + 1].Tint = tint;
    // bottom-right
    vertices[i + 2].Position = DesignToNDCNonFlipped(bottomRightV);
    vertices[i + 2].UV = bottomRightUV;
    vertices[i + 2].Tint = tint;
    // top-right
    vertices[i + 3].Position = DesignToNDCNonFlipped(topRightV);
    vertices[i + 3].UV = topRightUV;
    vertices[i + 3].Tint = tint;
  }

  Flush();
}

void Renderer::DrawSprite(Sprite const& sprite, CornersQuad const& dest,
                          std::array<glm::vec4, 4> const& tints, float angle,
                          bool inverted) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawSprite() called before BeginFrame()\n");
    return;
  }

  if (!sprite.Sheet.Texture) return;
  if (Textures.count(sprite.Sheet.Texture) == 0) return;

  // Are we in sprite mode?
  if (inverted)
    EnsureMode(PipelineSpriteInverted);
  else
    EnsureMode(PipelineSprite);

  // Do we have the texture assigned?
  EnsureTextureBound(sprite.Sheet.Texture);

  // OK, all good, make quad
  MakeQuad();

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferOffset +
                             VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  QuadSetUV(sprite.Bounds, sprite.Sheet.DesignWidth, sprite.Sheet.DesignHeight,
            (uintptr_t)&vertices[0].UV, sizeof(VertexBufferSprites));
  QuadSetPosition(dest, angle, (uintptr_t)&vertices[0].Position,
                  sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tints[i];
}

void Renderer::DrawSpriteOffset(Sprite const& sprite, glm::vec2 topLeft,
                                glm::vec2 centerSprite, glm::vec4 tint,
                                glm::vec2 scale, float angle, bool inverted) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawSprite() called before BeginFrame()\n");
    return;
  }

  if (!sprite.Sheet.Texture) return;
  if (Textures.count(sprite.Sheet.Texture) == 0) return;

  // Are we in sprite mode?
  if (inverted)
    EnsureMode(PipelineSpriteInverted);
  else
    EnsureMode(PipelineSprite);

  // Do we have the texture assigned?
  EnsureTextureBound(sprite.Sheet.Texture);

  // OK, all good, make quad
  MakeQuad();

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferOffset +
                             VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  QuadSetUV(sprite.Bounds, sprite.Sheet.DesignWidth, sprite.Sheet.DesignHeight,
            (uintptr_t)&vertices[0].UV, sizeof(VertexBufferSprites));

  QuadSetPositionOffset(sprite.Bounds, topLeft, centerSprite, scale, angle,
                        (uintptr_t)&vertices[0].Position,
                        sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void Renderer::DrawMaskedSprite(Sprite const& sprite, Sprite const& mask,
                                RectF const& dest, glm::vec4 tint, int alpha,
                                int fadeRange, bool isInverted,
                                bool isSameTexture) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawMaskedSprite() called before BeginFrame()\n");
    return;
  }

  if (Textures.count(sprite.Sheet.Texture) == 0 ||
      Textures.count(mask.Sheet.Texture) == 0)
    return;

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
  MakeQuad();

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferOffset +
                             VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  QuadSetUV(sprite.Bounds, sprite.Sheet.DesignWidth, sprite.Sheet.DesignHeight,
            (uintptr_t)&vertices[0].UV, sizeof(VertexBufferSprites));
  QuadSetUV(sprite.Bounds, sprite.Bounds.Width, sprite.Bounds.Height,
            (uintptr_t)&vertices[0].MaskUV, sizeof(VertexBufferSprites));

  QuadSetPosition(dest, 0.0f, (uintptr_t)&vertices[0].Position,
                  sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void Renderer::DrawVertices(SpriteSheet const& sheet,
                            std::span<const glm::vec2> sheetPositions,
                            std::span<const glm::vec2> displayPositions,
                            int width, int height, glm::vec4 tint,
                            bool inverted, bool disableBlend) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawVertices() called before BeginFrame()\n");
    return;
  }

  if (Textures.count(sheet.Texture) == 0) return;
  const int verticesCount = sheetPositions.size();

  if (verticesCount != displayPositions.size()) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawVertices() called with mismatched vertices count\n");
    return;
  }

  Flush();

  // Are we in sprite mode?
  if (inverted)
    EnsureMode(PipelineSpriteInverted);
  else if (disableBlend)
    EnsureMode(PipelineSpriteNoBlending);
  else
    EnsureMode(PipelineSprite);

  // Do we have the texture assigned?
  EnsureTextureBound(sheet.Texture);

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferOffset +
                             VertexBufferFill);

  VertexBufferFill += verticesCount * sizeof(VertexBufferSprites);

  int indexBufferOffset = IndexBufferOffset / sizeof(uint16_t);

  // Generate indices for triangles
  for (int y = 0; y < height - 1; y++) {
    for (int x = 0; x < width - 1; x++) {
      uint16_t v0 = y * width + x;
      uint16_t v1 = y * width + (x + 1);
      uint16_t v2 = (y + 1) * width + x;
      uint16_t v3 = (y + 1) * width + (x + 1);

      // First triangle
      for (auto v : {v1, v0, v2}) {
        IndexBuffer[indexBufferOffset + IndexBufferFill++] = v;
      }
      // Second triangle
      for (auto v : {v3, v1, v2}) {
        IndexBuffer[indexBufferOffset + IndexBufferFill++] = v;
      }
    }
  }
  assert(IndexBufferFill == (width - 1) * (height - 1) * 6);

  for (int i = 0; i < verticesCount; i++) {
    vertices[i].Position = DesignToNDCNonFlipped(displayPositions[i]);
    vertices[i].Tint = tint;
    glm::vec2 uv =
        sheetPositions[i] / glm::vec2(sheet.DesignWidth, sheet.DesignHeight);
    vertices[i].UV = uv;
  }
  Flush();
}

void Renderer::DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                                RectF const& dest, glm::vec4 tint, int alpha,
                                int fadeRange, float effectCt) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawCCMessageBox() called before BeginFrame()\n");
    return;
  }

  if (Textures.count(sprite.Sheet.Texture) == 0 ||
      Textures.count(mask.Sheet.Texture) == 0)
    return;

  if (alpha < 0) alpha = 0;
  if (alpha > fadeRange + 256) alpha = fadeRange + 256;

  float alphaRange = 256.0f / fadeRange;
  float constAlpha = ((255.0f - alpha) * alphaRange) / 255.0f;

  EnsureMode(PipelineCCMessageBox);

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

  CCBoxPushConstants constants = {};
  constants.CCBoxAlpha = glm::vec4(alphaRange, constAlpha, effectCt, 0.0f);
  vkCmdPushConstants(
      CommandBuffers[CurrentFrameIndex], CurrentPipeline->PipelineLayout,
      VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(CCBoxPushConstants), &constants);

  // OK, all good, make quad
  MakeQuad();

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferOffset +
                             VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  QuadSetUV(sprite.Bounds, sprite.Sheet.DesignWidth, sprite.Sheet.DesignHeight,
            (uintptr_t)&vertices[0].UV, sizeof(VertexBufferSprites));
  QuadSetUV(mask.Bounds, mask.Sheet.DesignWidth, mask.Sheet.DesignHeight,
            (uintptr_t)&vertices[0].MaskUV, sizeof(VertexBufferSprites));

  QuadSetPosition(dest, 0.0f, (uintptr_t)&vertices[0].Position,
                  sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void Renderer::DrawMaskedSpriteOverlay(Sprite const& sprite, Sprite const& mask,
                                       RectF const& dest, glm::vec4 tint,
                                       int alpha, int fadeRange,
                                       bool isInverted, float angle,
                                       bool useMaskAlpha) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawMaskedSpriteOverlay() called before BeginFrame()\n");
    return;
  }

  if (sprite.Sheet.IsScreenCap) Flush();

  if (Textures.count(sprite.Sheet.Texture) == 0 ||
      Textures.count(mask.Sheet.Texture) == 0)
    return;

  if (alpha < 0) alpha = 0;
  if (alpha > fadeRange + 256) alpha = fadeRange + 256;

  float alphaRange = 256.0f / fadeRange;
  float constAlpha = ((255.0f - alpha) * alphaRange) / 255.0f;

  VkSamplerCreateInfo samplerInfo = {};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.pNext = nullptr;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  samplerInfo.anisotropyEnable = VK_TRUE;
  samplerInfo.maxAnisotropy = 16;

  VkDescriptorImageInfo imageBufferInfo[2];
  imageBufferInfo[0].sampler = Sampler;
  imageBufferInfo[0].imageView = Textures[sprite.Sheet.Texture].ImageView;
  imageBufferInfo[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  vkCreateSampler(Device, &samplerInfo, nullptr, &imageBufferInfo[1].sampler);
  imageBufferInfo[1].imageView = Textures[mask.Sheet.Texture].ImageView;
  imageBufferInfo[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  VkWriteDescriptorSet writeDescriptorSet{};
  writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writeDescriptorSet.dstSet = 0;
  writeDescriptorSet.dstBinding = 0;
  writeDescriptorSet.descriptorCount = 2;
  writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  writeDescriptorSet.pImageInfo = imageBufferInfo;

  if (useMaskAlpha) {
    EnsureMode(PipelineMaskedSprite, true);
    vkCmdPushDescriptorSetKHR(
        CommandBuffers[CurrentFrameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS,
        CurrentPipeline->PipelineLayout, 0, 1, &writeDescriptorSet);
    SpritePushConstants constants = {};
    constants.Alpha = glm::vec2(alphaRange, constAlpha);
    constants.IsInverted = isInverted;
    constants.IsSameTexture = false;
    vkCmdPushConstants(CommandBuffers[CurrentFrameIndex],
                       CurrentPipeline->PipelineLayout,
                       VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                       sizeof(SpritePushConstants), &constants);

  } else {
    EnsureMode(PipelineMaskedSpriteNoAlpha, true);
    vkCmdPushDescriptorSetKHR(
        CommandBuffers[CurrentFrameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS,
        CurrentPipeline->PipelineLayout, 0, 1, &writeDescriptorSet);
    MaskedNoAlphaPushConstants constants = {};
    constants.Alpha = glm::vec2(alphaRange, constAlpha);
    constants.IsInverted = isInverted;
    vkCmdPushConstants(CommandBuffers[CurrentFrameIndex],
                       CurrentPipeline->PipelineLayout,
                       VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                       sizeof(MaskedNoAlphaPushConstants), &constants);
  }

  // OK, all good, make quad
  MakeQuad();

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferOffset +
                             VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  QuadSetUV(sprite.Bounds, sprite.Sheet.DesignWidth, sprite.Sheet.DesignHeight,
            (uintptr_t)&vertices[0].UV, sizeof(VertexBufferSprites));
  QuadSetUV(mask.Bounds, mask.Sheet.DesignWidth, mask.Sheet.DesignHeight,
            (uintptr_t)&vertices[0].MaskUV, sizeof(VertexBufferSprites), angle);

  QuadSetPosition(dest, 0.0f, (uintptr_t)&vertices[0].Position,
                  sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void Renderer::DrawCHLCCMenuBackground(const Sprite& sprite, const Sprite& mask,
                                       const RectF& dest, float alpha) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->DrawCCMessageBox() called before BeginFrame()\n");
    return;
  }

  if (Textures.count(sprite.Sheet.Texture) == 0 ||
      Textures.count(mask.Sheet.Texture) == 0)
    return;

  if (alpha < 0.0f)
    alpha = 0;
  else if (alpha > 1.0f)
    alpha = 1.0f;

  EnsureMode(PipelineCHLCCMenuBackground);

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

  CCBoxPushConstants constants = {};
  constants.CCBoxAlpha = glm::vec4(alpha, 0.0f, 0.0f, 0.0f);
  vkCmdPushConstants(
      CommandBuffers[CurrentFrameIndex], CurrentPipeline->PipelineLayout,
      VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(CCBoxPushConstants), &constants);

  // OK, all good, make quad
  MakeQuad();

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferOffset +
                             VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  QuadSetUV(sprite.Bounds, sprite.Sheet.DesignWidth, sprite.Sheet.DesignHeight,
            (uintptr_t)&vertices[0].UV, sizeof(VertexBufferSprites));
  QuadSetUV(mask.Bounds, mask.Sheet.DesignWidth, mask.Sheet.DesignHeight,
            (uintptr_t)&vertices[0].MaskUV, sizeof(VertexBufferSprites));

  QuadSetPosition(dest, 0.0f, (uintptr_t)&vertices[0].Position,
                  sizeof(VertexBufferSprites));
}

inline void Renderer::MakeQuad() {
  int indexBufferOffset = IndexBufferOffset / sizeof(uint16_t);
  if (IndexBufferFill + 6 <= IndexBufferCount) {
    // bottom-left -> top-left -> top-right
    IndexBuffer[indexBufferOffset + IndexBufferFill] = VertexBufferCount + 0;
    IndexBuffer[indexBufferOffset + IndexBufferFill + 1] =
        VertexBufferCount + 1;
    IndexBuffer[indexBufferOffset + IndexBufferFill + 2] =
        VertexBufferCount + 2;
    // bottom-left -> top-right -> bottom-right
    IndexBuffer[indexBufferOffset + IndexBufferFill + 3] =
        VertexBufferCount + 0;
    IndexBuffer[indexBufferOffset + IndexBufferFill + 4] =
        VertexBufferCount + 2;
    IndexBuffer[indexBufferOffset + IndexBufferFill + 5] =
        VertexBufferCount + 3;
    IndexBufferFill += 6;
    VertexBufferCount += 4;
  }
}

inline void Renderer::QuadSetUV(RectF const& spriteBounds, float designWidth,
                                float designHeight, uintptr_t uvs, int stride,
                                float angle) {
  float topUV = (spriteBounds.Y / designHeight);
  float leftUV = (spriteBounds.X / designWidth);
  float bottomUV = ((spriteBounds.Y + spriteBounds.Height) / designHeight);
  float rightUV = ((spriteBounds.X + spriteBounds.Width) / designWidth);

  glm::vec2 bottomLeft(leftUV, bottomUV);
  glm::vec2 topLeft(leftUV, topUV);
  glm::vec2 topRight(rightUV, topUV);
  glm::vec2 bottomRight(rightUV, bottomUV);

  if (angle != 0.0f) {
    glm::vec2 center = (bottomLeft + topRight) * 0.5f;  // Center of the quad
    glm::mat2 rot = Rotate2D(angle);

    bottomLeft = rot * (bottomLeft - center) + center;
    topLeft = rot * (topLeft - center) + center;
    topRight = rot * (topRight - center) + center;
    bottomRight = rot * (bottomRight - center) + center;
  }

  // top-left
  *(glm::vec2*)(uvs + 0 * stride) = topLeft;
  // bottom-left
  *(glm::vec2*)(uvs + 1 * stride) = bottomLeft;
  // bottom-right
  *(glm::vec2*)(uvs + 2 * stride) = bottomRight;
  // top-right
  *(glm::vec2*)(uvs + 3 * stride) = topRight;
}

inline void Renderer::QuadSetPositionOffset(RectF const& spriteBounds,
                                            glm::vec2 displayXY,
                                            glm::vec2 displayOffset,
                                            glm::vec2 scale, float angle,
                                            uintptr_t positions, int stride) {
  glm::vec2 bottomLeft =
      glm::vec2(spriteBounds.X, spriteBounds.Y + spriteBounds.Height);
  glm::vec2 topLeft = glm::vec2(spriteBounds.X, spriteBounds.Y);
  glm::vec2 topRight =
      glm::vec2(spriteBounds.X + spriteBounds.Width, spriteBounds.Y);
  glm::vec2 bottomRight = glm::vec2(spriteBounds.X + spriteBounds.Width,
                                    spriteBounds.Y + spriteBounds.Height);

  // reset origin to top left
  bottomLeft -= topLeft;
  topRight -= topLeft;
  bottomRight -= topLeft;
  topLeft -= topLeft;

  // Rotate
  if (angle != 0.0f) {
    glm::mat2 rot = Rotate2D(angle);

    bottomLeft = rot * (bottomLeft - displayOffset) + displayOffset;
    topLeft = rot * (topLeft - displayOffset) + displayOffset;
    topRight = rot * (topRight - displayOffset) + displayOffset;
    bottomRight = rot * (bottomRight - displayOffset) + displayOffset;
  }

  // Scale
  bottomLeft = bottomLeft * scale;
  topLeft = topLeft * scale;
  topRight = topRight * scale;
  bottomRight = bottomRight * scale;

  // Translate to the desired screen position
  glm::vec2 newPos = displayXY - displayOffset * scale + displayOffset;
  bottomLeft += newPos;
  topLeft += newPos;
  topRight += newPos;
  bottomRight += newPos;

  // Store the transformed positions
  *(glm::vec2*)(positions + 0 * stride) = DesignToNDCNonFlipped(topLeft);
  *(glm::vec2*)(positions + 1 * stride) = DesignToNDCNonFlipped(bottomLeft);
  *(glm::vec2*)(positions + 2 * stride) = DesignToNDCNonFlipped(bottomRight);
  *(glm::vec2*)(positions + 3 * stride) = DesignToNDCNonFlipped(topRight);
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
  // bottom-left
  *(glm::vec2*)(positions + 1 * stride) = DesignToNDCNonFlipped(bottomLeft);
  // bottom-right
  *(glm::vec2*)(positions + 2 * stride) = DesignToNDCNonFlipped(bottomRight);
  // top-right
  *(glm::vec2*)(positions + 3 * stride) = DesignToNDCNonFlipped(topRight);
}

inline void Renderer::QuadSetPosition(CornersQuad destQuad, float angle,
                                      uintptr_t positions, int stride) {
  if (angle != 0.0f) {
    glm::vec2 center = (destQuad.BottomLeft + destQuad.TopRight) * 0.5f;
    glm::mat2 rot = Rotate2D(angle);

    destQuad.BottomLeft = rot * (destQuad.BottomLeft - center) + center;
    destQuad.TopLeft = rot * (destQuad.TopLeft - center) + center;
    destQuad.TopRight = rot * (destQuad.TopRight - center) + center;
    destQuad.BottomRight = rot * (destQuad.BottomRight - center) + center;
  }

  // bottom-left
  *(glm::vec2*)(positions + 0 * stride) =
      DesignToNDCNonFlipped(destQuad.TopLeft);
  // top-left
  *(glm::vec2*)(positions + 1 * stride) =
      DesignToNDCNonFlipped(destQuad.BottomLeft);
  // top-right
  *(glm::vec2*)(positions + 2 * stride) =
      DesignToNDCNonFlipped(destQuad.BottomRight);
  // bottom-right
  *(glm::vec2*)(positions + 3 * stride) =
      DesignToNDCNonFlipped(destQuad.TopRight);
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
    ImpLogSlow(LogLevel::Trace, LogChannel::Render,
               "Renderer->EnsureTextureBound flushing because texture {:d} is "
               "not {:d}\n",
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
    ImpLogSlow(LogLevel::Trace, LogChannel::Render,
               "Renderer2D changing mode\n");
    if (flush) Flush();
    vkCmdBindPipeline(CommandBuffers[CurrentFrameIndex],
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      pipeline->GraphicsPipeline);
    CurrentPipeline = pipeline;
  }
}

void Renderer::Flush() {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Renderer->Flush() called before BeginFrame()\n");
    return;
  }

  if (VertexBufferFill > 0 && IndexBufferFill > 0) {
    VkBuffer vertexBuffers[] = {VertexBufferAlloc.Buffer};
    VkDeviceSize offsets[] = {(VkDeviceSize)VertexBufferOffset};
    vkCmdBindVertexBuffers(CommandBuffers[CurrentFrameIndex], 0, 1,
                           vertexBuffers, offsets);
    vkCmdBindIndexBuffer(CommandBuffers[CurrentFrameIndex],
                         IndexBufferAlloc.Buffer, IndexBufferOffset,
                         VK_INDEX_TYPE_UINT16);

    vkCmdDrawIndexed(CommandBuffers[CurrentFrameIndex], IndexBufferFill, 1, 0,
                     0, 0);
  }
  IndexBufferOffset += IndexBufferFill * sizeof(uint16_t);
  IndexBufferFill = 0;
  VertexBufferOffset += VertexBufferFill;
  VertexBufferFill = 0;
  VertexBufferCount = 0;
  CurrentTexture = 0;
}

void Renderer::DrawVideoTexture(YUVFrame* tex, RectF const& dest,
                                glm::vec4 tint, float angle, bool alphaVideo) {
  if (!Drawing) {
    ImpLog(LogLevel::Error, LogChannel::Render,
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

  YUVFramePushConstants constants = {};
  constants.IsAlpha = alphaVideo;
  vkCmdPushConstants(CommandBuffers[CurrentFrameIndex],
                     CurrentPipeline->PipelineLayout,
                     VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                     sizeof(YUVFramePushConstants), &constants);

  // OK, all good, make quad
  MakeQuad();

  VertexBufferSprites* vertices =
      (VertexBufferSprites*)(VertexBuffer + VertexBufferOffset +
                             VertexBufferFill);
  VertexBufferFill += 4 * sizeof(VertexBufferSprites);

  QuadSetUV(RectF(0.0f, 0.0f, tex->Width, tex->Height), tex->Width, tex->Height,
            (uintptr_t)&vertices[0].UV, sizeof(VertexBufferSprites));
  QuadSetPosition(dest, angle, (uintptr_t)&vertices[0].Position,
                  sizeof(VertexBufferSprites));

  for (int i = 0; i < 4; i++) vertices[i].Tint = tint;
}

void Renderer::CaptureScreencap(Sprite& sprite) {
  if (Textures.count(sprite.Sheet.Texture) == 0) return;
  sprite.Sheet.IsScreenCap = true;
  sprite.Sheet.DesignWidth = Window->WindowWidth;
  sprite.Sheet.DesignHeight = Window->WindowHeight;

  // Here we go...
  Flush();
  vkCmdEndRenderPass(CommandBuffers[CurrentFrameIndex]);

  // Capture here
  VkImageSubresourceRange range = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

  VkImageMemoryBarrier imageBarrierToTransfer = {};
  imageBarrierToTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  imageBarrierToTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageBarrierToTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  imageBarrierToTransfer.image = Textures[sprite.Sheet.Texture].Image.Image;
  imageBarrierToTransfer.subresourceRange = range;
  imageBarrierToTransfer.srcAccessMask = 0;
  imageBarrierToTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  vkCmdPipelineBarrier(CommandBuffers[CurrentFrameIndex],
                       VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                       nullptr, 1, &imageBarrierToTransfer);

  imageBarrierToTransfer.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  imageBarrierToTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  imageBarrierToTransfer.image = SwapChainImages[CurrentFrameIndex];
  imageBarrierToTransfer.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  imageBarrierToTransfer.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
  vkCmdPipelineBarrier(CommandBuffers[CurrentFrameIndex],
                       VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                       nullptr, 1, &imageBarrierToTransfer);

  VkOffset3D blitSize;
  blitSize.x = SwapChainExtent.width;
  blitSize.y = SwapChainExtent.height;
  blitSize.z = 1;
  VkImageBlit imageBlitRegion{};
  imageBlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  imageBlitRegion.srcSubresource.layerCount = 1;
  imageBlitRegion.srcOffsets[1] = blitSize;
  imageBlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  imageBlitRegion.dstSubresource.layerCount = 1;
  imageBlitRegion.dstOffsets[1] = blitSize;

  vkCmdBlitImage(CommandBuffers[CurrentFrameIndex],
                 SwapChainImages[CurrentFrameIndex],
                 VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                 Textures[sprite.Sheet.Texture].Image.Image,
                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageBlitRegion,
                 VK_FILTER_LINEAR);

  imageBarrierToTransfer.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  imageBarrierToTransfer.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  imageBarrierToTransfer.image = Textures[sprite.Sheet.Texture].Image.Image;
  imageBarrierToTransfer.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  imageBarrierToTransfer.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  vkCmdPipelineBarrier(CommandBuffers[CurrentFrameIndex],
                       VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                       nullptr, 1, &imageBarrierToTransfer);

  imageBarrierToTransfer.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  imageBarrierToTransfer.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  imageBarrierToTransfer.image = SwapChainImages[CurrentFrameIndex];
  imageBarrierToTransfer.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
  imageBarrierToTransfer.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  vkCmdPipelineBarrier(CommandBuffers[CurrentFrameIndex],
                       VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                       nullptr, 1, &imageBarrierToTransfer);

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = RenderPass;
  renderPassInfo.framebuffer = SwapChainFramebuffers[CurrentImageIndex];
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = SwapChainExtent;
  renderPassInfo.clearValueCount = 0;

  vkCmdBeginRenderPass(CommandBuffers[CurrentFrameIndex], &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);
}

int Renderer::GetSpriteSheetImage(SpriteSheet const& sheet,
                                  std::span<uint8_t> outBuffer) {
  const int bufferSize = sheet.DesignWidth * sheet.DesignHeight * 4;
  assert(outBuffer.size() >= bufferSize);

  // Create a staging buffer to copy the image data to
  AllocatedBuffer stagingBuffer = CreateBuffer(
      bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

  // Transition the image layout to VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
  VkImageSubresourceRange subresourceRange = {};
  subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subresourceRange.baseMipLevel = 0;
  subresourceRange.levelCount = 1;
  subresourceRange.baseArrayLayer = 0;
  subresourceRange.layerCount = 1;

  ImmediateSubmit([&](VkCommandBuffer cmd) {
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = Textures[sheet.Texture].Image.Image;
    barrier.subresourceRange = subresourceRange;
    barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                         nullptr, 1, &barrier);

    // Copy the image to the staging buffer
    VkBufferImageCopy copyRegion = {};
    copyRegion.bufferOffset = 0;
    copyRegion.bufferRowLength = 0;
    copyRegion.bufferImageHeight = 0;
    copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyRegion.imageSubresource.mipLevel = 0;
    copyRegion.imageSubresource.baseArrayLayer = 0;
    copyRegion.imageSubresource.layerCount = 1;
    copyRegion.imageOffset = {0, 0, 0};
    copyRegion.imageExtent = {static_cast<uint32_t>(sheet.DesignWidth),
                              static_cast<uint32_t>(sheet.DesignHeight), 1};

    vkCmdCopyImageToBuffer(cmd, Textures[sheet.Texture].Image.Image,
                           VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           stagingBuffer.Buffer, 1, &copyRegion);

    // Transition the image layout back to
    // VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
                         0, nullptr, 1, &barrier);
  });

  // Map the staging buffer and copy the data to the output buffer
  void* data;
  vmaMapMemory(Allocator, stagingBuffer.Allocation, &data);
  memcpy(outBuffer.data(), data, bufferSize);
  vmaUnmapMemory(Allocator, stagingBuffer.Allocation);

  // Clean up the staging buffer
  vmaDestroyBuffer(Allocator, stagingBuffer.Buffer, stagingBuffer.Allocation);
  return bufferSize;
}

void Renderer::EnableScissor() {}

void Renderer::SetScissorRect(RectF const& rect) {
  if (rect.X != PreviousScissorRect.X && rect.Y != PreviousScissorRect.Y &&
      rect.Width != PreviousScissorRect.Width &&
      rect.Height != PreviousScissorRect.Height) {
    float scale = fmin((float)Window->WindowWidth / Profile::DesignWidth,
                       (float)Window->WindowHeight / Profile::DesignHeight);
    float rectX = rect.X * scale;
    float rectY = rect.Y * scale;
    float rectWidth = rect.Width * scale;
    float rectHeight = rect.Height * scale;
    VkExtent2D scissorExtent;
    scissorExtent.width = (int)(rectWidth);
    scissorExtent.height = (int)(rectHeight);
    VkRect2D scissor{};
    scissor.offset = {(int)rectX, (int)rectY};
    scissor.extent = scissorExtent;
    Flush();
    vkCmdSetScissor(CommandBuffers[CurrentFrameIndex], 0, 1, &scissor);
    PreviousScissorRect = rect;
  }
}

void Renderer::DisableScissor() {
  if (PreviousScissorRect.X != 0.0f && PreviousScissorRect.Y != 0.0f &&
      PreviousScissorRect.Width != SwapChainExtent.width &&
      PreviousScissorRect.Height != SwapChainExtent.height) {
    Flush();
    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = SwapChainExtent;
    vkCmdSetScissor(CommandBuffers[CurrentFrameIndex], 0, 1, &scissor);
    PreviousScissorRect = RectF(0.0f, 0.0f, (float)SwapChainExtent.width,
                                (float)SwapChainExtent.height);
  }
}

}  // namespace Vulkan
}  // namespace Impacto
