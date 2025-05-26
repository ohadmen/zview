#!/bin/bash
# docker run  --rm -it -e PLAT=manylinux_2_28_x86_64 --entrypoint bash  -v $PWD:/io quay.io/pypa/manylinux_2_28_x86_64  /io/packaging/build_wheels.sh

set -e -u -x

dnf install dnf-plugins-core
dnf copr enable ohadm/bazel -y
dnf install bazel8 -y

 # Compile wheels
cd /io

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
    
    
    
done
mkdir -p wheelhouse
auditwheel repair bazel-bin/packaging/*.whl --plat $PLAT -w /io/wheelhouse/
mv -f MODULE.bazel_save MODULE.bazel

#twine upload wheelhouse/*