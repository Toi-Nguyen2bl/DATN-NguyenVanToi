#!/bin/sh

git clone --recurse-submodules https://github.com/Xilinx/Vitis-AI

sudo chmod -R 777 Vitis-AI

cd Vitis-AI
git checkout v1.2.1
docker pull xilinx/vitis-ai:v1.2.1

cd 