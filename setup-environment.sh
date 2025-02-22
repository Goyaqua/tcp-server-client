#!/bin/bash

# Define default values
DEFAULT_IP="127.0.0.1"
DEFAULT_PORT="8888"

# Prompt the user to enter an IP address (or use default)
read -p "Enter server IP (default: $DEFAULT_IP): " APP_IP
APP_IP=${APP_IP:-$DEFAULT_IP}  # If empty, use default

# Prompt the user to enter a port (or use default)
read -p "Enter server port (default: $DEFAULT_PORT): " APP_PORT
APP_PORT=${APP_PORT:-$DEFAULT_PORT}  # If empty, use default

# Validate if the entered port is a number
if ! [[ "$APP_PORT" =~ ^[0-9]+$ ]]; then
    echo "Error: Port must be a number."
    exit 1
fi

# Export environment variables
export APP_IP
export APP_PORT

# Display the set values
echo "✅ Environment variables set:"
echo "APP_IP=$APP_IP"
echo "APP_PORT=$APP_PORT"


