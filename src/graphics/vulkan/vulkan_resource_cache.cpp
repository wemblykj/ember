#include "vulkan_resource_cache.h"

#include <logger.h>

namespace ember::graphics::vulkan {

VulkanResourceCache::VulkanResourceCache(const ResourceCacheConfig& config, 
	std::shared_ptr<VulkanContext> context,
    std::unique_ptr<VulkanResourceCompiler> compiler)
    : config_(config)
	, context_(context)
	, compiler_(std::move(compiler))
{
}

VulkanResourceCache::~VulkanResourceCache() {
    clearAllResources();
}

TechniqueRecord VulkanResourceCache::ResolveTechnique(TechniqueID id)
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

MaterialRecord VulkanResourceCache::ResolveMaterial(MaterialID id)
{
    if (id == BuiltinMaterial::Undefined) {
        EMBER_LOG_WARN("Material ID {} is undefined. Returning unassigned material.", id);
        return materials_[unassignedMaterialId_].record;
	}

	auto it = materials_.find(id);
    if (it == materials_.end()) {
        EMBER_LOG_WARN("Material ID {} is not registered. Returning default material.", id);
        return materials_[unresolvedMaterialId_].record;
    }

    return it->second.record;
}

GeometryRecord VulkanResourceCache::ResolveGeometry(GeometryID id)
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

void VulkanResourceCache::clearAllResources() {
    for (auto& [id, entry] : materials_) { compiler_->destroyMaterial(entry.record); }
    for (auto& [id, entry] : techniques_) { compiler_->destroyTechnique(entry.record); }
    for (auto& [id, entry] : geometry_) {
        compiler_->destroyGeometry(entry.record, entry.allocation);
    }

    materials_.clear();
    techniques_.clear();
    geometry_.clear();
    groupMembers_.clear();
    groupNames_.clear();
    nextGroupId_ = BuiltinResourceGroup::Custom;
    nextTechniqueId_ = BuiltinTechnique::Custom;
    nextMaterialId_ = BuiltinMaterial::Custom;
    nextGeometryId_ = BuiltinGeometry::Custom;
}

ResourceGroupID VulkanResourceCache::createResourceGroup(const ResourceGroupDesc& desc) {
    ResourceGroupID id = nextGroupId_++;
    groupNames_[id] = desc.debugName;
    return id;
}

void VulkanResourceCache::releaseResourceGroup(ResourceGroupID group) {
    auto it = groupMembers_.find(group);
    if (it == groupMembers_.end()) return;

    for (auto id : it->second.materials) { destroyMaterial(id);  materials_.erase(id); }
    for (auto id : it->second.techniques) { destroyTechnique(id); techniques_.erase(id); }
    for (auto id : it->second.geometry) { destroyGeometry(id);  geometry_.erase(id); }

    groupMembers_.erase(it);
    EMBER_LOG_INFO("Released resource group '{}'", groupNames_[group]);
    groupNames_.erase(group);
}

bool VulkanResourceCache::assertTechnique(TechniqueID id)
{
    return techniques_.contains(id);
}

bool VulkanResourceCache::assertMaterial(MaterialID id)
{
    return materials_.contains(id);
}

bool VulkanResourceCache::assertGeometry(GeometryID id)
{
    return geometry_.contains(id);
}

TechniqueID VulkanResourceCache::registerTechnique(const TechniqueDesc& desc, ResourceGroupID group) {
    TechniqueID id = nextTechniqueId_++;
    TechniqueEntry entry;
    entry.group = group;
    if (!compiler_->compileTechnique(desc, entry.record)) {
        EMBER_LOG_ERROR("Failed to compile technique '{}'", desc.name);
        return BuiltinTechnique::Opaque; // or an error sentinel
    }
    techniques_[id] = std::move(entry);
    groupMembers_[group].techniques.push_back(id);
    return id;
}

MaterialID VulkanResourceCache::registerMaterial(const MaterialDesc& desc, ResourceGroupID group) {
    if (!assertTechnique(desc.technique)) {
        EMBER_LOG_ERROR("Technique ID {} is not registered. Cannot register material '{}'", desc.technique, desc.name);
        return invalidMaterialId_;
    }

	auto technique = ResolveTechnique(desc.technique);

    MaterialEntry entry;
    entry.group = group;
    if (!compiler_->compileMaterial(desc, technique, entry.record)) {
        EMBER_LOG_ERROR("Failed to compile material '{}'", desc.name);
        return invalidMaterialId_;
    }
    MaterialID id = nextMaterialId_++;
    materials_[id] = std::move(entry);
    groupMembers_[group].materials.push_back(id);
    return id;
}

GeometryID VulkanResourceCache::registerGeometry(const GeometryDesc& desc, ResourceGroupID group) {
    GeometryID id = nextGeometryId_++;
	GeometryEntry entry;
	entry.group = group;
    if (!compiler_->compileGeometry(desc, entry.record, entry.allocation)) {
        EMBER_LOG_ERROR("Failed to compile geometry");
        return invalidGeometryId_;
	}
    geometry_[id] = std::move(entry);
    groupMembers_[group].geometry.push_back(id);
    return id;
}

TechniqueID VulkanResourceCache::setInvalidTechniqueId(TechniqueID id) {
    auto previousId = invalidTechniqueId_;
    invalidTechniqueId_ = id;

    return previousId;
}

TechniqueID VulkanResourceCache::setUnassignedTechniqueId(TechniqueID id) {
    auto previousId = unassignedTechniqueId_;
    unassignedTechniqueId_ = id;

    return previousId;
}

TechniqueID VulkanResourceCache::setUnresolvedTechniqueId(TechniqueID id) {
    auto previousId = unresolvedTechniqueId_;
	unresolvedTechniqueId_ = id;

	return previousId;
}

MaterialID VulkanResourceCache::setInvalidMaterialId(MaterialID id) {
    auto previousId = invalidMaterialId_;
    invalidMaterialId_ = id;

    return previousId;
}

MaterialID VulkanResourceCache::setUnassignedMaterialId(MaterialID id) {
    auto previousId = unassignedMaterialId_;
    unassignedMaterialId_ = id;

    return previousId;
}

MaterialID VulkanResourceCache::setUnresolvedMaterialId(MaterialID id) {
    auto previousId = unresolvedMaterialId_;
    unresolvedMaterialId_ = id;

    return previousId;
}

GeometryID VulkanResourceCache::setInvalidGeometryId(GeometryID id) {
    auto previousId = invalidGeometryId_;
    invalidGeometryId_ = id;

    return previousId;
}

MaterialID VulkanResourceCache::bindMaterial(MaterialID id) {
    const auto it = materials_.find(id);
    if (it == materials_.end()) {
        EMBER_LOG_WARN("Material ID {} is not registered. Falling back to unassigned material.", id);
        id = unassignedMaterialId_;
    }
    else {
        // Bind the material's pipeline and descriptor sets here
        // vkCmdBindPipeline, vkCmdBindDescriptorSets, etc.
    }

    return id;
}

void VulkanResourceCache::destroyMaterial(MaterialID id) {
    const auto it = materials_.find(id);
    if (it != materials_.end()) {
        compiler_->destroyMaterial(it->second.record);
    }
}

void VulkanResourceCache::destroyTechnique(TechniqueID id) {
    const auto it = techniques_.find(id);
    if (it != techniques_.end()) {
        compiler_->destroyTechnique(it->second.record);
    }
}

void VulkanResourceCache::destroyGeometry(GeometryID id) {
    const auto it = geometry_.find(id);
    if (it != geometry_.end()) {
        compiler_->destroyGeometry(it->second.record, it->second.allocation);
    }
}

}  // namespace ember::graphics::vulkan
