#!/bin/bash
echo "[*] Початок збірки проєкту..."
mkdir -p build
cd build
cmake ..
make -j$(nproc)
echo "[+] Збірку завершено."
