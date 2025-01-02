CWD := $(shell pwd)

fob:
	west build -b heltec_wifi_lora32_v3/esp32s3/procpu -s app --pristine -- -DBOARD_ROOT=$(CWD)

show:
	@echo CWD: $(CWD)