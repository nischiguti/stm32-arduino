#!/bin/bash

# ========= Configuration ==========
SKETCH_NAME="$1"
SKETCH_DIR="$HOME/sketches/$SKETCH_NAME"
FQBN="STMicroelectronics:stm32:GenF1:pnum=GENERIC_F103C8TX,usb=CDC"
PORT="/dev/ttyUSB0"
BAUD="115200"
OUTPUT_DIR="$SKETCH_DIR"
BIN_NAME="${SKETCH_NAME}.ino.bin"
BIN_PATH="$OUTPUT_DIR/$BIN_NAME"
FLASH_ADDR="0x08000000"
# ===================================

if [[ -z "$SKETCH_NAME" ]]; then
  echo "Usage: $0 <sketch_folder_name>"
  exit 1
fi

if [[ ! -d "$SKETCH_DIR" ]]; then
  echo "❌ Sketch folder not found at $SKETCH_DIR"
  exit 2
fi

# Step 1: Compile the sketch
echo "🔨 Compiling sketch: $SKETCH_NAME..."
arduino-cli compile --fqbn "$FQBN" --output-dir "$OUTPUT_DIR" "$SKETCH_DIR"
if [[ $? -ne 0 ]]; then
  echo "❌ Compilation failed"
  exit 3
fi

# Step 2: Confirm binary exists
if [[ ! -f "$BIN_PATH" ]]; then
  echo "❌ Binary not found: $BIN_PATH"
  exit 4
fi

# Step 3: Prompt for manual reset before flash
read -p "🔁 Set BOOT0=1 and press RESET, then press Enter to flash..."

# Step 4: Flash using STM32_Programmer_CLI
echo "⚡ Flashing via UART on $PORT..."
STM32_Programmer_CLI -c port="$PORT" -d "$BIN_PATH" "$FLASH_ADDR" -v
if [[ $? -ne 0 ]]; then
  echo "❌ Flashing failed"
  exit 5
fi

echo "✅ Flash completed. Set BOOT0=0 and press RESET to run your sketch."

