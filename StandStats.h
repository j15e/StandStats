# define DISPLAY_ADDR 0x70
# define STANDING_SENSOR ADC1_CHANNEL_6
// A2 (GPI #34)
# define STANDING_MAX_VOLT 0.70
// < 0.70v for more than 90cm
// To adjust see voltage chart for medium dinstance sensor
// https://cdn-shop.adafruit.com/product-files/1031/GP2Y0A02YK-DATA-SHEET.PDF

# define PRESENCE_SENSOR ADC1_CHANNEL_3
// A3 (GPI #39)
# define PRESENCE_MIN_VOLT 1.25
// > 1.25v for less than 20cm
// To adjust see voltages chart for short distance sensor
// http://www.sharp-world.com/products/device/lineup/data/pdf/datasheet/gp2y0a21yk_e.pdf
