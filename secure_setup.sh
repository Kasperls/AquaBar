#!/bin/bash

# Secure Flask app for public network access via Tailscale only

# 1. Install ufw (Uncomplicated Firewall)
sudo apt update
sudo apt install ufw

# 2. Allow SSH (important - don't lock yourself out!)
sudo ufw allow ssh

# 3. Allow Tailscale traffic
# Get your Tailscale IP
TAILSCALE_IP=$(tailscale ip -4)
echo "Your Tailscale IP: $TAILSCALE_IP"

# Allow traffic only from Tailscale IP to port 5000
sudo ufw allow from $TAILSCALE_IP to any port 5000

# 4. Enable firewall
sudo ufw --force enable

# 5. Verify rules
sudo ufw status

echo "Firewall configured. Flask app is now only accessible via Tailscale IP: $TAILSCALE_IP:5000"
echo "To disable firewall temporarily: sudo ufw disable"
echo "To allow more IPs: sudo ufw allow from <ip> to any port 5000"