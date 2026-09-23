#include "vulkan_resource_cache_vma.h"

#include <logger.h>

namespace ember::graphics::vulkan {

VulkanResourceCacheVma::VulkanResourceCacheVma(const ResourceCacheConfig& config, std::shared_ptr<VulkanContextVma> context)
    : config_(config)
	, context_(context){
}

VulkanResourceCacheVma::~VulkanResourceCacheVma() {
    clearAllResources();
}

TechniqueRecord VulkanResourceCacheVma::ResolveTechnique(TechniqueID id)
{
    if (id == BuiltinTechnique::Undefined) {
        EMBER_LOG_WARN("Technique ID {} is undefined. Returning default technique.", id);
        return techniques_[BuiltinTechnique::Undefined].record;
	}

	auto it = techniques_.find(id);
    if (it == techniques_.end()) {
        EMBER_LOG_WARN("Technique ID {} is not registered. Returning default technique.", id);
        return techniques_[BuiltinTechnique::Undefined].record;
    }

    return it->second.record;
}

MaterialRecord VulkanResourceCacheVma::ResolveMaterial(MaterialID id)
{
    if (id == BuiltinMaterial::Undefined) {
        EMBER_LOG_WARN("Material ID {} is undefined. Returning unassigned material.", id);
        return materials_[unassignedMaterial_].record;
	}

	auto it = materials_.find(id);
    if (it == materials_.end()) {
        EMBER_LOG_WARN("Material ID {} is not registered. Returning default material.", id);
        return materials_[unresolvedMaterial_].record;
    }

    return it->second.record;
}

GeometryRecord VulkanResourceCacheVma::ResolveGeometry(GeometryID id)
{
    if (id == BuiltinGeometry::Undefined) {
        EMBER_LOG_WARN("Geometry ID {} is undefined. Returning default geometry.", id);
        return geometry_[BuiltinGeometry::Undefined].record;
    }

	auto it = geometry_.find(id);
    if (it == geometry_.end()) {
        EMBER_LOG_WARN("Geometry ID {} is not registered. Returning default geometry.", id);
        return geometry_[BuiltinGeometry::Undefined].record;
    }

    return it->second.record;
}   

void VulkanResourceCacheVma::clearAllResources() {
    groupMembers_.clear();
    groupNames_.clear();
    nextGroupId_ = BuiltinResourceGroup::Custom;
    nextTechniqueId_ = BuiltinTechnique::Custom;
    nextMaterialId_ = BuiltinMaterial::Custom;
    nextGeometryId_ = BuiltinGeometry::Custom;
}

ResourceGroupID VulkanResourceCacheVma::createResourceGroup(const ResourceGroupDesc& desc) {
    ResourceGroupID id = nextGroupId_++;
    groupNames_[id] = desc.debugName;
    return id;
}

void VulkanResourceCacheVma::releaseResourceGroup(ResourceGroupID group) {
    auto it = groupMembers_.find(group);
    if (it == groupMembers_.end()) return;

    for (auto id : it->second.materials) { destroyMaterial(id);  materials_.erase(id); }
    for (auto id : it->second.techniques) { destroyTechnique(id); techniques_.erase(id); }
    for (auto id : it->second.geometry) { destroyGeometry(id);  geometry_.erase(id); }

    groupMembers_.erase(it);
    EMBER_LOG_INFO("Released resource group '{}'", groupNames_[group]);
    groupNames_.erase(group);
}

bool VulkanResourceCacheVma::assertTechnique(TechniqueID id)
{
    return techniques_.contains(id);
}

bool VulkanResourceCacheVma::assertMaterial(MaterialID id)
{
    return materials_.contains(id);
}

bool VulkanResourceCacheVma::assertGeometry(GeometryID id)
{
    return geometry_.contains(id);
}

TechniqueID VulkanResourceCacheVma::registerTechnique(const TechniqueDesc& desc, ResourceGroupID group) {
    TechniqueID id = nextTechniqueId_++;
    techniques_[id] = { group };
    //techniques_[id] = compilePipeline(desc); // expensive, done once
    groupMembers_[group].techniques.push_back(id);
    return id;
}

MaterialID VulkanResourceCacheVma::registerMaterial(const MaterialDesc& desc, ResourceGroupID group) {
    MaterialID id = nextMaterialId_++;
    materials_[id] = { group };
    //materials_[id] = compileMaterial(desc); // expensive, done once
    groupMembers_[group].materials.push_back(id);
    return id;
}

GeometryID VulkanResourceCacheVma::registerGeometry(const GeometryDesc& desc, ResourceGroupID group) {
    GeometryID id = nextGeometryId_++;
    geometry_[id] = { group };
    //geometry_[id] = compilePipeline(desc); // expensive, done once
    groupMembers_[group].geometry.push_back(id);
    return id;
}

MaterialID VulkanResourceCacheVma::setUnassignedMaterial(MaterialID id)
{
    auto previousId = unassignedMaterial_;
    unassignedMaterial_ = id;

    return previousId;
}

MaterialID VulkanResourceCacheVma::setUnresolvedMaterial(MaterialID id)
{
    auto previousId = unresolvedMaterial_;
    unresolvedMaterial_ = id;

    return previousId;
}

MaterialID VulkanResourceCacheVma::bindMaterial(MaterialID id)
{
    const auto it = materials_.find(id);
    if (it == materials_.end()) {
        EMBER_LOG_WARN("Material ID {} is not registered. Falling back to unassigned material.", id);
        id = unassignedMaterial_;
    }
    else {
        // Bind the material's pipeline and descriptor sets here
        // vkCmdBindPipeline, vkCmdBindDescriptorSets, etc.
    }

    return id;
}

void VulkanResourceCacheVma::destroyMaterial(MaterialID id)
{
    const auto it = materials_.find(id);
    if (it != materials_.end()) {

    }
}

void VulkanResourceCacheVma::destroyTechnique(TechniqueID id)
{
    const auto it = techniques_.find(id);
    if (it != techniques_.end()) {

    }
}

void VulkanResourceCacheVma::destroyGeometry(GeometryID id)
{
    const auto it = geometry_.find(id);
    if (it != geometry_.end()) {

    }
}

}  // namespace ember::graphics::vulkan
