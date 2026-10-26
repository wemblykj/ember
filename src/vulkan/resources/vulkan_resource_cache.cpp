#include "vulkan_resource_cache.h"

#include <logger.h>

using namespace ember::resources;

namespace ember::vulkan {

VulkanResourceCache::VulkanResourceCache(const ResourceCacheConfig& config, 
	std::shared_ptr<VulkanContext> context,
    std::unique_ptr<VulkanResourceBuilder> resourceBuilder,
    std::unique_ptr<resources::CannedResourceProvider> cannedResourceProvider)
    : config_(config)
	, context_(context)
	, resourceBuilder_(std::move(resourceBuilder))
    , cannedResourceProvider_(std::move(cannedResourceProvider))
{
}

VulkanResourceCache::~VulkanResourceCache() {
    clearAllResources();
}

const ShaderRecord& VulkanResourceCache::resolveShader(ShaderID id) const
{
    if (id == BuiltinShader::Undefined) {
        EMBER_LOG_WARN("Shader ID {} is undefined. Returning default shader.", id);
        return shaders_[BuiltinShader::Undefined].record;
    }
    const auto it = shaders_.find(id);
    if (it == shaders_.end()) {
        EMBER_LOG_WARN("Shader ID {} is not registered. Returning default shader.", id);
        return shaders_[BuiltinShader::Undefined].record;
    }
    return it->second.record;
}
const TechniqueRecord& VulkanResourceCache::resolveTechnique(TechniqueID id) const
{
    if (id == BuiltinTechnique::Undefined) {
        EMBER_LOG_WARN("Technique ID {} is undefined. Returning default technique.", id);
        return techniques_[BuiltinTechnique::Undefined].record;
	}

	const auto it = techniques_.find(id);
    if (it == techniques_.end()) {
        EMBER_LOG_WARN("Technique ID {} is not registered. Returning default technique.", id);
        return techniques_[BuiltinTechnique::Undefined].record;
    }

    return it->second.record;
}

const MaterialRecord& VulkanResourceCache::resolveMaterial(MaterialID id) const
{
    if (id == BuiltinMaterial::Undefined) {
        EMBER_LOG_WARN("Material ID {} is undefined. Returning unassigned material.", id);
        return materials_[unassignedMaterialId_].record;
	}

	const auto it = materials_.find(id);
    if (it == materials_.end()) {
        EMBER_LOG_WARN("Material ID {} is not registered. Returning default material.", id);
        return materials_[unresolvedMaterialId_].record;
    }

    return it->second.record;
}

const GeometryRecord& VulkanResourceCache::resolveGeometry(GeometryID id) const
{
    if (id == BuiltinGeometry::Undefined) {
        EMBER_LOG_WARN("Geometry ID {} is undefined. Returning default geometry.", id);
        return geometry_[BuiltinGeometry::Undefined].record;
    }

	const auto it = geometry_.find(id);
    if (it == geometry_.end()) {
        EMBER_LOG_WARN("Geometry ID {} is not registered. Returning default geometry.", id);
        return geometry_[BuiltinGeometry::Undefined].record;
    }

    return it->second.record;
}

const VertexFormatRecord& VulkanResourceCache::resolveVertexFormat(VertexFormatID id) const {
    if (id == BuiltinVertexFormat::Undefined) {
        EMBER_LOG_WARN("VertexFormat ID {} is undefined. Returning default vertex format.", id);
        return vertexFormats_[BuiltinVertexFormat::Undefined].record;
    }

	const auto it = vertexFormats_.find(id);
    if (it == vertexFormats_.end()) {
        EMBER_LOG_WARN("VertexFormat ID {} is not registered. Returning default vertex format.", id);
        return vertexFormats_[BuiltinVertexFormat::Undefined].record;
    }
	return it->second.record;
}

void VulkanResourceCache::clearAllResources() {
	for (auto& [id, entry] : shaders_) { resourceBuilder_->destroyShader(entry.record); }
    for (auto& [id, entry] : materials_) { resourceBuilder_->destroyMaterial(entry.record); }
    for (auto& [id, entry] : techniques_) { resourceBuilder_->destroyTechnique(entry.record); }
    for (auto& [id, entry] : geometry_) {
        resourceBuilder_->destroyGeometry(entry.record, entry.allocation);
    }
	for (auto& [id, entry] : vertexFormats_) { resourceBuilder_->destroyVertexFormat(entry.record); }

	shaders_.clear();
    materials_.clear();
    techniques_.clear();
    geometry_.clear();
    vertexFormats_.clear();
    groupMembers_.clear();
    groupNames_.clear();
    nextGroupId_ = BuiltinResourceGroup::Custom;
    nextTechniqueId_ = BuiltinTechnique::Custom;
    nextMaterialId_ = BuiltinMaterial::Custom;
    nextGeometryId_ = BuiltinGeometry::Custom;
    nextVertexFormatId_ = BuiltinVertexFormat::Custom;
}

ResourceGroupID VulkanResourceCache::createResourceGroup(const ResourceGroupDesc& desc) {
    ResourceGroupID id = nextGroupId_++;
    groupNames_[id] = desc.name;
    return id;
}

void VulkanResourceCache::releaseResourceGroup(ResourceGroupID group) {
    auto it = groupMembers_.find(group);
    if (it == groupMembers_.end()) return;

	for (auto id : it->second.shaders) { destroyShader(id); shaders_.erase(id); }
    for (auto id : it->second.materials) { destroyMaterial(id);  materials_.erase(id); }
    for (auto id : it->second.techniques) { destroyTechnique(id); techniques_.erase(id); }
    for (auto id : it->second.geometry) { destroyGeometry(id);  geometry_.erase(id); }
    for (auto id : it->second.vertexFormats) { destroyVertexFormat(id); vertexFormats_.erase(id); }

    groupMembers_.erase(it);
    EMBER_LOG_INFO("Released resource group '{}'", groupNames_[group]);
    groupNames_.erase(group);
}

bool VulkanResourceCache::assertShader(ShaderID id)
{
    if (shaders_.contains(id)) {
        return true;
    }
    
    if (id < BuiltinShader::Custom) {
        EMBER_LOG_WARN("Shader ID {} is not registered. Returning default shader.", id);
        
        ShaderRecord shader{};
        if (!resourceBuilder_->buildCannedShader(id, shader)) {
            EMBER_LOG_ERROR("Cannot find native shader '{}'", id);

			ShaderDesc descriptor;
            if (!cannedResourceProvider_->GetShaderDescriptor(id, descriptor)) {
                EMBER_LOG_ERROR("Failed to get descriptor for canned shader ID {}", id);
                return false;
			}

			return resourceBuilder_->buildShader(descriptor, shader);
        }
        
        addShader(BuiltinResourceGroup::Global, id, shader);
    } else {
        EMBER_LOG_WARN("Shader ID {} is not registered. Returning unassigned shader.", id);
    }
    
    return false;
}

bool VulkanResourceCache::assertTechnique(TechniqueID id)
{
    if (techniques_.contains(id)) {
        return true;
    }
    
    if (id < BuiltinTechnique::Custom) {
        EMBER_LOG_WARN("Technique ID {} is not registered. Returning default technique.", id);
        
        TechniqueRecord technique{};
        if (!resourceBuilder_->buildCannedTechnique(id, *this, technique)) {
            EMBER_LOG_ERROR("Cannot find native technique '{}'", id);

            TechniqueDesc descriptor;
            if (!cannedResourceProvider_->GetTechniqueDescriptor(id, descriptor)) {
                EMBER_LOG_ERROR("Failed to get descriptor for canned technique ID {}", id);
                return false;
            }

            return resourceBuilder_->buildTechnique(descriptor, *this, technique);
		}
    	
        addTechnique(BuiltinResourceGroup::Global, id, technique);
    } else {
        EMBER_LOG_WARN("Technique ID {} is not registered. Returning unassigned technique.", id);
	}
    
    return false;
}

bool VulkanResourceCache::assertMaterial(MaterialID id)
{
    return materials_.contains(id);
}

bool VulkanResourceCache::assertGeometry(GeometryID id)
{
    return geometry_.contains(id);
}

bool VulkanResourceCache::assertVertexFormat(VertexFormatID id) {
    if (vertexFormats_.contains(id)) {
        return true;
    }

    if (id < BuiltinVertexFormat::Custom) {
        EMBER_LOG_WARN("VertexFormat ID {} is not registered. Returning default vertex format.", id);

        VertexFormatRecord vertexFormat{};
        if (resourceBuilder_->buildCannedVertexFormat(id, vertexFormat)) {
            addVertexFormat(BuiltinResourceGroup::Global, id, vertexFormat);

            return true;
        }
        
    	EMBER_LOG_ERROR("Failed to compile canned vertex format '{}'", id);
    }
    else {
        EMBER_LOG_WARN("VertexFormat ID {} is not registered. Returning unassigned vertex format.", id);
    }

    return false;
}

ShaderID VulkanResourceCache::registerShader(const ShaderDesc& desc, ResourceGroupID group) {
    ShaderRecord record;
    if (!resourceBuilder_->buildShader(desc, record)) {
        EMBER_LOG_ERROR("Failed to compile shader '{}'", desc.name);
        return BuiltinShader::Undefined; // or an error sentinel
    }
    
    ShaderID id = nextShaderId_++;
    addShader(group, id, record);
    return id;
}

TechniqueID VulkanResourceCache::registerTechnique(const TechniqueDesc& desc, ResourceGroupID group) {
    TechniqueRecord record;
    if (!resourceBuilder_->buildTechnique(desc, *this, record)) {
        EMBER_LOG_ERROR("Failed to compile technique '{}'", desc.name);
        return BuiltinTechnique::Opaque; // or an error sentinel
    }
    
    TechniqueID id = nextTechniqueId_++;

    addTechnique(group, id, record);

    return id;
}

MaterialID VulkanResourceCache::registerMaterial(const MaterialDesc& desc, ResourceGroupID group) {
    if (!assertTechnique(desc.technique)) {
        EMBER_LOG_ERROR("Technique ID {} is not registered. Cannot register material '{}'", desc.technique, desc.name);
        return invalidMaterialId_;
    }

	MaterialRecord record;
    if (!resourceBuilder_->buildMaterial(desc, record)) {
        EMBER_LOG_ERROR("Failed to compile material '{}'", desc.name);
        return invalidMaterialId_;
    }

    MaterialID id = nextMaterialId_++;

    addMaterial(group, id, record);

    return id;
}

GeometryID VulkanResourceCache::registerGeometry(const GeometryDesc& desc, ResourceGroupID group) {
    GeometryRecord record;
	GeometryAllocation allocation;
    if (!resourceBuilder_->buildGeometry(desc, record, allocation)) {
        EMBER_LOG_ERROR("Failed to compile geometry '{}'", desc.name);
        return invalidGeometryId_;
	}

    GeometryID id = nextGeometryId_++;

	addGeometry(group, id, record, allocation);

    return id;
}

VertexFormatID VulkanResourceCache::registerVertexFormat(const VertexFormatDesc& desc, ResourceGroupID group) {
    VertexFormatRecord record;
    if (!resourceBuilder_->buildVertexFormat(desc, record)) {
        EMBER_LOG_ERROR("Failed to compile vertex format '{}'", desc.name);
        return BuiltinVertexFormat::Undefined; // or an error sentinel
	}

    VertexFormatID id = nextVertexFormatId_++;

    addVertexFormat(group, id, record);

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

void VulkanResourceCache::destroyShader(ShaderID id) {
    const auto it = shaders_.find(id);
    if (it != shaders_.end()) {
        resourceBuilder_->destroyShader(it->second.record);
    }
}

void VulkanResourceCache::destroyTechnique(TechniqueID id) {
    const auto it = techniques_.find(id);
    if (it != techniques_.end()) {
        resourceBuilder_->destroyTechnique(it->second.record);
    }
}

void VulkanResourceCache::destroyMaterial(MaterialID id) {
    const auto it = materials_.find(id);
    if (it != materials_.end()) {
        resourceBuilder_->destroyMaterial(it->second.record);
    }
}

void VulkanResourceCache::destroyGeometry(GeometryID id) {
    const auto it = geometry_.find(id);
    if (it != geometry_.end()) {
        resourceBuilder_->destroyGeometry(it->second.record, it->second.allocation);
    }
}

void VulkanResourceCache::destroyVertexFormat(VertexFormatID id)
{
	const auto it = vertexFormats_.find(id);
    if (it != vertexFormats_.end()) {
        resourceBuilder_->destroyVertexFormat(it->second.record);
	}
}

void VulkanResourceCache::addShader(ResourceGroupID group, ShaderID id, const ShaderRecord& record)
{
    ShaderEntry entry {
        { group },
        record
    };

    shaders_[id] = entry;
    groupMembers_[group].shaders.push_back(id);
}

void VulkanResourceCache::addTechnique(ResourceGroupID group, TechniqueID id, const TechniqueRecord& record)
{
    TechniqueEntry entry {
		{ group },
        record
    };

    techniques_[id] = entry;
    groupMembers_[group].techniques.push_back(id);
}

void VulkanResourceCache::addMaterial(ResourceGroupID group, MaterialID id, const MaterialRecord& record)
{
    MaterialEntry entry{
        { group },
        record
    };

    materials_[id] = entry;
    groupMembers_[group].materials.push_back(id);
}

void VulkanResourceCache::addGeometry(ResourceGroupID group, GeometryID id, const GeometryRecord& record,
	const GeometryAllocation& allocation) {
	GeometryEntry entry{
		{ group },
		record,
		allocation
	};

	geometry_[id] = entry;
	groupMembers_[group].geometry.push_back(id);
}

void VulkanResourceCache::addVertexFormat(ResourceGroupID group, VertexFormatID id, const VertexFormatRecord& record) {
	
    VertexFormatEntry entry{
        { group },
        record
	};

	vertexFormats_[id] = entry;
	groupMembers_[group].vertexFormats.push_back(id);
}

}  // namespace ember::vulkan
