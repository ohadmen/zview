#! /bin/bash
bz build //zview:zview_binary
ver=2.0.0 
rm -rf pack
mkdir pack 
cd pack
taget_base=zview-$ver

target_dir=$taget_base/opt/zview
mkdir -p $target_dir
cp ../../bazel-bin/zview/zview_binary $target_dir/zview
cp ../zview.png $target_dir


target_dir=$taget_base/usr/share/applications/
mkdir -p $target_dir
cp ../zview.desktop $target_dir


target_dir=$taget_base/DEBIAN
mkdir -p $target_dir
control_file=$target_dir/control
echo "Source: zview" >> $control_file
echo "Version: $ver" >> $control_file
echo "Section: visualization" >> $control_file
echo "Priority: optional" >> $control_file
echo "Maintainer: Ohad <ohad.men@gmail.com>" >> $control_file
echo "Build-Depends: debhelper (>= 10), autotools-dev" >> $control_file
echo "Standards-Version: 4.1.2" >> $control_file
echo "Homepage: https://github.com/ohadmen/zview" >> $control_file
echo "Package: zview" >> $control_file
echo "Architecture: amd64" >> $control_file
echo "Description: Generic mesh&cloud visualization tool" >> $control_file
echo  "" >> $control_file
dpkg-deb --build $taget_base
dpkg-deb -c $taget_base.deb

rm -rf $taget_base

bazel run --stamp --embed_label=$ver -- //packaging:wheel.publish --repository pypi