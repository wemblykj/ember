#include "default_canned_resource_provider.h"

#include <stdexcept>

#include <logger.h>

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

bool DefaultCannedResourceProvider::GetShaderDescriptor(ShaderID id, ShaderDesc& desc) {
	switch (id)
	{
	case CannedShader::Invalid:
	case CannedShader::Undefined:// Add shader stages for opaque technique
		throw std::runtime_error("Canned shader ID is invalid or undefined. Cannot provide descriptor.");

	case CannedShader::InvalidVertex:
		desc.name = "canned.shader.invalid_vertex";
		desc.type = ShaderType::Vertex;
		desc.entryPoint = "main";
		desc.sourcePath = "shaders/canned/generic_transform.vert";
		break;

	case CannedShader::UndefinedVertex:
		desc.name = "canned.shader.undefined_vertex";
		desc.type = ShaderType::Vertex;
		desc.entryPoint = "main";
		desc.sourcePath = "shaders/canned/generic_transform.vert";
		break;

	case CannedShader::GenericTransform:
		desc.name = "canned.shader.generic_transform";
		desc.type = ShaderType::Vertex;
		desc.entryPoint = "main";
		desc.sourcePath = "shaders/canned/generic_transform.vert";
		break;

	case CannedShader::InvalidFragment:
		desc.name = "canned.shader.invalid_fragment";
		desc.type = ShaderType::Fragment;
		desc.entryPoint = "main";
		desc.sourcePath = "shaders/canned/opaque_pbr.frag";
		break;

	case CannedShader::UndefinedFragment:
		desc.name = "canned.shader.undefined_fragment";
		desc.type = ShaderType::Fragment;
		desc.entryPoint = "main";
		desc.sourcePath = "shaders/canned/opaque_pbr.frag";
		break;

	case CannedShader::OpaquePBR:
		desc.name = "canned.shader.opaque_pbr";
		desc.type = ShaderType::Fragment;
		desc.entryPoint = "main";
		desc.sourcePath = "shaders/canned/opaque_pbr.frag";
		break;

	default:
		return false;
	}

	return true;
}

bool DefaultCannedResourceProvider::GetTechniqueDescriptor(TechniqueID id, TechniqueDesc& desc) {
	switch (id)
	{
		case CannedTechnique::Invalid:
			desc.name = "canned.technique.invalid";
			desc.blendMode = core::BlendMode::Opaque;
			desc.cullMode = core::CullMode::Back;
			desc.depthTest = true;
			desc.depthWrite = true;
			// Add shader stages for opaque technique
			desc.shaderStages.vertex = CannedShader::InvalidVertex;
			desc.shaderStages.fragment = CannedShader::InvalidFragment;
			break;

		case CannedTechnique::Undefined:
			desc.name = "canned.technique.undefined";
			desc.blendMode = core::BlendMode::Opaque;
			desc.cullMode = core::CullMode::Back;
			desc.depthTest = true;
			desc.depthWrite = true;
			// Add shader stages for opaque technique
			desc.shaderStages.vertex = CannedShader::UndefinedVertex;
			desc.shaderStages.fragment = CannedShader::UndefinedFragment;
			break;
		case CannedTechnique::Opaque:
			desc.name = "canned.technique.opaque";
			desc.blendMode = core::BlendMode::Opaque;
			desc.cullMode = core::CullMode::Back;
			desc.depthTest = true;
			desc.depthWrite = true;
			// Add shader stages for opaque technique
			desc.shaderStages.vertex = CannedShader::GenericTransform;
			desc.shaderStages.fragment = CannedShader::OpaquePBR;
			break;

		case CannedTechnique::Transparent:
			desc.name = "canned.technique.transparent";
			desc.blendMode = core::BlendMode::AlphaBlend;
			desc.cullMode = core::CullMode::Back;
			desc.depthTest = true;
			desc.depthWrite = false;
			// Add shader stages for transparent technique
			desc.shaderStages.vertex = CannedShader::GenericTransform;
			desc.shaderStages.fragment = CannedShader::OpaquePBR;
			break;
	
		default:
			return false;
	}

	return true;
}

bool DefaultCannedResourceProvider::GetMaterialDescriptor(MaterialID id, MaterialDesc& desc) {
	switch (id)
	{
		case CannedMaterial::Invalid:
			desc.name = "canned.material.invalid";
			desc.technique = CannedTechnique::Invalid;
			return false;

		case CannedMaterial::Undefined:
			desc.name = "canned.material.undefined";
			desc.technique = CannedTechnique::Undefined;
			break;

		default:
			return false;
	}

	return true;
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
			break;

		case CannedGeometry::Undefined:
			desc.name = "canned.geometry.undefined";
			//desc.vertexData;
			//desc.indexData;
			desc.indexCount = 0;
			desc.indexType = IndexType::Undefined;
			desc.vertexFormat = CannedVertexFormat::Undefined;
			break;

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
			break;
		default:
			return false;
	}

	return true;
}

bool DefaultCannedResourceProvider::GetVertexFormatDescriptor(VertexFormatID id, VertexFormatDesc& desc) {
	switch (id)
	{
		case CannedVertexFormat::Undefined:
			desc.stride = 0;
			desc.attributes.clear();
			break;

		case CannedVertexFormat::PositionUV:
			desc.name = "canned.vertex_format.position_uv";
			desc.stride = sizeof(QuadVertex);
			desc.attributes = {
				{ "position", 0, VertexFormat::R32G32_SFLOAT, 0 },
				{ "uv", sizeof(float) * 2, VertexFormat::R32G32_SFLOAT, 1 }
			};
			break;

		default:
			return false;
	}

	return true;
}

} // namespace ember::resources