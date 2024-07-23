#pragma once

/*
  Headers rules:
  1. You can't include GLFW before an OpenGL entry, unless you use a define macro for GLFW
  2. For some reason if you include an OpenGL entry before spdlog, it prints a warning
*/

#include "math/types.h"
#include "util/log.h"
#include "util/profile.h"
#include "util/path_manager.h"
#include "scene/transform.h"
#include "scene/node_3d.h"
#include "scene/camera.h"
#include "render/mesh_manager.h"
#include "render/rendering_server.h"
#include "terrain/terrain.h"
#include "terrain/object_placer.h"
#include "application.h"
#include "rendering_application.h"