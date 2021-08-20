## Data Pergerakan
### Input

Setiap data pergerakan yang berhasil diperoleh akan disimpan dalam berkas `input` yang terdapat dalam berkas `data`.

Setiap lokasi yang tercatat akan disimpan dalam satu baris yang memiliki format berikut:

[_frame-time_ id x z y v_x v_z v_y]

### Output

Setiap data akan menghasilkan identifikasi rombongan dengan nama berkas yang sesuai dengan nama berkas _input_ dalam format `.txt`. Sebagai contoh, hasil identifikasi dari berkas `seq_eth` akan menghasilkan output `seq_eth-<unix time>.txt`.

Keluaran akan menyimpan 3 hasil yang masing-masing ditulis dalam format `csv` namun dipisahkan menggunakan satu baris kosong. Berikut merupakan hasil-hasil yang disimpan dari sebuah berkas _output_:

1. Parameter yang digunakan
2. Nilai evaluasi kuantitatif (_precision_, _recall_, _F1 score_)
3. Kumpulan rombongan yang teridentifikasi.

Setiap rombongan yang berhasil teridentifikasi akan ditulis dalam format berikut:

[anggota_rombongan waktu_mulai waktu_selesai]

`anggota_rombongan` adalah kumpulan bilangan bulat yang merupakan nomor identitas entitas bergerak yang dipisahkan menggunakan koma.

### Result

`result` merupakan _folder_ yang digunakan untuk menyimpan hasil identifikasi yang dihasilkan oleh manusia yang akan digunakan sebagai pembanding pada evaluasi kualitatif dari hasil identifikasi rombongan yang dilakukan oleh perangkat lunak.

Hasil identifikasi yang dijadikan pembanding menyimpan informasi rombongan yang berhasil teridentifikasi oleh manusia yang direpresentasikan dalam satu baris. Setiap anggota dari rombongan akan diwakilkan menggunakan nomor identitas yang dipisahkan menggunakan spasi.
