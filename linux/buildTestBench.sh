[ ! -d "bin/" ] && mkdir bin
g++ --std=c++17 -DCARES_STATICLIB -DHAVE_CONFIG_H=1 -Wall -I../dependencies/c-areslinux/ ../src/c-aresppTestBench.cpp -lcares -obin/c-arespp