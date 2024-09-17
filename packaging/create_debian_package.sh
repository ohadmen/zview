#! /bin/bash
bazel build //zview:zview_binary ||exit
bazel build //packaging:wheel ||exit

ver=2.0.0 
bazel_bin=$(bazel info bazel-bin)
output_dir=$bazel_bin/packaging
temp_folder=$(mktemp -d)
temp_base=$temp_folder/zview-$ver
target_name=zview-$ver


target_dir=$temp_base/opt/zview
mkdir -p $target_dir
cp ../bazel-bin/zview/zview_binary $target_dir/zview
cp zview.png $target_dir


target_dir=$temp_base/usr/share/applications/
mkdir -p $target_dir
cp zview.desktop $target_dir

target_dir=$temp_base/DEBIAN
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
dpkg-deb --build $temp_base
mv $temp_folder/$target_name.deb $output_dir || exit

rm -rf $temp_folder

# bazel run --stamp --embed_label=$ver -- //packaging:wheel.publish --repository pypi