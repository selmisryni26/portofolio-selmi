# Web Portofolio — Selmi Suryani

Situs portofolio statis (HTML + CSS + JS, tanpa build step).
Seluruh isi website berbahasa Inggris (`<html lang="en">`).

## 🌐 Link publik

**https://selmisryni26.github.io/portofolio-selmi/**

Diterbitkan lewat GitHub Pages dari repo:
https://github.com/selmisryni26/portofolio-selmi (branch `main`, folder root `/`)

Setiap kali ada perubahan, cukup commit lalu push — GitHub Pages membangun
ulang otomatis dalam 1–2 menit:

```bash
cd "Web Portofolio Selmi"
git add -A
git commit -m "perbarui isi portofolio"
git push
```

## Deploy ke internet

Lihat **`DEPLOY.md`** — panduan lengkap menjadikan situs link publik gratis
(Netlify Drop, Netlify CLI, GitHub Pages, Vercel, Cloudflare Pages).

## Menjalankan

Buka `index.html` langsung di browser, atau jalankan server lokal:

```bash
cd "Web Portofolio Selmi"
python -m http.server 8742
# buka http://localhost:8742
```

## Struktur

```
index.html                 # halaman portofolio (one-page)
journal.html               # halaman Journal (terpisah)
preview-fullpage.jpg       # pratinjau seluruh halaman
assets/
  css/style.css            # design system + layout
  js/main.js               # nav, scroll-spy, reveal, filter proyek, lightbox
  js/journal.js            # tab bulan di halaman Journal
  Selmi-Suryani-Resume.pdf # tombol "Download Resume"
  docs/                    # 3 laporan lengkap (PDF)
  img/projects/            # 24 gambar proyek + 12 halaman laporan (capstone/kp/ta)
  img/photo/
    portrait.jpg           # foto hero + kartu Tentang (4:5, dari foto outdoor)
    avatar.jpg             # avatar bulat kecil di hero (1:1, dari foto formal)
    foto-formal.jpg        # sumber asli (cadangan)
    hero.jpg               # sumber asli (cadangan)
```

## Tema

Cream hangat + pastel lembut + aksen terracotta. Kartu membulat, judul serif besar
(Fraunces), sisa teks sans-serif. Gaya diambil dari referensi desain, **tanpa**
elemen yang membuat ramai: tanpa testimoni, tanpa coretan/dekorasi tulisan tangan,
tanpa kartu layanan berjumlah banyak.

| Token | Nilai |
|---|---|
| Background | `#FAF6EE` (cream) |
| Section selang-seling | `#F4EDE1` |
| Kartu | `#FFFFFF` |
| Terracotta (aksen) | `#C0693C` |
| Pastel peach | `#FCEBDD` |
| Pastel sage | `#E8EFE2` |
| Pastel blue | `#E4EDF6` |
| Teks utama | `#201B16` |
| Teks sekunder | `#5C544A` |
| Judul | Fraunces (serif) |
| Teks | Plus Jakarta Sans |

## Bagian halaman

| No | Bagian | Isi |
|---|---|---|
| — | Home | Nama besar biru, tag "siap bekerja", ringkasan, tombol, 4 statistik (IPK 3.79, 3× juara, 2 magang, 6 proyek) |
| 01 | Tentang Saya | Profil dengan dropcap + panel "Fokus Karier" (bar progres) dan kutipan |
| 02 | Pendidikan & Penghargaan | UTY (S1 Teknik Komputer, 3.79/4.00) · SMK N 1 Wanareja (Multimedia, 89.64) + 3 kartu prestasi |
| 03 | Pengalaman | Baris tabel: Tim Akreditasi D3 SI, Dinas PUPR Kebumen, HIMTEK, Kunjungan Industri, IMI, Cokro Digital Printing |
| 04 | Skill | Soft skills (kartu titik) + hard skills (Administrasi, Editing, UI/UX, Pemrograman) |
| 05 | Project Portfolio | 9 projects, 35 images, category filter, lightbox |
| 07 | Contact | Panel biru full-bleed: email, telepon, lokasi, tombol kirim email & unduh resume |

## Filter proyek

Tombol filter menyaring proyek berdasarkan `data-cat`:

| Filter | Proyek |
|---|---|
| Desain Grafis | 01 Poster Infografis · 02 Stiker/ID Card/Sertifikat/X-Banner |
| Media Sosial | 03 Feed Instagram |
| Fotografi | 04 Fotografi |
| UI/UX | 05 UI Design |
| Administrasi | 06 Akreditasi |
| Capstone | 07 Capstone Project — Aquarium Monitoring & Treatment (deskripsi saja, tanpa foto/laporan) |
| IoT | 08 Kerja Praktik — SHMS Jembatan · 09 Tugas Akhir — Sistem Monitoring & Kendali Akuarium |

Lightbox otomatis mengabaikan proyek yang sedang tersembunyi.

## Proyek laporan (07–09)

Ketiga proyek laporan kini **hanya berupa judul + deskripsi** di halaman web —
tanpa galeri gambar dan tanpa tautan PDF.

| Proyek | Isi |
|---|---|
| 07 Capstone Project — Aquarium Monitoring & Treatment | judul formal, deskripsi, + 4 foto prototipe |
| 08 Kerja Praktik — SHMS Jembatan Berbasis IoT | judul formal + deskripsi |
| 09 Tugas Akhir — Sistem Monitoring & Kendali Akuarium | judul formal + deskripsi |

Proyek capstone (4 foto `capstone-photo-1..4.jpg`), internship (2 foto
`internship-photo-1..2.jpg`), dan final project (5 foto `final-photo-1..5.jpg`)
semuanya memakai kelas `.gal--strip`: satu baris thumbnail dengan **lebar tetap
169px**, jadi ukurannya seragam di semua proyek berapa pun jumlah fotonya.
Tanda panah ↗ di sudut kanan bawah tiap thumbnail membuka foto ukuran penuh
di lightbox. Tanda panah ↗ di sudut kanan bawah tiap thumbnail
membuka foto berukuran penuh di lightbox.

File PDF (`assets/docs/`) dan pratinjau halaman laporan (`capstone-*.jpg`, `kp-*.jpg`,
`ta-*.jpg`) masih tersimpan di folder, tapi tidak lagi dirujuk oleh HTML.

## Catatan

- Isi setiap laporan hanya dimuat sebagian (dari Bab I/II sampai Bab IV) sesuai file
  yang tersedia — bukan dokumen penuh dari sampul sampai daftar pustaka.

## Journal (halaman terpisah)

Menu **Journal** di navigasi atas mengarah ke `journal.html` — halaman sendiri,
bukan bagian dari portofolio. Portofolio tetap one-page tanpa section journal.

Isi halaman Journal: 6 tab bulan (September–February), tiap bulan punya panel sendiri dan
menampilkan keadaan kosong sampai diisi. Bulan yang dipilih disimpan di URL
(`journal.html?month=11`) sehingga bisa di-bookmark atau dibagikan.
Tautan "Back to portfolio" di atas untuk kembali ke `index.html`.

Cara menambah entri: ganti isi `.jempty` pada panel bulan yang dituju di
`journal.html`. Entri memakai struktur `.jentry` (tanggal, judul, tag, isi, dan
kotak ringkasan):

```html
<div class="jpanel" data-m="11" role="tabpanel">
  <article class="jentry">
    <header class="jentry__head">
      <span class="jentry__date">15 Nov 2026</span>
      <h2 class="jentry__title">Judul entri</h2>
      <ul class="jentry__tags"><li>Tag</li></ul>
    </header>
    <div class="jentry__body">
      <p>Isi catatan ...</p>
      <p class="jentry__take"><b>What I took from it:</b> ringkasan pelajaran.</p>
    </div>
  </article>
</div>
```

September (`data-m="09"`) sudah terisi entri magang AirNav Indonesia
(21 Sep – 1 Oct 2026); bulan lain masih menampilkan keadaan kosong.

`data-m` pada tombol `.jtab` dan panel `.jpanel` harus sama — dipakai `09`, `10`,
`11`, `12` (September–December) dan `01`, `02` (January–February).

## Ganti isi

- Foto proyek: ganti file di `assets/img/projects/` dengan nama yang sama.
- Foto diri: ganti `assets/img/photo/portrait.jpg` (rasio 4:5) dan `avatar.jpg` (1:1).
- Data diri: cari teksnya di `index.html`.
- Warna: ubah variabel di `:root` pada `assets/css/style.css`.


---

## Mode Terang / Gelap

Situs punya dua tema yang bisa diganti sendiri oleh pengunjung lewat tombol
di kanan atas (ikon matahari / bulan).

- **Tanpa pilihan tersimpan** → tema mengikuti pengaturan sistem pengunjung
  (`prefers-color-scheme`).
- **Setelah tombol ditekan** → pilihan disimpan di `localStorage('theme')`
  dan dipakai lagi pada kunjungan berikutnya.
- Pilihan berlaku di **kedua halaman** (portofolio & jurnal).

Berkas terkait:

| Berkas | Isi |
|---|---|
| `assets/js/theme.js` | logika ganti tema + simpan pilihan |
| `assets/css/style.css` | blok `:root` (terang) dan `[data-theme="dark"]` (gelap) |
| `index.html`, `journal.html` | tombol `#themeBtn` + skrip kecil di `<head>` agar tidak berkedip |

Menambah warna baru: pakai variabel yang sudah ada
(`--cream`, `--ink`, `--terra`, `--white`, `--line`, …), jangan tulis warna
tetap — kalau tidak, warna itu tidak ikut berubah saat tema diganti.
