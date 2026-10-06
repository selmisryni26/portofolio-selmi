/* ═══════════════════════════════════════════════════════
   Selmi Suryani — Portofolio · main.js
   ═══════════════════════════════════════════════════════ */
(function () {
  'use strict';

  var reduceMotion = window.matchMedia('(prefers-reduced-motion: reduce)').matches;

  /* ── Nav: sticky + progress ──────────────────────── */
  var nav = document.getElementById('nav');

  function onScroll() {
    var y = window.scrollY || document.documentElement.scrollTop;
    nav.classList.toggle('is-stuck', y > 12);
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

  /* ── Scroll-spy ──────────────────────────────────── */
  var sections = [].slice.call(document.querySelectorAll('section[id]'));
  var anchors = links ? [].slice.call(links.querySelectorAll('a[href^="#"]')) : [];

  function spy() {
    var pos = window.scrollY + window.innerHeight * 0.3;
    var current = sections.length ? sections[0].id : null;
    sections.forEach(function (s) { if (s.offsetTop <= pos) current = s.id; });

    if (sections.length &&
        window.innerHeight + window.scrollY >= document.documentElement.scrollHeight - 8) {
      current = sections[sections.length - 1].id;
    }
    anchors.forEach(function (a) {
      a.classList.toggle('is-on', a.getAttribute('href') === '#' + current);
    });
  }
  window.addEventListener('scroll', spy, { passive: true });
  window.addEventListener('resize', spy);
  spy();

  /* ── Reveal ──────────────────────────────────────── */
  var reveals = [].slice.call(document.querySelectorAll('.reveal'));

  if ('IntersectionObserver' in window && !reduceMotion) {
    var io = new IntersectionObserver(function (entries) {
      entries.forEach(function (en) {
        if (en.isIntersecting) {
          en.target.classList.add('is-in');
          io.unobserve(en.target);
        }
      });
    }, { threshold: 0.06, rootMargin: '0px 0px -50px 0px' });
    reveals.forEach(function (el) { io.observe(el); });
  } else {
    reveals.forEach(function (el) { el.classList.add('is-in'); });
  }

  /* ── Project filter ──────────────────────────────── */
  var bar = document.getElementById('bar');
  var projects = [].slice.call(document.querySelectorAll('.proj'));

  if (bar) {
    bar.addEventListener('click', function (e) {
      var btn = e.target.closest('.bar__btn');
      if (!btn) return;

      var f = btn.dataset.f;
      bar.querySelectorAll('.bar__btn').forEach(function (b) {
        b.classList.toggle('is-on', b === btn);
      });

      projects.forEach(function (p) {
        var show = (f === 'all') || (p.dataset.cat === f);
        p.classList.toggle('is-hidden', !show);
        if (show) p.classList.add('is-in');
      });
      spy();
    });
  }

  /* ── Lightbox ────────────────────────────────────── */
  var box = document.getElementById('lightbox');
  var boxImg = document.getElementById('lbImg');
  var boxCap = document.getElementById('lbCap');
  var idx = 0;

  function visibleShots() {
    return [].slice.call(document.querySelectorAll('.shot img')).filter(function (img) {
      var proj = img.closest('.proj');
      return !proj || !proj.classList.contains('is-hidden');
    });
  }

  function captionOf(img) {
    var fig = img.closest('figure');
    var cap = fig && fig.querySelector('figcaption');
    return cap ? cap.textContent : (img.alt || '');
  }

  function show(i) {
    var shots = visibleShots();
    if (!shots.length) return;
    idx = (i + shots.length) % shots.length;
    var img = shots[idx];
    boxImg.src = img.currentSrc || img.src;
    boxImg.alt = img.alt || '';
    boxCap.textContent = captionOf(img);
  }

  function openBox(img) {
    var shots = visibleShots();
    var at = shots.indexOf(img);
    if (at < 0) return;
    show(at);
    box.classList.add('is-open');
    box.setAttribute('aria-hidden', 'false');
    document.body.style.overflow = 'hidden';
  }

  function closeBox() {
    box.classList.remove('is-open');
    box.setAttribute('aria-hidden', 'true');
    document.body.style.overflow = '';
  }

  document.addEventListener('click', function (e) {
    var shot = e.target.closest('.shot');
    if (!shot) return;
    var img = shot.querySelector('img');
    if (img) openBox(img);
  });

  document.getElementById('lbClose').addEventListener('click', closeBox);
  document.getElementById('lbPrev').addEventListener('click', function (e) { e.stopPropagation(); show(idx - 1); });
  document.getElementById('lbNext').addEventListener('click', function (e) { e.stopPropagation(); show(idx + 1); });
  box.addEventListener('click', function (e) { if (e.target === box) closeBox(); });

  document.addEventListener('keydown', function (e) {
    if (!box.classList.contains('is-open')) return;
    if (e.key === 'Escape') closeBox();
    if (e.key === 'ArrowLeft') show(idx - 1);
    if (e.key === 'ArrowRight') show(idx + 1);
  });

  /* ── Year ────────────────────────────────────────── */
  var y = document.getElementById('year');
  if (y) y.textContent = String(new Date().getFullYear());
})();
