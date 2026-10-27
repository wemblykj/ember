#pragma once

#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <cstddef>

namespace ember::core {

/**
 * @brief A simple Action class that allows adding, removing, and invoking handlers (callbacks) in the style of C# Action.
 *
 * Handlers can be added with a unique ID, removed by that ID, and invoked with arguments.
 *
 * @tparam Args The types of arguments that the handlers will accept.
 * 
 * @example
 * @code
 * // No parameters (like Action in C#)
 * Action<> noArgs;
 * noArgs += [] { std::cout << "No-arg handler\n"; };
 * noArgs.invoke();
 *
 * // One parameter (like Action<int>)
 * Action<int> oneArg;
 * auto id1 = oneArg.add([](int x) { std::cout << "Handler with int: " << x << "\n"; });
 * if (oneArg) {
 *     oneArg.invoke(42);
 * }
 * @endcode
 */
template<typename... Args>
class Action {
public:
    // Public alias for the callable type
    using Callback = std::function<void(Args...)>;
    struct OptionalCallback {
        OptionalCallback() {}
		explicit OptionalCallback(const Callback& cb) : value(cb) {}

        std::optional<Callback> value;

		void invoke(Args... args) const {
			if (value) {
				value.value()(args...);
			}
		}
    }; 

    // Add a callback and return its ID
    std::size_t add(const Callback& cb) {
        callbacks.push_back({ ++lastId, cb });
        return lastId;
    }

    std::size_t add(const OptionalCallback& cb) {
		if (!cb.has_value()) {
			return 0; // Return 0 for no-op
		}

		return add(cb.value());
    }

    // Remove a callback by ID
    bool remove(std::size_t id) {
        auto it = std::remove_if(callbacks.begin(), callbacks.end(),
            [id](const auto& pair) { return pair.first == id; });
        if (it != callbacks.end()) {
            callbacks.erase(it, callbacks.end());
            return true;
        }
        return false;
    }

    // Invoke all callbacks (safe if empty)
    void invoke(Args... args) const {
        if (empty()) return; // Guard against empty
        for (const auto& [id, cb] : callbacks) {
            cb(args...);
        }
    }

    // Check if there are no callbacks
    bool empty() const noexcept {
        return callbacks.empty();
    }

    // Allow `if (myAction)` syntax
    explicit operator bool() const noexcept {
        return !empty();
    }

    // C++-style add/remove operators
    Action& operator+=(const Callback& cb) {
        add(cb);
        return *this;
    }

    Action& operator-=(std::size_t id) {
        remove(id);
        return *this;
    }

private:
    std::vector<std::pair<std::size_t, Callback>> callbacks;
    std::size_t lastId = 0;
};

} // namespace ember::core