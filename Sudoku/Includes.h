#pragma once

// imgui
#define IM_VEC2_CLASS_EXTRA \
inline bool operator<=(const ImVec2& other) const { return x <= other.x && y <= other.y; } \
inline bool operator>=(const ImVec2& other) const { return x >= other.x && y >= other.y; }

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <imgui_stdlib.h>

// glfw3
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3.h>
#include <glfw3native.h>

// std
#include <memory>
#include <functional>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#define INT3 (__debugbreak())