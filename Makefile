.PHONY: test fetch-sdk prepare build build-v1 build-v2 package

test:
	./scripts/test.sh

fetch-sdk:
	./scripts/fetch-sdk.sh

prepare:
	./scripts/prepare-targets.sh all

build:
	BUILD_MODE=docker ./scripts/build.sh all

build-v1:
	./scripts/build.sh nutekt-digital minilogue-xd prologue

build-v2:
	./scripts/build.sh nts-1_mkii

package:
	./scripts/package.sh v1.0.0
