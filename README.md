# gar
find transmitter locations using a Gps logger And Rtl_power

Gps and Rtl_power, or gar, is a command line utility for generating Google Earth .kml files with the locations of broadcast sources.

It can be downloaded and built traditionally, with cmake:
``` sh
git clone https://github.com/DomMarcone/gar
cd gar
mkdir build
cd build
cmake ..
```
And from there, it depends on your OS. In Unix-like operating systems, run `make`. In Windows, open the gar.sln.

Once the executable is built, you can generate a .kml file by running gar -g 'gps_file.csv' -r 'output_from_rtl_power.csv'

Other parameters are 
```-o 'output_file.kml'``` sets the output file
```-n 40``` noise floor cutoff
```-t 4``` number of threads
```-h``` show the help screen

by Dominic Marcone