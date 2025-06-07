#!/bin/sh

usage() {
	echo "Usage: $0 <arch> <config>"
	exit 1
}

if [ "$#" -ne 2 ]; then
	usage
fi

arch="$1"
config="$2"

ret=0

if ! res="$(make build ARCH="$arch" CONFIG="$config" 2>&1)"; then
	echo "Failed to compile build"
	echo "$res"
	exit 1
fi

mk() {
	p_arch=$1
	p_config=$2
	proj=$3
	exp_res=$4
	targets=$5

	dir="./test/$proj"
	bin="./test/$proj/bin"
	tmp="./test/$proj/tmp"

	printf "%s %-7s %-12s " "$p_arch" "$p_config" "$proj"

	out="$(./bin/"$arch"-"$config"/exes/build -s "$dir" -g M 2>&1)"
	act_res=$?

	if [ $act_res -ne "$exp_res" ]; then
		printf "\033[0;31mFAIL\033[0m\n"
		echo "Failed to generate makefiles"
		echo "$out"
		ret=1
		return
	fi

	if [ "$exp_res" -ne 0 ]; then
		printf "\033[0;32mPASS\033[0m\n"
		return
	fi

	if ! out="$(make -C "$dir/tmp/build" ARCH="$p_arch" CONFIG="$p_config" 2>&1)"; then
		printf "\033[0;31mFAIL\033[0m\n"
		echo "Failed to build project"
		echo "$out"
		ret=1
		return
	fi

	for target in $targets; do
		if [ ! -f "$bin/$p_arch-$p_config/$target" ]; then
			printf "\033[0;31mFAIL\033[0m\n"
			echo "Target not found: $target"
			ret=1
			return
		fi
	done

	printf "\033[0;32mPASS\033[0m\n"

	rm -rf "$bin" "$tmp"
}

test() {
	mk "$1" "$2" ext 0 bin/ext
	mk "$1" "$2" exe 0 bin/exe
	mk "$1" "$2" exe_dep_lib 0 "bin/exe lib/lib.a"
	mk "$1" "$2" lib 0 lib/lib.a
}

test x64 Debug
test x64 Release
test x86 Debug
test x86 Release

exit $ret
