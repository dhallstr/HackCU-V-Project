#ifndef HandSignalH
#define handSignalH

#include <iostream>
#include <cstring>
#include <vector>
#include "Leap.h"

class HandSignal {
    public:
        HandSignal(const std::vector<const Leap::FingerList> &list);
        bool matchesSignal(const Leap::FingerList &fingers, int &errorcode) const;
        bool isValid() const;
        
    private:
        int fingers;
        Leap::Finger::Type fingerTypes[20];
        float fingerLengths[20]
        int boneTypes[20][4];
        float boneStarts[20][4][3];
        float boneEnds[20][4][3];
        float boneDirs[20][4][3];
}
#endif