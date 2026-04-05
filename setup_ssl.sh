#!/bin/bash

# Install certbot for Let's Encrypt SSL
sudo apt install snapd
sudo snap install core; sudo snap refresh core
sudo snap install --classic certbot

# Create symlink
sudo ln -s /snap/bin/certbot /usr/bin/certbot

# Get SSL certificate (replace yourdomain.com with your actual domain)
# sudo certbot --nginx -d yourdomain.com

# For testing with self-signed certificate:
sudo openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
    -keyout /etc/ssl/private/aqua-bar.key \
    -out /etc/ssl/certs/aqua-bar.crt \
    -subj "/C=NO/ST=State/L=City/O=Organization/CN=aqua-bar.local"

# Update nginx config for HTTPS
sudo tee /etc/nginx/sites-available/aqua-bar-ssl <<EOF
server {
    listen 443 ssl http2;
    server_name aqua-bar.local;

    ssl_certificate /etc/ssl/certs/aqua-bar.crt;
    ssl_certificate_key /etc/ssl/private/aqua-bar.key;

    location / {
        proxy_pass http://127.0.0.1:5000;
        proxy_set_header Host \$host;
        proxy_set_header X-Real-IP \$remote_addr;
        proxy_set_header X-Forwarded-For \$proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto \$scheme;
    }
}

server {
    listen 80;
    server_name aqua-bar.local;
    return 301 https://\$server_name\$request_uri;
}
EOF