#!/bin/sh
# Downloads the latest Trinity mod pk3s from GitHub into the
# Android assets directory for local builds.

REPO="ernie/trinity"
BASE_URL="https://github.com/$REPO/releases/latest/download"

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ASSETS="$SCRIPT_DIR/../android/app/src/main/assets"

echo "Downloading latest Trinity pk3s..."
curl -fL -o "$ASSETS/pak8t.pk3" "$BASE_URL/pak8t.pk3" && echo "  pak8t.pk3 OK" || echo "  pak8t.pk3 FAILED"
curl -fL -o "$ASSETS/pak3t.pk3" "$BASE_URL/pak3t.pk3" && echo "  pak3t.pk3 OK" || echo "  pak3t.pk3 FAILED"
curl -fL -o "$ASSETS/zzz-trinity-announcer.pk3" "$BASE_URL/zzz-trinity-announcer.pk3" && echo "  zzz-trinity-announcer.pk3 OK" || echo "  zzz-trinity-announcer.pk3 FAILED"

echo "Done."
