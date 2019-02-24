#include <iostream>
#include <cstring>
#include <cstdlib>
#include "Leap.h"
#include "HandSignal.h"

using namespace Leap;
using namespace std;



HandSignal::HandSignal(const vector<Hand> &list, sensitivity_t config) : HandSignal(list) { settings = config; }
    
HandSignal::HandSignal(const vector<Hand> &list) {
    
    if (list.size() > 20) {
        fingers = 0;
        return;
    }
    fingers = list[0].count();
    int ind = -1;
    for (Hand hand : list) {
        ind++;
        FingerList fl = hand.fingers();
        if (fl.count() != fingers) {
            cout << "Error: inconsistent number of fingers at frame " << ind << ": expected: " << fingers << " got: " << fl.count() << endl;
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
            fingerLengths[i] /= list.size();
            for (int b = 0; b < 4; b++) {
                for (int w = 0; w < 3; w++) {
                    boneStarts[i][b][w] /= list.size();
                    boneEnds[i][b][w] /= list.size();
                    boneDirs[i][b][w] /= list.size();
                    boneStarts[i][b][w] -= boneStarts[0][0][w];
                    boneEnds[i][b][w] -= boneStarts[0][0][w];
                }
            }
        }
    }
}

ostream &operator<<(ostream &os, const HandSignal &hs) {
    if (hs.fingers == 0) return os << "Invalid HandSignal";
    
    for (int i = 0; i < hs.fingers; i++) {
      os << string(4, ' ') <<  fingerNames[hs.fingerTypes[i]]
                << " finger, id: " << i
                << ", length: " << hs.fingerLengths[i]
                << "mm" << endl;

      // Get finger bones
      for (int b = 0; b < 4; ++b) {
        os << string(6, ' ') <<  "unknown"
                  << " bone, start: (" << hs.boneStarts[i][b][0] << ", " << hs.boneStarts[i][b][1] << ", " << hs.boneStarts[i][b][2] << ")"
                  << ", end: (" << hs.boneEnds[i][b][0] << ", " << hs.boneEnds[i][b][1] << ", " << hs.boneEnds[i][b][2] << ")"
                  << ", direction: (" << hs.boneDirs[i][b][0] << ", " << hs.boneDirs[i][b][1] << ", " << hs.boneDirs[i][b][2] << ")" << endl;
      }
    }
    return os;
}

bool HandSignal::isValid() const {
    return fingers > 0;
}

float valueDiff(float base, float val) {
    return abs(val - base);
}

bool HandSignal::matchesSignal(const Hand &hand, int &errorcode) const {
    const FingerList curr_fingers = hand.fingers();
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
    Vector first = curr_fingers[0].bone(static_cast<Bone::Type>(0)).prevJoint();
    float offset[] = {-first.x, -first.y, -first.z};
    for (FingerList::const_iterator fl_iter = curr_fingers.begin(); fl_iter != curr_fingers.end(); ++fl_iter, i++) {
        const Finger finger = *fl_iter;
        if (valueDiff(fingerLengths[i], finger.length()) > settings.fingerLengthDiff) {
            return false;
        }
        for (int b = 0; b < 4; ++b) {
            Bone::Type boneType = static_cast<Bone::Type>(b);
            Bone bone = finger.bone(boneType);
            for (int w = 0; w < 2; w++) {
                if (valueDiff(boneStarts[i][b][w], bone.prevJoint()[w] + offset[w]) < settings.positionDiff) {
                    return false;
                }
                if (valueDiff(boneEnds[i][b][w], bone.nextJoint()[w] + offset[w]) < settings.positionDiff) {
                    return false;
                }
                if (valueDiff(boneDirs[i][b][w], bone.direction()[w]) < settings.directionDiff) {
                    return false;
                }
            }
        }
    }
    return true;
    
}