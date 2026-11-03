{
  checkout_options: { 'fetch-depth': 0 },
  configure_command: 'cmake -S . -B build-scan -DCMAKE_BUILD_TYPE=Debug -DWITH_TESTS=ON -DWITH_EXAMPLES=ON',
  documentation_steps: [
    {
      name: 'Install dependencies',
      run: 'sudo apt-get update\nsudo apt-get install -y doxygen graphviz ninja-build\n',
    },
    {
      name: 'Configure',
      run: 'cmake -G Ninja -DWITH_DOCS=ON -DCMAKE_BUILD_TYPE=Release -B "${GITHUB_WORKSPACE}/build"',
    },
    {
      name: 'Build documentation',
      run: 'cmake --build "${GITHUB_WORKSPACE}/build" --verbose --config Release --target docs',
    },
  ],
  env: {},
  setup_steps: [
    {
      name: 'Install build dependencies',
      run: 'sudo apt-get update\nsudo apt-get install -y libcmocka-dev\n',
    },
    {
      name: 'Install current CMake',
      run: 'pipx install cmake',
    },
  ],
  workflow_file: 'pages.yml',
}
