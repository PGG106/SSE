#!/bin/bash

make cleanall build=x86-64-avx2 KAGGLE=true NOSTDLIB=true
sstrip Alexandria
ls -la Alexandria
md5sum nn.net
md5sum Alexandria

for TARGET in nn.net Alexandria
do
	echo Finding best compression parameters for $TARGET
	SMALLEST=1000000
	for MF in hc3 hc4 bt2 bt3 bt4
	do
		for NICE in {4..273}
		do
			xz -f -k --lzma2=preset=9,lc=2,lp=0,pb=0,mf=$MF,nice=$NICE $TARGET
			FILESIZE=$(stat -c%s "$TARGET.xz")
			if [ "$FILESIZE" -lt "$SMALLEST" ]; then
				echo mf=$MF nice=$NICE size=$FILESIZE
				cp -f $TARGET.xz $TARGET-smallest.xz
				SMALLEST=$FILESIZE
			fi
		done
	done
	rm -f $TARGET.xz
	mv -f $TARGET-smallest.xz $TARGET.xz
done

dos2unix main.py

tar -cf submission.tar Alexandria.xz nn.net.xz main.py
ls -la submission.tar
zopfli --i1000 submission.tar
ls -la submission.tar.gz
