#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <unordered_set>
#include <cstdio>

using namespace std;

// --- Doc CSV va xac dinh ma san pham ---

bool readProductIDs(const string& filename, vector<string>& productIDs) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << " [LOI] Khong the mo file san pham: " << filename << "\n";
        return false;
    }

    string line;
    if (!getline(file, line)) {
        cout << " [LOI] File rong: " << filename << "\n";
        return false;
    }

    while (getline(file, line)) {
        if (line.empty()) continue;
        size_t comma = line.find(',');
        if (comma == string::npos) continue;
        string id = line.substr(0, comma);
        if (!id.empty()) {
            productIDs.push_back(id);
        }
    }

    file.close();
    return true;
}

bool readExistingStockProductIDs(const string& filename, unordered_set<string>& productSet) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << " [LOI] Khong the mo file StockLevel: " << filename << "\n";
        return false;
    }

    string line;
    if (!getline(file, line)) {
        cout << " [LOI] File StockLevel rong: " << filename << "\n";
        return false;
    }

    while (getline(file, line)) {
        if (line.empty()) continue;
        size_t comma = line.find(',');
        if (comma == string::npos) continue;
        string id = line.substr(0, comma);
        if (!id.empty()) {
            productSet.insert(id);
        }
    }

    file.close();
    return true;
}

// Ham sinh ngay ngau nhien dinh dang YYYY-MM-DD
string getRandomDate() {
    int year = 2023 + rand() % 4; // Ngau nhien nam tu 2023 - 2026
    int month = 1 + rand() % 12;  // Ngau nhien thang tu 1 - 12
    int day = 1 + rand() % 28;    // Ngau nhien ngay tu 1 - 28 (tranh loi thang 2)
    char buf[15];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d", year, month, day);
    return string(buf);
}

// --- Bo sung ma san pham chua co trong StockLevel ---
int supplementMissingStockLevels(
    const string& productFile,
    const string& stockFile
) {
    vector<string> productIDs;
    unordered_set<string> existingStockIDs;

    if (!readProductIDs(productFile, productIDs)) {
        return 1;
    }

    if (!readExistingStockProductIDs(stockFile, existingStockIDs)) {
        return 1;
    }

    ofstream stockOut(stockFile, ios::app);
    if (!stockOut.is_open()) {
        cout << " [LOI] Khong the mo file de ghi: " << stockFile << "\n";
        return 1;
    }

    srand(static_cast<unsigned int>(time(0)));
    int addedCount = 0;

    for (const string& id : productIDs) {
        if (existingStockIDs.find(id) == existingStockIDs.end()) {
            int randomWarehouse = rand() % 3 + 1; // Sinh ngẫu nhiên mã kho 1, 2, hoặc 3
            int randomQuantity = rand() % 86 + 15; // 15..100
            string randomDate = getRandomDate();
            stockOut << id << "," << randomWarehouse << "," << randomQuantity << "," << randomDate << "\n";
            addedCount++;
        }
    }

    stockOut.close();

    cout << "\n [HOAN TAT] Da bo sung " << addedCount
         << " ma san pham chua co ton kho vao file: " << stockFile << "\n";
    return 0;
}

// --- Them cot Unit va MinStockLevel vao Product.csv ---
int updateProductCSV(const string& filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cout << " [LOI] Khong the mo file: " << filename << "\n";
        return 1;
    }

    vector<string> lines;
    string line;
    bool isUpdated = false;

    // Đọc dòng tiêu đề trước
    if (getline(inFile, line)) {
        // Xóa ký tự \r ở cuối dòng nếu bị lỗi format file
        if (!line.empty() && line.back() == '\r') line.pop_back();
        
        if (line.find("MinStockLevel") != string::npos) {
            isUpdated = true;
        } else {
            line += ",Unit,MinStockLevel";
        }
        lines.push_back(line);
    }

    if (isUpdated) {
        cout << " [INFO] File " << filename << " da co cot Unit va MinStockLevel, khong can cap nhat nua.\n";
        inFile.close();
        return 0;
    }

    while (getline(inFile, line)) {
        if (line.empty() || line == "\r") continue;
        if (line.back() == '\r') line.pop_back();
        
        int minStock = rand() % 16 + 5; // Ngẫu nhiên ngưỡng tồn kho tối thiểu từ 5 đến 20
        line += ",Cai," + to_string(minStock);
        lines.push_back(line);
    }
    inFile.close();

    ofstream outFile(filename, ios::out);
    if (!outFile.is_open()) {
        cout << " [LOI] Khong the mo file de ghi: " << filename << "\n";
        return 1;
    }
    for (const string& l : lines) {
        outFile << l << "\n";
    }
    outFile.close();

    cout << " [HOAN TAT] Da tu dong them cot Unit va MinStockLevel vao " << filename << "!\n";
    return 0;
}

// --- Ham Main ---
int main() {
    cout << "Dang kiem tra va cap nhat file Product.csv...\n";
    updateProductCSV("Product.csv");

    cout << "Dang bo sung cac ma san pham chua co stock level...\n";
    supplementMissingStockLevels("Product.csv", "StockLevel.csv");
    return 0;
}