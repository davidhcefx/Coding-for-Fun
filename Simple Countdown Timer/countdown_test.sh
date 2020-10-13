#! /bin/bash
#
# Testing code written in 2020.10.12., as there were too many bugs ;(

SBID=$RANDOM
SANDBOX=~/Workspace/tmp/$SBID/  # for avoiding AVG
SANDBOX_WIN="c:/Users/davidhcefx/Workspace/tmp/$SBID/"
SRC="./countdown.cpp"
CPP="g++_mingw64 -static -o $SANDBOX/countdown.exe"
RUN="cmd.exe /c \"$SANDBOX_WIN/countdown.exe "


setup() {
	echo "Creating sandbox $SBID..."
	mkdir -p $SANDBOX

	echo "Compiling..."
	# filter out pause
	cat "$SRC" | sed 's/system("pause");//g' > /tmp/src.cpp
	eval "$CPP /tmp/src.cpp"
}

run_test() {
	min=$1
	sec=$2
	export TIMEFORMAT=%0lR
	echo -n -e "${min}m${sec}s  \t-->  \t"
	bash -c "time $RUN $min $sec\">/dev/null"
}

basic_test() {
	echo "Basic test..."
	run_test 0 3
}

boundary_test() {
	echo "Boundary test..."
	for m in 0 1 2 58 59 ; do
		for s in 0 1 2 58 59 ; do
			run_test $m $s
		done
	done
}

invalid_test() {
	echo "Invalid test..."
	for m in -1 -2 60 61 120 ; do
		for s in -1 -2 60 61 120 ; do
			run_test $m $s
		done
	done
}

random_test() {
	echo "Random test..."
	for i in {1..3}; do
		run_test $(( RANDOM % 60 )) $(( RANDOM % 60 ))
	done
}

setup
basic_test
boundary_test
invalid_test
random_test
