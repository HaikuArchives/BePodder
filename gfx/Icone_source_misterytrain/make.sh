#!/bin/sh

cd $(dirname $0)

mkdir distro

for a in kind*; do
widget=$(echo "$a" | sed 's/.svg//g' | sed 's/kind-//g')
for b in logo*; do
type=$(echo "$b" | sed 's/.svg//g' | sed 's/logo-//g')
for c in emblem*; do
name=$(echo "$c" | sed 's/.svg//g' | sed 's/emblem-//g')
file="distro/$name-$type-$widget.svg"

if [ "$widget" == "button" ]
then
echo "<svg width=\"96\" height=\"64\" viewBox=\"0 0 96 64\">" >  "$file"
else
echo "<svg width=\"64\" height=\"64\" viewBox=\"0 0 64 64\">" >  "$file"
fi

cat "$a" | sed '/svg/d' >> "$file"
if [ "$widget" == "button" ]
then
echo "<g opacity=\"0.25\" transform=\"matrix(1.00 0.00 0.00 1.00 16.00 20.00)\">" >> "$file"
else
echo "<g opacity=\"0.25\" transform=\"matrix(1.00 0.00 0.00 1.00 16.00 28.00)\">" >> "$file"
fi
cat "$b" | sed '/svg/d' >> "$file"
echo "</g>" >> "$file"
if [ "$widget" == "button" ]
then
echo "<g transform=\"matrix(1.00 0.00 0.00 1.00 52.00 16.00)\">" >> "$file"
else
echo "<g transform=\"matrix(1.00 0.00 0.00 1.00 32.00 0.00)\">" >> "$file"
fi
cat "$c" | sed '/svg/d' >> "$file"
echo "</g>" >> "$file"
echo "</svg>" >> "$file"
echo "$file - FATTO"
done
done
done