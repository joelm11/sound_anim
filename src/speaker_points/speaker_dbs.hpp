#include <AudioFile.h>
#include <vector>

struct LoudnessEpoch {
  float timeStamp;
  std::vector<float> speakerDbs;
};

class LoudnessGenerator {
public:
  LoudnessGenerator(const std::string inPath, const float epochLength_s)
      : kInputPath(inPath), kEpochLength_s(epochLength_s) {

    // Load input audio file here and record its sample rate and stuff.
    inFile.load(kInputPath);
    sampleRate = inFile.getSampleRate();
    length_s = inFile.getLengthInSeconds();
    samplesPerEpoch = kEpochLength_s / sampleRate;
  }

  float getLength_s() const { return length_s; }

  LoudnessEpoch nextLoudnessEpoch() {
    if (sampleIdx + samplesPerEpoch > inFile.getNumSamplesPerChannel()) {
      return {-1, {}};
    }
    std::vector<float> ldness(inFile.getNumChannels());
    for (int ch = 0; ch < inFile.getNumChannels(); ++ch) {
      float rms = 0;
      for (int sample = 0; sample < samplesPerEpoch; ++sample) {
        rms += inFile.samples[ch][float] * inFile.samples[ch][float];
      }
      rms /= samplesPerEpoch;
      ldness[ch] = std::sqrt(rms);
    }
    sampleIdx += samplesPerEpoch;
    const float kTimeStamp = sampleIdx / sampleRate;
    return {1.f, ldness};
  };

private:
  const std::string kInputPath;
  const float kEpochLength_s;
  float sampleRate, length_s, samplesPerEpoch;
  unsigned sampleIdx = 0;
  AudioFile<float> inFile;
};