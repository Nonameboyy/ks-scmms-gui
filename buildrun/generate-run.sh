#!/bin/bash
export KS_PROJECK_NAME=$1
export KS_PROJECK_VERSION=$2
export KS_ALLOW_OS=("3.2-8")

CURR_PATH=$(cd $(dirname $0); pwd -P)

if [ "$KS_PROJECK_NAME" == "" ]; then
        echo "please input ks projeck name!"
        echo "./generate-run.sh [KS_PROJECK_NAME] [KS_PROJECK_VERSION]"
        exit 1
fi

if [ "$KS_PROJECK_VERSION" == "" ]; then
        echo "please input ks projeck version!"
        echo "./generate-run.sh [KS_PROJECK_NAME] [KS_PROJECK_VERSION]"
        exit 1
fi

SRC_PATH=$CURR_PATH/ks-run/$KS_PROJECK_NAME/$KS_PROJECK_VERSION
ls -l $SRC_PATH > /dev/null 2>&1 || { echo "$SRC_PATH not exist"; exit 1; }
export KS_ALLOW_ARCH=($(cd ${SRC_PATH}; find . -maxdepth 1 -mindepth 1 -type d -not -empty -printf '%f\n'))
if [ ${#KS_ALLOW_ARCH[@]} -eq 0 ]; then
        echo "$SRC_PATH is empty!"
        exit 1
fi

echo "allow os: ${KS_ALLOW_OS[@]}"

RUN_PATH=$CURR_PATH/$KS_PROJECK_NAME-$KS_PROJECK_VERSION-$(date "+%Y%m%d")
TMP_PATH=$CURR_PATH/ks-run-tmp/$KS_PROJECK_NAME/$KS_PROJECK_VERSION
mkdir -p $TMP_PATH
cp -r $SRC_PATH/* $TMP_PATH
cp -r $CURR_PATH/ks-run/install.sh $CURR_PATH/ks-run-tmp/

readmefile=$RUN_PATH.readme
cat > $readmefile <<EOF

集中运维GUI软件安装部署包

1、支持平台
硬件架构：${KS_ALLOW_ARCH[@]}
操作系统：大于等于 ${KS_ALLOW_OS[@]}

2、使用说明
执行 sh $KS_PROJECK_NAME-$KS_PROJECK_VERSION-$(date "+%Y%m%d").run 完成安装
执行 ${KS_PROJECK_NAME}-uninstall.sh 完成卸载

3、软件用到的依赖包，已经全部整理到 run 包内

EOF

for KS_ARCH_NAME in ${KS_ALLOW_ARCH[@]}
do
    echo "${KS_ARCH_NAME} 依赖包：" >> $readmefile
    find $TMP_PATH/${KS_ARCH_NAME} -name *.rpm 2>/dev/null | while read line;do
        echo $(basename $line) >> $readmefile
    done
    echo "" >> $readmefile
done


sed -i "s/KS_PROJECK_NAME/$KS_PROJECK_NAME/g" $CURR_PATH/ks-run.sh
sed -i "s/KS_ALLOW_OS/\"${KS_ALLOW_OS[*]}\"/g" $CURR_PATH/ks-run.sh
tar cvf $CURR_PATH/ks-run-tmp.tar.gz -C $CURR_PATH ks-run-tmp
cat $CURR_PATH/ks-run.sh $CURR_PATH/ks-run-tmp.tar.gz > $RUN_PATH.run
chmod +x $RUN_PATH.run
cd $CURR_PATH
md5sum $(basename $RUN_PATH.run) > $(basename $RUN_PATH).md5sum
sed -i "s/$KS_PROJECK_NAME/KS_PROJECK_NAME/g" $CURR_PATH/ks-run.sh
sed -i "s/\"${KS_ALLOW_OS[*]}\"/KS_ALLOW_OS/g" $CURR_PATH/ks-run.sh
rm -rf $CURR_PATH/ks-run-tmp.tar.gz $CURR_PATH/ks-run-tmp

