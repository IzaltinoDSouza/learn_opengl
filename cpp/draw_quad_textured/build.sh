#!usr/bin/bash

g++ -std=c++20 glad/glad.c quad.cpp -o quad -lglfw -lGL -ldl
