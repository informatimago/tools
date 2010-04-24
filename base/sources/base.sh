#!/bin/sh
pname="$0"
if [ $# != 2 ] ; then
	echo 'Usage:'
	echo "	$pname number base"
	exit 1
fi
(
	echo obase=$2 ;\
	echo $1 ;\
)|bc|sed \
	-e 's/ 00/0/g' \
	-e 's/ 01/1/g' \
	-e 's/ 02/2/g' \
	-e 's/ 03/3/g' \
	-e 's/ 04/4/g' \
	-e 's/ 05/5/g' \
	-e 's/ 06/6/g' \
	-e 's/ 07/7/g' \
	-e 's/ 08/8/g' \
	-e 's/ 09/9/g' \
	-e 's/ 10/A/g' \
	-e 's/ 11/B/g' \
	-e 's/ 12/C/g' \
	-e 's/ 13/D/g' \
	-e 's/ 14/E/g' \
	-e 's/ 15/F/g' \
	-e 's/ 16/G/g' \
	-e 's/ 17/H/g' \
	-e 's/ 18/I/g' \
	-e 's/ 19/J/g' \
	-e 's/ 20/K/g' \
	-e 's/ 21/L/g' \
	-e 's/ 22/M/g' \
	-e 's/ 23/N/g' \
	-e 's/ 24/O/g' \
	-e 's/ 25/P/g' \
	-e 's/ 26/Q/g' \
	-e 's/ 27/R/g' \
	-e 's/ 28/S/g' \
	-e 's/ 29/T/g' \
	-e 's/ 30/U/g' \
	-e 's/ 31/V/g' \
	-e 's/ 32/W/g' \
	-e 's/ 33/X/g' \
	-e 's/ 34/Y/g' \
	-e 's/ 35/Z/g'

