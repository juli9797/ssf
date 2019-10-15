#include "log.hpp"

namespace ssf
{

// Exception from the no globals rule bc log
// Only needs to be instantiated in main.cpp
DummyLog log;

} // namespace ssf