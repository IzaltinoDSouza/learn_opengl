#!usr/bin/bash

g++ -std=c++20 glad/glad.c triangle.cpp -o triangle -lglfw -lGL -ldl
