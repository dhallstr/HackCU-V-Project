#include <iostream>
#include <cstring>
#include "Leap.h"

using namespace std;
using namespace Leap;

class EventListener : public Listener {
  public:
    virtual void onInit(const Controller&);
    virtual void onConnect(const Controller&);
    virtual void onDisconnect(const Controller&);
    virtual void onExit(const Controller&);
    virtual void onFrame(const Controller&);
    virtual void onFocusGained(const Controller&);
    virtual void onFocusLost(const Controller&);
    virtual void onDeviceChange(const Controller&);
    virtual void onServiceConnect(const Controller&);
    virtual void onServiceDisconnect(const Controller&);

  private:
};

const string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
const string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};

void EventListener::onInit(const Controller& controller) {
  cout << "Initialized" << endl;
}

void EventListener::onConnect(const Controller& controller) {
  cout << "Connected" << endl;
  //controller.enableGesture(Gesture::TYPE_CIRCLE);
  //controller.enableGesture(Gesture::TYPE_KEY_TAP);
  //controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
  //controller.enableGesture(Gesture::TYPE_SWIPE);
}

void EventListener::onDisconnect(const Controller& controller) {
  // Note: not dispatched when running in a debugger.
  cout << "Disconnected" << endl;
}

void EventListener::onExit(const Controller& controller) {
  cout << "Exited" << endl;
}

void EventListener::onFrame(const Controller& controller) {
  // Get the most recent frame and report some basic information
  const Frame frame = controller.frame();
  /*cout << "Frame id: " << frame.id()
            << ", timestamp: " << frame.timestamp()
            << ", hands: " << frame.hands().count()
            << ", extended fingers: " << frame.fingers().extended().count()
            << ", tools: " << frame.tools().count()
            << ", gestures: " << frame.gestures().count() << endl;*/

  cout << "hands: " << frame.hands().count() << ", fingers: " << frame.fingers().extended().count() << endl;
  HandList hands = frame.hands();
  for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
    // Get the first hand
    const Hand hand = *hl;
    string handType = hand.isLeft() ? "Left hand" : "Right hand";
    /*cout << string(2, ' ') << handType << ", id: " << hand.id()
              << ", palm position: " << hand.palmPosition() << endl;*/
    // Get the hand's normal vector and direction
    //const Vector normal = hand.palmNormal();
    //const Vector direction = hand.direction();

    // Calculate the hand's pitch, roll, and yaw angles
    /*cout << string(2, ' ') <<  "pitch: " << direction.pitch() * RAD_TO_DEG << " degrees, "
              << "roll: " << normal.roll() * RAD_TO_DEG << " degrees, "
              << "yaw: " << direction.yaw() * RAD_TO_DEG << " degrees" << endl;*/

    // Get the Arm bone
    Arm arm = hand.arm();
    /*cout << string(2, ' ') <<  "Arm direction: " << arm.direction()
              << " wrist position: " << arm.wristPosition()
              << " elbow position: " << arm.elbowPosition() << endl;*/

    // Get fingers
    const FingerList fingers = hand.fingers();
    for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl) {
      const Finger finger = *fl;
      cout << string(4, ' ') <<  fingerNames[finger.type()]
                << " finger, id: " << finger.id()
                << ", length: " << finger.length()
                << "mm, width: " << finger.width() << endl;

      // Get finger bones
      for (int b = 0; b < 4; ++b) {
        Bone::Type boneType = static_cast<Bone::Type>(b);
        Bone bone = finger.bone(boneType);
        cout << string(6, ' ') <<  boneNames[boneType]
                  << " bone, start: " << bone.prevJoint()
                  << ", end: " << bone.nextJoint()
                  << ", direction: " << bone.direction() << endl;
      }
      break;
    }
  }

  // Get tools
  const ToolList tools = frame.tools();
  for (ToolList::const_iterator tl = tools.begin(); tl != tools.end(); ++tl) {
    const Tool tool = *tl;
    /*cout << string(2, ' ') <<  "Tool, id: " << tool.id()
              << ", position: " << tool.tipPosition()
              << ", direction: " << tool.direction() << endl;*/
  }
}

void EventListener::onFocusGained(const Controller& controller) {
  cout << "Focus Gained" << endl;
}

void EventListener::onFocusLost(const Controller& controller) {
  cout << "Focus Lost" << endl;
}

void EventListener::onDeviceChange(const Controller& controller) {
  cout << "Device Changed" << endl;
  const DeviceList devices = controller.devices();

  for (int i = 0; i < devices.count(); ++i) {
    cout << "id: " << devices[i].toString() << endl;
    cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << endl;
  }
}

void EventListener::onServiceConnect(const Controller& controller) {
  cout << "Service Connected" << endl;
}

void EventListener::onServiceDisconnect(const Controller& controller) {
  cout << "Service Disconnected" << endl;
}

int main(int argc, char** argv) {
  // Create a sample listener and controller
  EventListener listener;
  Controller controller;

  // Have the sample listener receive events from the controller
  controller.addListener(listener);

  if (argc > 1 && strcmp(argv[1], "--bg") == 0)
    controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);

  // Keep this process running until Enter is pressed
  cout << "Press Enter to quit..." << endl;
  cin.get();

  // Remove the sample listener when done
  controller.removeListener(listener);

  return 0;
}
