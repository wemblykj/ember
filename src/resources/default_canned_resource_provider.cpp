#include "default_canned_resource_provider.h"

namespace {
	struct QuadVertex {
		float position[2];  // VK_FORMAT_R32G32_SFLOAT
		float uv[2];         // VK_FORMAT_R32G32_SFLOAT
	};

	constexpr QuadVertex kUnitQuadVertices[] = {
	{ {-0.5f, -0.5f}, {0.0f, 0.0f} },
	{ { 0.5f, -0.5f}, {1.0f, 0.0f} },
	{ { 0.5f,  0.5f}, {1.0f, 1.0f} },
	{ {-0.5f,  0.5f}, {0.0f, 1.0f} },
	};

	constexpr uint16_t kUnitQuadIndices[] = { 0, 1, 2, 2, 3, 0 };
}
namespace ember::resources {
	
bool DefaultCannedResourceProvider::GetTechniqueDescriptor(TechniqueID id, TechniqueDesc& desc) {
	switch (id)
	{
		case CannedTechnique::Invalid:
		case CannedTechnique::Undefined:
		case CannedTechnique::Opaque:
			desc.name = "canned.technique.opaque";
			desc.blendMode = core::BlendMode::Opaque;
			desc.cullMode = core::CullMode::Back;
			desc.depthTest = true;
			desc.depthWrite = true;
			// Add shader stages for opaque technique
			desc.shaderStages.push_back({ ShaderStage::Vertex, "main", "shaders/canned/opaque.vert" });
			desc.shaderStages.push_back({ ShaderStage::Fragment, "main", "shaders/canned/opaque.frag" });
			return true;

		case CannedTechnique::Transparent:
			desc.name = "canned.technique.transparent";
			desc.blendMode = core::BlendMode::AlphaBlend;
			desc.cullMode = core::CullMode::Back;
			desc.depthTest = true;
			desc.depthWrite = false;
			// Add shader stages for transparent technique
			desc.shaderStages.push_back({ ShaderStage::Vertex, "main", "shaders/canned/transparent.vert" });
			desc.shaderStages.push_back({ ShaderStage::Fragment, "main", "shaders/canned/transparent.frag" });
			return true;
	
		default:
			break;
	}
	return false;
}

bool DefaultCannedResourceProvider::GetMaterialDescriptor(MaterialID id, MaterialDesc& desc) {
	switch (id)
	{
		case CannedMaterial::Invalid:
			desc.name = "canned.material.invalid";
			desc.technique = CannedTechnique::Invalid;
			return true;

		case CannedMaterial::Undefined:
			desc.name = "canned.material.undefined";
			desc.technique = CannedTechnique::Undefined;
			return true;

		default:
			break;
	}
	return false;
}

bool DefaultCannedResourceProvider::GetGeometryDescriptor(GeometryID id, GeometryDesc& desc) {
	switch (id)
	{
		case CannedGeometry::Invalid:
			desc.name = "canned.geometry.invalid";
			//desc.vertexData;
			//desc.indexData;
			desc.indexCount = 0;
			desc.vertexFormat = CannedVertexFormat::Undefined;
			return true;

		case CannedGeometry::Undefined:
			desc.name = "canned.geometry.undefined";
			//desc.vertexData;
			//desc.indexData;
			desc.indexCount = 0;
			desc.indexType = IndexType::Undefined;
			desc.vertexFormat = CannedVertexFormat::Undefined;
			return true;

		case CannedGeometry::UnitQuad:
			// Provide vertex and index data for a unit quad
			// This is just an example; actual data would be filled in here
			desc.name = "canned.geometry.unit_quad";
			desc.vertexData = std::as_bytes(std::span(kUnitQuadVertices));
			desc.vertexCount = static_cast<uint32_t>(std::size(kUnitQuadVertices));
			desc.indexData = std::as_bytes(std::span(kUnitQuadIndices));
			desc.indexCount = static_cast<uint16_t>(std::size(kUnitQuadIndices));
			desc.indexType = IndexType::UInt16;
			desc.vertexFormat = CannedVertexFormat::PositionUV;
			return true;
		default:
			break;
	}
	return false;
}

bool DefaultCannedResourceProvider::GetVertexFormatDescriptor(VertexFormatID id, VertexFormatDesc& desc) {
	switch (id)
	{
		case CannedVertexFormat::Undefined:
			desc.stride = 0;
			desc.attributes.clear();
			return true;
		case CannedVertexFormat::PositionUV:
			desc.name = "canned.vertex_format.position_uv";
			desc.stride = sizeof(QuadVertex);
			desc.attributes = {
				{ "position", 0, VertexFormat::R32G32_SFLOAT, 0 },
				{ "uv", sizeof(float) * 2, VertexFormat::R32G32_SFLOAT, 1 }
			};
			return true;
		default:
			break;
	}

	return false;
}

} // namespace ember::resources