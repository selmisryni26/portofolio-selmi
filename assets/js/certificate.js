/* ═══════════════════════════════════════════════════════
   Selmi Suryani — Certificate page · certificate.js
   Navigasi, menu mobile, animasi muncul, dan filter jenis.
   ═══════════════════════════════════════════════════════ */
(function () {
  'use strict';

  /* ── Nav: sticky ─────────────────────────────────── */
  var nav = document.getElementById('nav');
  function onScroll() {
    if (nav) nav.classList.toggle('is-stuck', (window.scrollY || 0) > 12);
  }
  window.addEventListener('scroll', onScroll, { passive: true });
  onScroll();

  /* ── Mobile menu ─────────────────────────────────── */
  var burger = document.getElementById('burger');
  var links = document.getElementById('navLinks');
  if (burger && links) {
    burger.addEventListener('click', function () {
      var open = links.classList.toggle('is-open');
      burger.setAttribute('aria-expanded', String(open));
    });
    links.addEventListener('click', function (e) {
      if (e.target.tagName === 'A') {
        links.classList.remove('is-open');
        burger.setAttribute('aria-expanded', 'false');
      }
    });
  }

  /* ── Reveal ──────────────────────────────────────── */
  var reveals = [].slice.call(document.querySelectorAll('.reveal'));
  var reduceMotion = window.matchMedia('(prefers-reduced-motion: reduce)').matches;

  if ('IntersectionObserver' in window && !reduceMotion) {
    var io = new IntersectionObserver(function (entries) {
      entries.forEach(function (en) {
        if (en.isIntersecting) {
          en.target.classList.add('is-in');
          io.unobserve(en.target);
        }
      });
    }, { threshold: 0.05, rootMargin: '0px 0px -40px 0px' });
    reveals.forEach(function (el) { io.observe(el); });
  } else {
    reveals.forEach(function (el) { el.classList.add('is-in'); });
  }

  /* ── Filter jenis sertifikat ─────────────────────── */
  var bar = document.getElementById('cbar');
  var items = [].slice.call(document.querySelectorAll('.cert'));

  if (bar) {
    bar.addEventListener('click', function (e) {
      var btn = e.target.closest('.bar__btn');
      if (!btn) return;

      var f = btn.dataset.f;
      bar.querySelectorAll('.bar__btn').forEach(function (b) {
        b.classList.toggle('is-on', b === btn);
        b.setAttribute('aria-pressed', String(b === btn));
      });

      items.forEach(function (c) {
        var show = (f === 'all') || (c.dataset.cat === f);
        c.classList.toggle('is-hidden', !show);
        if (show) c.classList.add('is-in');
      });

      // perbarui jumlah yang tampil
      var shown = items.filter(function (c) { return !c.classList.contains('is-hidden'); }).length;
      var cnt = document.querySelector('.js-count');
      if (cnt) cnt.textContent = String(shown);
    });
  }

  /* ── Tahun di footer ────────────────────────────── */
  var y = document.getElementById('year');
  if (y) y.textContent = String(new Date().getFullYear());
})();
