#!/bin/bash
# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color (reset)

echo -e "${GREEN}Starting automated EU tft picture production${NC}"

echo -e "${GREEN}Producing pictures for tft1${NC}"
gimp -i -b '(nspanel-eu-tft1 "./tft_EU.xcf")' -b '(gimp-quit 0)' &
gimp_pid=$!

spin='-\|/'; i=0
echo -n ""
while kill -0 $gimp_pid 2> /dev/null; do
    i=$(( (i+1) %4 ))
    printf "\b${spin:$i:1}"
    sleep 0.1
done
echo -e "\bDone!"

echo -e "${GREEN}Producing pictures for tft2${NC}"
gimp -i -b '(nspanel-eu-tft2 "./tft_EU.xcf")' -b '(gimp-quit 0)' &
gimp_pid=$!

spin='-\|/'; i=0
echo -n ""
while kill -0 $gimp_pid 2> /dev/null; do
    i=$(( (i+1) %4 ))
    printf "\b${spin:$i:1}"
    sleep 0.1
done
echo -e "\bDone!"

echo -e "${GREEN}Producing pictures for tft3${NC}"
gimp -i -b '(nspanel-eu-tft3 "./tft_EU.xcf")' -b '(gimp-quit 0)' &
gimp_pid=$!

spin='-\|/'; i=0
echo -n ""
while kill -0 $gimp_pid 2> /dev/null; do
    i=$(( (i+1) %4 ))
    printf "\b${spin:$i:1}"
    sleep 0.1
done
echo -e "\bDone!"

echo -e "${GREEN}Producing pictures for tft4${NC}"
gimp -i -b '(nspanel-eu-tft4 "./tft_EU.xcf")' -b '(gimp-quit 0)' &
gimp_pid=$!

spin='-\|/'; i=0
echo -n ""
while kill -0 $gimp_pid 2> /dev/null; do
    i=$(( (i+1) %4 ))
    printf "\b${spin:$i:1}"
    sleep 0.1
done
echo -e "\bDone!"

#echo -e "${GREEN}Removing old pictures${NC}"
#rm ./tft1/bmp/*.bmp;
#rm ./tft2/bmp/*.bmp;
#rm ./tft3/bmp/*.bmp;
#rm ./tft4/bmp/*.bmp;

#rm /home/erik/Public/eu_automation_pictures/pictures_tft1/*.bmp;
#rm /home/erik/Public/eu_automation_pictures/pictures_tft2/*.bmp;
#rm /home/erik/Public/eu_automation_pictures/pictures_tft3/*.bmp;
#rm /home/erik/Public/eu_automation_pictures/pictures_tft4/*.bmp;

echo -e "${GREEN}Converting tft1 pictures to specific bmp settings${NC}"
convert 'pictures_tft1/*.bmp' -set filename:fn '%[basename]' -ordered-dither o2x2,32,64,32 pictures_tft1/dithered_numbered/'%[filename:fn].bmp'
echo -e "${GREEN}Converting tft2 pictures to specific bmp settings${NC}"
convert 'pictures_tft2/*.bmp' -set filename:fn '%[basename]' -ordered-dither o2x2,32,64,32 pictures_tft2/dithered_numbered/'%[filename:fn].bmp'
echo -e "${GREEN}Converting tft3 pictures to specific bmp settings${NC}"
convert 'pictures_tft3/*.bmp' -set filename:fn '%[basename]' -ordered-dither o2x2,32,64,32 pictures_tft3/dithered_numbered/'%[filename:fn].bmp'
echo -e "${GREEN}Converting tft4 pictures to specific bmp settings${NC}"
convert 'pictures_tft4/*.bmp' -set filename:fn '%[basename]' -ordered-dither o2x2,32,64,32 pictures_tft4/dithered_numbered/'%[filename:fn].bmp'

for f in ./pictures_tft1/dithered_numbered/*.bmp; do
  base=$(basename "$f")
  new="${base%%_*}.bmp"
  dest="./pictures_tft1/all_numbered/$new"
  echo "Moving $f → $dest"
  mv "$f" "$dest"
done
for f in ./pictures_tft2/dithered_numbered/*.bmp; do
  base=$(basename "$f")
  new="${base%%_*}.bmp"
  dest="./pictures_tft2/all_numbered/$new"
  echo "Moving $f → $dest"
  mv "$f" "$dest"
done
for f in ./pictures_tft3/dithered_numbered/*.bmp; do
  base=$(basename "$f")
  new="${base%%_*}.bmp"
  dest="./pictures_tft3/all_numbered/$new"
  echo "Moving $f → $dest"
  mv "$f" "$dest"
done
for f in ./pictures_tft4/dithered_numbered/*.bmp; do
  base=$(basename "$f")
  new="${base%%_*}.bmp"
  dest="./pictures_tft4/all_numbered/$new"
  echo "Moving $f → $dest"
  mv "$f" "$dest"
done
