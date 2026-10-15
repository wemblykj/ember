#pragma once

#include "resource_types.h"
#include "resource_descriptors.h"

namespace ember::resources {

class CannedResourceProvider {
public:
	virtual ~CannedResourceProvider() = default;

	virtual bool GetTechniqueDescriptor(TechniqueID id, TechniqueDesc& desc) = 0;
	virtual bool GetMaterialDescriptor(MaterialID id, MaterialDesc& desc) = 0;
	virtual	bool GetGeometryDescriptor(GeometryID id, GeometryDesc& desc) = 0;
	virtual bool GetVertexFormatDescriptor(VertexFormatID id, VertexFormatDesc& desc) = 0;
};

}