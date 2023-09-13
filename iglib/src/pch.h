#pragma once
#ifndef PCH_H
#define PCH_H

#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <unordered_map>
#include <exception>
#include <intrin.h>

#include <Windows.h>
#include <Bite.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define PEEK(p) std::cout << #p << ": " << (p) << '\n'

typedef GLFWwindow *WindowPtr_t;
typedef GLFWmonitor *MonitorPtr_t;

namespace ig {}

using namespace ig;
using namespace bite;

#endif