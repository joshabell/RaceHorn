// Oil Rig Cookies™ — parody prototype interactions.
// Nothing here sends, stores, or transmits any data. It's a prop form.

(function () {
  "use strict";

  const form = document.getElementById("orderForm");
  const note = document.getElementById("formNote");

  if (!form || !note) return;

  const cheers = [
    "🛢️ Barrel reserved! (Not really — this is a joke.)",
    "🦅 Freedom secured. No cookies were actually drilled.",
    "🍪 Pre-order logged to /dev/null. Stay patriotic.",
    "⛏️ Rig fired up! Reminder: please do not eat petroleum.",
  ];

  form.addEventListener("submit", function (e) {
    e.preventDefault();

    const name = (document.getElementById("name").value || "").trim();
    const barrels = parseInt(document.getElementById("barrels").value, 10) || 1;

    if (!name) {
      note.textContent = "⚠️ Every barrel needs a patriot name.";
      return;
    }

    const cheer = cheers[Math.floor(Math.random() * cheers.length)];
    note.textContent =
      `${cheer} ${name}, your ${barrels} ` +
      (barrels === 1 ? "barrel" : "barrels") +
      " of peanut butter Oil Rig Cookies would be on the way — if they existed.";

    form.reset();
    document.getElementById("barrels").value = barrels;
  });
})();
