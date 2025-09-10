#!/bin/bash
set -e  # Exit on first error
trap "exit" INT  # Exit cleanly on Ctrl+C

# closing possible live server 
echo "🛑 Stopping any existing frontend server..."
pkill -f "python3 -m http.server" || true

# Go to backend folder and build
echo "🔧 Building backend..."
cd back-end

mkdir -p build
cd build

cmake ..
make -j"$(nproc)"
echo "✅ Backend compiled successfully."

# Run backend in background
./backend &
BACKEND_PID=$!
echo "🧠 Backend running (PID: $BACKEND_PID)"

# Start frontend
echo "🌐 Starting frontend server..."
cd ../../front-end
python3 -m http.server &
FRONTEND_PID=$!

# Start Node.js server
echo "📦 Preparing Node.js server..."
cd ../server

if [ ! -d "node_modules" ]; then
  echo "📦 Installing Node.js dependencies..."
  npm install
fi

echo "🚀 Starting Node.js server..."
npm start &
NODE_PID=$!

# Wait for backend, frontend, and node
echo "⏳ Servers running..."
wait $BACKEND_PID $FRONTEND_PID $NODE_PID
