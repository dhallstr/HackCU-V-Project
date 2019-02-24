#include <iostream>
#include <cstring>
#include <cstdlib>
#include "Leap.h"
#include "HandSignal.h"

using namespace Leap;
using namespace std;



HandSignal::HandSignal(const vector<Hand> &list, sensitivity_t config) : HandSignal(list)
{
  settings = config;
  cout << "Position Diff: " << settings.positionDiff << endl;
}

HandSignal::HandSignal(const vector<Hand> &list) {
    cout << "[HandSignal] handSignal ctor called!" << endl;
    if (list[0].fingers().count() > 20) {
        fingers = 0;
        return;
    }
    
    fingers = list[0].fingers().count();
    
    for (int i = 0; i < fingers; i++) {
        fingerLengths[i] = 0;
        fingerExtended[i] = false;
        for (int b = 0; b < 4; b++) {
            for (int w = 0; w < 3; w++) {
                boneStarts[i][b][w] = 0;
                boneEnds[i][b][w] = 0;
                boneDirs[i][b][w] = 0;
            }
        }
    }
    
    
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
            fingerExtended[i] = finger.isExtended();
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
            float norm[3];
            for (int w = 0; w < 3; w++) {
                norm[w] = boneStarts[i][0][w] / list.size();
            }
            for (int b = 0; b < 4; b++) {


                for (int w = 0; w < 3; w++) {
                    boneStarts[i][b][w] /= list.size();
                    boneEnds[i][b][w] /= list.size();
                    boneDirs[i][b][w] /= list.size();
                    boneStarts[i][b][w] -= norm[w];//list[0].palmPosition()[w];
                    boneEnds[i][b][w] -= norm[w];//list[0].palmPosition()[w];
                }
            }
        }
    }
}

ostream &operator<<(ostream &os, const HandSignal &hs) {
    if (hs.fingers == 0) return os << "Invalid HandSignal";

    for (int i = 0; i < hs.fingers; i++) {
      os << string(4, ' ') <<  fingerNames[hs.fingerTypes[i]]
                << " finger, length: " << hs.fingerLengths[i]
                << "mm" << endl;

      // Get finger bones
      for (int b = 0; b < 4; ++b) {
        os << string(6, ' ') <<  boneNames[static_cast<Bone::Type>(b)]
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
    //Vector first = hand.palmPosition();
    // Each finger
    for (FingerList::const_iterator fl_iter = curr_fingers.begin(); fl_iter != curr_fingers.end(); ++fl_iter, i++) {
        const Finger finger = *fl_iter;
        
        if (finger.isExtended() != fingerExtended[i]) { // if the finger extended status is not the same, it's not the same signal
                errorcode = 7;
                return false;
        }
        //if (!finger.isExtended()) continue; // we don't care about position of non-extended fingers
        // Finger length
        if (valueDiff(fingerLengths[i], finger.length()) > settings.fingerLengthDiff) {
            errorcode = 3;
            return false;
        }
        // Each bone position
        for (int b = 0; b < 4; b++) {
            Bone::Type boneType = static_cast<Bone::Type>(b);
            Bone bone = finger.bone(boneType);
            // XYZ differences
            for (int w = 0; w < 3; w++)
            {
                if (valueDiff(boneStarts[i][b][w], bone.prevJoint()[w] - finger.bone(static_cast<Bone::Type>(0)).prevJoint()[w]) > settings.positionDiff) {
                    errorcode = 4;
                    cout << "startDiff = " << boneStarts[i][b][w] - (bone.prevJoint()[w] - finger.bone(static_cast<Bone::Type>(0)).prevJoint()[w]) << " ";
                }
                else if (valueDiff(boneEnds[i][b][w], bone.nextJoint()[w] - finger.bone(static_cast<Bone::Type>(0)).prevJoint()[w]) > settings.positionDiff) {
                    errorcode = 5;
                    cout << "endDiff = " << boneEnds[i][b][w] - (bone.nextJoint()[w] - finger.bone(static_cast<Bone::Type>(0)).prevJoint()[w]) << " > " << settings.positionDiff << " and new bone pos is " << bone.nextJoint()[w] - finger.bone(static_cast<Bone::Type>(0)).prevJoint()[w] << " and trained position is " << boneEnds[i][b][w] << " ";
                }
                else if (valueDiff(boneDirs[i][b][w], bone.direction()[w]) > settings.directionDiff) {
                    errorcode = 6;
                }

                if(errorcode != 0)
                {
                  cout << fingerNames[fingerTypes[i]];
                  return false;
                }
            }
        }
    }
    return true;

}
