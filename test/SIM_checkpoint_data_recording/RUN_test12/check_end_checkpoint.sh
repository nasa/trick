#!/bin/bash
# Run from the top of the Trick repository after RUN_test12/unit_test.py.
# The end checkpoint is written by a forked process at shutdown. The simulation waits for it
# before exiting, so it must exist and be complete: the last line of a checkpoint is a
# complete assignment ending with ';'.
FILE=test/SIM_checkpoint_data_recording/RUN_test12/chkpnt_end

if [ ! -s "$FILE" ]; then
    echo "$FILE does not exist or is empty."
    exit 1
fi

if ! grep -q "// Variable Assignments." "$FILE"; then
    echo "$FILE is incomplete: no variable assignments."
    exit 1
fi

LAST_LINE=$(grep -v '^[[:space:]]*$' "$FILE" | tail -n 1)
if [[ "$LAST_LINE" != *";" ]]; then
    echo "$FILE is incomplete: last line is \"$LAST_LINE\"."
    exit 1
fi

echo "$FILE is complete."
exit 0
