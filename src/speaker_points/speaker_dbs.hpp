#include <vector>

struct LoudnessEpoch {
  float timeStamp;
  std::vector<float> speakerDbs;
};

class LoudnessGenerator {
public:
  LoudnessGenerator(const std::string inFile)
      : kInFile(inFile), kInSampleRate() {
    // Load input audio file here and record its sample rate and stuff.
    kInSampleRate = 0;
  }

  LoudnessEpoch nextLoudnessEpoch() { return {}; };

private:
  const std::string kInFile;
  float kInSampleRate;
};