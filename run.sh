#!/bin/bash
set -e  # stop on first error

# Go to backend folder
cd back-end

# Create build folder if not exists
mkdir -p build
cd build

# Run cmake & compile
cmake ..
make -j$(nproc)

echo "✅ Backend compiled successfully."

# Run backend in background
./backend &
BACKEND_PID=$!

# Go back to server folder
cd ../../server

# Install dependencies if not installed yet
if [ ! -d "node_modules" ]; then
  echo "📦 Installing Node.js dependencies..."
  npm install
fi

# Run Node server
echo "🚀 Starting Node.js server..."
npm start &

# Save both PIDs and wait
wait $BACKEND_PID
