#!/bin/bash
# DOCKER_IMAGE=quay.io/pypa/manylinux_2_28_x86_64
# docker run  --rm -it --entrypoint bash  -v $PWD:/io $DOCKER_IMAGE  /io/packaging/build_wheels.sh
set -e -u -x

function repair_wheel {
    wheel="$1"
    if ! auditwheel show "$wheel"; then
        echo "Skipping non-platform wheel $wheel"
    else
        auditwheel repair "$wheel" --plat "$PLAT" -w /io/wheelhouse/
    fi
}
echo HELLO
# # Install a system package required by our library
wget https://copr.fedorainfracloud.org/coprs/vbatts/bazel/repo/epel-9/vbatts-bazel-epel-9.repo
mv vbatts-bazel-epel-9.repo /etc/yum.repos.d/

yum install -y bazel6

# # Compile wheels
# for PYBIN in /opt/python/*/bin; do
#     "${PYBIN}/pip" install -r /io/dev-requirements.txt
#     "${PYBIN}/pip" wheel /io/ --no-deps -w wheelhouse/
# done

# # Bundle external shared libraries into the wheels
# for whl in wheelhouse/*.whl; do
#     repair_wheel "$whl"
# done

# # Install packages and test
# for PYBIN in /opt/python/*/bin/; do
#     "${PYBIN}/pip" install python-manylinux-demo --no-index -f /io/wheelhouse
#     (cd "$HOME"; "${PYBIN}/nosetests" pymanylinuxdemo)
# done