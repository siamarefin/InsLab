#!/bin/bash
echo "=== Lab 1 Verification Script ==="
echo "Testing all domains..."
echo "1. Testing example.com:"
curl -s http://example.com | grep -o "<title>.*</title>"
echo "2. Testing anothervhost.com:"
curl -s http://anothervhost.com | grep -o "<title>.*</title>"
echo "3. Testing webserverlab.com:"
curl -s http://webserverlab.com | grep -o "<title>.*</title>"
echo "4. Apache status:"
sudo systemctl is-active apache2
echo "=== Verification Complete ==="
