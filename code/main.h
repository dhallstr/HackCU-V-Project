#ifndef mainH
#define mainH

#include <string>

#include "Leap.h"

constexpr int DEBUG = 3; // 0 off, 1 few, 2, more, 3, all

const std::string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
const std::string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const std::string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};
std::ostream &operator<<(std::ostream &o, const Leap::FingerList &fingers);

#endif
