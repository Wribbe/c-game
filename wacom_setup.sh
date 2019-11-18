#!/bin/sh


[ $# -gt 0 ] || { echo "Usage: $0 --desktop/--laptop"; exit -1; }



[ ! $1 = "--desktop" ] || xsetwacom set "Wacom Intuos4 6x9 Pen stylus" MapToOutput HDMI-A-0

IFS='
'
pad="Wacom Intuos4 6x9 Pad pad"

devices="\
Wacom Intuos4 6x9 Pen stylus
Wacom Intuos4 6x9 Pad pad
Wacom Intuos4 6x9 Pen eraser
Wacom Intuos4 6x9 Pen cursor
"

for d in $devices; do
  xsetwacom set $d rotate half
done

xsetwacom set $pad Button 11 "key tab"          # Maximize canvas.
xsetwacom set $pad Button 10 "key +ctrl z"      # Undo.
# Touch ring.
xsetwacom set $pad Button 9 "key ctrl"          # Color-pick.
xsetwacom set $pad Button 8 "key +shift +space" # Rotate.
xsetwacom set $pad Button 3 "key /"             # Toggle A/B tool.
xsetwacom set $pad Button 2 "key shift"         # Change cursor.
