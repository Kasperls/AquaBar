#!/bin/bash

# Change Flask Server Password
# Usage: ./change_password.sh newpassword

if [ $# -eq 0 ]; then
    echo "Usage: $0 <new_password>"
    echo "Example: $0 mysecurepassword123"
    exit 1
fi

NEW_PASSWORD=$1

# Generate new hash
echo "Generating password hash for: $NEW_PASSWORD"
HASH=$(python3 -c "from werkzeug.security import generate_password_hash; print(generate_password_hash('$NEW_PASSWORD'))")

# Update config.py
sed -i "s|PASSWORD_HASH = .*|PASSWORD_HASH = \"$HASH\"|" python/config.py

echo "✅ Password updated successfully!"
echo "New password: $NEW_PASSWORD"
echo "Hash saved to python/config.py"