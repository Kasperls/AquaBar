# Install Flask server as systemd service
# This runs independently of your C++ application

# 1. Copy service file to systemd
sudo cp aqua-bar-flask.service /etc/systemd/system/

# 2. Reload systemd and enable service
sudo systemctl daemon-reload
sudo systemctl enable aqua-bar-flask

# 3. Start the service
sudo systemctl start aqua-bar-flask

# 4. Check status
sudo systemctl status aqua-bar-flask

# 5. View logs
sudo journalctl -u aqua-bar-flask -f

# Management commands:
# sudo systemctl stop aqua-bar-flask
# sudo systemctl restart aqua-bar-flask
# sudo systemctl disable aqua-bar-flask