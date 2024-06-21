#include <string.h>

extern "C" {
    __declspec(dllexport) char* encrypt(char* rawText, int key) {
        key = key % 26;
        for (int i=0; i < strlen(rawText); i++) {
            if (rawText[i] >= 'A' && rawText[i] <= 'Z') {
                rawText[i] = (rawText[i] + key - 'A') % 26 + 'A';
            }
            else if (rawText[i] >= 'a' && rawText[i] <= 'z') {
                rawText[i] = (rawText[i] + key - 'a') % 26 + 'a';
            }
        }
        return rawText;
    }

    __declspec(dllexport) char* decrypt(char* rawText, int key) {
        key = key % 26;
        for (int i=0; i < strlen(rawText); i++) {
            if (rawText[i] >= 'A' && rawText[i] <= 'Z') {
                rawText[i] = ((rawText[i] - key - 'A' + 26) % 26) + 'A';
            }
            else if (rawText[i] >= 'a' && rawText[i] <= 'z') {
                rawText[i] = (rawText[i] - key - 'a' + 26) % 26 + 'a';
            }
        }
        return rawText;
    }
}
