#pragma once
#include <mutex>

inline std::mutex consoleMutex; // Synchronizes console output between threads