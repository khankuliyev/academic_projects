const statusEl = document.getElementById("status");
const playerNameEl = document.getElementById("playerName");
const roomCodeEl = document.getElementById("roomCode");
const tournamentIdEl = document.getElementById("tournamentId");
const roomMetaEl = document.getElementById("roomMeta");
const roomPlayersEl = document.getElementById("roomPlayers");
const tournamentMetaEl = document.getElementById("tournamentMeta");
const tournamentPlayersEl = document.getElementById("tournamentPlayers");
const tableSeatsEl = document.getElementById("tableSeats");
const potAmountEl = document.getElementById("potAmount");
const timerRingEl = document.getElementById("timerRing");
const timerValueEl = document.getElementById("timerValue");
const raiseSliderEl = document.getElementById("raiseSlider");
const raiseValueEl = document.getElementById("raiseValue");

const wsUrl = `ws://${window.location.hostname}:8080`;
const ws = new WebSocket(wsUrl);

let sessionId = null;
let roomState = null;
let tournamentState = null;
let timerInterval = null;

function logStatus(data) {
  statusEl.textContent = JSON.stringify(data, null, 2);
}

function renderRoom() {
  if (!roomState) {
    roomMetaEl.textContent = "Комната не выбрана.";
    roomPlayersEl.innerHTML = "";
    tableSeatsEl.innerHTML = "";
    return;
  }
  const seedInfo = roomState.seedHash
    ? `seedHash: ${roomState.seedHash}\nseedReveal: ${roomState.seedReveal || "ожидание"}`
    : "seed: нет";
  roomMetaEl.textContent = `ID: ${roomState.roomId || "-"}\nКод: ${roomState.code || "-"}\nСостояние: ${
    roomState.state
  }\n${seedInfo}`;
  roomPlayersEl.innerHTML = "";
  (roomState.players || []).forEach((player) => {
    const card = document.createElement("div");
    card.className = "player-card";
    const header = document.createElement("div");
    header.className = "player-header";
    header.innerHTML = `<span>${player.name}</span><span class="status-pill">${player.status}</span>`;
    card.appendChild(header);
    const cardsRow = document.createElement("div");
    cardsRow.className = "cards-row";
    (player.cards || []).forEach((cardLabel) => {
      const chip = document.createElement("div");
      chip.className = "card";
      chip.textContent = cardLabel;
      cardsRow.appendChild(chip);
    });
    card.appendChild(cardsRow);
    roomPlayersEl.appendChild(card);
  });

  renderTableSeats(roomState.players || []);
}

function renderTableSeats(players) {
  tableSeatsEl.innerHTML = "";
  const total = Math.max(players.length, 2);
  const center = { x: 50, y: 50 };
  const radiusX = 42;
  const radiusY = 32;
  players.forEach((player, index) => {
    const angle = (Math.PI * 2 * index) / total - Math.PI / 2;
    const x = center.x + radiusX * Math.cos(angle);
    const y = center.y + radiusY * Math.sin(angle);
    const seat = document.createElement("div");
    seat.className = "seat";
    seat.style.left = `${x}%`;
    seat.style.top = `${y}%`;
    seat.innerHTML = `
      <div class="seat-card">
        <div class="seat-header">
          <span>${player.name}</span>
          <span class="status-pill">${player.status}</span>
        </div>
        <div class="seat-stack">Стек: ${player.stack || 2000}</div>
        <div class="seat-actions">${player.lastAction || "..."}</div>
        <div class="seat-cards">
          ${(player.cards || [])
            .map((cardLabel) => `<span class="seat-card-chip">${cardLabel}</span>`)
            .join("")}
        </div>
      </div>
    `;
    tableSeatsEl.appendChild(seat);
  });
}

function startTimer(seconds = 10) {
  let remaining = seconds;
  timerValueEl.textContent = `${remaining}s`;
  timerRingEl.style.background = `conic-gradient(#38bdf8 ${0}deg, #1e293b 0deg)`;
  if (timerInterval) {
    clearInterval(timerInterval);
  }
  timerInterval = setInterval(() => {
    remaining -= 1;
    const percent = Math.max(0, (seconds - remaining) / seconds);
    timerValueEl.textContent = `${Math.max(remaining, 0)}s`;
    timerRingEl.style.background = `conic-gradient(#38bdf8 ${percent * 360}deg, #1e293b 0deg)`;
    if (remaining <= 0) {
      clearInterval(timerInterval);
    }
  }, 1000);
}

function renderTournament() {
  if (!tournamentState) {
    tournamentMetaEl.textContent = "Турнир не выбран.";
    tournamentPlayersEl.innerHTML = "";
    return;
  }
  tournamentMetaEl.textContent = `ID: ${tournamentState.tournamentId}\nСостояние: ${
    tournamentState.state
  }\nПобедитель: ${tournamentState.winnerId || "-"}`;
  tournamentPlayersEl.innerHTML = "";
  (tournamentState.participants || []).forEach((player) => {
    const row = document.createElement("div");
    row.textContent = `${player.name} (${player.id})`;
    tournamentPlayersEl.appendChild(row);
  });
}

ws.addEventListener("open", () => {
  logStatus({ status: "connected", ws: wsUrl });
  startTimer();
});

ws.addEventListener("error", () => {
  logStatus({ status: "ws_error", message: "Не удалось подключиться к серверу.", ws: wsUrl });
});

ws.addEventListener("message", (event) => {
  const data = JSON.parse(event.data);
  if (data.sessionId) {
    sessionId = data.sessionId;
  }
  if (data.room) {
    roomState = data.room;
    renderRoom();
  }
  if (data.tournament) {
    tournamentState = data.tournament;
    renderTournament();
  }
  logStatus(data);
});

function send(type, payload = {}) {
  ws.send(JSON.stringify({ type, sessionId, name: playerNameEl.value, ...payload }));
}

raiseSliderEl.addEventListener("input", () => {
  raiseValueEl.textContent = raiseSliderEl.value;
});

document.getElementById("quickMatch").addEventListener("click", () => {
  send("quick_match");
});

document.getElementById("createPrivate").addEventListener("click", () => {
  send("create_private");
});

document.getElementById("startRoom").addEventListener("click", () => {
  if (!roomState?.roomId) {
    logStatus({ error: "Нет активной комнаты." });
    return;
  }
  send("start_room", { roomId: roomState.roomId });
});

document.getElementById("joinPrivate").addEventListener("click", () => {
  send("join_private", { code: roomCodeEl.value });
});

document.getElementById("createTournament").addEventListener("click", () => {
  send("create_tournament");
});

document.getElementById("joinTournament").addEventListener("click", () => {
  send("join_tournament", { tournamentId: tournamentIdEl.value });
});

document.getElementById("advanceTournament").addEventListener("click", () => {
  if (!tournamentState?.tournamentId) {
    logStatus({ error: "Нет активного турнира." });
    return;
  }
  send("advance_tournament", { tournamentId: tournamentState.tournamentId });
});

document.getElementById("loadDemo").addEventListener("click", () => {
  roomState = {
    roomId: "demo-room",
    code: "DEMO01",
    state: 1,
    seedHash: "demo_seed_hash",
    seedReveal: "demo_seed_reveal",
    players: [
      { id: "p1", name: "Вы", status: 0, cards: ["A♠", "10♦", "Joker"], stack: 1840, lastAction: "Raise 40" },
      { id: "p2", name: "Bot 1", status: 0, cards: ["Q♠", "Q♥", "Q♦"], stack: 1760, lastAction: "Call 40" },
      { id: "p3", name: "Bot 2", status: 0, cards: ["K♠", "10♠", "J♠"], stack: 1920, lastAction: "Fold" },
      { id: "p4", name: "Bot 3", status: 0, cards: ["10♣", "A♦", "K♥"], stack: 2000, lastAction: "..." }
    ]
  };
  tournamentState = {
    tournamentId: "demo-tournament",
    state: 1,
    winnerId: "",
    participants: [
      { id: "p1", name: "Вы", status: 0 },
      { id: "p2", name: "Bot 1", status: 0 },
      { id: "p3", name: "Bot 2", status: 0 }
    ]
  };
  potAmountEl.textContent = "120";
  renderRoom();
  renderTournament();
  startTimer();
  logStatus({ status: "demo_loaded" });
});
