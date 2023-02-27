#!/bin/bash
# Render image sequence to an mp4

ffmpeg -v -i 'img_%4d.bmp' -framerate 10 -r 10 -c:v libx264 -preset slow -crf 21 $1
