

//Ham Bo Sung
double ChuyenSoThuc(string str) {
    if (str == "" || str == " ") return 0.0;
    try {
        return stod(str);
    } catch (...) {
        return 0.0;
    }
}

int ChuyenSoNguyen(string str) {
    if (str == "" || str == " ") return 0;
    try {
        return stoi(str);