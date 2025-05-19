#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <locale>

using namespace std;

vector<int> encoded_lzw(const string& text) {
    if (text.empty()) return {};

    unordered_map<string, int> dictionary;
    for (int i = 0; i <= 255; i++) {
        string ch(1, static_cast<char>(i));
        dictionary[ch] = i;
    }

    string current_string(1, text[0]);
    int next_code = 256;
    vector<int> encoded_text;

    for (size_t i = 1; i < text.length();) {
        string next_element(1, text[i]);
        if (dictionary.find(current_string + next_element) != dictionary.end()) {
            current_string += next_element;
            i++;
        }
        else {
            encoded_text.push_back(dictionary[current_string]);
            if (next_code < 4096) {
                dictionary[current_string + next_element] = next_code++;
            }
            current_string = next_element;
            i++;
        }
    }
    encoded_text.push_back(dictionary[current_string]);
    return encoded_text;
}

string decoded_lzw(const vector<int>& encoded_text) {
    if (encoded_text.empty()) return "";

    unordered_map<int, string> dictionary;
    for (int i = 0; i <= 255; i++) {
        string ch(1, static_cast<char>(i));
        dictionary[i] = ch;
    }

    int next_code = 256;
    int old_code = encoded_text[0];
    string decoded_text = dictionary[old_code];

    for (size_t i = 1; i < encoded_text.size(); i++) {
        int current_code = encoded_text[i];
        string string_of_current_code;

        if (dictionary.count(current_code)) {
            string_of_current_code = dictionary[current_code];
        }
        else {
            string_of_current_code = dictionary[old_code] + dictionary[old_code][0];
        }

        decoded_text += string_of_current_code;

        if (next_code < 4096) {
            dictionary[next_code++] = dictionary[old_code] + string_of_current_code[0];
        }
        old_code = current_code;
    }
    return decoded_text;
}

void write_binary(const string& filename, const vector<int>& data) {
    ofstream out(filename, ios::binary);
    if (!out) {
        cerr << "Error opening file for writing: " << filename << endl;
        return;
    }
    for (int num : data) {
        out.write(reinterpret_cast<const char*>(&num), sizeof(num));
    }
}

vector<int> read_binary(const string& filename) {
    ifstream in(filename, ios::binary);
    vector<int> data;
    if (!in) {
        cerr << "Error opening file for reading: " << filename << endl;
        return data;
    }
    int num;
    while (in.read(reinterpret_cast<char*>(&num), sizeof(num))) {
        data.push_back(num);
    }
    return data;
}

int main() {
    setlocale(LC_ALL, "ru");

    string input_filename, encoded_filename, decoded_filename;
    cout << "Введите имя файла с исходным текстом: ";
    cin >> input_filename;
    cout << "Введите имя файла для закодированного текста: ";
    cin >> encoded_filename;
    cout << "Введите имя файла для декодированного текста: ";
    cin >> decoded_filename;

    ifstream in_file(input_filename);
    if (!in_file) {
        cerr << "Error opening input file!" << endl;
        return 1;
    }
    string text((istreambuf_iterator<char>(in_file)), istreambuf_iterator<char>());

    vector<int> encoded = encoded_lzw(text);
    write_binary(encoded_filename, encoded);

    vector<int> read_codes = read_binary(encoded_filename);
    string decoded = decoded_lzw(read_codes);

    ofstream out_file(decoded_filename);
    if (!out_file) {
        cerr << "Error opening output file!" << endl;
        return 1;
    }
    out_file << decoded;

    cout << "Обработка завершена!" << endl;
    cout << "Исходный размер: " << text.size() << " байт" << endl;
    cout << "Закодированный размер: " << encoded.size() * sizeof(int) << " байт" << endl;
    cout << "Декодированный размер: " << decoded.size() << " байт" << endl;

    if (text == decoded) {
        cout << "Проверка: исходный и декодированный текст совпадают!" << endl;
    }
    else {
        cout << "Проверка: исходный и декодированный текст различаются!" << endl;
    }

    return 0;
}
