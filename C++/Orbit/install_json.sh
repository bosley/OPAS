wget https://github.com/nlohmann/json/archive/v3.7.3.tar.gz -O /tmp/json.tar.gz

tar -xzvf /tmp/json.tar.gz -C /tmp

if [ "$CC" = "clang" ]; then export CXX="clang++"; fi && cd /tmp/json* && cmake ./ && make -j4 && sudo make install
