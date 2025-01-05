CWD := $(shell pwd)

boot_fob:
	west build -b heltec_wifi_lora32_v3/esp32s3/procpu ~/zephyrproject/bootloader/mcuboot/boot/zephyr --pristine -- -DBOARD_ROOT=$(CWD)

fob:
	west build -b heltec_wifi_lora32_v3/esp32s3/procpu --sysbuild -s app --pristine -- -DBOARD_ROOT=$(CWD)

flash:
	west flash

mon:
	minicom -D /dev/ttyUSB0 -b 115200

show:
	@echo CWD: $(CWD)
