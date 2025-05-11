#!/bin/bash

# --- Approach 1: Using the '&' operator (with argument and process control) ---

# Define the path to your executable
executable="./build/window"

# Get the audio file path from the first command-line argument
audio_file="$1"

# Check if an argument was provided
if [ -z "$audio_file" ]; then
  echo "Usage: $0 <path_to_audio_file.wav>"
  exit 1
fi

# Check if the audio file exists
if [ ! -f "$audio_file" ]; then
  echo "Error: Audio file not found at $audio_file!"
  exit 1
fi

# Check if the executable exists and is executable
if [ ! -x "$executable" ]; then
  echo "Error: Executable not found or not executable at $executable!"
  exit 1
fi

# Function to stop the audio playback
stop_audio() {
  echo "Stopping audio..."
  # Use the stored PID to kill the background audio process gracefully
  if [ -n "$audio_pid" ] && ps -p "$audio_pid" > /dev/null; then
    kill "$audio_pid"
  fi
}

# Trap SIGINT (Ctrl+C) and SIGTERM signals and call the stop_audio function
trap stop_audio SIGINT SIGTERM

# Play the audio file in the background using afplay (for macOS)
# Replace 'aplay' with your preferred command-line audio player if not on macOS
afplay "$audio_file" &

# Store the PID of the background audio process
audio_pid=$!

# Run your executable
"$executable"

# Wait for the executable to finish (it's the foreground process)
# The trap will handle stopping audio if the script is interrupted

# After the executable finishes, explicitly stop the audio just in case
stop_audio

echo "Executable finished."