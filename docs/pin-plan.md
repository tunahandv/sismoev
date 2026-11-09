# Pin plan — ESP32-WROOM-32D DevKit

All grounds are common. 3V3 = ESP32 3.3 V output. 5V = UPS holder output rail.

| Signal | ESP32 pin | Connection | Notes |
|---|---|---|---|
| I2C SDA | GPIO21 | MPU6050, ADXL345, OLED (shared) | |
| I2C SCL | GPIO22 | MPU6050, ADXL345, OLED (shared) | |
| MPU6050 | I2C 0x68 | VCC→3V3, GND, AD0→GND | screw tight to the board |
| ADXL345 (GY-291) | I2C 0x53 | VCC→3V3, GND, **CS→3V3**, **SDO→GND** | CS must be high for I2C mode |
| OLED SSD1306 | I2C 0x3C | VCC→3V3, GND | |
| MPU6050 INT | GPIO19 | optional | |
| SW-18020P vibration switch | GPIO18 | one leg → GPIO18, other leg → GND | `INPUT_PULLUP` |
| Relay IN | GPIO26 | relay VCC→5V, COM←5V, NO→fan(+) | fan stands in for the cut-off circuit |
| Emergency LED (MOSFET gate) | GPIO25 | 220 Ω to gate, 10 kΩ gate→GND | IRLZ44N; strip (−) → drain |
| Buzzer module | GPIO27 | VCC→5V | active buzzer module |
| Green LED | GPIO16 | 220–330 Ω to GND | normal |
| Yellow LED | GPIO17 | 220–330 Ω to GND | verifying / offline / on battery |
| Red LED | GPIO4 | 220–330 Ω to GND | alarm |
| Button: restore power | GPIO32 | other leg → GND | `INPUT_PULLUP` |
| Button: test mode | GPIO33 | other leg → GND | `INPUT_PULLUP` |
| Button: mute | GPIO14 | other leg → GND | `INPUT_PULLUP` |
| Mains detect | GPIO35 (ADC1) | charger 5V → 10 kΩ → pin → 20 kΩ → GND | 20 kΩ = two 10 kΩ in series |
| Relay-state feedback | GPIO34 (ADC1) | relay NO → 10 kΩ → pin → 20 kΩ → GND | |
| Battery voltage | GPIO36 (ADC1) | 18650 (+) → 10 kΩ → pin → 20 kΩ → GND | optional |

Why these pins: analog inputs use ADC1 (ADC2 stops working while Wi-Fi is on); strapping pins
0, 2, 5, 12 and 15 are avoided so the board always boots.
