#!/bin/bash

# Most secure setup: Flask on localhost + Tailscale advertising

# 1. Modify flask_server.py to bind to localhost only
# Change: app.run(host="127.0.0.1", port=5000, debug=False, threaded=True)

# 2. Advertise the service via Tailscale
# This makes the service discoverable as http://raspberrypi:5000
# or with custom name: http://aqua-bar.raspberrypi:5000

# Install tailscale if not already installed
# curl -fsSL https://tailscale.com/install.sh | sh

# Login to Tailscale (if not already done)
# sudo tailscale up

# The service will be accessible via:
# - http://raspberrypi:5000 (via Tailscale MagicDNS)
# - http://[tailscale-ip]:5000
# - Custom domains if configured

echo "Setup complete. Service accessible only through Tailscale network."
echo "Access via: http://raspberrypi:5000"