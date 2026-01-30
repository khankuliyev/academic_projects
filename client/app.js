const statusEl = document.getElementById("status");
const playerNameEl = document.getElementById("playerName");
const roomCodeEl = document.getElementById("roomCode");
const tournamentIdEl = document.getElementById("tournamentId");
const roomMetaEl = document.getElementById("roomMeta");
const roomPlayersEl = document.getElementById("roomPlayers");
const tournamentMetaEl = document.getElementById("tournamentMeta");
const tournamentPlayersEl = document.getElementById("tournamentPlayers");

const wsUrl = `ws://${window.location.hostname}:8080`;
const ws = new WebSocket(wsUrl);

let sessionId = null;
let roomState = null;
let tournamentState = null;

function logStatus(data) {
  statusEl.textContent = JSON.stringify(data, null, 2);
}

function renderRoom() {
  if (!roomState) {
    roomMetaEl.textContent = "Комната не выбрана.";
    roomPlayersEl.innerHTML = "";
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
      { id: "p1", name: "Вы", status: 0, cards: ["A♠", "10♦", "Joker"] },
      { id: "p2", name: "Bot 1", status: 0, cards: ["Q♠", "Q♥", "Q♦"] },
      { id: "p3", name: "Bot 2", status: 0, cards: ["K♠", "10♠", "J♠"] }
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
  renderRoom();
  renderTournament();
  logStatus({ status: "demo_loaded" });
});
