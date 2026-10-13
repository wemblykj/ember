#pragma once

#include <utility>

namespace ember::core {

/**
 * @brief Runs a cleanup function when the guard goes out of scope,
 * unless dismiss() has been called.
 *
 * Useful for ensuring resources created mid-function are cleaned up
 * on any early-return/failure path without manual duplicated cleanup code.
 */
template <typename F>
class ScopeGuard {
public:
    explicit ScopeGuard(F&& fn) : fn_(std::forward<F>(fn)), active_(true) {}
    ~ScopeGuard() { if (active_) fn_(); }

    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    ScopeGuard(ScopeGuard&& other) noexcept
        : fn_(std::move(other.fn_)), active_(other.active_) {
        other.active_ = false;
    }

    /// @brief Cancels the cleanup — call this once the guarded resource
    /// has been successfully handed off (e.g. stored in outRecord).
    void dismiss() { active_ = false; }

private:
    F fn_;
    bool active_;
};

template <typename F>
ScopeGuard<F> makeScopeGuard(F&& fn) {
    return ScopeGuard<F>(std::forward<F>(fn));
}

} // namespace ember::core