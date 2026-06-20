#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdio>

using namespace std;

const string FILE_PRODUCT = "Product.csv";
const string FILE_STOCK = "StockLevel.csv";
const string FILE_TRANSACTION = "InventoryTransaction.csv";

//Cau Truc Du Lieu
struct Product {
    string ProductID;
    string ProductName;
    string CategoryName;
    string ProductDescription;
    double ProductStandardCost;
    double ProductListPrice;
    double Profit;
    string Unit;        
    int MinStockLevel; 
    int QuantityOnHand;
};

struct ProductNode {
    Product data;
    ProductNode* next;
};

ProductNode* headSP = NULL;

struct StockLevel {
    string ProductID;
    int WarehouseID;
    int QuantityOnHand;
    string LastUpdated;
};

struct StockLevelNode {
    StockLevel data;
    StockLevelNode* next;
};

StockLevelNode* headKho = NULL;

struct InventoryTransaction {
    int TransactionID;
    string ProductID;
    int WarehouseID;
    string TransactionType;
    int Quantity;
    string TransactionDate;
};

//Xu ly loi
int BaoLoi(int errCode) {
    switch(errCode) {
        case 1: cout << "[err 01] Khong tim thay san pham trong he thong!\n"; break;
        case 2: cout << "[err 02] So luong khong hop le (phai lon hon 0)!\n"; break;
        case 3: cout << "[err 03] Ton kho hien tai khong du de xuat!\n"; break;
        case 4: cout << "[err 04] Ma san pham da ton tai!\n"; break;
        case 5: cout << "[err 05] Thao tac khong hop le (lam ton kho bi am)!\n"; break;
        default: cout << "[err] Xay ra loi khong xac dinh (ma loi: " << errCode << ")!\n"; break;
    }
    return 0;
}

//Ham Bo Sung
double ChuyenSoThuc(string str) {
    if (str == "" || str == " ") return 0.0;
    try {
        return stod(str);
    } catch (...) {
        return 0.0; // Neu loi (co chu cai, ky tu la...), tu dong tra ve 0.0
    }
}

int ChuyenSoNguyen(string str) {
    if (str == "" || str == " ") return 0;
    try {
        return stoi(str);
    } catch (...) {
        return 0; // Neu loi, tu dong tra ve 0
    }
}
string LayNgayHienTai() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buf[11];
    sprintf(buf, "%d-%02d-%02d", 1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
    return string(buf);
}

ProductNode* TimSanPham(const string& productID) {
    ProductNode* current = headSP;
    while (current != NULL) {
        if (current->data.ProductID == productID) return current;
        current = current->next;
    }
    return NULL;
}

StockLevelNode* TimTonKho(const string& productID, int warehouseID) {
    StockLevelNode* current = headKho;
    while (current != NULL) {
        if (current->data.ProductID == productID && current->data.WarehouseID == warehouseID)
            return current;
        current = current->next;
    }
    return NULL;
}

//ham quan ly danh sach
int AddProduct(Product p) {
    ProductNode* newNode = new ProductNode;
    newNode->data = p;
    newNode->next = NULL;

    if (headSP == NULL) {
        headSP = newNode;
    } else {
        ProductNode* temp = headSP;
        while (temp->next != NULL) temp = temp->next;
        temp->next = newNode;
    }
    return 0;
}

int AddStockLevel(StockLevel s) {
    StockLevelNode* newNode = new StockLevelNode;
    newNode->data = s;
    newNode->next = NULL;

    if (headKho == NULL) {
        headKho = newNode;
    } else {
        StockLevelNode* temp = headKho;
        while (temp->next != NULL) temp = temp->next;
        temp->next = newNode;
    }
    return 0;
}

int DongBoTonKho() {
    ProductNode* p = headSP;
    while (p != NULL) {
        p->data.QuantityOnHand = 0;
        p = p->next;
    }

    StockLevelNode* s = headKho;
    while (s != NULL) {
        ProductNode* prod = TimSanPham(s->data.ProductID);
        if (prod != NULL) {
            prod->data.QuantityOnHand += s->data.QuantityOnHand;
        }
        s = s->next;
    }
    return 0;
}

int XoaDanhSachSP() {
    ProductNode* current = headSP;
    while (current != NULL) {
        ProductNode* nextNode = current->next;
        delete current;
        current = nextNode;
    }
    headSP = NULL;
    return 0;
}

int XoaDanhSachTonKho() {
    StockLevelNode* current = headKho;
    while (current != NULL) {
        StockLevelNode* nextNode = current->next;
        delete current;
        current = nextNode;
    }
    headKho = NULL;
    return 0;
}

int XoaTonKhoTheoSanPham(const string& productID) {
    StockLevelNode* current = headKho;
    StockLevelNode* previous = NULL;

    while (current != NULL) {
        if (current->data.ProductID == productID) {
            StockLevelNode* toDelete = current;
            if (previous == NULL) {
                headKho = current->next;
            } else {
                previous->next = current->next;
            }
            current = current->next;
            delete toDelete;
        } else {
            previous = current;
            current = current->next;
        }
    }
    return 0;
}

//Ham Xu Ly File
int DocProductCSV(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << " [LOI] Khong the mo file " << filename << "!\n";
        return 0;
    }

    string line;
    getline(file, line);

    int count = 0;
    while (getline(file, line)) {
        if (line.empty()) continue;
        Product p;
        stringstream ss(line);
        string token;
        
        getline(ss, p.ProductID, ',');
        getline(ss, p.ProductName, ',');
        getline(ss, p.CategoryName, ',');
        
        if (ss.peek() == '"') {
            ss.ignore();
            getline(ss, p.ProductDescription, '"');
            if (ss.peek() == ',') ss.ignore();
        } else {
            getline(ss, p.ProductDescription, ',');
        }
        
        getline(ss, token, ','); 
        p.ProductStandardCost = ChuyenSoThuc(token);
        
        getline(ss, token, ','); 
        p.ProductListPrice = ChuyenSoThuc(token);
        
        getline(ss, token, ','); 
        p.Profit = ChuyenSoThuc(token);
        
        getline(ss, p.Unit, ',');
        
        getline(ss, token); 
        p.MinStockLevel = ChuyenSoNguyen(token);
        
        p.QuantityOnHand = 0; 
        AddProduct(p);
        count++;
    }
    file.close();
    cout << " => Da tai " << count << " san pham tu " << filename << "!\n";
    return 0;
}

int LuuProductCSV(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << " [LOI] Khong the ghi file " << filename << "!\n";
        return 0;
    }

    file << "ProductID,ProductName,CategoryName,ProductDescription,ProductStandardCost,ProductListPrice,Profit,Unit,MinStockLevel\n";
    ProductNode* current = headSP;
    while (current != NULL) {
        Product& p = current->data;
        file << p.ProductID << ","
             << p.ProductName << ","
             << p.CategoryName << ","
             << p.ProductDescription << ","
             << p.ProductStandardCost << ","
             << p.ProductListPrice << ","
             << p.Profit << ","
             << p.Unit << ","
             << p.MinStockLevel << "\n";
        current = current->next;
    }
    file.close();
    return 0;
}

int DocStockLevelCSV(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << " [LOI] Khong the mo file " << filename << "!\n";
        return 0;
    }
    
    string line;
    getline(file, line);
    
    int count = 0;
    while (getline(file, line)) {
        if (line.empty()) continue;

        // Don \r cho dong bo
        if (!line.empty() && line[line.length() - 1] == '\r') {
            line.erase(line.length() - 1);
        }

        StockLevel s;
        string cols[5];
        int colIdx = 0;
        string temp = "";
        bool inQuotes = false;

        for (int i = 0; i < line.length(); i++) {
            if (line[i] == '"') {
                inQuotes = !inQuotes;
            } else if (line[i] == ',' && !inQuotes) {
                cols[colIdx] = temp;
                colIdx++;
                temp = "";
                if (colIdx >= 4) break;
            } else {
                temp += line[i];
            }
        }
        if (colIdx < 5) cols[colIdx] = temp;

        s.ProductID = cols[0];
        s.WarehouseID = ChuyenSoNguyen(cols[1]);
        s.QuantityOnHand = ChuyenSoNguyen(cols[2]);
        s.LastUpdated = cols[3];
        
        AddStockLevel(s);
        count++;
    }
    file.close();
    cout << " => Da tai " << count << " ban ghi ton kho tu " << filename << "!\n";
    return 0;
} 

int LuuStockLevelCSV(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << " [LOI] Khong the ghi file " << filename << "!\n";
        return 0;
    }

    file << "ProductID,WarehouseID,QuantityOnHand,LastUpdated\n";
    StockLevelNode* current = headKho;
    while (current != NULL) {
        StockLevel& s = current->data;
        file << s.ProductID << ","
             << s.WarehouseID << ","
             << s.QuantityOnHand << ","
             << s.LastUpdated << "\n";
        current = current->next;
    }
    file.close();
    return 0;
}

int LayMaGiaoDichMoi(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return 1;

    string line;
    getline(file, line);
    int maxId = 0;

    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string temp;
        getline(ss, temp, ',');
        int id = stoi(temp);
        if (id > maxId) maxId = id;
    }
    file.close();
    return maxId + 1;
}

int GhiGiaoDichCSV(const string& filename, const InventoryTransaction& t) {
    ofstream file(filename, ios::app);
    if (!file.is_open()) {
        cout << " [LOI] Khong the ghi file " << filename << "!\n";
        return 0;
    }

    file << t.TransactionID << ","
         << t.ProductID << ","
         << t.WarehouseID << ","
         << t.TransactionType << ","
         << t.Quantity << ","
         << t.TransactionDate << "\n";
    file.close();
    return 0;
}

//Ham Quan Ly San Pham
int InDanhSachSanPham() {
    if (headSP == NULL) {
        cout << "Kho hang hien dang trong!\n";
        return 0;
    }

    cout << string(105, '-') << endl;
    cout << left << setw(10) << "Ma SP"
         << setw(30) << "Ten San Pham"
         << setw(20) << "Loai Hang"
         << setw(15) << "Gia Ban"
         << setw(15) << "Ton Kho" 
         << "Toi Thieu" << endl;
    cout << string(105, '-') << endl;

    ProductNode* current = headSP;
    while (current != NULL) {
        string tonKhoStr = to_string(current->data.QuantityOnHand) + " " + current->data.Unit;
        cout << left << setw(10) << current->data.ProductID
             << setw(30) << current->data.ProductName
             << setw(20) << current->data.CategoryName
             << setw(15) << current->data.ProductListPrice
             << setw(15) << tonKhoStr
             << current->data.MinStockLevel << endl;
        current = current->next;
    }
    cout << string(105, '-') << endl;
    return 0;
}

int InChiTietSP(const string& targetID) {
    ProductNode* current = TimSanPham(targetID);
    if (current != NULL) {
        cout << "\n=== THONG TIN CHI TIET ===\n";
        cout << "- Ma SP      : " << current->data.ProductID << endl;
        cout << "- Ten SP     : " << current->data.ProductName << endl;
        cout << "- Loai Hang  : " << current->data.CategoryName << endl;
        cout << "- Gia Von    : $" << current->data.ProductStandardCost << endl;
        cout << "- Gia Ban    : $" << current->data.ProductListPrice << endl;
        cout << "- Loi Nhuan  : $" << current->data.Profit << endl;
        cout << "- DVT        : " << current->data.Unit << endl;
        cout << "- Ton kho    : " << current->data.QuantityOnHand << " (Toi thieu: " << current->data.MinStockLevel << ")" << endl;
        cout << "-------------------------------------\n";
    } else {
        cout << "\n[Canh bao] Khong tim thay du lieu voi ma: " << targetID << endl;
    }
    return 0;
}

//Ham Kiem Tra Ton Kho
int KiemTraTonKho(const string& productID, int warehouseID) {
    ProductNode* product = TimSanPham(productID);
    if (product == NULL) {
        cout << "\n[Canh bao] Khong tim thay san pham: " << productID << endl;
        return 1;
    }

    cout << "\n=== KIEM TRA TON KHO ===\n";
    cout << "Ma SP     : " << product->data.ProductID << endl;
    cout << "Ten SP    : " << product->data.ProductName << endl;

    if (warehouseID > 0) {
        StockLevelNode* stock = TimTonKho(productID, warehouseID);
        if (stock == NULL) {
            cout << "Kho " << warehouseID << " : 0 (chua co du lieu ton kho)\n";
        } else {
            cout << "Kho " << warehouseID << " : " << stock->data.QuantityOnHand
                 << " (cap nhat: " << stock->data.LastUpdated << ")\n";
        }
    } else {
        StockLevelNode* current = headKho;
        int total = 0;
        bool found = false;

        cout << string(50, '-') << endl;
        cout << left << setw(12) << "Ma kho" << setw(12) << "So luong" << "Ngay cap nhat" << endl;
        cout << string(50, '-') << endl;

        while (current != NULL) {
            if (current->data.ProductID == productID) {
                cout << left << setw(12) << current->data.WarehouseID
                     << setw(12) << current->data.QuantityOnHand
                     << current->data.LastUpdated << endl;
                total += current->data.QuantityOnHand;
                found = true;
            }
            current = current->next;
        }

        if (!found) {
            cout << "Chua co du lieu ton kho cho san pham nay.\n";
        } else {
            cout << string(50, '-') << endl;
            cout << "Tong ton kho tat ca cac kho: " << total << endl;
        }
    }
    cout << string(50, '-') << endl;
    return 0;
}

//Ham Quan Ly San Pham
Product NhapThongTinSanPham() {
    Product p;
    cout << "Ma SP: ";
    getline(cin, p.ProductID);
    cout << "Ten SP: ";
    getline(cin, p.ProductName);
    cout << "Loai hang: ";
    getline(cin, p.CategoryName);
    cout << "Mo ta: ";
    getline(cin, p.ProductDescription);
    cout << "Gia von: ";
    while (!(cin >> p.ProductStandardCost) || p.ProductStandardCost < 0) {
        cout << " [LOI] Gia von phai la so hop le va >= 0! Nhap lai: ";
        cin.clear();
        cin.ignore(1000, '\n');
    }
    cout << "Gia ban: ";
    while (!(cin >> p.ProductListPrice) || p.ProductListPrice < 0) {
        cout << " [LOI] Gia ban phai la so hop le va >= 0! Nhap lai: ";
        cin.clear();
        cin.ignore(1000, '\n');
    }
    p.Profit = p.ProductListPrice - p.ProductStandardCost;
    cin.ignore(1000, '\n');
    cout << "Don vi tinh (VD: Cai, Chiec): ";
    getline(cin, p.Unit);
    cout << "Nguong ton kho toi thieu: ";
    while (!(cin >> p.MinStockLevel) || p.MinStockLevel < 0) {
        cout << " [LOI] Nguong toi thieu phai la so nguyen >= 0! Nhap lai: ";
        cin.clear();
        cin.ignore(1000, '\n');
    }
    p.QuantityOnHand = 0;
    cin.ignore(1000, '\n');
    return p;
}

int SuaThongTinSanPham(Product p) {
    ProductNode* current = TimSanPham(p.ProductID);
    if (current == NULL) return 1;

    int oldQty = current->data.QuantityOnHand;
    current->data = p;
    current->data.QuantityOnHand = oldQty;
    return 0;
}

int XoaSanPham(const string& targetID) {
    ProductNode* current = headSP;
    ProductNode* previous = NULL;

    while (current != NULL) {
        if (current->data.ProductID == targetID) {
            if (previous == NULL) {
                headSP = current->next;
            } else {
                previous->next = current->next;
            }
            delete current;

            XoaTonKhoTheoSanPham(targetID);
            LuuStockLevelCSV(FILE_STOCK);
            return 0;
        }
        previous = current;
        current = current->next;
    }
    return 1;
}

//Ham Xuat Nhap Ton Kho
int CapNhatTonKho(const string& productID, int warehouseID, int soLuongThayDoi) {
    StockLevelNode* stock = TimTonKho(productID, warehouseID);

    if (stock == NULL) {
        if (soLuongThayDoi < 0) return 3;
        StockLevel s;
        s.ProductID = productID;
        s.WarehouseID = warehouseID;
        s.QuantityOnHand = soLuongThayDoi;
        s.LastUpdated = LayNgayHienTai();
        AddStockLevel(s);
        return 0;
    }

    if (stock->data.QuantityOnHand + soLuongThayDoi < 0) return 5;
    stock->data.QuantityOnHand += soLuongThayDoi;
    stock->data.LastUpdated = LayNgayHienTai();
    return 0;
}

int InputTonKho(const string& targetID, int warehouseID, int quantity) {
    if (TimSanPham(targetID) == NULL) return 1;
    if (quantity <= 0) return 2;
    
    int res = CapNhatTonKho(targetID, warehouseID, quantity);
    if (res != 0) return res;

    ProductNode* pNode = TimSanPham(targetID);
    if (pNode != NULL) pNode->data.QuantityOnHand += quantity;

    LuuStockLevelCSV(FILE_STOCK);

    InventoryTransaction t;
    t.TransactionID = LayMaGiaoDichMoi(FILE_TRANSACTION);
    t.ProductID = targetID;
    t.WarehouseID = warehouseID;
    t.TransactionType = "IN";
    t.Quantity = quantity;
    t.TransactionDate = LayNgayHienTai();
    GhiGiaoDichCSV(FILE_TRANSACTION, t);

    return 0;
}

int OutputTonKho(const string& targetID, int warehouseID, int quantity) {
    if (TimSanPham(targetID) == NULL) return 1;
    if (quantity <= 0) return 2;

    StockLevelNode* stock = TimTonKho(targetID, warehouseID);
    if (stock == NULL || stock->data.QuantityOnHand < quantity) {
        int currentQty = (stock == NULL) ? 0 : stock->data.QuantityOnHand;
        cout << "[chi tiet] Ton kho tai kho " << warehouseID << " chi con: " << currentQty << "\n";
        return 3;
    }

    int res = CapNhatTonKho(targetID, warehouseID, -quantity);
    if (res != 0) return res;

    ProductNode* pNode = TimSanPham(targetID);
    if (pNode != NULL) pNode->data.QuantityOnHand -= quantity;

    LuuStockLevelCSV(FILE_STOCK);

    if (pNode != NULL && pNode->data.QuantityOnHand < pNode->data.MinStockLevel) {
        cout << "[canh bao] San pham " << targetID << " rot xuong duoi muc ton kho toi thieu (" 
             << pNode->data.QuantityOnHand << " < " << pNode->data.MinStockLevel << ")!\n";
    }

    InventoryTransaction t;
    t.TransactionID = LayMaGiaoDichMoi(FILE_TRANSACTION);
    t.ProductID = targetID;
    t.WarehouseID = warehouseID;
    t.TransactionType = "OUT";
    t.Quantity = quantity;
    t.TransactionDate = LayNgayHienTai();
    GhiGiaoDichCSV(FILE_TRANSACTION, t);

    return 0;
}

int KiemKeKhoThucTe(const string& targetID, int warehouseID, int actualQty) {
    ProductNode* pNode = TimSanPham(targetID);
    if (pNode == NULL) return 1;
    if (actualQty < 0) return 2;

    StockLevelNode* stock = TimTonKho(targetID, warehouseID);
    int systemQty = (stock == NULL) ? 0 : stock->data.QuantityOnHand;
    int diff = actualQty - systemQty;

    cout << " - So luong he thong : " << systemQty << "\n";
    cout << " - So luong thuc te  : " << actualQty << "\n";
    cout << " - Chenh lech        : " << diff << "\n";

    if (diff == 0) {
        cout << "=> Khong co chenh lech, khong can dieu chinh.\n";
        return 0;
    }

    int res = CapNhatTonKho(targetID, warehouseID, diff);
    if (res != 0) return res;

    pNode->data.QuantityOnHand += diff;
    LuuStockLevelCSV(FILE_STOCK);

    InventoryTransaction t;
    t.TransactionID = LayMaGiaoDichMoi(FILE_TRANSACTION);
    t.ProductID = targetID;
    t.WarehouseID = warehouseID;
    
    if (diff > 0) {
        t.TransactionType = "IN";
        t.Quantity = diff;
    } else {
        t.TransactionType = "OUT";
        t.Quantity = -diff;
    }
    
    t.TransactionDate = LayNgayHienTai();
    GhiGiaoDichCSV(FILE_TRANSACTION, t);

    return 0;
}

//Ham Bao Cao Va Thong Ke
int BaoCaoTonKho() {
    cout << "\n=== BAO CAO TON KHO ===\n";
    cout << string(70, '-') << endl;
    cout << left << setw(10) << "Ma SP"
         << setw(28) << "Ten SP"
         << setw(8) << "Kho"
         << setw(10) << "So luong"
         << "Ngay cap nhat" << endl;
    cout << string(70, '-') << endl;

    StockLevelNode* current = headKho;
    while (current != NULL) {
        ProductNode* p = TimSanPham(current->data.ProductID);
        string name = (p != NULL) ? p->data.ProductName : "Khong xac dinh";
        cout << left << setw(10) << current->data.ProductID
             << setw(28) << name
             << setw(8) << current->data.WarehouseID
             << setw(10) << current->data.QuantityOnHand
             << current->data.LastUpdated << endl;
        current = current->next;
    }
    cout << string(70, '-') << endl;
    return 0;
}

int BaoCaoSapHet() {
    if (headSP == NULL) {
        cout << "Kho hang hien dang trong!\n";
        return 0;
    }

    cout << "\n=== DANH SACH SAN PHAM SAP HET HANG ===\n";
    cout << string(105, '-') << endl;
    cout << left << setw(10) << "Ma SP"
         << setw(30) << "Ten San Pham"
         << setw(20) << "Loai Hang"
         << setw(15) << "Gia Ban"
         << setw(15) << "Ton Kho" 
         << "Toi Thieu" << endl;
    cout << string(105, '-') << endl;

    ProductNode* current = headSP;
    bool found = false;
    while (current != NULL) {
        if (current->data.QuantityOnHand < current->data.MinStockLevel) {
            string tonKhoStr = to_string(current->data.QuantityOnHand) + " " + current->data.Unit;
            cout << left << setw(10) << current->data.ProductID
                 << setw(30) << current->data.ProductName
                 << setw(20) << current->data.CategoryName
                 << setw(15) << current->data.ProductListPrice
                 << setw(15) << tonKhoStr
                 << current->data.MinStockLevel << endl;
            found = true;
        }
        current = current->next;
    }
    if (!found) {
         cout << "Tuyet voi! Khong co san pham nao ton kho duoi muc toi thieu.\n";
    }
    cout << string(105, '-') << endl;
    return 0;
}

int ThongKeLoiNhuan() {
    cout << "\n=== THONG KE LOI NHUAN ===\n";
    ProductNode* current = headSP;
    while (current != NULL) {
        cout << current->data.ProductID << " - "
             << current->data.ProductName << " - "
             << current->data.Profit << endl;
        current = current->next;
    }
    return 0;
}

int ThongKeGiaTriKho() {
    cout << "\n=== THONG KE GIA TRI HANG TON KHO ===\n";
    if (headSP == NULL) {
        cout << "Kho hang hien dang trong!\n";
        return 0;
    }

    double totalValue = 0;
    cout << string(95, '-') << endl;
    cout << left << setw(10) << "Ma SP"
         << setw(30) << "Ten San Pham"
         << setw(15) << "So Luong"
         << setw(15) << "Gia Von ($)"
         << "Tong Gia Tri ($)" << endl;
    cout << string(95, '-') << endl;

    ProductNode* current = headSP;
    while (current != NULL) {
        if (current->data.QuantityOnHand > 0) {
            double value = current->data.QuantityOnHand * current->data.ProductStandardCost;
            totalValue += value;
            cout << left << setw(10) << current->data.ProductID
                 << setw(30) << current->data.ProductName
                 << setw(15) << current->data.QuantityOnHand
                 << setw(15) << current->data.ProductStandardCost
                 << fixed << setprecision(2) << value << endl;
        }
        current = current->next;
    }
    cout << string(95, '-') << endl;
    cout << "=> TONG GIA TRI TOAN KHO: $" << fixed << setprecision(2) << totalValue << endl;
    return 0;
}

int NhatKyGiaoDich(const string& filename) {
    string targetMonth;
    cout << "\nNhap thang can xem (Dinh dang YYYY-MM, VD: 2017-08 hoac 2026-06): ";
    getline(cin, targetMonth);

    ifstream file(filename);
    if (!file.is_open()) {
        cout << " [LOI] Khong the mo file " << filename << "!\n";
        return 1;
    }

    cout << "\n=== NHAT KY GIAO DICH THANG " << targetMonth << " ===\n";
    cout << string(80, '-') << endl;
    cout << left << setw(10) << "ID" << setw(15) << "Ma SP" << setw(10) << "Kho"
         << setw(15) << "Loai GD" << setw(15) << "So Luong" << "Ngay GD" << endl;
    cout << string(80, '-') << endl;

    string line, id, pid, wid, type, qty, date;
    getline(file, line); 
    bool found = false;

    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        getline(ss, id, ','); getline(ss, pid, ','); getline(ss, wid, ',');
        getline(ss, type, ','); getline(ss, qty, ','); getline(ss, date);

        if (date.find(targetMonth) == 0) { 
            cout << left << setw(10) << id << setw(15) << pid << setw(10) << wid
                 << setw(15) << type << setw(15) << qty << date << endl;
            found = true;
        }
    }
    file.close();

    if (!found) cout << "Khong co giao dich nao trong thang " << targetMonth << ".\n";
    cout << string(80, '-') << endl;
    return 0;
}

//Main
int main() {
    int choice;
    int choiceN1, choiceN2, choiceN3;
    Product p;
    string targetID;
    int warehouseID, quantity, res;

    cout << "He thong dang khoi dong...\n";
    DocProductCSV(FILE_PRODUCT);
    DocStockLevelCSV(FILE_STOCK);
    DongBoTonKho();
    cout << " => Da dong bo ton kho tu StockLevel vao Product!\n";

    do {
        system("cls");
        cout << "\n=== CHUONG TRINH QUAN LY KHO HANG ===\n";
        cout << "1. Quan ly san pham\n";
        cout << "2. Quan ly nhap/xuat san pham\n";
        cout << "3. Bao cao va thong ke\n";
        cout << "4. Thoat\n";
        cout << "Chon chuc nang (1-4): ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Lua chon khong hop le!\n";
            choice = 0;
            continue;
        }
        cin.ignore(1000, '\n');

        switch (choice) {
            case 1:
                cout << "\n=== QUAN LY SAN PHAM ===\n";
                cout << "1. Hien thi danh sach san pham\n";
                cout << "2. Them san pham\n";
                cout << "3. Sua san pham\n";
                cout << "4. Xoa san pham\n";
                cout << "5. Tim kiem theo ID\n";
                cout << "6. Thoat\n";
                cout << "Chon chuc nang (1-6): ";
                if (!(cin >> choiceN1)) {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    cout << "Lua chon khong hop le!\n";
                    break;
                }
                cin.ignore(1000, '\n');
                switch (choiceN1) {
                    case 1:
                        InDanhSachSanPham();
                        break;
                    case 2:
                        p = NhapThongTinSanPham();
                        if (TimSanPham(p.ProductID) != NULL) {
                            BaoLoi(4);
                            break;
                        }
                        AddProduct(p);
                        LuuProductCSV(FILE_PRODUCT);
                        cout << "Da them san pham va luu vao " << FILE_PRODUCT << "!\n";
                        break;
                    case 3:
                        p = NhapThongTinSanPham();
                        res = SuaThongTinSanPham(p);
                        if (res == 0) {
                            LuuProductCSV(FILE_PRODUCT);
                            cout << "Da cap nhat san pham va luu vao " << FILE_PRODUCT << "!\n";
                        } else {
                            BaoLoi(res);
                        }
                        break;
                    case 4:
                        cout << "Nhap ma SP can xoa: ";
                        getline(cin, targetID);
                        res = XoaSanPham(targetID);
                        if (res == 0) {
                            LuuProductCSV(FILE_PRODUCT);
                            DongBoTonKho();
                            cout << "Da xoa san pham va cap nhat CSV!\n";
                        } else {
                            BaoLoi(res);
                        }
                        break;
                    case 5:
                        cout << "Nhap ma SP can tim: ";
                        getline(cin, targetID);
                        InChiTietSP(targetID);
                        break;
                    default:
                        cout << "Lua chon khong hop le!\n";
                }
                break;
            case 2:
                cout << "\n=== QUAN LY NHAP/XUAT SAN PHAM ===\n";
                cout << "1. Nhap kho\n";
                cout << "2. Xuat kho\n";
                cout << "3. Kiem tra ton kho\n";
                cout << "4. Kiem ke kho thuc te\n";
                cout << "5. Thoat\n";
                cout << "Chon chuc nang (1-5): ";
                if (!(cin >> choiceN2)) {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    cout << "Lua chon khong hop le!\n";
                    break;
                }
                cin.ignore(1000, '\n');
                switch (choiceN2) {
                    case 1:
                        cout << "Nhap ma SP: ";
                        getline(cin, targetID);
                        cout << "Nhap ma kho (1-3): ";
                        while (!(cin >> warehouseID)) {
                            cout << " [LOI] Nhap sai dinh dang! Nhap lai ma kho: ";
                            cin.clear();
                            cin.ignore(1000, '\n');
                        }
                        cout << "So luong nhap: ";
                        while (!(cin >> quantity)) {
                            cout << " [LOI] Nhap sai dinh dang! Nhap lai so luong: ";
                            cin.clear();
                            cin.ignore(1000, '\n');
                        }
                        cin.ignore(1000, '\n');
                        res = InputTonKho(targetID, warehouseID, quantity);
                        if (res == 0) {
                            cout << "Nhap kho thanh cong! Da luu StockLevel + InventoryTransaction.\n";
                        } else {
                            BaoLoi(res);
                        }
                        break;
                    case 2:
                        cout << "Nhap ma SP: ";
                        getline(cin, targetID);
                        cout << "Nhap ma kho (1-3): ";
                        while (!(cin >> warehouseID)) {
                            cout << " [LOI] Nhap sai dinh dang! Nhap lai ma kho: ";
                            cin.clear();
                            cin.ignore(1000, '\n');
                        }
                        cout << "So luong xuat: ";
                        while (!(cin >> quantity)) {
                            cout << " [LOI] Nhap sai dinh dang! Nhap lai so luong: ";
                            cin.clear();
                            cin.ignore(1000, '\n');
                        }
                        cin.ignore(1000, '\n');
                        res = OutputTonKho(targetID, warehouseID, quantity);
                        if (res == 0) {
                            cout << "Xuat kho thanh cong! Da luu StockLevel + InventoryTransaction.\n";
                        } else {
                            BaoLoi(res);
                        }
                        break;
                    case 3:
                        cout << "Nhap ma SP: ";
                        getline(cin, targetID);
                        cout << "Nhap ma kho (0 = tat ca cac kho): ";
                        while (!(cin >> warehouseID)) {
                            cout << " [LOI] Nhap sai dinh dang! Nhap lai ma kho: ";
                            cin.clear();
                            cin.ignore(1000, '\n');
                        }
                        cin.ignore(1000, '\n');
                        KiemTraTonKho(targetID, warehouseID);
                        break;
                    case 4:
                        cout << "Nhap ma SP can kiem ke: ";
                        getline(cin, targetID);
                        cout << "Nhap ma kho (1-3): ";
                        while (!(cin >> warehouseID)) {
                            cout << " [LOI] Nhap sai dinh dang! Nhap lai ma kho: ";
                            cin.clear();
                            cin.ignore(1000, '\n');
                        }
                        cout << "So luong kiem ke thuc te tai kho: ";
                        while (!(cin >> quantity)) {
                            cout << " [LOI] Nhap sai dinh dang! Nhap lai so luong: ";
                            cin.clear();
                            cin.ignore(1000, '\n');
                        }
                        cin.ignore(1000, '\n');
                        res = KiemKeKhoThucTe(targetID, warehouseID, quantity);
                        if (res == 0) {
                            cout << "Kiem ke hoan tat! Da luu dieu chinh vao StockLevel.\n";
                        } else if (res != 1 && res != 2) {
                            BaoLoi(res);
                        }
                        break;
                    default:
                        cout << "Lua chon khong hop le!\n";
                }
                break;
            case 3:
                cout << "\n=== BAO CAO VA THONG KE ===\n";
                cout << "1. Bao cao ton kho (theo tung kho)\n";
                cout << "2. Thong ke loi nhuan\n";
                cout << "3. San pham sap het hang\n";
                cout << "4. Thong ke tong gia tri ton kho\n";
                cout << "5. Nhat ky giao dich trong ky (Thang)\n";
                cout << "6. Thoat\n";
                cout << "Chon chuc nang (1-6): ";
                if (!(cin >> choiceN3)) {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    cout << "Lua chon khong hop le!\n";
                    break;
                }
                cin.ignore(1000, '\n');
                switch (choiceN3) {
                    case 1:
                        BaoCaoTonKho();
                        break;
                    case 2:
                        ThongKeLoiNhuan();
                        break;
                    case 3:
                        BaoCaoSapHet();
                        break;
                    case 4:
                        ThongKeGiaTriKho();
                        break;
                    case 5:
                        NhatKyGiaoDich(FILE_TRANSACTION);
                        break;
                    default:
                        cout << "Lua chon khong hop le!\n";
                }
                break;
            case 4:
                cout << "Dang thoat va giai phong bo nho...\n";
                XoaDanhSachSP();
                XoaDanhSachTonKho();
                break;
            default:
                cout << "Lua chon khong hop le!\n";
        }
        
        if (choice != 4) {
            cout << "\n";
            system("pause");
        }
    } while (choice != 4);

    return 0;
}
