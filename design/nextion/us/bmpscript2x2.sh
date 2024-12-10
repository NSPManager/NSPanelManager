#!/bin/bash
echo "Hello, World"
convert '*.bmp' -set filename:fn '%[basename]' -ordered-dither o2x2,32,64,32 bmp/'%[filename:fn].bmp'

#convert /home/erik/Documents/Projekt/NSPanel\ Manager/Design/automation/eu/*.bmp -ordered-dither o2x2,32,64,32 /home/erik/Documents/Projekt/NSPanel\ Manager/Design/automation/eu/bmp/*.bmp
