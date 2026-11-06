#pragma once

#include "resource_types.hpp"
#include "resource_descriptors.hpp"

namespace ember::resources {

class CannedResourceProvider {
public:
	virtual ~CannedResourceProvider() = default;

	virtual bool GetShaderDescriptor(ShaderID id, ShaderDesc& desc) = 0;
	virtual bool GetTechniqueDescriptor(TechniqueID id, TechniqueDesc& desc) = 0;
	virtual bool GetMaterialDescriptor(MaterialID id, MaterialDesc& desc) = 0;
	virtual	bool GetGeometryDescriptor(GeometryID id, GeometryDesc& desc) = 0;
	virtual bool GetVertexFormatDescriptor(VertexFormatID id, VertexFormatDesc& desc) = 0;
};

}