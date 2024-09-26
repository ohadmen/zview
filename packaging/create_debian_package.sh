#! /bin/bash

bazel build //packaging:wheel  --compilation_mode=opt ||exit
bazel build //packaging:debian-package --compilation_mode=opt ||exit

dput ppa:ohadmen/zview ../bazel-bin/packaging/zview_2.0.2_all.changes