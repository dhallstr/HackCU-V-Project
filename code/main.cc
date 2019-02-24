#include <iostream>
#include <cstring>
#include <vector>
#include <unistd.h>
#include "Leap.h"

#include "HandSignal.h"

using namespace std;
using namespace Leap;

static int mode = 0;
static vector<HandSignal> gestureBank;

ostream &operator<<(ostream &o, const FingerList &fingers)
{
  for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl) {
    const Finger finger = *fl;
    if(!finger.isValid())
    {
      o << "[ERROR] invalid finger!!" << endl;
      continue;
    }
    o  << string(4, ' ') << fingerNames[finger.type()]
              << " finger, length: " << finger.length() << "mm" << endl;

    // Get finger bones
    for (int b = 0; b < 4; ++b) {
      Bone::Type boneType = static_cast<Bone::Type>(b);
      Bone bone = finger.bone(boneType);
      o << string(6, ' ') <<  boneNames[boneType]
                << " bone, start: " << bone.prevJoint()
                << ", end: " << bone.nextJoint()
                << ", direction: " << bone.direction() << endl;
    }
  }
  return o;
}


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

void EventListener::onInit(const Controller& controller) {
  cout << "[Controller] Initialized" << endl;
}

void EventListener::onConnect(const Controller& controller) {
  cout << "[Controller] Connected" << endl;
  //controller.enableGesture(Gesture::TYPE_CIRCLE);
  //controller.enableGesture(Gesture::TYPE_KEY_TAP);
  //controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
  //controller.enableGesture(Gesture::TYPE_SWIPE);
}

void EventListener::onDisconnect(const Controller& controller) {
  // Note: not dispatched when running in a debugger.
  cout << "[Controller] Disconnected" << endl;
}

void EventListener::onExit(const Controller& controller) {
  cout << "[Controller] Exited" << endl;
}

void EventListener::onFrame(const Controller& controller) {
  if (mode == 0)
    return;
  // Get the most recent frame and report some basic information
  const Frame frame = controller.frame();
  /*cout << "Frame id: " << frame.id()
            << ", timestamp: " << frame.timestamp()
            << ", hands: " << frame.hands().count()
            << ", extended fingers: " << frame.fingers().extended().count()
            << ", tools: " << frame.tools().count()
            << ", gestures: " << frame.gestures().count() << endl;*/

  //cout << "hands: " << frame.hands().count() << ", fingers: " << frame.fingers().extended().count() << endl;
  HandList hands = frame.hands();
  if(hands.count() < 1)
    return;
  // Get the first hand
  const Hand hand = *hands.begin();
  string handType = hand.isLeft() ? "Left hand" : "Right hand";
  cout << "[Listener] " << handType << " pos: " << hand.palmPosition() << ", extended fingers: " << hand.fingers().extended().count() << endl;
  // Get the hand's normal vector and direction
  //const Vector normal = hand.palmNormal();
  //const Vector direction = hand.direction();
  // Calculate the hand's pitch, roll, and yaw angles
  /*cout << string(2, ' ') <<  "pitch: " << direction.pitch() * RAD_TO_DEG << " degrees, "
            << "roll: " << normal.roll() * RAD_TO_DEG << " degrees, "
            << "yaw: " << direction.yaw() * RAD_TO_DEG << " degrees" << endl;*/

  // Get fingers
  sensitivity_t s = {40, 60, 0.6};
  static vector<Hand> currentGesture;
  currentGesture.push_back(hand);
  if(currentGesture.size() == 50) // keep the vector at size 50
    currentGesture.erase(currentGesture.begin());

  if(mode == 1) // normal operation
  {
    for(const HandSignal &h : gestureBank)
    {
      // send the hand be to processed
      cout << "[Listener] Match? ";
      int errorCode = 0;
      string success = (h.matchesSignal(hand, errorCode)) ? "Yes" : "No" ;
      cout << " " << success << ", code:  " << errorCode << endl;
    }
  }
  else if(mode == 2)
  {
    // Send the vector to train
    cout << "[Listener] Sending training vector!" << endl;
    cout << "[Listener] Last seen Hand was:\n" << hand.fingers() << endl;
    HandSignal h(currentGesture, s);
    gestureBank.clear();
    gestureBank.push_back(h);
    cout << "[Listener] HandSignal is:\n" << h << endl;
    mode = 0; // exit training mode
  }
}

void EventListener::onFocusGained(const Controller& controller) {
  cout << "[Controller] Focus Gained" << endl;
}

void EventListener::onFocusLost(const Controller& controller) {
  cout << "[Controller] Focus Lost" << endl;
}

void EventListener::onDeviceChange(const Controller& controller) {
  cout << "[Controller] Device Changed" << endl;
  const DeviceList devices = controller.devices();

  for (int i = 0; i < devices.count(); ++i) {
    cout << "[Controller] ID: " << devices[i].toString();
    cout << ", Streaming: " << (devices[i].isStreaming() ? "true" : "false") << endl;
  }
}

void EventListener::onServiceConnect(const Controller& controller) {
  cout << "[Controller] Service Connected" << endl;
}

void EventListener::onServiceDisconnect(const Controller& controller) {
  cout << "[Controller] Service Disconnected" << endl;
}

int main(int argc, char** argv) {
  // Create a sample listener and controller
  EventListener listener;
  Controller controller;

  // Have the listener receive events from the controller
  controller.addListener(listener);

  bool running = true;
  while (running)
  {
    int command = 0;
    cout << "\t\t0. exit \t 1. run \t 2. train" << endl;
    cin >> command;
    cin.get();
    switch(command)
    {
      case 0:
        running = false;
      break;
      case 1:
        mode = 1;
        cout << "[System] Running!" << endl;
        cin.get();
        mode = 0;
      break;
      case 2:
        cout << "[System] Training..." << endl;
        sleep(1);
        mode = 2;
        while(mode == 2);
      break;
    }
    cout << "[System] Done!" << endl;
  }

  cout << "[System] Exiting..." << endl;
  // Remove the sample listener when done
  controller.removeListener(listener);
  return 0;
}
