#!/usr/bin/env bash
set -euo pipefail

# ------------------------------------------------------------------
# run-clang-checks.sh
#
# Run clang-tidy on the specified directories.
#
# Usage:
#   ./run-clang-checks.sh -s <source_dirs> -b <build_dir>
#   ./run-clang-checks.sh --help
# ------------------------------------------------------------------

function print_help() {
    cat <<EOF
Usage: $0 -s <source_dirs> -b <build_dir>

Options:
  -s, --source     Comma-separated list of directories to analyze
  -b, --build      Path to the build directory (compile_commands.json must be here)
  -h, --help       Show this help message

Example:
  $0 -s app,modules -b build/Qt_6_10_1_desktop-Release
EOF
}

# Parse arguments
SOURCE_DIRS=""
BUILD_DIR=""

while [[ $# -gt 0 ]]; do
    case $1 in
        -s|--source)
            SOURCE_DIRS="$2"
            shift 2
            ;;
        -b|--build)
            BUILD_DIR="$2"
            shift 2
            ;;
        -h|--help)
            print_help
            exit 0
            ;;
        *)
            echo "Unknown argument: $1"
            print_help
            exit 1
            ;;
    esac
done

if [[ -z "$SOURCE_DIRS" || -z "$BUILD_DIR" ]]; then
    echo "Error: --source and --build must be specified"
    print_help
    exit 1
fi

# Convert comma-separated dirs to space-separated
DIR_LIST=$(echo "$SOURCE_DIRS" | tr ',' ' ')

echo "Running clang-tidy on: $DIR_LIST"
echo "Using build directory: $BUILD_DIR"

# Run clang-tidy
find $DIR_LIST -name '*.cpp' -print0 | \
xargs -0 run-clang-tidy-19 \
  -p "$BUILD_DIR" \
  -use-color \
  -extra-arg=-fdiagnostics-color=always

