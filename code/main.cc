#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <vector>
#include <chrono>

#include "Leap.h"
#include "main.h"
#include "HandSignal.h"
#include "HandSignalCollection.h"

using namespace std;
using namespace Leap;

static int mode = 0;
static HandSignalCollection gestureCollection;
static long last_call = 0;
static int last_command_id = -1;

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

long currTime() {
    return (chrono::duration_cast< chrono::milliseconds >(chrono::system_clock::now().time_since_epoch())).count();
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
  if(DEBUG > 1) cout << "[Controller] Initialized" << endl;
}

void EventListener::onConnect(const Controller& controller) {
  if(DEBUG > 0) cout << "[Controller] Connected" << endl;
  //controller.enableGesture(Gesture::TYPE_CIRCLE);
  //controller.enableGesture(Gesture::TYPE_KEY_TAP);
  //controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
  //controller.enableGesture(Gesture::TYPE_SWIPE);
}

void EventListener::onDisconnect(const Controller& controller) {
  // Note: not dispatched when running in a debugger.
  if(DEBUG > 0) cout << "[Controller] Disconnected" << endl;
}

void EventListener::onExit(const Controller& controller) {
  if(DEBUG > 1) cout << "[Controller] Exited" << endl;
}

void EventListener::onFrame(const Controller& controller) {
  if (mode == 0)
    return;
  // Get the most recent frame and report some basic information
  const Frame frame = controller.frame();

  if(DEBUG > 3) cout << "hands: " << frame.hands().count() << ", fingers: " << frame.fingers().extended().count() << endl;
  HandList hands = frame.hands();
  if(hands.count() < 1)
    return;
  // Get the first hand
  const Hand hand = *hands.begin();
  string handType = hand.isLeft() ? "Left hand" : "Right hand";
  if(DEBUG > 1) cout << "[Listener] " << handType << " pos: " << hand.palmPosition() << ", extended fingers: " << hand.fingers().extended().count() << endl;

  // Get fingers
  sensitivity_t s = {50, 60, 0.6};

  if(mode == 1) // normal operation
  {
    int i = 0;
    for(const HandSignal &h : gestureCollection.signals)
    {
      // send the hand be to processed
      if(DEBUG > 1) cout << "[Listener] Signal " << i << ": match? ";
      int errorCode = 0;
      bool success = h.matchesSignal(hand, errorCode);
      if(DEBUG > 1) cout << " " << (success ? "Yes" : "No") << ", code:  " << errorCode << endl;
      if(success && last_call + 3000 < currTime() && last_command_id != i)
      {
        cout << "[Listener] Gesture " << gestureCollection.getName(i) << " triggered!" << endl;
        last_call = currTime();
        last_command_id = i;
        //sleep(1);
        return;
      }
      i++;
    }
    last_command_id = -1;
  }
  else if(mode == 2)
  {
    // Send the vector to train
    if(DEBUG > 2) cout << "[Listener] Sending training vector!" << endl;
    if(DEBUG > 2) cout << "[Listener] Last seen Hand was:\n" << hand.fingers() << endl;
    HandSignal hs(hand , s);
    cout << "Saw extended fingers: " << hand.fingers().extended().count() << endl;
    string gname, gcomm;
    cout << "Enter name for new gesture: ";
    cin >> gname;
    cout << "Enter script name: ";
    cin >> gcomm;
    gestureCollection.add(hs, gname, gcomm);
    if(DEBUG > 2) cout << "[Listener] HandSignal is:\n" << hs << endl;
    mode = 0; // exit training mode
  }
}

void EventListener::onFocusGained(const Controller& controller) {
  if(DEBUG > 0) cout << "[Controller] Focus Gained" << endl;
}

void EventListener::onFocusLost(const Controller& controller) {
  if(DEBUG > 0) cout << "[Controller] Focus Lost" << endl;
}

void EventListener::onDeviceChange(const Controller& controller) {
  if(DEBUG > 1) cout << "[Controller] Device Changed" << endl;
  const DeviceList devices = controller.devices();

  for (int i = 0; i < devices.count(); ++i) {
    if(DEBUG > 1) cout << "[Controller] ID: " << devices[i].toString();
    if(DEBUG > 1) cout << ", Streaming: " << (devices[i].isStreaming() ? "true" : "false") << endl;
  }
}

void EventListener::onServiceConnect(const Controller& controller) {
  if(DEBUG > 0) cout << "[Controller] Service Connected" << endl;
}

void EventListener::onServiceDisconnect(const Controller& controller) {
  if(DEBUG > 0) cout << "[Controller] Service Disconnected" << endl;
}

int main(int argc, const char* argv[])
{
  // Create a sample listener and controller
  EventListener listener;
  Controller controller;

  // Have the listener receive events from the controller
  controller.addListener(listener);

  bool running = true;
  while (running)
  {
    string command = "";
    cout << "Typey > ";
    cin >> command;
    if (command.compare("exit") == 0)
    {
        running = false;
    }
    else if (command.compare("run") == 0)
    {
        mode = 1;
        cout << "Running" << endl;
    }
    else if (command.compare("pause") == 0)
    {
        mode = 0;
        cout << "Paused" << endl;
    }
    else if (command.compare("train") == 0)
    {
      cin.get();
      cout << "Prepare gesture, and press enter...";
      cin.get();
      cout << "Training..." << endl;
      sleep(1);
      mode = 2;
      while(mode == 2); // handler will change mode
      cout << "Done!" << endl;
    }
    else if (command.compare("remove") == 0)
    {
      string target = "";

      cin >> target;
      string success = gestureCollection.remove(target) ? "Removed " : "Couldn't find ";
      cout << success << target << endl;
    }
    else if (command.compare("list") == 0)
    {
      cout << gestureCollection << endl;
    }
    else if (command.compare("help") == 0)
    {
      cout << "Commands: run, pause, train, remove, list, help" << endl;
    }
    else
    {
      cout << "Unknown command!" << endl;
    }
  }
  cout << "[System] Exiting..." << endl;
  // Remove the sample listener when done
  controller.removeListener(listener);
  return 0;
}
