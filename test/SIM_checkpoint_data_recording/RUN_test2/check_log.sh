FILE=log_foo.csv
if test -f "$FILE"; then
    echo "$FILE exists."
    exit 1
else
    # Expect the file not to exist.
    echo "$FILE does not exist."
    exit 0
fi