local utils = import 'utils.libjsonnet';

{
  project_type: 'c',
  project_name: 'bincookie',
  version: '0.1.7',
  description: 'Header-only binarycookies parser.',
  keywords: ['macos', 'ios', 'cookies', 'binarycookies', 'parser', 'library'],
  security_policy_supported_versions: { '0.1.x': ':white_check_mark:' },
  want_main: false,
  want_codeql: false,
  want_tests: false,
  copilot+: {
    intro: "bincookie is a library to read Apple's binarycookies format.",
  },
  // C++ only
  vcpkg+: {
    dependencies: [{
      name: 'cmocka',
      platform: 'linux|mingw',
    }],
  },
  package_json+: {
    scripts+: {
      'check-formatting': "cmake-format --check CMakeLists.txt && clang-format -n *.c *.h && prettier -c . && markdownlint-cli2 '**/*.md' '#node_modules'",
      format: 'cmake-format -i CMakeLists.txt && clang-format -i *.c *.h && yarn prettier -w .',
    },
  },
  vscode+: {
    c_cpp+: {
      configurations: [
        {
          cStandard: 'gnu23',
          compilerPath: '/usr/bin/gcc',
          cppStandard: 'gnu++23',
          defines: [],
          includePath: ['${workspaceFolder}/**'],
          name: 'Linux',
        },
      ],
    },
    launch+: {
      configurations: [
        {
          cwd: '${workspaceRoot}',
          name: 'Run tests',
          program: '${workspaceRoot}/build/general_tests',
          request: 'launch',
          type: 'cppdbg',
        },
      ],
    },
  },
  cz+: {
    commitizen+: {
      version_files+: [
        'man/bincookie.h.3',
      ],
    },
  },
}
