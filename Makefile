MCU = atmega328p
F_CPU = 16000000UL
TARGET = main
SRC = $(wildcard src/*.c)

CC = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall

all: $(TARGET).hex

$(TARGET).elf: $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# ---- Upload using Arduino bootloader via USB ----
flash: $(TARGET).hex
	avrdude -c arduino -p m328p -P COM4 -b 115200 -U flash:w:$(TARGET).hex:i

clean:
	rm -f *.elf *.hex