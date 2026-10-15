#include "default_canned_resource_provider.h"

namespace ember::resources {
	
bool DefaultCannedResourceProvider::GetTechniqueDescriptor(TechniqueID id, TechniqueDesc& desc) {
	switch (id)
	{
		case BuiltinTechnique::Invalid:
		case BuiltinTechnique::Undefined:
		case BuiltinTechnique::Opaque:
			desc.name = "canned.technique.opaque";
			desc.blendMode = core::BlendMode::Opaque;
			desc.cullMode = core::CullMode::Back;
			desc.depthTest = true;
			desc.depthWrite = true;
			// Add shader stages for opaque technique
			desc.shaderStages.push_back({ ShaderStage::Vertex, "main", "shaders/opaque.vert" });
			desc.shaderStages.push_back({ ShaderStage::Fragment, "main", "shaders/opaque.frag" });
			return true;

		case BuiltinTechnique::Transparent:
			desc.name = "canned.technique.transparent";
			desc.blendMode = core::BlendMode::AlphaBlend;
			desc.cullMode = core::CullMode::Back;
			desc.depthTest = true;
			desc.depthWrite = false;
			// Add shader stages for transparent technique
			desc.shaderStages.push_back({ ShaderStage::Vertex, "main", "shaders/transparent.vert" });
			desc.shaderStages.push_back({ ShaderStage::Fragment, "main", "shaders/transparent.frag" });
			return true;
	
		default:
			break;
	}
	return false;
}

bool DefaultCannedResourceProvider::GetMaterialDescriptor(MaterialID id, MaterialDesc& desc) {
	switch (id)
	{
		case BuiltinMaterial::Invalid:
			desc.name = "canned.material.invalid";
			desc.technique = BuiltinTechnique::Invalid;
			return true;

		case BuiltinMaterial::Undefined:
			desc.name = "canned.material.undefined";
			desc.technique = BuiltinTechnique::Undefined;
			return true;

		default:
			break;
	}
	return false;
}

bool DefaultCannedResourceProvider::GetGeometryDescriptor(GeometryID id, GeometryDesc& desc) {
	switch (id)
	{
		case BuiltinGeometry::Invalid:
			//desc.vertexData;
			//desc.indexData;
			desc.indexCount = 0;
			desc.vertexFormat = BuiltinVertexFormat::Undefined;
			return true;

		case BuiltinGeometry::Undefined:
			//desc.vertexData;
			//desc.indexData;
			desc.indexCount = 0;
			desc.indexType = IndexType::Undefined;
			desc.vertexFormat = BuiltinVertexFormat::Undefined;
			return true;

		case BuiltinGeometry::UnitQuad:
			// Provide vertex and index data for a unit quad
			// This is just an example; actual data would be filled in here
			//desc.vertexData;
			//desc.indexData;
			desc.indexCount = 0;
			desc.indexType = IndexType::UInt16;
			desc.vertexFormat = BuiltinVertexFormat::Custom; // or appropriate format ID
			return true;
		default:
			break;
	}
	return false;
}

} // namespace ember::resources