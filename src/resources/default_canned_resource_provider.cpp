#include "default_canned_resource_provider.h"

namespace ember::resources {
	
bool DefaultCannedResourceProvider::GetTechniqueDescriptor(TechniqueID id, TechniqueDesc& desc) {
	return false;
}

bool DefaultCannedResourceProvider::GetMaterialDescriptor(MaterialID id, MaterialDesc& desc) {
	return false;
}

bool DefaultCannedResourceProvider::GetGeometryDescriptor(GeometryID id, GeometryDesc& desc) {
	return false;
}

} // namespace ember::resources