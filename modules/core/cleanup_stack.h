#pragma once

#include <functional>
#include <vector>

namespace ember::core {

/**
 * @brief Accumulates cleanup actions and runs them in reverse order
 * (LIFO) when destroyed, unless dismissAll() has been called.
 *
 * Useful for multi-step resource creation (e.g. Vulkan pipeline
 * construction) where any failure partway through should unwind
 * everything created so far, in the correct reverse order, without
 * requiring manually duplicated cleanup code at each failure point.
 */
class CleanupStack {
public:
    CleanupStack() = default;
    ~CleanupStack() { runAll(); }

    CleanupStack(const CleanupStack&) = delete;
    CleanupStack& operator=(const CleanupStack&) = delete;
    CleanupStack(CleanupStack&&) = default;
    CleanupStack& operator=(CleanupStack&&) = default;

    /// @brief Registers a cleanup action to run (in reverse order) on destruction.
    void push(std::function<void()> action) {
        actions_.push_back(std::move(action));
    }

    /// @brief Cancels all pending cleanup actions — call once ownership of
    /// all guarded resources has been successfully transferred out.
    void dismissAll() {
        actions_.clear();
    }

private:
    void runAll() {
        for (auto it = actions_.rbegin(); it != actions_.rend(); ++it) {
            (*it)();
        }
        actions_.clear();
    }

    std::vector<std::function<void()>> actions_;
};

} // namespace ember::core