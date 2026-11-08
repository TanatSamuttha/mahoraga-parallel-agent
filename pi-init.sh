
#!/usr/bin/env bash
set -Eeuo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

# Load .env from the script directory
ENV_FILE="$SCRIPT_DIR/.env"

if [[ ! -f "$ENV_FILE" ]]; then
    echo "Error: cannot find .env at $ENV_FILE" >&2
    exit 1
fi

source "$ENV_FILE"

if [[ -z "${TYPHOON_API_KEY:-}" ]]; then
    echo "Error: TYPHOON_API_KEY is missing in .env" >&2
    exit 1
fi

export TYPHOON_API_KEY

# Locate Pi test script
if [[ -f "$SCRIPT_DIR/pi-test.sh" ]]; then
    ROOT="$SCRIPT_DIR"
elif [[ -f "$SCRIPT_DIR/pi/pi-test.sh" ]]; then
    ROOT="$SCRIPT_DIR/pi"
else
    echo "Error: cannot find pi-test.sh" >&2
    exit 1
fi

PI_TEST="$ROOT/pi-test.sh"
MODEL="typhoon-v2.5-30b-a3b-instruct"

# Create temporary Pi configuration directory
umask 077
TEMP_AGENT_DIR="$(mktemp -d "$ROOT/.pi-runtime.XXXXXX")"

cleanup() {
    local status=$?
    trap - EXIT INT TERM
    rm -rf -- "$TEMP_AGENT_DIR"
    unset TYPHOON_API_KEY
    exit "$status"
}

trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM

# Generate temporary model configuration
node - "$TEMP_AGENT_DIR/models.json" <<'NODE'
const fs = require("node:fs");

const file = process.argv[2];
const apiKey = process.env.TYPHOON_API_KEY;

const config = {
  "$schema": "https://pi.dev/schemas/models.schema.json",
  providers: {
    typhoon: {
      name: "Typhoon",
      baseUrl: "https://api.opentyphoon.ai/v1",
      api: "openai-completions",
      apiKey,
      models: [
        {
          id: "typhoon-v2.5-30b-a3b-instruct",
          name: "Typhoon v2.5 30B A3B Instruct"
        }
      ]
    }
  }
};

fs.writeFileSync(file, JSON.stringify(config, null, 2) + "\n", {
  mode: 0o600
});
NODE

export PI_CODING_AGENT_DIR="$TEMP_AGENT_DIR"

echo "Launching Pi with temporary Typhoon configuration." >&2

cd "$ROOT"
bash "$PI_TEST" --no-session --model "$MODEL" "$@"