#pragma once

#include <span>

namespace ember::vulkan {

template <typename T>
const T* updateInfoSpanLinking(std::span<T> infos) {
	if (infos.empty()) {
		return nullptr;
	}

	auto& current = infos.front();

	current.pNext = updateInfoSpanLinking(infos.subspan(1));

	return &current;
}

} // namespace ember::vulkan