# libSDL2.so

Pre-built binary, not built locally. Identifies itself as:

```
version   SDL 2.0.8
revision  hg-11914:f1084c419f33
sha256    068606f413fc122a6e5b61cc5b068cb60a20847197cc4827a018517c0245a02d
```

Re-read at any time with
`strings -a libSDL2.so | grep -oE "SDL2-2\.[0-9.]+|hg-[0-9]+:[0-9a-f]+"`.

Its origin is not recorded and cannot be settled by checksum, since upstream ships source rather than
Android binaries. The Mercurial revision predates SDL's move to git in the 2.0.9 cycle (2.0.8 was
released January 2018), which is consistent with it arriving via the ioquake3 Quest fork this client
descends from.

To replace it, build from source for `arm64-v8a` as `BUILD_LIBOPENAL.md` does for OpenAL Soft, and
record the tag and checksum here. 2.0.8 is old enough that a newer build is not a drop-in certainty —
check the engine's SDL usage against whatever replaces it.

Upstream: https://github.com/libsdl-org/SDL (branch `SDL2`)

## License

zlib — https://github.com/libsdl-org/SDL/blob/SDL2/LICENSE.txt

The license text ships to users in `assets/THIRD-PARTY-NOTICES.txt`.
