#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: run.sh [options] SERVER [SERVER_ARGS...]

Starts SERVER, drives it with oha (https://github.com/hatoo/oha) through each
phase in phases.conf, kills it, and prints a markdown report to stdout.

Options:
  --url URL             Endpoint to hit [default: http://localhost:8080/]
  --duration SECONDS    Length of each phase [default: 10]
  --phases FILE         Phase list [default: phases.conf next to this script]
  --no-pin              Don't split the CPUs between server and client
  -h, --help            Show this help

Requires oha and jq. On Linux with taskset and at least 4 CPUs the server is
pinned to the first half of the CPUs and oha to the second half, so the load
generator's own cost doesn't steal cycles from what is being measured.
USAGE
}

here=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
url="http://localhost:8080/"
duration=10
phases_file="$here/phases.conf"
pin=true

while [[ $# -gt 0 ]]; do
  case "$1" in
    --url) url=$2; shift 2 ;;
    --duration) duration=$2; shift 2 ;;
    --phases) phases_file=$2; shift 2 ;;
    --no-pin) pin=false; shift ;;
    -h | --help) usage; exit 0 ;;
    --) shift; break ;;
    -*) usage >&2; exit 1 ;;
    *) break ;;
  esac
done
[[ $# -gt 0 ]] || { usage >&2; exit 1; }
server=$1
shift

for tool in oha jq; do
  command -v "$tool" >/dev/null || { echo "$tool is required" >&2; exit 1; }
done

server_prefix=()
client_prefix=()
ncpu=$(nproc 2>/dev/null || sysctl -n hw.ncpu)
if [[ $pin == true ]] && command -v taskset >/dev/null && [[ $ncpu -ge 4 ]]; then
  server_prefix=(taskset -c "0-$((ncpu / 2 - 1))")
  client_prefix=(taskset -c "$((ncpu / 2))-$((ncpu - 1))")
fi

host_info() {
  local cpu mem_mb
  if [[ -r /proc/cpuinfo ]]; then
    cpu=$(sed -n 's/^model name\s*: //p' /proc/cpuinfo | head -1)
    mem_mb=$(( $(sed -n 's/^MemTotal:\s*\([0-9]*\) kB/\1/p' /proc/meminfo) / 1024 ))
  else
    cpu=$(sysctl -n machdep.cpu.brand_string)
    mem_mb=$(( $(sysctl -n hw.memsize) / 1024 / 1024 ))
  fi
  echo "# Host Information"
  echo
  echo "- **Platform**: $(uname -s) $(uname -r) ($(uname -m))"
  echo "- **CPU**: ${ncpu}-thread $cpu"
  echo "- **Memory**: ${mem_mb} MB"
  echo "- **Load generator**: $(oha --version)"
  if [[ ${#server_prefix[@]} -gt 0 ]]; then
    echo "- **CPU pinning**: server on ${server_prefix[2]}, oha on ${client_prefix[2]}"
  else
    echo "- **CPU pinning**: none"
  fi
}

# Bash's /dev/tcp is enough to know the server is accepting.
wait_for_server() {
  local host port i
  host=$(sed -E 's#^[a-z]+://([^:/]+).*#\1#' <<<"$url")
  port=$(sed -nE 's#^[a-z]+://[^:/]+:([0-9]+).*#\1#p' <<<"$url")
  for i in $(seq 1 50); do
    if (exec 3<>"/dev/tcp/$host/${port:-80}") 2>/dev/null; then return 0; fi
    sleep 0.1
  done
  echo "server did not open $host:${port:-80}" >&2
  return 1
}

"${server_prefix[@]}" "$server" "$@" >/dev/null 2>&1 &
server_pid=$!
trap 'kill "$server_pid" 2>/dev/null || true' EXIT
wait_for_server

host_info
echo
echo "# Benchmarks"
echo
echo "Using server \`$(basename "$server")\`, each phase runs for ${duration}s against \`$url\`."
echo
echo "| Phase | Connections | Rate | Req/s | Success | p50 ms | p90 ms | p99 ms | p99.9 ms | Max ms |"
echo "|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|"

errors=()
mapfile -t phases < <(grep -vE '^\s*(#|$)' "$phases_file")
for phase in "${phases[@]}"; do
  read -r name connections rate extra <<<"$phase"
  args=(-z "${duration}s" -c "$connections" --no-tui --output-format json)
  if [[ $rate -gt 0 ]]; then args+=(-q "$rate" --latency-correction); fi
  # shellcheck disable=SC2206 # extra is intentionally word-split into flags
  args+=(${extra:-})

  result=$("${client_prefix[@]}" oha "${args[@]}" "$url")
  jq -r --arg name "$name" --arg conns "$connections" --arg rate "$rate" '
    def ms: . * 1000 | . * 1000 | round / 1000;
    "| \($name) | \($conns) | \(if $rate == "0" then "max" else $rate end)"
    + " | \(.summary.requestsPerSec | round)"
    + " | \(.summary.successRate * 100 | . * 10 | round / 10)%"
    + " | \(.latencyPercentiles.p50 | ms)"
    + " | \(.latencyPercentiles.p90 | ms)"
    + " | \(.latencyPercentiles.p99 | ms)"
    + " | \(.latencyPercentiles["p99.9"] | ms)"
    + " | \(.summary.slowest | ms) |"' <<<"$result"

  # Every phase is cut off mid-flight by -z, so the deadline abort is expected.
  phase_errors=$(jq -r --arg name "$name" '
    (.statusCodeDistribution | to_entries | map(select(.key != "200")))
    + (.errorDistribution | to_entries | map(select(.key != "aborted due to deadline")))
    | map("\(.key): \(.value)") | join(", ")
    | select(. != "") | "- **\($name)**: \(.)"' <<<"$result")
  if [[ -n $phase_errors ]]; then errors+=("$phase_errors"); fi
done

if [[ ${#errors[@]} -gt 0 ]]; then
  echo
  echo "## Non-200 responses and errors"
  echo
  printf '%s\n' "${errors[@]}"
fi
