
cd /tmp
git clone https://github.com/joshbosley/BSL

cd BSL
mkdir build

cd build

mkdir core
cd core
cmake ../../bslCore
make -j5
sudo make install

cd ../
mkdir net
cd net
cmake ../../bslNet

make -j5
sudo make install

