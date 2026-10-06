/* ═══════════════════════════════════════════════════════
   Selmi Suryani — Journal page · journal.js
   Month tab switcher for journal.html
   ═══════════════════════════════════════════════════════ */
(function () {
  'use strict';

  /* ── Mobile menu (same behaviour as the portfolio) ── */
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

  /* ── Month tabs ──────────────────────────────────── */
  var jbar = document.getElementById('jbar');
  var jtabs = jbar ? [].slice.call(jbar.querySelectorAll('.jtab')) : [];
  var jpanels = [].slice.call(document.querySelectorAll('.jpanel'));

  function activate(m) {
    jtabs.forEach(function (b) {
      var on = (b.dataset.m === m);
      b.classList.toggle('is-on', on);
      b.setAttribute('aria-selected', String(on));
    });
    jpanels.forEach(function (p) {
      p.classList.toggle('is-on', p.dataset.m === m);
    });
  }

  if (jbar) {
    jbar.addEventListener('click', function (e) {
      var btn = e.target.closest('.jtab');
      if (!btn) return;
      activate(btn.dataset.m);
      // keep the chosen month in the URL so the view can be shared or bookmarked
      if (history.replaceState) {
        history.replaceState(null, '', '?month=' + btn.dataset.m);
      }
    });

    // open directly on ?month=03 if that month exists
    var q = new URLSearchParams(location.search).get('month');
    if (q && jtabs.some(function (b) { return b.dataset.m === q; })) activate(q);
  }

  /* ── Year ────────────────────────────────────────── */
  var y = document.getElementById('year');
  if (y) y.textContent = String(new Date().getFullYear());
})();
