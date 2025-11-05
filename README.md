# 🌌 SusanooOS Demo Series

SusanooOS は「神話と技術の融合」をテーマに開発中の独自OSプロジェクトです。  
このリポジトリはその **グラフィカル・シェル試作シリーズ（v1〜v3）** をまとめたデモ集です。

---

## 🧩 バージョン構成

| バージョン | ファイル | 概要 |
|-------------|-----------|------|
| v1 | `susanoo_demo.c` | 初期版。SDL2によるウィンドウ描画テスト。 |
| v2 | `susanoo_demo_gui.c` | GUIランチャー試作版（ボタン3種・クリック反応付き）。 |
| v3 | `susanoo_window_demo.c` | ウィンドウマネージャ試作（ドラッグ移動可能ウィンドウ2つ）。 |

---

## 🛠️ ビルド手順（Debian / Ubuntu）

```bash
sudo apt install -y build-essential libsdl2-dev libsdl2-ttf-dev
gcc susanoo_demo.c -o susanoo_demo -lSDL2
gcc susanoo_demo_gui.c -o susanoo_demo_gui -lSDL2 -lSDL2_ttf
gcc susanoo_window_demo.c -o susanoo_window_demo -lSDL2 -lSDL2_ttf

