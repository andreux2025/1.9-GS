#pragma once
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <Windows.h>
#include <iostream>
#include "SDK/SDK.hpp"
#include "Unreal.h"
using namespace SDK;
#include "minhook/Minhook.h"
#pragma comment(lib, "minhook/minhook.lib")
#define LOG(msg) (std::cout << "[SERVER LOG]: " << msg << "\n")
#include "SDK/SDK/Engine_parameters.hpp"
using namespace Params;