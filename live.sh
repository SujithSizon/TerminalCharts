#!/usr/bin/env bash
	y=0
	while true; do
		for i in $(seq 14); do
			y=$((y + RANDOM % 9 - 4))
			if (( $y > 100 )); then
				y=100
			elif (( $y < -50 )); then
				y=-50
			fi
			if (( $i != 1 )); then
				echo -n ,
			fi
			echo $y
		done
		sleep 0.1
	done | 

./offline -s bar '$Sample Live Market'



