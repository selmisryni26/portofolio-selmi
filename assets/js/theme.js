/* ═══════════════════════════════════════════════════════
   Selmi Suryani — Mode tema (terang / gelap)
   Dipakai oleh index.html dan journal.html.

   Cara kerja:
   - Tanpa pilihan tersimpan, tema mengikuti pengaturan sistem.
   - Saat tombol ditekan, pilihan disimpan di localStorage('theme')
     dan berlaku juga untuk kunjungan berikutnya.
   - Atribut data-theme pada <html> yang menentukan tampilan.
   ═══════════════════════════════════════════════════════ */
(function () {
  'use strict';

  var root = document.documentElement;
  var mq = window.matchMedia('(prefers-color-scheme: dark)');

  function saved() {
    try {
      var v = localStorage.getItem('theme');
      return (v === 'light' || v === 'dark') ? v : null;
    } catch (e) { return null; }
  }

  function current() {
    return saved() || (mq.matches ? 'dark' : 'light');
  }

  function paint(theme) {
    root.setAttribute('data-theme', theme);

    var btn = document.getElementById('themeBtn');
    if (!btn) return;
    var next = (theme === 'dark') ? 'light' : 'dark';
    btn.setAttribute('aria-label', 'Switch to ' + next + ' theme');
    btn.setAttribute('title', 'Switch to ' + next + ' theme');
    btn.setAttribute('aria-pressed', String(theme === 'dark'));
  }

  function init() {
    paint(current());

    var btn = document.getElementById('themeBtn');
    if (btn) {
      btn.addEventListener('click', function () {
        var next = (current() === 'dark') ? 'light' : 'dark';
        try { localStorage.setItem('theme', next); } catch (e) {}
        paint(next);
      });
    }

    // ikuti perubahan pengaturan sistem selama pengguna belum memilih sendiri
    var onSys = function () { if (!saved()) paint(current()); };
    if (mq.addEventListener) mq.addEventListener('change', onSys);
    else if (mq.addListener) mq.addListener(onSys);
  }

  if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', init);
  } else {
    init();
  }
})();
