#ifndef HandSignalCollectionH
#define HandSignalCollectionH

class HandSignalCollection {
    public:
      HandSignalCollection();
      bool save() const;
      void exec(int i) const;
      bool add(HandSignal::HandSignal &hs, std::string &name, std::string &command);
      bool remove(int i);
      bool remove(std::string &name);
    private:
      std::vector<HandSignal::HandSignal> signals;
      std::vector<std::string> names;
      std::vector<std::string> commands;
};

#endif