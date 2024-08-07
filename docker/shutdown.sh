#!/usr/bin/env bash
cd "$(dirname "$0")"

# Kill previous container if necessary
docker kill glomap_container &> /dev/null
docker remove glomap_container &> /dev/null

echo
echo
echo "Docker container dissapeared :|"
echo
echo