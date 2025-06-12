#!/bin/bash

set -e

g_basedir=$(cd $(dirname $0); pwd -P)
cd $g_basedir

pkgname=ks-scmms-gui

declare -a support_archs=(x86_64 aarch64)

function process_install_shell()
{
    local projname=$1
    local osversion=$2

    installsh=$g_basedir/ks-run/$projname/$osversion/install-${projname}.sh
    echo "process install shell: $installsh"
    cat > $installsh <<EOF
#!/bin/bash

# check the os version
# has checked in ks-run/install.sh

# check arch type
ARCH_TYPE=\$(uname -i)
OS_VERSION="\$(cat /etc/.kyinfo | grep milestone | awk -F= '{print \$2}' | tr -d ' ')"

if [ -d "\$1" ]; then
    CURR_PATH=\$1
else
    CURR_PATH=\$(pwd)
fi

echo "Current os is \${OS_VERSION}, arch is \${ARCH_TYPE}"
if [[ \$ARCH_TYPE =~ \${support_archs} ]]; then
    RPM_PATH=\${CURR_PATH}/\${ARCH_TYPE}
else
    echo "Cannot find os or arch"
    exit 1
fi

if [ -d "\${RPM_PATH}" ]; then
    echo "The rpm path \${RPM_PATH}"
else
    echo "Cannot find path \${RPM_PATH}"
    exit 1
fi

# install pkg one by one, mariadb-server cannot be started
cd \$RPM_PATH

deppgs=\$(ls | grep -v '^$pkgname')
kspkgs=\$(ls | grep -E "^$pkgname")

if [[ ! \$deppgs == "" ]];then
    yum localinstall \$deppgs -y --nogpgcheck --disablerepo=*
    if [[ \$? -ne 0 ]];then
        echo "unknown error, please connact administrator"
        exit 1
    else
        echo "yum localinstall depend packages succeed"
    fi 
fi

rpm -Uvh \$kspkgs --nodeps --force
echo "rpm -Uvh \$kspkgs --nodeps --force succeed"

cd -

EOF

}

function process_uninstall_shell()
{
    local projname=$1
    local osversion=$2

    uninstallsh=$g_basedir/ks-run/$projname/$osversion/uninstall-${projname}.sh
    echo "process uninstall shell: $uninstallsh"
    cat > $uninstallsh <<EOF
#!/bin/bash

while pidof ks-scmms-gui > /dev/null
do
    echo "检测到应用程序正在运行，请确认是否卸载（y/n）？"
    read confirm
    if [[ "x\$confirm" == "xy" || "x\$confirm" == "xY" || "x\$confirm" == "xn" || "x\$confirm" == "xN" ]]; then
        if [[ "\$confirm" == "n" || "\$confirm" == "N" ]]; then
            exit 1
        fi
        break
    fi
done

shelldir=\$(cd "\$(dirname "\$0")";pwd)
shellname=\$(basename "\$0")
kspkgs="\$(rpm -qa | grep -E '^$pkgname')"

echo "rpm -e \$kspkgs --nodeps"
[[ -z \$kspkgs ]] || rpm -e \$kspkgs --nodeps

echo "rm -rf \$shelldir/\$shellname"
rm -rf \$shelldir/\$shellname

EOF

}

ls $g_basedir/ks-run | grep ^$pkgname | while read proj; do
    ls $g_basedir/ks-run/$proj | while read ksver; do

        rm -f $g_basedir/ks-run/$proj/$ksver/*.sh
        process_install_shell $proj $ksver
        process_uninstall_shell $proj $ksver
        if [ -e $g_basedir/ks-run/$proj/$ksver ];then
            cmd="sh ./generate-run.sh $proj $ksver"
            echo $cmd
            $cmd
        fi
    done
done
