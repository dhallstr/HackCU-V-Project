#ifndef HandSignalH
#define HandSignalH

#include <iostream>
#include <cstring>
#include <vector>
#include "Leap.h"

typedef struct sensitivityConfig_s {
    
    float fingerLengthDiff = 3;
    
    float positionDiff = 3;
    
    float directionDiff = 0.1;
    
} sensitivity_t;

class HandSignal {
    public:
        HandSignal() = default;
        HandSignal(const std::vector<Leap::Hand> &list);
        HandSignal(const std::vector<Leap::Hand> &list, sensitivity_t config);
        
        bool matchesSignal(const Leap::Hand &curr_fingers, int &errorcode) const;
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