# Vault
1. the .sln file for Visual Studio is in libbcrypt within vs2019.
2. the path for the final build (.exe): ...\vs2019\libbcrypt\x64\Debug
3. the code is in test (main.cpp).
<hr>

1. [ ] a password vault; it stores passwords and usernames and allows copying to clipboard
2. [ ] a password manager, that prompts to autofill passwords on different platforms
3. [ ] syncing the data between different devices, as in having it on the web
<hr>

## Concerns
1. the data being stored in a map is RAM heavy and might be a bad idea for larger files. suggested alternatives: use database or indexing.
2. security concerns. two possible ones are keyloggers and clipboard hijacking.
