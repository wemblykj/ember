#include "shaderc_vulkan_resource_compiler.h"

namespace ember::graphics::vulkan {

ShadercVulkanResourceCompiler::ShadercVulkanResourceCompiler(std::shared_ptr<VulkanContext> context)
    : context_(std::move(context))
{
	
}

bool ShadercVulkanResourceCompiler::compileTechnique(const TechniqueDesc& desc, TechniqueRecord& outRecord)
{
	throw std::runtime_error("Not implemented");
}

bool ShadercVulkanResourceCompiler::compileMaterial(const MaterialDesc& desc, const TechniqueRecord& technique,
	MaterialRecord& outRecord)
{
	throw std::runtime_error("Not implemented");
}

bool ShadercVulkanResourceCompiler::compileGeometry(const GeometryDesc& desc, GeometryRecord& outRecord,
	GeometryAllocation& outAllocation)
{
	throw std::runtime_error("Not implemented");
}

void ShadercVulkanResourceCompiler::destroyTechnique(TechniqueRecord& record)
{
	throw std::runtime_error("Not implemented");
}

void ShadercVulkanResourceCompiler::destroyMaterial(MaterialRecord& record)
{
	throw std::runtime_error("Not implemented");
}

void ShadercVulkanResourceCompiler::destroyGeometry(GeometryRecord& record, GeometryAllocation& allocation)
{
	throw std::runtime_error("Not implemented");
}

} // namespace ember::graphics::vulkan