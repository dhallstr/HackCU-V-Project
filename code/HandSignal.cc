#include <iostream>
#include <cstring>
#include <cstdlib>
#include "Leap.h"
#include "HandSignal.h"

using namespace Leap;
using namespace std;



HandSignal::HandSignal(const vector<const FingerList> &list, sensitivity_t config) : HandSignal(list) { settings = config; }
    
HandSignal::HandSignal(const vector<const FingerList> &list) {
    if (list.size() > 20) {
        fingers = 0;
        return;
    }
    fingers = list[0].count();
    for (FingerList fl : list) {
        if (fl.count() != fingers) {
            fingers = 0;
            return;
        }
        int i = 0;
        for (FingerList::const_iterator fl_iter = fl.begin(); fl_iter != fl.end(); ++fl_iter, i++) {
            const Finger finger = *fl_iter;
            fingerTypes[i] = finger.type();
            fingerLengths[i] += finger.length();
            for (int b = 0; b < 4; ++b) {
                Bone::Type boneType = static_cast<Bone::Type>(b);
                Bone bone = finger.bone(boneType);
                
                boneStarts[i][b][0] += bone.prevJoint().x;
                boneStarts[i][b][1] += bone.prevJoint().y;
                boneStarts[i][b][2] += bone.prevJoint().z;
                
                boneEnds[i][b][0] += bone.nextJoint().x;
                boneEnds[i][b][1] += bone.nextJoint().y;
                boneEnds[i][b][2] += bone.nextJoint().z;
                
                boneDirs[i][b][0] += bone.direction().x;
                boneDirs[i][b][1] += bone.direction().y;
                boneDirs[i][b][2] += bone.direction().z;
            }
        }
        for (i = 0; i < fingers; i++) {
            fingerLengths[i] /= fingers;
            for (int b = 0; b < 4; b++) {
                for (int w = 0; w < 3; w++) {
                    boneStarts[i][b][w] /= fingers;
                    boneEnds[i][b][w] /= fingers;
                    boneDirs[i][b][w] /= fingers;
                    boneStarts[i][b][w] -= boneStarts[0][0][w];
                    boneEnds[i][b][w] -= boneStarts[0][0][w];
                }
            }
        }
        
    }
}

bool HandSignal::isValid() const {
    return fingers > 0;
}

float percentDiff(float base, float val) {
    return abs(val - base) / base;
}

bool HandSignal::matchesSignal(const FingerList &curr_fingers, int &errorcode) const {
    errorcode = 0;
    if (fingers == 0) {
        errorcode = 1;
        return false;
    }
    else if (curr_fingers.count() != fingers) {
        errorcode = 2;
        return false;
    }
    int i = 0;
    Vector first = curr_fingers[static_cast<Bone::Type>(0)].bone(0).prevJoint();
    float offset[] = {-first.x, -first.y, -first.z};
    for (FingerList::const_iterator fl_iter = curr_fingers.begin(); fl_iter != curr_fingers.end(); ++fl_iter, i++) {
        const Finger finger = *fl_iter;
        if (percentDiff(fingerLengths[i], finger.length()) > settings.fingerLengthPercent) {
            return false;
        }
        for (int b = 0; b < 4; ++b) {
            Bone::Type boneType = static_cast<Bone::Type>(b);
            Bone bone = finger.bone(boneType);
            for (int w = 0; w < 2; w++) {
                if (percentDiff(boneStarts[i][b][w], bone.prevJoint()[w] + offset[w]) < settings.positionPercent) {
                    return false;
                }
                if (percentDiff(boneEnds[i][b][w], bone.nextJoint()[w] + offset[w]) < settings.positionPercent) {
                    return false;
                }
                if (percentDiff(boneDirs[i][b][w], bone.direction()[w]) < settings.directionPercent) {
                    return false;
                }
            }
        }
    }
    return true;
    
}