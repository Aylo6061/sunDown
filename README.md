# sunDown
Color temperature tunable lamp.

# Programming over USB
This program pulls the wifi credentials from a file stored in SPIFFS.  This file should be called "creds.txt" and needs to be added to /data.  The contents should be something like:

	{"ssid":"my_network_name", "pwd":"my_secret_password"}

This file is not included, for obvious reasons.  Once this file is in place, proceed to the next step.

Open sunDown.ino in the arduino IDE, and build/upload the code with ctrl+u or the upload button. Once this is done, use the [ESP32 sketch data upload](https://github.com/esp8266/arduino-esp8266fs-plugin) to upload the data folder.

This firmware uses [elegant ota](https://github.com/ayushsharma82/ElegantOTA). Refer to documentation there on running an OTA update.