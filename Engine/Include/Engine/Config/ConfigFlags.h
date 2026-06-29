#pragma once
#include <cstdint>

enum class DrawMode : uint32_t {
    None = 0,
    Models = 1 << 0,
    Shadow = 1 << 1,
    Collision = 1 << 2,
    PostProcess = 1 << 3,
    All = ~0u
};

inline DrawMode operator|(DrawMode lhs, DrawMode rhs) {
    return static_cast<DrawMode>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

inline DrawMode operator&(DrawMode lhs, DrawMode rhs) {
    return static_cast<DrawMode>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

inline DrawMode operator~(DrawMode flag) {
    return static_cast<DrawMode>(~static_cast<uint32_t>(flag));
}

inline bool HasFlag(const DrawMode mask, const DrawMode flag) {
    return (mask & flag) == flag;
}

inline void AddFlag(DrawMode& mask, const DrawMode flag) {
    mask = mask | flag;
}

inline void RemoveFlag(DrawMode& mask, const DrawMode flag) {
    mask = mask & ~flag;
}

inline void ToggleFlag(DrawMode& mask, DrawMode flag) {
    mask = static_cast<DrawMode>(static_cast<uint32_t>(mask) ^ static_cast<uint32_t>(flag));
}