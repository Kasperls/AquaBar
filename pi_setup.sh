#!/bin/bash

# Complete Aqua Bar Flask Server Setup for Raspberry Pi
# Run this script on your Raspberry Pi after git pull

echo "🚀 Starting Aqua Bar Flask Server Setup..."
echo "=========================================="

# 1. Update system
echo "📦 Updating system packages..."
sudo apt update && sudo apt upgrade -y

# 2. Install Python dependencies
echo "🐍 Installing Python dependencies..."
sudo apt install -y python3-pip python3-flask python3-werkzeug

# 3. Verify installations
echo "✅ Verifying installations..."
python3 -c "import flask; print('Flask version:', flask.__version__)"
python3 -c "import werkzeug; print('Werkzeug version:', werkzeug.__version__)"

# 4. Navigate to project directory
echo "📁 Setting up project directory..."
cd /home/piaqua/Desktop/AquaBar

# 5. Check if config.py exists and has password
echo "🔐 Checking password configuration..."
if [ -f "python/config.py" ]; then
    echo "✅ config.py exists"
    if grep -q "PASSWORD_HASH" python/config.py; then
        echo "✅ Password hash found in config.py"
        echo "   Current password is: admin"
        echo "   To change it, run:"
        echo "   python3 -c \"from werkzeug.security import generate_password_hash; print('New hash:', generate_password_hash('your_new_password'))\""
        echo "   Then edit python/config.py"
    else
        echo "❌ Password hash not found in config.py"
        exit 1
    fi
else
    echo "❌ config.py not found"
    exit 1
fi

# 6. Test Flask server startup
echo "🧪 Testing Flask server..."
cd python
timeout 10s python3 flask_server.py &
FLASK_PID=$!
sleep 3

if kill -0 $FLASK_PID 2>/dev/null; then
    echo "✅ Flask server started successfully"
    kill $FLASK_PID
    echo "✅ Flask server test passed"
else
    echo "❌ Flask server failed to start"
    exit 1
fi

cd ..

# 7. Check Tailscale status
echo "🔗 Checking Tailscale..."
if command -v tailscale &> /dev/null; then
    echo "✅ Tailscale is installed"
    tailscale status
    TAILSCALE_IP=$(tailscale ip -4 2>/dev/null)
    if [ $? -eq 0 ]; then
        echo "✅ Tailscale IP: $TAILSCALE_IP"
        echo "🌐 Web server will be accessible at: http://raspberrypi:5000"
    else
        echo "⚠️  Tailscale not connected - run 'sudo tailscale up'"
    fi
else
    echo "❌ Tailscale not installed"
fi

# 8. Final instructions
echo ""
echo "🎉 Setup Complete!"
echo "=================="
echo ""
echo "To run your application:"
echo "1. Build your C++ application: make"
echo "2. Run: ./your_app_name"
echo ""
echo "The Flask web server will start automatically with your C++ app."
echo ""
echo "Access the web interface via Tailscale:"
echo "http://raspberrypi:5000"
echo ""
echo "Default login: admin / admin"
echo ""
echo "To change password, edit python/config.py with a new hash"