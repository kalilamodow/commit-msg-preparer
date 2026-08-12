A tool which asks for your name, does some validation, and prints it to stdout.

It's only 3.7kb, I optimized out the CRT and as much extra stuff as I could
without making it look too ugly. It runs with under a single 4kb cluster on my
machine, but it ~~might~~ will probably explode on other people's computers.

Here's a sample `prepare-commit-msg` hook which uses it to nametag each commit:

```bash
#!/bin/bash

BINARY_PATH="$LOCALAPPDATA/precommit-questionnaire/precommit.exe"
URL="https://github.com/kalilamodow/commit-msg-preparer/releases/latest/download/precommit.exe"

# download if it doesnt exist
if [[ ! -f "$BINARY_PATH" ]]; then
    mkdir -p "$(dirname "$BINARY_PATH")"
    echo "downloading precommit executable..." >&2

    if ! curl.exe -fL "$URL" -o "$BINARY_PATH"; then
        rm -f "$BINARY_PATH"
        echo "failed to download precommitter!\nyou can try doing it manually by downloading $URL to $BINARY_PATH" >&2
        exit 1
    fi
fi

# right after downloading, bash still doesnt think it has executable permissions
# so we have to do this
chmod +x "$BINARY_PATH"

USERNAME=$("$BINARY_PATH")

# checks exit code
if [[ $? -ne 0 ]]; then
    echo "precommit binary failed... $USERNAME" >&2
    exit 1
fi

COMMIT_MSG_FILE="$1"

# if the commit msg is already in the format [name]: commit, replace the name
# otherwise prepend it normally
if [[ "$(head -n 1 "$COMMIT_MSG_FILE")" == \[* ]]; then
    sed -i "1s/\[[^]]*\]/[$USERNAME]/" "$COMMIT_MSG_FILE"
else
    sed -i "1s/^/[$USERNAME]: /" "$COMMIT_MSG_FILE"
fi
```
