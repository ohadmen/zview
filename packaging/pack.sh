#! /bin/bash

# wheels
docker run  --rm -it -e PLAT=manylinux_2_28_x86_64 --entrypoint bash  -v $PWD:/io quay.io/pypa/manylinux_2_28_x86_64  /io/packaging/build_wheels.sh
twine upload wheelhouse/*


# debian package
debuild -S -sa # upload
# find first file that ends with .changes
TARGET_FILE=$(ls -t ../*_source.changes | head -n 1)
dput ppa:ohadmen/zview $TARGET_FILE
#local - optional
debuild -us -uc 
