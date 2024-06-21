#include <filesystem>
#include <iostream>
#include <string.h>
#include <fstream>
#include <stack>
#include <unordered_map>
#include <windows.h>
using namespace std;

char* getUserText() {
    cout << "Enter text to append: " << endl;
    int allocated_size = 10;
    int len = 0;
    auto userText = static_cast<char *>(calloc(allocated_size, sizeof(char)));
    char ch;
    do {
        ch = getchar();
        userText[len] = ch;
        len++;

        if (len > allocated_size) {
            allocated_size *= 2;
            char *temp = static_cast<char *>(realloc(userText, allocated_size * sizeof(char)));
            if (temp == nullptr) {
                free(userText);
                exit(1);
            }
            userText = temp;
        }
    } while(ch != '\n');
    if (userText[len-1] == '\n'){
        userText[len-1] = '\0';}
    return userText;
}

class Text {
public:
    char* show;
    int size;
    Text(){
        array = static_cast<char *>(calloc(10, sizeof(char)));
        size = strlen(array);
        show = array;
    }
    char* array;
    void changeSize(int newSize) {
        array = static_cast<char *>(realloc(array, newSize * sizeof(char)));
    }

    void changeText(char* newText) {
        array = newText;
    }
    void changeTextByIndex(int index, char element) {
        array[index] = element;
    }

    ~Text() {
        free(array);
    }
};

class CaesarCipher {
    HINSTANCE handle;
public:
    CaesarCipher() {
        handle = LoadLibrary(TEXT("C:\\Users\\kovko\\CLionProjects\\Paradigms_Assignment3\\cezar.dll"));
        if (handle == nullptr || handle == INVALID_HANDLE_VALUE)
        {
            cout << "Lib not found" << endl;
        }
    }

    char* encrypt(char* text, int key) {
        typedef char*(*function_ptr)(char*, int);
        function_ptr encrypt = (function_ptr)GetProcAddress(handle, TEXT("encrypt"));
        if (encrypt == nullptr)
        {
            cout << "Proc not found" << endl;
        }
        return encrypt(text, key);
    }

    char* decrypt(char* text, int key) {
        typedef char*(*function_ptr)(char*, int);
        function_ptr decrypt = (function_ptr)GetProcAddress(handle, TEXT("decrypt"));
        if (decrypt == nullptr)
        {
            cout << "Proc not found" << endl;
        }
        return decrypt(text, key);
    }
};

class TextEditor {
    Text text;
    char* buffer;
    int currentLine;
    stack<char*> undoStackText;
    stack<char*> redoStackText;
    stack<unordered_map<int, int>> undoStackCursor;
    stack<unordered_map<int, int>> redoStackCursor;
    unordered_map<int, int> cursor;

public:
    TextEditor() {
        buffer = static_cast<char *>(calloc(10, sizeof(char)));
        currentLine = 0;
        cursor[currentLine] = 0;
    }
    void pushStackText() {
        char* undoCopy = new char[text.size + 1];
        strcpy(undoCopy, text.show);
        undoStackText.push(undoCopy);
    }

    void pushStackCursor() {
        unordered_map<int, int> copyCursor(cursor);
        undoStackCursor.push(copyCursor);
    }

    void append(char* userText) {
        pushStackText();
        pushStackCursor();

        if (text.size + strlen(userText) > sizeof(text)) {
            text.changeSize(text.size + strlen(userText)+1);
        }

        strcat(text.array, userText);
        cursor[currentLine] += strlen(userText);
        free(userText);
    }

    void show() const {
        cout << text.array << endl;
    }

    void newline() {
        if (text.size + 1 > sizeof(text)) {
            text.changeSize(text.size + 2);
        }
        pushStackText();
        pushStackCursor();
        strcat(text.array, "\n");
        currentLine++;
        cursor[currentLine] = 0;
    }

    void insert(int line, int index, char* userText) {
        pushStackText();
        pushStackCursor();
        int numberOfSymbols = 0;
        int totalSymbols = 0;
        if (text.size + strlen(userText) > sizeof(text)) {
            text.changeSize(text.size + strlen(userText) + 1);
        }

        int sizeInput = strlen(userText);

        for (int i = 0; i < line; i++) {
            numberOfSymbols += cursor[i] + 1; //+1 for "\n"
        }
        totalSymbols = numberOfSymbols;
        for (int i = line; i <=currentLine; i++) {
             totalSymbols += cursor[i] + 1;
        }

        for (int i = totalSymbols + sizeInput - 1; i >= numberOfSymbols + sizeInput + index; i--) {
            text.array[i] = text.array[i - sizeInput];
        }

        for (int i = numberOfSymbols + index; i < numberOfSymbols + index + sizeInput; i++) {
            text.array[i] = userText[i - numberOfSymbols - index];
        }
        cursor[line] += sizeInput;
    }

    void search(char* userText) {
        int k = 0;
        int line = 0;
        int n = 0;
        int indexes = 0;
        int sizeText = text.size;
        int sizeInput = strlen(userText);
        for (int i=0; i < sizeText; i++) {
            if (text.array[i] == '\n') {line++; indexes += n; n=0;}
            else {n++;}
            if (text.array[i] == userText[k]) {
                k++;
                if (k == sizeInput) {
                    cout<< line<< i - sizeInput + 1 - line - indexes << endl;
                    k = 0;
                }
            }
            else{
                if (text.array[i] == userText[0]){k = 1;}
                else {k = 0;}
            }
        }
    }

    void pack(char *nameFile, char mode) {
        ofstream file;
        if (mode == 'w')
        {file.open(nameFile, std::ios_base::out);}
        else
        {file.open(nameFile, std::ios_base::app);}

        if (file.is_open()) {
            file << text.array;
            file.close();
            std::cout << "Text has been saved successfully \n";
        }
        else {
            std::cerr << "Error opening file\n";
        }
    }

    void load(char* nameFile) {
        pushStackText();
        pushStackCursor();
        currentLine = 0;
        int numberOfSymbols = 0;
        ifstream file (nameFile);

        if (file.is_open())
        {
            int allocated_size = 10;
            int len = 0;
            char* newText = static_cast<char *>(calloc(10, sizeof(char)));
            char ch;
            while(file.get(ch)) {
                newText[len] = ch;
                len++;
                if (ch == '\n') {
                    currentLine++;
                    for (int i=0; i < currentLine; i++) {
                        numberOfSymbols += cursor[i] + 1; //+1 for "\n"
                    }
                    cursor[currentLine] = len - numberOfSymbols;
                }
                else {
                    cursor[currentLine] = len;
                }
                if (len >= allocated_size) {
                    allocated_size *=2;
                    char *temp = static_cast<char *>(realloc(newText, allocated_size * sizeof(char)));
                    if (temp == nullptr) {
                        free(newText);
                    }
                    newText = temp;
                }
            }
            newText[len] = '\0';
            file.close();
            text.array = newText;
        }
        else cout << "Error opening file";
    }

    void replacement(int line, int index, char* userText) {
        pushStackText();
        pushStackCursor();

        int numberOfSymbols= 0;
        int sizeInput = strlen(userText);

        if (text.size + sizeInput > sizeof(text)) {
            text.changeSize(text.size + sizeInput + 1);
        }

        for (int i = 0; i < line; i++) {
            numberOfSymbols += cursor[i] + 1; //+1 for "\n"
        }
        for (int i = numberOfSymbols + index; i < numberOfSymbols + index + sizeInput; i++) {
            text.array[i] = userText[i - numberOfSymbols - index];
        }
        free(userText);
    }

    void del(int line, int index, int number){
        pushStackText();
        pushStackCursor();

        int numberOfSymbols = 0;
        int totalSymbols = 0;
        int sizeText = text.size;
        if (sizeText + number > sizeof(text)) {
            text.changeSize(text.size + number);
        }
        for (int i = 0; i < line; i++) {
            numberOfSymbols += cursor[i] + 1; //+1 for "\n"
        }
        totalSymbols = numberOfSymbols;
        for (int i = line; i <=currentLine; i++) {
            totalSymbols += cursor[i] + 1;
        }
        for (int i = numberOfSymbols + index + number; i < totalSymbols + number; i++) {
            text.array[i - number] = text.array[i];
        }
        cursor[line] -=number;
    }

    void copy(int line, int index, int number) {
        buffer = static_cast<char *>(realloc(buffer, (number + 1) * sizeof(char)));
        int numberOfSymbols = 0;

        for (int i = 0; i < line; i++) {
            numberOfSymbols += cursor[i] + 1; //+1 for "\n"
        }

        for (int i = numberOfSymbols + index; i < numberOfSymbols + index + number; i++) {
            buffer[i-numberOfSymbols-index] = text.array[i];
        }
        buffer[number] = '\0';
    }

    char* getBuffer() {
        return buffer;
    }

    void undo() {
        if (!undoStackText.empty()) {
            char* redoCopy = new char[text.size + 1];
            strcpy(redoCopy, text.array);
            redoStackText.push(redoCopy);

            unordered_map<int, int> copyCursor(cursor);
            redoStackCursor.push(copyCursor);

            //free(text);
            text.array = undoStackText.top();
            cursor = undoStackCursor.top();
            undoStackText.pop();
            undoStackCursor.pop();
        }
        else {
            cout <<"You are stupid" <<endl;
        }
    }

    void redo() {
        if (!redoStackText.empty()) {
            pushStackText();
            pushStackCursor();

            //free(text);
            text.array = redoStackText.top();
            cursor = redoStackCursor.top();
            redoStackText.pop();
            redoStackCursor.pop();
        }
        else {
            cout <<"You are stupid" <<endl;
        }
    }
    void encrypt(int key) {
        pushStackText();
        pushStackCursor();
        CaesarCipher cipher;
        text.array = cipher.encrypt(text.array, key);
    }
    void decrypt(int key) {
        pushStackText();
        pushStackCursor();
        CaesarCipher cipher;
        text.array = cipher.decrypt(text.array, key);
    }

    void encryptFile(int key, char* inputFile, char* outputFile) {
        CaesarCipher cipher;
        size_t chunkSize = 128;
        ifstream inFile(inputFile, ios::binary);
        ofstream outFile(outputFile, ios::binary);
        char* newText = static_cast<char*>(calloc(chunkSize, sizeof(char)));
        while (!inFile.eof()) {
            inFile.read(newText, chunkSize);
            streamsize bytesRead = inFile.gcount();

            if (bytesRead > 0) {
                outFile.write(cipher.encrypt(newText, key), bytesRead);
            }
        }
        free(newText);
        inFile.close();
        outFile.close();
    }
    void decryptFile(int key, char* inputFile, char* outputFile) {
        CaesarCipher cipher;
        size_t chunkSize = 128;
        ifstream inFile(inputFile, ios::binary);
        ofstream outFile(outputFile, ios::binary);
        char* newText = static_cast<char*>(calloc(chunkSize, sizeof(char)));
        while (!inFile.eof()) {
            inFile.read(newText, chunkSize);
            streamsize bytesRead = inFile.gcount();

            if (bytesRead > 0) {
                outFile.write(cipher.decrypt(newText, key), bytesRead);
            }
        }

        free(newText);
        inFile.close();
        outFile.close();
    }
};

int main()
{
    TextEditor text_editor;
    for (;;) {
        int c;
        cout <<"\nChoose the command: " << endl;
        cin >> c;
        fflush(stdin);
        switch (c) {
            case 1:
                text_editor.append(getUserText());
                break;
            case 2:
                text_editor.newline();
                break;
            case 3:
                char nameFile[20];
                char mode;
                cout << "\nEnter the file name for saving and the mode (w/a): " << endl;
                cin >> nameFile >> mode;
                fflush(stdin);
                text_editor.pack(nameFile, mode);
                break;
            case 4:
                char nameFile1[20];
                cout << "\nEnter the file name for loading: " << endl;
                cin >> nameFile1;
                text_editor.load(nameFile1);
                break;
            case 5:
                text_editor.show();
                break;
            case 6: {
                int line, index;
                cout << "\nChoose line and index: "<<endl;
                cin >> line >> index;
                fflush(stdin);
                text_editor.insert(line, index, getUserText());
                break;
            }
            case 7:
                text_editor.search(getUserText());
                break;

            case 8:
                int line2, index2, number;
                cout << "Choose line, index and number of symbols: "<<endl;
                cin >> line2 >> index2 >> number;
                text_editor.del(line2, index2, number);
                break;
            case 9:
                text_editor.undo();
                break;
            case 10:
                text_editor.redo();
                break;
            case 11:
                int line4, index4, number2;
                cout << "Choose line, index and number of symbols: "<<endl;
                cin >> line4 >> index4 >> number2;
                text_editor.copy(line4, index4, number2);
                text_editor.del(line4, index4, number2);
                break;
            case 13:
                int line3, index3, number1;
                cout << "Choose line, index and number of symbols: "<<endl;
                cin >> line3 >> index3 >> number1;
                text_editor.copy(line3, index3, number1);
                break;
            case 12:
                int line5, index5;
                cout << "Choose line, index: "<<endl;
                cin >> line5 >> index5;
                text_editor.insert(line5, index5, text_editor.getBuffer());
                break;
            case 14:
                int line1, index1;
                cout << "\nChoose line and index: "<<endl;
                cin >> line1 >> index1;
                fflush(stdin);
                text_editor.replacement(line1, index1, getUserText());
                break;
            case 15:
                int key;
                cout << "Enter key: " << endl;
                cin >> key;
                text_editor.encrypt(key);
                break;
            case 16:
                int key1;
                cout << "Enter key: " << endl;
                cin >> key1;
                text_editor.decrypt(key1);
                break;
            case 17:
                int key2;
                char inputFile[20];
                char outputFile[20];
                cout << "Enter name of the input file, output file and key: " << endl;
                cin >> inputFile >> outputFile>> key2;
                text_editor.encryptFile(key2, inputFile, outputFile);
                break;
            case 18:
                int key3;
                char inputFile1[20];
                char outputFile1[20];
                cout << "Enter name of the input file, output file and key: " << endl;
                cin >> inputFile1 >> outputFile1>> key3;
                text_editor.decryptFile(key3, inputFile1, outputFile1);
                break;
            case 0:
                cout << "Thank you " <<endl;
                return 0;
            default:
                cout << "This command is not avaible" <<endl;
                break;
        }
    }
}
