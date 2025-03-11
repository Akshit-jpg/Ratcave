# Vault
1. the .sln file for Visual Studio is in libbcrypt (vs2019/libbcrypt).
2. the code is in test (vs2019/test/main.cpp).
3. bcrypt library: [libbcrypt](https://github.com/trusch/libbcrypt/tree/master)
<hr>

1. [ ] a password vault; it stores passwords and usernames and allows copying to clipboard. also allows updating and deleting entries
2. [ ] a password manager, that prompts to autofill passwords on different platforms. it also has a password generator that suggests a strong password on a sign up screen
3. [ ] syncing the data between different devices, as in having it on the web
<hr>

## Concerns
1. security concerns. two possible ones are keyloggers and clipboard hijacking.
