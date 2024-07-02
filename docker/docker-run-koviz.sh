#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd $SCRIPT_DIR

# Check if the path argument is provided
if [ -z "$1" ]; then
  echo "Usage: $0 <path-to-data>"
  exit 1
fi

# Assign the first argument to KOVIZ_DATA_PATH
KOVIZ_DATA_PATH=$1
shift   # Consume the first argument

# Check if the path exists
if [ ! -d "$KOVIZ_DATA_PATH" ]; then
  echo "The specified path does not exist: $KOVIZ_DATA_PATH"
  exit 1
fi

# Export the KOVIZ_DATA_PATH to be used in docker-compose
export KOVIZ_DATA_PATH

# Start the Docker container and run koviz, passing arguments to the entrypoint
docker compose build
docker compose run --rm koviz "$@"
