#!/usr/bin/env bash
# Run cppcheck on specified directories with optional exclusions
# Supports both short and long options

set -euo pipefail

show_help() {
    cat << EOF
Usage: $0 -s <source_dirs> [-e <exclude_dirs>] 
       $0 --sources <source_dirs> [--exclude <exclude_dirs>] 
       $0 -h | --help

  -s, --sources   Comma-separated list of directories to analyze (required)
  -e, --exclude   Comma-separated list of directories to exclude (optional)
  -h, --help      Show this help message
EOF
}

# Default values
SRC_DIRS=""
EXCLUDE_DIRS=""

# Parse args manually to support long options
while [[ $# -gt 0 ]]; do
    case "$1" in
        -s|--sources)
            SRC_DIRS="$2"
            shift 2
            ;;
        -e|--exclude)
            EXCLUDE_DIRS="$2"
            shift 2
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

if [[ -z "$SRC_DIRS" ]]; then
    echo "Error: source directories must be specified with -s or --sources"
    show_help
    exit 1
fi

# Convert comma-separated lists to space-separated for find
SRC_DIRS_LIST=${SRC_DIRS//,/ }
EXCLUDE_ARGS=""
if [[ -n "$EXCLUDE_DIRS" ]]; then
    for dir in ${EXCLUDE_DIRS//,/ }; do
        EXCLUDE_ARGS="$EXCLUDE_ARGS -i $dir"
    done
fi

echo "Running cppcheck on: $SRC_DIRS_LIST"
[[ -n "$EXCLUDE_DIRS" ]] && echo "Excluding: $EXCLUDE_DIRS"

# Run cppcheck with nice defaults
cppcheck \
    --enable=all \
    --inconclusive \
    --std=c++23 \
    --force \
    --suppress=unusedFunction \
    --suppress=missingInclude \
    --suppress=missingIncludeSystem \
    --quiet \
    $EXCLUDE_ARGS $SRC_DIRS_LIST
