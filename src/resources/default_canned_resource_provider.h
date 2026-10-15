#pragma once

#include "canned_resource_provider.h"

#include "resource_types.h"

namespace ember::resources {

class DefaultCannedResourceProvider : public CannedResourceProvider {
public:
	DefaultCannedResourceProvider() = default;

	bool GetTechniqueDescriptor(TechniqueID id, TechniqueDesc& desc) override;
	bool GetMaterialDescriptor(MaterialID id, MaterialDesc& desc) override;
	bool GetGeometryDescriptor(GeometryID id, GeometryDesc& desc) override;
	bool GetVertexFormatDescriptor(VertexFormatID id, VertexFormatDesc& desc) override;
};

}
