@echo off
"C:\Program Files\Atmel\Atmel Studio 6.0\extensions\Atmel\AVRGCC\3.4.0.65\AVRToolchain\bin\avr-gcc.exe" -o GccApplication1.elf  GccApplication1.o   -Wl,-Map="GccApplication1.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group  -mmcu=atmega64  
