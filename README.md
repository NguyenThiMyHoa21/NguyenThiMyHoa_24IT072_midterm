# Midterm Project: Implementation of `ls` Utility

> **Học phần:** Lập Trình Hệ Thống  
> **Sinh viên thực hiện:** Nguyễn Thị Mỹ Hoa  
> **Mã số sinh viên:** 24IT072  
> **GitHub Repository:** [NguyenThiMyHoa_24IT072_midterm](https://github.com/NguyenThiMyHoa21/NguyenThiMyHoa_24IT072_midterm)  
> **Môi trường thực thi:** NetBSD/amd64

---

## 📌 1. Giới thiệu dự án (Project Description)

Dự án hiện thực lại một phiên bản của tiện ích dòng lệnh **`ls(1)`** trên hệ điều hành **NetBSD**.

Chương trình cho phép liệt kê thông tin tệp và thư mục trong hệ thống, hỗ trợ các tùy chọn được yêu cầu trong phạm vi bài tập và mô phỏng các chức năng chính của `ls(1)` trên NetBSD.

- Có kiểm tra lỗi cấp phát và thao tác hệ thống nhằm hạn chế lỗi bộ nhớ.
- Các vùng nhớ động được giải phóng sau khi sử dụng trong các luồng xử lý chính.
- Chương trình được tổ chức theo dạng mô-đun, hỗ trợ xử lý file, thư mục, file ẩn, sắp xếp và liệt kê đệ quy.

---

## 📁 2. Cấu trúc mã nguồn (Project Structure)

Chương trình được thiết kế theo cấu trúc **mô-đun hóa (modular design)** với thư mục chứa tệp tiêu đề (`include/`) và thư mục chứa tệp nguồn (`src/`), giúp dự án dễ phát triển, kiểm thử, bảo trì và mở rộng:

```text
NguyenThiMyHoa_24IT072_midterm/
├── include/
│   └── ls.h            # Tệp tiêu đề chung: Định nghĩa cấu trúc dữ liệu và khai báo prototype hàm
├── src/
│   ├── main.c          # Điểm nhập chương trình (entry point) và điều phối xử lý operands
│   ├── options.c       # Phân tích tùy chọn dòng lệnh (command-line options parsing)
│   ├── util.c          # Hàm tiện ích bổ trợ và phân loại loại tệp (file classification)
│   ├── sort.c          # Các hàm sắp xếp (theo tên, dung lượng -S, thời gian -t, đảo ngược -r)
│   ├── format.c        # Định dạng đầu ra (-l, -n, -h, -i, -s) và hiển thị thông tin tệp
│   └── list.c          # Đọc thư mục (opendir/readdir) và liệt kê nội dung
├── Makefile            # Makefile biên dịch dự án với cc (cờ -Wall -Wextra -Werror -std=c11)
├── .gitignore          # Cấu hình bỏ qua các tệp đối tượng (.o) và tệp thực thi nhị phân
└── README.md           # Báo cáo và tài liệu hướng dẫn dự án
```

### 📄 Chi tiết các tệp nguồn (`Source Files`)

- **`src/main.c`**: Điểm nhập chính của chương trình, tiếp nhận các tham số truyền vào và điều phối xử lý operands.
- **`src/options.c`**: Phân tích cờ lệnh (`getopt`) và áp dụng các quy tắc ưu tiên / ghi đè (_mutual overrides_).
- **`src/util.c`**: Cung cấp các hàm tiện ích phân loại tệp (`-F`), xử lý ký tự không in được (`-q`, `-w`), ...
- **`src/sort.c`**: Hiện thực sắp xếp `qsort` theo tên, dung lượng (`-S`), thời gian (`-t`), hoặc đảo ngược (`-r`).
- **`src/format.c`**: Đảm nhận định dạng đầu ra: hiển thị chi tiết (`-l`), UID/GID dạng số (`-n`), inode (`-i`), block size (`-s`, `-h`, `-k`).
- **`src/list.c`**: Thực hiện đọc nội dung thư mục, thu thập thông tin tệp và hỗ trợ duyệt đệ quy (`-R`).
- **`include/ls.h`**: Chứa toàn bộ định nghĩa cấu trúc dữ liệu và khai báo hàm dùng chung cho tất cả các module.

---

## 🛠️ 3. Danh sách các tùy chọn đã hiện thực (Supported Options)

Chương trình sử dụng cú pháp:

```bash
ls [-AacdFfhiklnqRrSstuw] [file ...]
```

### 📋 Bảng mô tả chi tiết các tùy chọn

| Option | Description (Mô tả chi tiết)                                                                            |
| :----: | :------------------------------------------------------------------------------------------------------ |
|  `-A`  | Hiển thị tất cả các mục ngoại trừ `.` và `..` (_Show hidden files except . and .._).                    |
|  `-a`  | Hiển thị tất cả các mục bao gồm cả `.` và `..` (_Show all files including . and .._).                   |
|  `-c`  | Sử dụng thời gian thay đổi trạng thái tệp (`st_ctime`) thay cho mtime để sắp xếp/hiển thị.              |
|  `-d`  | Liệt kê thư mục như tệp thông thường, không duyệt đệ quy vào trong (_List directories as plain files_). |
|  `-F`  | Thêm ký tự phân loại loại tệp vào sau tên (_Classify file types_: `/`, `*`, `@`, `=`, `\|`).            |
|  `-f`  | Không thực hiện sắp xếp đầu ra (_Do not sort output_).                                                  |
|  `-h`  | Hiển thị dung lượng tệp/block theo dạng dễ đọc cho người dùng (_Human-readable sizes_: B, K, M, G).     |
|  `-i`  | Hiển thị số inode (`st_ino`) của mỗi tệp (_Display inode numbers_).                                     |
|  `-k`  | Hiển thị kích thước block theo đơn vị Kilobytes (1024 bytes) (_Display sizes in kilobytes_).            |
|  `-l`  | Hiển thị định dạng danh sách chi tiết (_Use long listing format_).                                      |
|  `-n`  | Định dạng chi tiết nhưng hiển thị UID và GID dạng số (_Display numeric user and group IDs_).            |
|  `-q`  | Thay thế các ký tự không in được bằng dấu `?` (_Replace non-printable characters with ?_).              |
|  `-R`  | Duyệt và liệt kê đệ quy các thư mục con (_List directories recursively_).                               |
|  `-r`  | Đảo ngược thứ tự sắp xếp (_Reverse sorting order_).                                                     |
|  `-S`  | Sắp xếp danh sách tệp theo kích thước giảm dần (_Sort by file size_).                                   |
|  `-s`  | Hiển thị số block hệ thống được cấp phát cho tệp (_Display allocated blocks_).                          |
|  `-t`  | Sắp xếp danh sách tệp theo thời gian sửa đổi gần nhất (_Sort by modification time_).                    |
|  `-u`  | Sử dụng thời gian truy cập gần nhất (`st_atime`) thay cho mtime để sắp xếp/hiển thị.                    |
|  `-w`  | Cho phép in thô ký tự không in được (_Display non-printable characters as raw characters_).             |

### 🔄 Quy tắc ưu tiên và ghi đè (Mutual Overrides)

Theo hướng dẫn của NetBSD `ls`, cờ xuất hiện **sau cùng (bên phải nhất)** trên dòng lệnh sẽ quyết định hành vi:

> [!NOTE]
>
> - **`-w` và `-q`**: Cờ xuất hiện sau cùng quyết định định dạng cho ký tự không in được.
> - **`-l` và `-n`**: Cờ xuất hiện sau cùng quyết định định dạng hiển thị chi tiết (Tên vs UID/GID số).
> - **`-c` và `-u`**: Cờ xuất hiện sau cùng quyết định trường thời gian sử dụng (`ctime` hay `atime`).
> - **`-R` và `-d`**: Cờ xuất hiện sau cùng quyết định hành vi duyệt đệ quy hay xem thư mục như tệp.
> - **`-k` và `-h`**: Cờ xuất hiện sau cùng quyết định tính kích thước block theo KB hay Human-readable.

---

## 💻 4. Biên dịch & Thực thi (Compilation & Usage)

### 4.1. Môi trường phát triển (Development Environment)

- **Hệ điều hành:** NetBSD/amd64
- **Ngôn ngữ lập trình:** C (chuẩn **C11**)
- **Trình biên dịch:** `cc` 
- **Công cụ build:** `Make`
- **Quản lý mã nguồn:** Git & GitHub

### 4.2. Biên dịch dự án (Compilation)

Dự án được biên dịch với các cờ cảnh báo nghiêm ngặt: `-Wall -Wextra -Werror -std=c11`.

```bash
# Biên dịch dự án (tạo file thực thi 'ls')
make

# Dọn dẹp các file .o và file nhị phân thực thi
make clean
```

### 4.3. Các ví dụ sử dụng (Usage Examples)

```bash
# 1. Liệt kê cơ bản
./ls

# 2. Liệt kê tất cả tệp (bao gồm tệp ẩn . và ..)
./ls -a

# 3. Liệt kê tệp ẩn trừ . và ..
./ls -A

# 4. Hiển thị định dạng chi tiết (long format)
./ls -l

# 5. Hiển thị số inode
./ls -i

# 6. Sắp xếp theo kích thước tệp giảm dần
./ls -S

# 7. Sắp xếp theo thời gian sửa đổi
./ls -t

# 8. Đảo ngược thứ tự sắp xếp
./ls -r

# 9. Liệt kê đệ quy các thư mục con
./ls -R

# 10. Liệt kê thư mục như tệp thông thường (không vào bên trong)
./ls -d src

# 11. Hiển thị chi tiết với kích thước dễ đọc (Human-readable)
./ls -lh

# 12. Hiển thị UID / GID dạng số
./ls -n
```

---

## ⚡ 5. Kiểm thử & Độ tin cậy (Testing & Robustness)

Dự án đã được kiểm thử trên môi trường **NetBSD/amd64** với nhiều tùy chọn và trường hợp sử dụng khác nhau:

1. **Biên dịch nghiêm ngặt:**
   - Biên dịch thành công với cờ `-Wall -Wextra -Werror -std=c11`, không có lỗi hay cảnh báo.

2. **Quản lý bộ nhớ an toàn (`Memory Safety`):**
   - Các vùng nhớ động được cấp phát trong quá trình xử lý được giải phóng sau khi sử dụng.

3. **Xử lý trường hợp biên & lỗi hệ thống:**
   - Đã kiểm thử với nhiều dạng operand như file, thư mục, file ẩn và nhiều operand.
   - Các thao tác chính với hệ thống file như `opendir()`, `readdir()` và `lstat()` được kiểm tra lỗi. Khi xảy ra lỗi, chương trình thông báo ra `stderr` và xử lý theo trạng thái lỗi phù hợp.

---

## 🔗 6. Thông tin Repository & Tham khảo

- **GitHub Repository:** [https://github.com/NguyenThiMyHoa21/NguyenThiMyHoa_24IT072_midterm](https://github.com/NguyenThiMyHoa21/NguyenThiMyHoa_24IT072_midterm)
- **Tham chiếu (Reference):** Dựa trên đặc tả trang hướng dẫn `ls(1)` manual của NetBSD.

---

<p align="center">
  <i>Nguyễn Thị Mỹ Hoa - 24IT072 | Lập trình hệ thống</i>
</p>
