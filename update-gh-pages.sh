#!/usr/bin/env bash
set -e
temp_dir=$(mktemp -d)
clean() {
    rm -fR "$temp_dir"
}
trap clean EXIT SIGINT
project_dir=$(pwd -P)
mkdir -p build
cd build
cmake -G Ninja -DWITH_DOCS=ON ..
ninja doc
rsync --progress --force --recursive --links --times --verbose --dirs "${project_dir}/" "$temp_dir"
pushd "$temp_dir"
git checkout .
git checkout gh-pages
git clean -dfx .
rsync --progress --force --recursive --links --times --verbose --dirs \
    "${project_dir}/build/docs/html/" .
git add .
git commit -m "Update documentation to ${CZ_POST_CURRENT_VERSION:-$(git tag -l | sort | tail -n 1)}"
git push
popd
