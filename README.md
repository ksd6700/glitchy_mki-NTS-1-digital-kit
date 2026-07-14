# GLITCHY — a KORG logue SDK modulation effect

> Cut, reverse, crush, freeze, and gate the last few milliseconds of sound.
> GLITCHY turns two knobs into a controlled digital accident.

[日本語](#日本語) · [English](#english)

## Quick downloads

The **NTS-1 digital kit mkI** file is here:
**[Download `glitchy.ntkdigunit`](https://github.com/ksd6700/glitchy_mki-NTS-1-digital-kit/releases/latest/download/glitchy.ntkdigunit)**

| Synth | Ready-to-load unit | Minimum firmware |
|---|---|---:|
| Nu:Tekt NTS-1 digital kit **mkI** | [Download `.ntkdigunit`](https://github.com/ksd6700/glitchy_mki-NTS-1-digital-kit/releases/latest/download/glitchy.ntkdigunit) | 1.02 |
| minilogue xd / xd module | [Download `.mnlgxdunit`](https://github.com/ksd6700/glitchy_mki-NTS-1-digital-kit/releases/latest/download/glitchy.mnlgxdunit) | 2.00 |
| prologue | [Download `.prlgunit`](https://github.com/ksd6700/glitchy_mki-NTS-1-digital-kit/releases/latest/download/glitchy.prlgunit) | 2.00 |
| Nu:Tekt NTS-1 digital kit mkII | [Download `.nts1mkiiunit`](https://github.com/ksd6700/glitchy_mki-NTS-1-digital-kit/releases/latest/download/glitchy.nts1mkiiunit) | 1.0.0 |
| microKORG2 | [Download `.mk2unit`](https://github.com/ksd6700/glitchy_mki-NTS-1-digital-kit/releases/latest/download/glitchy.mk2unit) | 2.0.0 |

[Download every build as one ZIP](https://github.com/ksd6700/glitchy_mki-NTS-1-digital-kit/releases/latest/download/glitchy-v1.0.0-all-logue-synths.zip) · [Release page](https://github.com/ksd6700/glitchy_mki-NTS-1-digital-kit/releases/latest)

The same v1.0.0 binaries are also committed under [`dist/v1.0.0`](./dist/v1.0.0) for a source-pinned download. SHA-256 hashes are included.

---

## 日本語

GLITCHYは、直前の音を短く切り取り、スタッター、リバース、マイクロループ、ハーフスピード、ビットクラッシュ、サンプルホールド、ランダムゲートを組み合わせるモジュレーション・エフェクトです。NTS-1 digital kit mkIを中心に、同じ音のキャラクターをKORG logue SDK対応シンセへ移植しています。

### 操作

| パラメーター | 内容 |
|---|---|
| **A / TIME** | 切り取る長さを約12–85 msで変更。左ほど細かく、右ほど大きく崩れます。 |
| **B / CHAOS** | エフェクトの発生率、ウェット量、モード、ビット深度、ゲート密度をまとめて変更。0では原音です。 |

CHAOSを少し上げると時々スタッターが入り、中央付近ではリバースやフリーズが混ざり、最大付近では4-bitクラッシュと細かなゲートまで加わります。切り替わりには短いクロスフェードを入れ、不要な大音量クリックを抑えています。

### NTS-1 mkIへの入れ方

1. 上の [`glitchy.ntkdigunit`](https://github.com/ksd6700/glitchy_mki-NTS-1-digital-kit/releases/latest/download/glitchy.ntkdigunit) をダウンロードします。
2. KORG NTS-1 Sound Librarianを開きます。
3. ファイルを **USER MODULATION FX** の空きスロットへドラッグします。
4. LibrarianからNTS-1へ送信します。
5. NTS-1のMODセクションで `glitchy` を選び、A/Bノブで操作します。

他機種では、対応するKORG Librarian／EditorまたはmicroKORG2のUSB Mass Storageモードを使い、上表の機種別ファイルをユーザーmod FXとして読み込んでください。

### ビルド

公式Docker環境がいちばん再現しやすい方法です。

```sh
./scripts/fetch-sdk.sh
BUILD_MODE=docker ./scripts/build.sh all
./scripts/package.sh v1.0.0
```

macOSでは、公式ARMツールチェーンをSDKの `tools/gcc` に導入したあと、v1系とNTS-1 mkIIをネイティブビルドできます。

```sh
make build-v1
make build-v2
```

microKORG2は公式Dockerビルド環境が必要です。SDKは再現性のためKORG logue SDK commit `c224d07` に固定しています。

---

## English

GLITCHY is a modulation effect that keeps a short rolling history of the input, then rearranges it into straight stutters, reverse slices, micro-loops, and half-speed lurches. As CHAOS rises, it also reduces bit depth, adds sample-and-hold grit, and punches rhythmic holes with a random gate.

The project started with the original Nu:Tekt NTS-1 digital kit and carries the same DSP core to every currently supported logue SDK synth target.

### Controls

| Parameter | What it does |
|---|---|
| **A / TIME** | Sets the captured slice from roughly 12 to 85 ms. Turn left for tight digital buzz; turn right for broader stutters. |
| **B / CHAOS** | Morphs glitch probability, wet level, playback mode, bit depth, sample hold, and gate density. At zero the signal is dry. |

Low CHAOS adds occasional repeats. Around the middle, reverse and frozen fragments start to appear. Near maximum, the signal can fall to 4-bit resolution and break into sparse gated shards. Short edge crossfades control accidental full-scale clicks without sanding away the glitch character.

### Install on NTS-1 mkI

1. Download [`glitchy.ntkdigunit`](https://github.com/ksd6700/glitchy_mki-NTS-1-digital-kit/releases/latest/download/glitchy.ntkdigunit).
2. Open KORG NTS-1 Sound Librarian.
3. Drag the file into an empty **USER MODULATION FX** slot.
4. Send the user data to the NTS-1.
5. Select `glitchy` in the MOD section and play with knobs A and B.

For the other synths, load the matching file from the download table with that instrument's supported KORG Librarian/Editor. microKORG2 also supports USB Mass Storage transfer.

### Build from source

The official Docker environment is the reproducible path and is also used by GitHub Actions:

```sh
./scripts/fetch-sdk.sh
BUILD_MODE=docker ./scripts/build.sh all
./scripts/package.sh v1.0.0
```

On macOS, after installing KORG's recommended ARM toolchains into the SDK's `tools/gcc` directory, the v1 targets and NTS-1 mkII can be built natively:

```sh
make build-v1
make build-v2
```

microKORG2 requires the official Docker build environment. For reproducibility, the scripts pin KORG logue SDK commit `c224d07`.

## Supported builds and architecture

| SDK family | Targets | Format | Shared DSP |
|---|---|---|---|
| logue SDK 1.1 | NTS-1 mkI, minilogue xd, prologue | Packaged Cortex-M4 unit | Stereo, plus prologue main/sub timbres |
| logue SDK 2.0 | NTS-1 mkII | ARM Cortex-M7 shared object | Stereo |
| logue SDK 2.1 | microKORG2 | ARM Cortex-A7 shared object | Stereo |

The audio callback performs no heap allocation. A 64 KiB rolling buffer lives in the external effect memory. The portable DSP is tested on the host before every target build. The release files are cross-compiled and format-validated; reports from different hardware and musical edge cases are very welcome.

## Development

```sh
./scripts/test.sh                    # host DSP behavior and safety checks
./scripts/prepare-targets.sh all     # overlay GLITCHY onto official templates
BUILD_MODE=docker ./scripts/build.sh all
```

Source layout:

- `dsp/glitch_core.hpp` — allocation-free portable DSP.
- `overlays/v1` — logue SDK 1.1 wrapper shared by the three binary-compatible targets.
- `overlays/nts-1_mkii` — SDK 2.0 unit definition and wrapper.
- `overlays/microkorg2` — SDK 2.1 unit definition and wrapper.
- `scripts` — pinned SDK fetch, target preparation, build, test, and packaging.

Contributions, bug reports, new glitch modes, and hardware test notes are welcome.

## License and trademark note

GLITCHY's original source is released under the [MIT License](./LICENSE). The build downloads KORG's separately licensed logue SDK and starts from its official templates; the SDK is not vendored here.

This is an independent community project and is not affiliated with or endorsed by KORG. KORG, Nu:Tekt, NTS-1, minilogue, prologue, microKORG, and logue are trademarks of their respective owner. Back up your user data and use third-party units at your own risk.
