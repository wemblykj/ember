#pragma once

#include <memory>

namespace ember::graphics {

class SurfaceProvider {
public:
	virtual ~SurfaceProvider() = default;
};

using SurfaceProviderPtr = std::shared_ptr<SurfaceProvider>;

}