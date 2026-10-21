#!/usr/bin/env bash
# Regenerate CubeMX initialization code from the project's .ioc file.
set -euo pipefail

project_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
cubemx_bin=${STM32CUBEMX:-stm32cubemx}
if ! command -v "$cubemx_bin" >/dev/null 2>&1; then
  printf 'CubeMX executable not found: %s\nSet STM32CUBEMX to its executable path.\n' "$cubemx_bin" >&2
  exit 1
fi

mx_script=$(mktemp "${TMPDIR:-/tmp}/balance-vehicle-cubemx.XXXXXX")
trap 'rm -f -- "$mx_script"' EXIT
printf '%s\n' \
  "config load \"$project_dir/Balance-vehicle.ioc\"" \
  "project path \"$project_dir\"" \
  'project generate' \
  'exit' > "$mx_script"

"$cubemx_bin" -q "$mx_script"
