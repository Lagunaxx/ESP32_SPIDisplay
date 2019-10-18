#/bin/bash

#########################################################################################
#
# Name:   install_fullenv.sh
#
# Info:   Script installs environment for building projects for SPIDisplays with hardware like esp32, arduino, etc...
#
# Author: Vitaliy Novoselov
#
# URI:    https://github.com/lagunax/SPIDisplay
#
#########################################################################################

if [ -f "tmp.h" ]
then 
        rm tmp.h
fi
if [ -f "tmp.h" ]
then 
        rm ttmp.h
fi

if [ -f "CMakeLists.txt" ]
then

# cd into arduino folder
#cd pwd

echo Adding ESP32-SPIDisplay to Library list...

cp CMakeLists.txt tmp.txt
src_line=$(grep -Hn "set(LIBRARY_SRCS" ./tmp.txt | sed 's/\(:[^:]*\)\{1\}$//' | sed 's/.*://')
tmp_s=$(bc -l <<<$src_line'+1')
sed $tmp_s' i   libraries/ESP32-SPIDisplay/ESP32-SPIDisplay.cpp' tmp.txt >> ttmp.txt

rm tmp.txt
mv ttmp.txt tmp.txt

###################################################################################################

echo Adding ESP32-SPIDisplay to Includes list...

	lib_line=$(grep -Hn "set(COMPONENT_ADD_INCLUDEDIRS" ./tmp.txt | sed 's/\(:[^:]*\)\{1\}$//' | sed 's/.*://')
	tmp_l=$(bc -l<<<$lib_line'+1')
	sed $tmp_l' i   libraries/ESP32-SPIDisplay' tmp.txt >> ttmp.txt

	rm tmp.txt

	#writing changes
	if [ -f "CMakeLists.txt.backup" ]
	then
	        rm CMakeLists.txt.backup
	fi

	mv CMakeLists.txt CMakeLists.txt.backup
	mv ttmp.txt CMakeLists.txt
else
	echo "   CMakeLists.txt - not found!"
fi

###################################################################################################

echo Adding ESP32-SPIDisplay to menuconfig...

if [ -f "Kconfig.projbuild" ]
then
	cp Kconfig.projbuild tmp.txt
	src_line=$(grep -Hn "config ARDUINO_SELECTIVE_SPIFFS" ./tmp.txt | sed 's/\(:[^:]*\)\{1\}$//' | sed 's/.*://')
	tmp=$(tail -n +$src_line ./tmp.txt | grep -Hn "default" | head -n +1 | sed 's/\(:[^:]*\)\{1\}$//' | sed 's/.*://' | sed 's/* //')
	dst_line=$(bc -l <<<$src_line'+1+'$tmp)
	sed $dst_line' i config ARDUINO_SELECTIVE_ESP32-SPIDisplay\n    bool "Enable ESP32-SPIDisplay"\n    depends on ARDUINO_SELECTIVE_COMPILATION\n    select ARDUINO_SELECTIVE_SPI\n    select ARDUINO_SELECTIVE_SPIFFS\n    default y\n' tmp.txt >> ttmp.txt
	rm tmp.txt

	#writing changes
	if [ -f "Kconfig.projbuild.backup" ]
	then
	        rm Kconfig.projbuild.backup
	fi

	mv Kconfig.projbuild Kconfig.projbuild.backup
	mv ttmp.txt Kconfig.projbuild

else
	echo "   Kconfig.projbuild - not found!"
fi

#cd $IDF_PATH
#cd ..
#git clone -b crosstool-ng-1.22.0 https://github.com/espressif/crosstool-NG
#cd crosstool-NG
#./bootstrap
#./configure --enable-local 
#make install
#./ct-ng xtensa-esp32-elf
