
#!/bin/sh

# Deploys a previously built Revise APK to a connected Android device.
#
# Usage:
#   ./deploy/mobile/deploy.sh <path-to-apk>
#
# Requirements:
#   - Android SDK platform-tools (adb)
#   - A connected Android device with USB debugging enabled
#
# The APK path can point to any CMake build configuration, allowing
# the same script to be used for both Debug and Release builds.
#
# If exactly one device is connected, adb selects it automatically.
# If multiple devices are connected, specify the device using ANDROID_SERIAL:
#
#   ANDROID_SERIAL=<device-serial> ./deploy/mobile/deploy.sh <path-to-apk>

set -eu

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <path-to-apk>" >&2
    exit 1
fi

APK_PATH=$(realpath "$1")

if [ ! -f "$APK_PATH" ]; then
    echo "APK not found: $APK_PATH" >&2
    exit 1
fi

ADB=""

if [ -n "${ANDROID_SDK_ROOT:-}" ] &&
   [ -x "$ANDROID_SDK_ROOT/platform-tools/adb" ]; then
    ADB="$ANDROID_SDK_ROOT/platform-tools/adb"
elif [ -n "${ANDROID_HOME:-}" ] &&
     [ -x "$ANDROID_HOME/platform-tools/adb" ]; then
    ADB="$ANDROID_HOME/platform-tools/adb"
else
    ADB=$(command -v adb || true)
fi

if [ -z "$ADB" ] || [ ! -x "$ADB" ]; then
    echo "adb executable not found. Check ANDROID_SDK_ROOT or PATH." >&2
    exit 1
fi

PACKAGE_NAME="org.maksimshchavelev.revise"

echo "Installing APK: $APK_PATH"

adb() {
    if [ -n "${ANDROID_SERIAL:-}" ]; then
        "$ADB" -s "$ANDROID_SERIAL" "$@"
    else
        "$ADB" "$@"
    fi
}

adb install -r "$APK_PATH"
adb shell am force-stop "$PACKAGE_NAME"

adb logcat -c

echo "Launching application..."
adb shell monkey -p "$PACKAGE_NAME" 1

echo "Waiting for application process..."

APP_PID=""

for i in 1 2 3 4 5; do
    APP_PID=$(adb shell pidof -s "$PACKAGE_NAME" | tr -d '\r')

    if [ -n "$APP_PID" ]; then
        break
    fi

    sleep 1
done

if [ -z "$APP_PID" ]; then
    echo "Failed to obtain application PID." >&2
    exit 1
fi

echo "Application PID: $APP_PID"
echo "Application logs:"

adb logcat -v color -s default:V qml:V --pid="$APP_PID"