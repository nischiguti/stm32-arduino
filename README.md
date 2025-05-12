# flash_bluepill.sh

A minimal shell script to compile and flash Arduino sketches to an STM32F103C8T6 ("Blue Pill") board using a USB-to-UART adapter and the STM32CubeProgrammer CLI.

---

## 📦 Requirements

- [Arduino CLI](https://arduino.github.io/arduino-cli/installation/)
- [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html)
- STM32 Arduino core installed:
  ```bash
  arduino-cli core install STMicroelectronics:stm32
  ```
- Sketches stored in `~/.sketches/<SketchName>/<SketchName>.ino`
- USB-UART adapter connected to PA9/PA10 on the STM32

---

## 🔧 Setup

```bash
mkdir -p ~/.local/bin
mv flash_bluepill.sh ~/.local/bin/
chmod +x ~/.local/bin/flash_bluepill.sh
```

Add to your `PATH`:
```bash
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

---

## 🚀 Usage

Make sure the STM32 board is in **bootloader mode**:

1. Set `BOOT0 = 1`
2. Press the `RESET` button
3. Run:

```bash
flash_bluepill.sh <SketchName>
```

Example:

```bash
flash_bluepill.sh HelloUART
```

After flashing:
- Set `BOOT0 = 0`
- Press `RESET` to run the program

---

## 📌 Notes

- Sketch must match the folder and file name: `~/.sketches/MySketch/MySketch.ino`
- USB CDC/DFU flashing is **not supported** by this script
- Uses: `GENERIC_F103C8TX` as FQBN target

---

## 📄 License

MIT License
