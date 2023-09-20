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
cmake -DWITH_DOCS=ON -G Ninja ..
ninja doc
rsync --dirs --force --links --progress --recursive --times "${project_dir}/" "$temp_dir"
pushd "$temp_dir"
git checkout .
git checkout gh-pages
git clean -dfx .
rsync --dirs --force --links --progress --recursive --times "${project_dir}/build/docs/html/" .
git add .
if git commit -m "Update documentation to ${CZ_POST_CURRENT_VERSION:-$(git tag -l | sort | tail -n 1)}"; then
    git push
fi
popd
git up
git push
git push --tags
