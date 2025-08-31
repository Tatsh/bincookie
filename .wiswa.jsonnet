local utils = import 'utils.libjsonnet';

(import 'defaults.libjsonnet') + {
  local top = self,
  // General settings
  project_type: 'c',

  // Shared
  github_username: 'Tatsh',
  security_policy_supported_versions: { '0.1.x': ':white_check_mark:' },
  authors: [
    {
      'family-names': 'Udvare',
      'given-names': 'Andrew',
      email: 'audvare@gmail.com',
      name: '%s %s' % [self['given-names'], self['family-names']],
    },
  ],
  project_name: 'bincookie',
  version: '0.1.7',
  description: 'Tool to export registry paths to script and code formats (reg add, PowerShell, C, C#).',
  keywords: ['macos', 'ios', 'cookies', 'binarycookies', 'parser', 'library'],
  want_main: false,
  copilot: {
    intro: "bincookie is a library to read Apple's binarycookies format.",
  },
  social+: {
    mastodon+: { id: '109370961877277568' },
  },

  // GitHub
  github+: {
    funding+: {
      ko_fi: 'tatsh2',
      liberapay: 'tatsh2',
      patreon: 'tatsh2',
    },
  },

  // C++ only
  vcpkg+: {
    dependencies: [{
      name: 'cmocka',
      platform: 'linux|mingw',
    }],
  },
}
