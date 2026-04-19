local utils = import 'utils.libjsonnet';

{
  uses_user_defaults: true,
  project_type: 'c',
  project_name: 'bincookie',
  version: '0.1.7',
  description: 'Header-only binarycookies parser.',
  keywords: ['macos', 'ios', 'cookies', 'binarycookies', 'parser', 'library'],
  security_policy_supported_versions: { '0.1.x': ':white_check_mark:' },
  want_main: false,
  want_codeql: false,
  want_tests: false,
  want_winget: false,
  // C only
  clang_format_args: '*.c *.h',
  vcpkg+: {
    dependencies: [{
      name: 'cmocka',
      platform: 'linux|mingw',
    }],
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
