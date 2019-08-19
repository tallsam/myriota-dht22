#!/bin/bash

#updater.py --id -p /dev/ttyUSB0

ID="003afda802"

echo "Getting messages from ${ID}..."
message_store.py query --limit=5 ${ID}
