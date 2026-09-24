#pragma once

namespace ember::graphics {

class SurfaceProvider {
public:
	virtual ~SurfaceProvider() = default;
};

using SurfaceProviderPtr = std::unique_ptr<SurfaceProvider>;

}