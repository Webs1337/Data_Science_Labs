#!/bin/bash
echo "[*] Встановлення залежностей..."
sudo apt-get update
sudo apt-get install -y build-essential cmake git libopencv-dev
echo "[+] Залежності успішно встановлено."
