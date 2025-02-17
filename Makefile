CWD := $(shell pwd)

fob:
	west build -b heltec_wifi_lora32_v3/esp32s3/procpu --sysbuild -s app -p auto -- -DBOARD_ROOT=$(CWD)

oled:
	west build -b heltec_wifi_lora32_v3/esp32s3/procpu --sysbuild -s ~/zephyrproject/zephyr/samples/subsys/display/lvgl -p auto -- -DBOARD_ROOT=/home/naquino14/proj/mbedded-proj/hyundai-remote

flash:
	west flash

mon:
	minicom -D /dev/ttyUSB0 -b 115200

show:
	@echo CWD: $(CWD)
