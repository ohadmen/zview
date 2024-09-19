#! /bin/bash

bazel build //packaging:wheel  --compilation_mode=opt ||exit
bazel build //packaging:debian-package --compilation_mode=opt ||exit

# bazel run --stamp --embed_label=$ZVIEW_VERSION -- //packaging:wheel.publish --repository pypi