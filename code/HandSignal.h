#ifndef HandSignalH
#define HandSignalH

#include <iostream>
#include <cstring>

typedef struct sensitivityConfig_s {

    float fingerLengthDiff = 3;

    float positionDiff = 3;

    float directionDiff = 0.1;

} sensitivity_t;

class HandSignal {
    public:
        HandSignal() = default;
        HandSignal(const Leap::Hand &hand);
        HandSignal(const Leap::Hand &hand, sensitivity_t config);
        HandSignal(const HandSignal &other);

        bool matchesSignal(const Leap::Hand &curr_fingers, int &errorcode) const;
        bool isValid() const;
        friend std::ostream &operator<<(std::ostream &os, const HandSignal &hs);
        //friend std::ostream &operator<<(std::ostream &os, const HandSignalCollection &hsc);
        friend class HandSignalCollection;

    private:
        int fingers;
        Leap::Finger::Type fingerTypes[20];
        float fingerLengths[20];
        bool fingerExtended[20];
        //int boneTypes[20][4];
        float boneStarts[20][4][3];
        float boneEnds[20][4][3];
        float boneDirs[20][4][3];

        sensitivity_t settings;
};
#endif
