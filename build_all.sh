#!/bin/bash

set -e

SHELL_FOLDER=$(cd "$(dirname "$0")";pwd)
cd $SHELL_FOLDER

pkg_name=$(cat $SHELL_FOLDER/buildrpm/ks-scmms-gui.spec | grep " pkg_name " | awk '{print $3}')
pkg_ver=$(cat $SHELL_FOLDER/buildrpm/ks-scmms-gui.spec | grep " pkg_ver " | awk '{print $3}')
pkg_rel=$(cat $SHELL_FOLDER/buildrpm/ks-scmms-gui.spec | grep " pkg_rel " | awk '{print $3}')

declare -a supported_arch=("x86_64" "aarch64")
declare -a supported_os=("3.2-8" "3.3-6")

dobuild() {
	for arch in ${supported_arch[*]};do
		rm -f $SHELL_FOLDER/buildrun/ks-run/$pkg_name/$pkg_ver/$arch/${pkg_name}*
		if [[ $arch == "$(uname -i)" ]];then
			pushd $SHELL_FOLDER/buildrpm
			bash ./build.sh "$1" 3.2-8
			popd
			cp -aR $SHELL_FOLDER/buildrpm/_buildres/${pkg_name}-*.$arch.rpm $SHELL_FOLDER/buildrun/ks-run/$pkg_name/$pkg_ver/$arch
		elif [[ $arch == "aarch64" ]];then
			compilerip=192.168.120.199
			ssh $compilerip /root/$pkg_name/buildrpm/build.sh "'$1' 3.3-6"
			scp $compilerip:/root/$pkg_name/buildrpm/_buildres/${pkg_name}-*.$arch.rpm $SHELL_FOLDER/buildrun/ks-run/$pkg_name/$pkg_ver/$arch
		fi
	done

	pushd $SHELL_FOLDER/buildrun
	bash ./build_all.sh
	popd
}

usage() {
	echo "Usage:"
	echo "    $0 [--help|-h|\$pkg_version]"
}

if [[ $1 == "--help" || $1 == "-h" ]];then
	usage
else
	dobuild $1
fi
