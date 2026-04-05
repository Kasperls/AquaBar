#!/bin/bash

# Install nginx
sudo apt update
sudo apt install nginx

# Create nginx configuration for aqua-bar
sudo tee /etc/nginx/sites-available/aqua-bar <<EOF
server {
    listen 80;
    server_name aqua-bar.local;  # Or your custom domain

    location / {
        proxy_pass http://127.0.0.1:5000;
        proxy_set_header Host \$host;
        proxy_set_header X-Real-IP \$remote_addr;
        proxy_set_header X-Forwarded-For \$proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto \$scheme;
    }
}
EOF

# Enable the site
sudo ln -s /etc/nginx/sites-available/aqua-bar /etc/nginx/sites-enabled/

# Remove default site
sudo rm /etc/nginx/sites-enabled/default

# Test configuration
sudo nginx -t

# Restart nginx
sudo systemctl restart nginx
sudo systemctl enable nginx