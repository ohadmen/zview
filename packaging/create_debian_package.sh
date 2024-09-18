#! /bin/bash

bazel build //packaging:wheel ||exit
bazel build //packaging:debian-package ||exit

# bazel run --stamp --embed_label=$ZVIEW_VERSION -- //packaging:wheel.publish --repository pypi