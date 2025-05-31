#pragma once

#include <iostream>

#define ASSERT(x) __debugbreak(); \
	std::cerr << x << std::endl;