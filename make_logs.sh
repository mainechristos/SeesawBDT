#!/bin/bash
# reads signal input file and generates new file with BDT variable
#mkdir /cms/mchristos/ANN/Seesaw/2016/outFiles/SeesawTMVAClassificationApplication$1/Signal
while read p 
do
  echo $p
  mkdir /cms/mchristos/ANN/Seesaw/2016/92X/outFiles/SeesawTMVAClassificationApplication4/Signal/$p
  mkdir /cms/mchristos/ANN/Seesaw/2016/92X/outFiles/SeesawTMVAClassificationApplication4/Signal/$p/logs/
done </cms/mchristos/ANN/Seesaw/2016/92X/signal_processes.txt
