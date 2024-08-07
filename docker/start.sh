#!/usr/bin/env bash
cd "$(dirname "$0")"

if [ "$(docker ps -a | grep glomap_container)" ]; then
  echo
  echo
  echo "Docker container is already running."
  echo "Use ./attach.sh to enter the container or use ./shutdown.sh to shutdown the container."
  echo "To restart container after shutdown, use ./start.sh" 
  echo
  echo

  exit 1
fi

# Run the container
docker run -it -d --rm --gpus=all --privileged  \
  --name glomap_container \
  --volume "$PWD/..:/ws/" \
  --volume="$HOME/.Xauthority:/root/.Xauthority:rw" \
  --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
  --net=host \
  -e NVIDIA_DRIVER_CAPABILITIES=all \
  -e DISPLAY \
  glomap_container
