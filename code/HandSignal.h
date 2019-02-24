#ifndef HandSignalH
#define HandSignalH

#include <iostream>
#include <cstring>
#include <vector>
#include "Leap.h"

typedef struct sensitivityConfig_s {
    
    float fingerLengthPercent = 0.25;
    
    float positionPercent = 0.2;
    
    float directionPercent = 0.2;
    
} sensitivity_t;

class HandSignal {
    public:
        HandSignal() = default;
        HandSignal(const std::vector<Leap::FingerList> &list);
        HandSignal(const std::vector<Leap::FingerList> &list, sensitivity_t config);
        
        bool matchesSignal(const Leap::FingerList &curr_fingers, int &errorcode) const;
        bool isValid() const;
        
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