# am-pll-trx
Modifying and using a POTATO AM transceiver kit

This kit comes in two frequency versions.
 type1 26.965-27.405MHz for 11m(CB)
 type2 28.400-29.300MHz for 10m(HAM)

However, this frequency is not used for 10m band AM in Japan.
I can't program the original board, I control it with a PIC microcontroller.
This board needs to controlling Si5351A of VFO and FM62429 of AF volume.

The original board uses a PIO controlled LED to display the frequency,
but this board has been changed to an I2C controlled LCD using ST7032i.
