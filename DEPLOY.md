# Cara Deploy — Menjadikan Portofolio Link Publik

Situs ini **statis** (HTML + CSS + JS, tanpa build step), jadi bisa di-host gratis.
Hasil akhir: alamat seperti `https://selmi-portofolio.vercel.app` yang bisa dibuka
siapa saja, bisa ditaruh di CV / LinkedIn.

**Sudah disiapkan** agar langsung bisa dipakai:

| File | Untuk |
|---|---|
| `vercel.json` | konfigurasi Vercel |
| `.vercelignore` | mengecualikan file catatan internal dari deploy |
| `netlify.toml` | konfigurasi Netlify (tanpa build, + header keamanan) |
| `_headers` | cadangan header untuk Netlify/Cloudflare Pages |
| `.gitignore` | mengecualikan folder CLI & arsip zip dari Git |

Situs sudah diperiksa: **52 referensi file cocok semua**, tidak ada nama file yang
beda huruf besar/kecil (penting karena server hosting pakai Linux yang
case-sensitive), dan tidak ada path lokal/`localhost`.

---

## Deploy ke Vercel

Dua cara. **Cara A tidak perlu install apa pun.**

### Cara A — Vercel Drop (drag & drop, ±1 menit)

1. Buka <https://vercel.com/drop> dan login (gratis, bisa pakai Google/GitHub).
2. Drag & drop file `portofolio-selmi.zip` (ada di
   `Documents\berkas kerja\portofolio-selmi.zip`) ke halaman itu.
   Bisa juga drag **folder** `Web Portofolio Selmi` langsung.
3. Pilih team, isi nama project (mis. `portofolio-selmi`), lalu **Deploy**.
4. Selesai → dapat link `https://nama-project.vercel.app`.

Catatan: Vercel otomatis membaca `index.html` di root sebagai halaman depan, jadi
tidak akan ditanya "pilih homepage". `journal.html` tetap bisa diakses di
`https://nama-project.vercel.app/journal.html`.

### Cara B — Vercel CLI (bisa update berulang dari terminal)

```bash
# 1. masuk ke folder situs (nama folder ada spasinya, jadi pakai tanda kutip)
cd "C:/Users/ASUS/Documents/berkas kerja/Web Portofolio Selmi"

# 2. login (sekali saja) — akan membuka browser
npx vercel login

# 3. deploy
npx vercel --prod
```

Saat pertama kali deploy, CLI akan bertanya. Jawaban yang benar untuk situs ini:

| Pertanyaan | Jawaban |
|---|---|
| Set up and deploy? | **Y** |
| Which scope? | pilih akunmu |
| Link to existing project? | **N** |
| What's your project's name? | `portofolio-selmi` (atau bebas) |
| In which directory is your code located? | **`./`** (langsung Enter) |
| Want to modify these settings? | **N** |

Perintah `vercel login` juga bisa diganti `npx vercel login`, dan setelah itu
`npx vercel --prod` bisa dijalankan berulang setiap kali isi situs diubah.

**Kalau tidak mau ditanya-tanya:** tambahkan `--yes` →

```bash
npx vercel --prod --yes
```

**Mengganti nama domain:** Vercel dashboard → project → *Settings → Domains* →
ubah `nama-project.vercel.app` menjadi yang diinginkan (mis. `selmi-suryani`).

> Deploy pertama otomatis jadi **production** walau tanpa `--prod`.

---

## Opsi lain

### Netlify Drop (juga paling cepat, tanpa install)

1. Zip isi folder situs — `index.html` harus di **dalam** zip, bukan di dalam
   satu folder lagi. Bisa pakai `portofolio-selmi.zip` yang sudah disiapkan.
2. Buka <https://app.netlify.com/drop>, drag & drop zip-nya.
3. Ganti nama link di *Site settings → Change site name*.

### Netlify CLI

```bash
cd "C:/Users/ASUS/Documents/berkas kerja/Web Portofolio Selmi"
npx netlify-cli deploy --prod --dir .
```

### GitHub Pages

```bash
cd "C:/Users/ASUS/Documents/berkas kerja/Web Portofolio Selmi"
git init -b main
git add .
git commit -m "Portofolio Selmi Suryani"
git remote add origin https://github.com/USERNAME/portofolio.git   # ganti USERNAME
git push -u origin main
```

Saat `git push`, GitHub akan minta login. **Jangan pakai password biasa** — GitHub
sudah tidak menerimanya. Gunakan **Git Credential Manager** (biasanya sudah
terpasang bersama Git for Windows; akan muncul jendela browser untuk login) atau
**Personal Access Token** dari <https://github.com/settings/tokens> dengan scope
**repo**.

Lalu di GitHub: **Settings → Pages → Source: Deploy from a branch →
Branch: `main` / `root` → Save.** Situs terbit di
`https://USERNAME.github.io/portofolio/` (butuh 1–2 menit).

### Cloudflare Pages

```bash
npx wrangler pages deploy .
```

---

## Setelah online — yang perlu dicek

1. Buka link-nya, pastikan **halaman depan tampil** dan **foto-foto muncul**.
2. Klik menu **Journal** → pastikan pindah ke `journal.html` (bukan halaman kosong).
3. Coba tombol **Download Resume** → PDF harus terbuka.
4. Buka dari **HP** untuk memastikan tampilan mobile rapi.

Kalau ada yang gagal, penyebab paling sering adalah **nama file beda huruf besar/kecil**
(Windows tidak peduli, Linux peduli). Sudah diperiksa dan saat ini tidak ada masalah —
tapi ingat hal ini setiap kali mengganti/menambah file baru.

---

## Update isi setelah online

Ubah file di folder ini, lalu:

- **Vercel CLI:** `npx vercel --prod`
- **Vercel Drop / Netlify Drop:** drag & drop ulang zip/folder-nya.
- **GitHub Pages:** `git add . && git commit -m "update" && git push`

## Setelah online — yang perlu dicek

1. Buka link-nya, pastikan **halaman depan tampil** dan **foto-foto muncul**.
2. Klik menu **Journal** → pastikan pindah ke `journal.html` (bukan halaman kosong).
3. Coba tombol **Download Resume** → PDF harus terbuka.
4. Buka dari **HP** untuk memastikan tampilan mobile rapi.

Kalau ada yang gagal, penyebab paling sering adalah **nama file beda huruf besar/kecil**
(Windows tidak peduli, Linux peduli). Sudah diperiksa dan saat ini tidak ada masalah —
tapi ingat hal ini setiap kali mengganti/menambah file baru.

---

## Update isi setelah online

Ubah file di folder ini, lalu:

- **Netlify Drop / dashboard:** drag & drop ulang zip/folder-nya.
- **Netlify CLI:** `npx netlify-cli deploy --prod --dir .`
- **GitHub Pages:** `git add . && git commit -m "update" && git push`
- **Vercel:** `npx vercel --prod`
