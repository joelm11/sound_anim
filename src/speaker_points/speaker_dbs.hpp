#include <AudioFile.h>
#include <algorithm> // For std::min
#include <cmath>     // For std::sqrt
#include <iostream>  // For debugging output
#include <numeric>   // Potentially for std::accumulate, but direct loop is fine
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
    // TODO: Add error handling for inFile.load()
    inFile.load(kInputPath);
    sampleRate = inFile.getSampleRate();
    length_s = inFile.getLengthInSeconds();
    // BUG FIX: samplesPerEpoch calculation was incorrect (division instead of
    // multiplication) Should also be an integer number of samples.
    samplesPerEpoch = static_cast<size_t>(kEpochLength_s * sampleRate);

    // Handle edge case where calculated samplesPerEpoch might be 0 or extremely
    // large
    if (samplesPerEpoch == 0 && kEpochLength_s > 0 && sampleRate > 0) {
      samplesPerEpoch =
          1; // Ensure at least one sample per epoch if duration is positive
    }
  }

  float getLength_s() const { return length_s; }

  LoudnessEpoch nextLoudnessEpoch() {
    const size_t kTotalSamples = inFile.getNumSamplesPerChannel();

    if (sampleIdx >= kTotalSamples) {
      return {-1, {}};
    }

    // Calculate the actual number of samples to read in this epoch
    size_t samplesToRead = samplesPerEpoch;
    if (sampleIdx + samplesToRead > kTotalSamples) {
      samplesToRead = kTotalSamples - sampleIdx;
    }

    std::vector<float> ldness(inFile.getNumChannels());
    for (int ch = 0; ch < inFile.getNumChannels(); ++ch) {
      float sum_sq = 0;
      for (size_t sample_offset = 0; sample_offset < samplesToRead;
           ++sample_offset) {
        size_t current_sample_idx = sampleIdx + sample_offset;
        sum_sq += inFile.samples[ch][current_sample_idx] *
                  inFile.samples[ch][current_sample_idx];
      }
      float db = 10 * std::log10(sum_sq / static_cast<float>(samplesToRead));
      ldness[ch] = db;
    }

    const float timeStamp = static_cast<float>(sampleIdx) / sampleRate;

    sampleIdx += samplesToRead;

    // std::cout << "Processed epoch starting at sample: "
    //           << sampleIdx - samplesToRead
    //           << ", samples read: " << samplesToRead
    //           << ", timestamp: " << timeStamp << "\n";

    return {timeStamp, ldness};
  };

private:
  const std::string kInputPath;
  const float kEpochLength_s;
  float sampleRate;
  float length_s;
  size_t samplesPerEpoch;
  size_t sampleIdx = 0;
  AudioFile<float> inFile;
};