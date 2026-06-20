# BÀI TẬP LỚN MÔN KỸ THUẬT LẬP TRÌNH

## Đề tài: Hệ thống quản lý kho bằng C++
* **Nhóm thực hiện:** Nhóm 18
* **Môi trường hệ thống:** Console, lưu trữ dữ liệu qua file CSV (`Product.csv`, `StockLevel.csv`, `InventoryTransaction.csv`).

## Thành viên nhóm
* Trần Vũ Anh - 202418840
* Trần Huy Tuấn - 202419009
* Nguyễn Đình Hải - 202419053

## Các tính năng chính
* Quản lý thông tin sản phẩm (Thêm, sửa, xóa, tìm kiếm).
* Quản lý nhập kho, xuất kho theo từng mã kho.
* Kiểm kê kho thực tế và tự động điều chỉnh chênh lệch.
* Báo cáo sản phẩm sắp hết hàng, thống kê giá trị kho và lợi nhuận.

## Hướng dẫn biên dịch và chạy chương trình (GitHub Codespaces)
1. Tại giao diện chính của kho lưu trữ, nhấn vào nút **Code** (màu xanh lá), chọn tab **Codespaces** và tạo một không gian làm việc mới (`Create codespace on main`).
2. Sau khi hệ thống tải xong giao diện, mở **Terminal** (tổ hợp phím `Ctrl + \``) và chạy lần lượt các lệnh sau:

```bash
cd BTLKTLT
g++ main.cpp -o main
./main
