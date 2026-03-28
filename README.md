# Voxelizer 

## 1. Penjelasan Singkat Program
Program ini adalah sebuah aplikasi *Voxelizer* berbasis *Command Line Interface* (CLI) yang dikembangkan menggunakan bahasa C++. Program ini berfungsi untuk membaca model geometri 3D dalam format `.obj` (berisi *vertices* dan *faces* segitiga) dan mengonversinya menjadi representasi spasial diskrit berupa kubus-kubus berukuran seragam (*voxel*). 

Program ini dibangun dengan algoritma **Divide and Conquer** yang diimplementasikan melalui struktur data **Octree**. Untuk memaksimalkan efisiensi komputasi dan memori, program menggunakan metode **Separating Axis Theorem (SAT)** untuk melakukan uji irisan (*intersection test*) dan pemangkasan (*pruning*). Selain itu, program ini juga menerapkan **Concurrency (Multithreading)** menggunakan pustaka standar C++ (`std::async` dan `std::mutex`) untuk mendistribusikan beban komputasi secara paralel, sehingga waktu eksekusi program menjadi sangat cepat. Program ini juga dilengkapi dengan `.obj` viewer yang dibangun dengan pustaka `raylib` untuk menampilkan hasil *Voxelization* yang telah dilakukan.

## 2. Requirement Program
Untuk melakukan kompilasi dan menjalankan program ini, sistem Anda harus memenuhi beberapa persyaratan berikut:
* **Sistem Operasi:** Lingkungan berbasis Unix (Linux, macOS) atau Windows Subsystem for Linux (WSL).
* **Compiler:** `g++` atau compiler C++ lain yang mendukung standar **C++11** atau lebih baru (dibutuhkan untuk *library* `<future>`, `<mutex>`, dan `<thread>`).
* **Build Tool:** GNU `make` untuk menjalankan instruksi kompilasi otomatis pada `Makefile`.

## 3. Cara Mengkompilasi Program
Program ini dilengkapi dengan `Makefile` untuk mempermudah proses kompilasi. Ikuti langkah-langkah berikut di dalam terminal:

1. Buka terminal dan navigasikan (*change directory*) ke folder *root* proyek ini.
2. (Opsional) Bersihkan *file* kompilasi sebelumnya dengan perintah:
   ```bash
   make clean
   ```
3. Lakukan kompilasi *source code* dengan menjalankan perintah:
    ```bash
    make all
    ```
4. Jika kompilasi berhasil, sebuah *executable file* `voxelizer.exe` dan `viewer.exe` akan terbentuk di dalam folder `bin` direktori proyek.

## 4. Cara Menjalankan dan Menggunakan Program
Setelah program berhasil dikompilasi, Anda dapat menjalankannya melalui terminal. Terdapat dua fitur untuk menjalankan *Voxelizer* dan *Viewer*

Untuk menjalankan *Voxelizer* Anda dapat menggunakan command sebagai berikut:
    ```bash
    ./bin/voxelizer.exe <path_ke_file_input.obj> <kedalaman_octree> <path_ke_file_output.obj>
    ```

Contoh penggunaan:
    ```bash
    ./voxelizer.exe test/pumpkin.obj 6 test/pumpkin-voxel.obj
    ```

Untuk menjalankan *Viewer* Anda dapat menggunakan command sebagai berikut:
    ```bash
    ./bin/viewer.exe <path_ke_file_input.obj>
    ```

Contoh penggunaan:
    ```bash
    ./bin/viewer.exe test/pumpkin-voxel.obj
    ```

## 5. Author
Nama: An-Dafa Anza Avansyah
NIM 13524038