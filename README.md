# Infitime - with extra apps!
This is a version of Infinitime I made for myself with some custom apps etc. for fun.
Here is the list of things that have been added:
 - Litany of Fear reader with a sandworm.
 - Conway's Game of Life (along with 2 other CAs I think look cool) on screen. 
   Does not cheat, runs a full 240x240 board (i.e. 1 pixel = 1 cell)
   This is not easy due to memory constraints (even in the most compressed form, it takes 7 KB/64 KB total) and uses bitwise techniques to run cgol with 32 cells in parallel. Also had to manually control display bypassing LVGL as there is not enough memory to store a full canvas.

<br>
<hr>
<br>

<div align="center">

![Header Image](doc/logo/watchface_collage.png)

<br>

# InfiniTime

*Fast open-source firmware for the [PineTime smartwatch](https://pine64.org/devices/pinetime/) with many features, written in modern C++.*

<br>

</div>