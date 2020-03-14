# Instructions

https://github.com/arduino/arduino-cli

1. curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=/usr/local/bin sh
2. arduino-cli core update-index --additional-urls https://arduino.esp8266.com/stable/package_esp8266com_index.json
3. arduino-cli lib install "DHT sensor library" "Adafruit Unified Sensor" "LiquidCrystal"
4. arduino-cli compile --fqbn esp8266:esp8266:d1 .
5. arduino-cli upload -p /dev/ttyUSB0 --fqbn esp8266:esp8266:d1 .
