# Rombongan Identification Algorithm Implementation

Perangkat lunak sebagai pemenuhan topik skripsi dengan judul **Identifikasi Pola Rombongan Pada Data Pejalan Kaki** UNPAR. Dibuat oleh 2017730017.

## Kebutuhan Perangkat Lunak

1. C Build Tools (CMake dan Make, tersedia pada distribusi standar Linux atau [`Cygwin`](https://www.cygwin.com/) pada Windows)
## Instalasi

1. _Clone_ repository ini.
2. Buatah sebuah `Makefile` dengan menjalankan perintah `cmake .` pada _terminal_ Anda.
3. Lakukan proses kompilasi perangkat lunak dengan menjalankan perintah `make`.
4. Apabila proses kompilasi sukses, maka akan muncul `main.exe` pada folder `bin`.
4. Anda dapat melihat cara penggunaan perangkat lunak dengan menjalankan perintah `./bin/main --help`

## Sumber Data Pergerakan

Terdapat 3 buah dapat pergerakan yang disediakan oleh berkas ini:

1. [BIWI ETH Walking Pedestrian](https://data.vision.ee.ethz.ch/cvl/aem/ewap_dataset_full.tgz)
2. Vittorio Emanuele II Gallery (VEIIG)
3. Crowds by Example (CBE)

Informasi selengkapnya mengenai data pergerakan dapat dilihat melalui [berkas berikut](./data/README.MD)

## Kontributor

1. Cristopher — sebagai mahasiswa
2. Lionov, Ph.D — sebagai pembimbing
