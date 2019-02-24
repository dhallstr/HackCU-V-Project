#ifndef HandSignalCollectionH
#define HandSignalCollectionH

class HandSignalCollection {
    public:
      HandSignalCollection();
      bool save();
      void exec(int i) const;
      bool add(HandSignal &hs, std::string &name, std::string &command);
      bool remove(int i);
      bool remove(std::string &name);

      friend class EventListener;

    private:
      std::vector<HandSignal> signals;
      std::vector<std::string> names;
      std::vector<std::string> commands;
};

#endif
