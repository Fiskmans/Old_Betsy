
list(APPEND SOURCE_FILES "Functors.h")
list(APPEND SOURCE_FILES "TimeHelper.cpp")
list(APPEND SOURCE_FILES "TimeHelper.h")
list(APPEND SOURCE_FILES "FloatCompare.h")
list(APPEND SOURCE_FILES "JSON.h")
list(APPEND SOURCE_FILES "JSON.cpp")
list(APPEND SOURCE_FILES "JSONCustomTypes.h")
list(APPEND SOURCE_FILES "DereferencingIteratorWrapper.h")
list(APPEND SOURCE_FILES "SubsystemManager.h")
list(APPEND SOURCE_FILES "SubsystemManager.cpp")
list(APPEND SOURCE_FILES "Singleton.h")
list(APPEND SOURCE_FILES "Stopwatch.h")
list(APPEND SOURCE_FILES "MathVector.h")
list(APPEND SOURCE_FILES "Utility.h")
list(APPEND SOURCE_FILES "ImGuiHelpers.h")
list(APPEND SOURCE_FILES "ImGuiHelpers.cpp")
list(APPEND SOURCE_FILES "StringManipulation.h")
list(APPEND SOURCE_FILES "StringManipulation.cpp")
list(APPEND SOURCE_FILES "Literals.cpp")
list(APPEND SOURCE_FILES "Literals.h")
list(APPEND SOURCE_FILES "FileHelpers.cpp")
list(APPEND SOURCE_FILES "FileHelpers.h")
list(APPEND SOURCE_FILES "Matrix3x3.h")
list(APPEND SOURCE_FILES "Matrix4x4.h")
list(APPEND SOURCE_FILES "Plane.h")
list(APPEND SOURCE_FILES "PlaneVolume.h")
list(APPEND SOURCE_FILES "Sphere.h")
list(APPEND SOURCE_FILES "Event.h")
list(APPEND SOURCE_FILES "Ray.h")
list(APPEND SOURCE_FILES "Intersection.h")
list(APPEND SOURCE_FILES "AABB3D.h")
list(APPEND SOURCE_FILES "NameThread.cpp")
list(APPEND SOURCE_FILES "NameThread.h")
list(APPEND SOURCE_FILES "LockedResource.h")
list(APPEND SOURCE_FILES "Solver.h")
list(APPEND SOURCE_FILES "Solver.cpp")

add_library(Tools STATIC "${SOURCE_FILES}")

project(Tools VERSION 1.0.0)

configure_file(config.h.in config.h)
target_include_directories(Tools PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(Tools PUBLIC ${CMAKE_BINARY_DIR})
