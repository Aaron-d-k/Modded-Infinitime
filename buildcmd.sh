export PATH="$PATH:~/node_modules/.bin"
cmake -DARM_NONE_EABI_TOOLCHAIN_PATH=/usr/ -DNRF5_SDK_PATH=../../nRF5_SDK_15.3.0/ -DCMAKE_BUILD_TYPE=Release -DBUILD_DFU=1 -S ..
make -j4 pinetime-mcuboot-app