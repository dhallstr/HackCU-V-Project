#ifndef HandSignalH
#define HandSignalH

#include <iostream>
#include <cstring>
#include <vector>
#include "Leap.h"

typedef struct sensitivityConfig_s {
    
    float fingerLengthDiff = 25;
    
    float positionDiff = 20;
    
    float directionDiff = 20;
    
} sensitivity_t;

class HandSignal {
    public:
        HandSignal() = default;
        HandSignal(const std::vector<Leap::FingerList> &list);
        HandSignal(const std::vector<Leap::FingerList> &list, sensitivity_t config);
        
        bool matchesSignal(const Leap::FingerList &curr_fingers, int &errorcode) const;
        bool isValid() const;
        friend std::ostream &operator<<(std::ostream &os, const HandSignal &hs);
        
    private:
        int fingers;
        Leap::Finger::Type fingerTypes[20];
        float fingerLengths[20];
        //int boneTypes[20][4];
        float boneStarts[20][4][3];
        float boneEnds[20][4][3];
        float boneDirs[20][4][3];
        
        sensitivity_t settings;
};
#endif