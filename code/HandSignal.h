#ifndef HandSignalH
#define handSignalH

#include <iostream>
#include <cstring>
#include <vector>
#include "Leap.h"

using namespace Leap;

class HandSignal {
    public:
        HandSignal(const &vector<const FingerList> list) const;
        bool matchesSignal(const FingerList &fingers, int &errorcode) const;
        bool isValid();
        
    private:
        int fingers;
        Finger::Type fingerTypes[20];
        float fingerLengths[20]
        int boneTypes[20][4];
        float boneStarts[20][4][3];
        float boneEnds[20][4][3];
        float boneDirs[20][4][3];
}
#endif