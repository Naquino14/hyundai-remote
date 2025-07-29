CWD := $(shell pwd)

fob:
	west build -b heltec_wifi_lora32_v3/esp32s3/procpu --sysbuild -s app -p auto -- -DBOARD_ROOT=$(CWD)

trc:
	west build -b heltec_htit_tracker/esp32s3/procpu --sysbuild -s app -p auto -- -DBOARD_ROOT=$(CWD)

flash:
	west flash

mon-fob:
	minicom -D /dev/ttyUSB0 -b 115200

mon-trc:
	minicom -D /dev/ttyACM0 -b 115200

mon:
	@if [ -e /dev/ttyUSB0 ]; then \
		echo "Using /dev/ttyUSB0"; \
		minicom -D /dev/ttyUSB0 -b 115200; \
	elif [ -e /dev/ttyACM0 ]; then \
		echo "Using /dev/ttyACM0"; \
		minicom -D /dev/ttyACM0 -b 115200; \
	else \
		echo "No serial device found."; \
	fi

menuconfig:
	west build -t menuconfig

show:
	@echo CWD: $(CWD)
