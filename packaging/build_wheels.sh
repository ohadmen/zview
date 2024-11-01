#!/bin/bash
# DOCKER_IMAGE=quay.io/pypa/manylinux_2_28_x86_64
# docker run  --rm -it -e PLAT=manylinux_2_28_x86_64 --entrypoint bash  -v $PWD:/io $DOCKER_IMAGE  /io/packaging/build_wheels.sh

set -e -u -x


yum install -y wget
# # Install a system package required by our library
wget https://copr.fedorainfracloud.org/coprs/vbatts/bazel/repo/epel-9/vbatts-bazel-epel-9.repo
mv vbatts-bazel-epel-9.repo /etc/yum.repos.d/
dnf install -y dnf-plugins-core
dnf copr enable -y vbatts/bazel
dnf install -y bazel4


yum install -y bazel6 --allowerasing

# # Compile wheels
cd /io
mkdir -p wheelhouse
cp MODULE.bazel MODULE.bazel_save
for FLDR in /opt/python/*; do
    
    PYBIN=${FLDR}/bin

    #support only python 3.9 to 3.12
    PY_MAJOR=`${PYBIN}/python3 -c "import platform; print(platform.python_version_tuple()[0])"`
    PY_MINOR=`${PYBIN}/python3 -c "import platform; print(platform.python_version_tuple()[1])"`
    if [[ $PY_MAJOR -ne "3" ]]; then
        continue
    fi
    if [[ $PY_MINOR -lt "9" ]]; then
        continue
    fi
    if [[ $PY_MINOR -gt "12" ]]; then
        continue
    fi
    
    
    PY_TAG=${FLDR##/opt/python/}
    PV=`${PYBIN}/python3 -c "import platform; print('.'.join(platform.python_version_tuple()[:2]))"` && export PV
    sed 's/\<PYTHON_VERSION\s*=[^\n]*/PYTHON_VERSION = "'$PV'" /g' MODULE.bazel_save > MODULE.bazel
    bazel build packaging:wheel
    rename 'py3-none' $PY_TAG bazel-bin/packaging/*.whl
    
    for whl in bazel-bin/packaging/*.whl; do
        auditwheel repair "$whl" --plat $PLAT -w /io/wheelhouse/
    done
    #test
    whl=`ls -Art wheelhouse/*.whl | tail -n 1` 
    ${PYBIN}/pip3 install $whl
    rm bazel-bin/packaging/*.whl
done
mv -f MODULE.bazel_save MODULE.bazel
mv -f MODULE.bazel_save MODULE.bazel