# SEKA Online (C++ backend + WebSocket)

Проект расширяет Секу до серверной логики с реальным временем и подготовкой к веб-клиенту.

## Архитектура
- `core/` — доменная логика: Card/Deck/Hand/Evaluator, SecureRng, shuffle и sha256.
- `server/` — WebSocket сервер, комнаты, матчмейкинг, турниры, state machines.
- `client/` — минимальный responsive веб-клиент.
- `tests/` — unit тесты и статистический тест shuffle.

## Выбранный вариант правил
- Колода: 21 карта — 10/J/Q/K/A (4 масти) + 1 Joker без масти.
- На руку: 3 карты.
- Джокер копирует лучшую карту по очкам в руке и усиливает комбинацию. Джокер не имеет масти и **не участвует** во флеше.
- Комбинации (по убыванию силы): три одинаковых, стрит-флеш, флеш, стрит, пара, старшая карта.

## Сборка
```bash
cmake -S . -B build
cmake --build build
```
> Сервер требует Boost.System. Если Boost недоступен, можно собрать только core/tests:
> `cmake -S . -B build -DSEKA_WITH_SERVER=OFF`.

## Запуск сервера
```bash
./build/seka_server 8080
```

## Запуск клиента
Откройте файл `client/index.html` в браузере (или через простой статический сервер).
Для визуальной проверки без сервера используйте кнопку “Демо-расклад” в клиенте.

## Тесты
```bash
./build/seka_tests
./build/seka_shuffle_tests
./build/seka_tournament_tests
```

## WebSocket протокол (JSON)
### Client -> Server
- `quick_match` — быстрый матчмейкинг
  ```json
  {"type":"quick_match","name":"Player"}
  ```
- `create_private` — создать приватную комнату
  ```json
  {"type":"create_private","name":"Player"}
  ```
- `join_private` — вход по коду
  ```json
  {"type":"join_private","code":"ABC123","name":"Player"}
  ```
- `start_room` — старт приватной комнаты
  ```json
  {"type":"start_room","roomId":"..."}
  ```
- `create_tournament`, `join_tournament`, `start_tournament`, `advance_tournament`

### Server -> Client
- `welcome` — сессия
  ```json
  {"type":"welcome","sessionId":"..."}
  ```
- `room_created`, `room_joined`, `room_started` — состояние комнаты
- `tournament_created`, `tournament_joined`, `tournament_round_started`, `tournament_finished`
- `error` — ошибки

## Честность RNG
- `core::SecureRng` использует системный источник энтропии (Linux `/dev/urandom`, macOS `arc4random_buf`, Windows `BCryptGenRandom`).
- `shuffle_deck` реализует Fisher–Yates.
- Сервер отправляет `seedHash` до раздачи и `seedReveal` после раздачи для минимальной проверки честности.

## Сценарии
1. Quick match: нажать “Быстрая игра” → ожидание 2..7 игроков → старт автоматически (или через 20 секунд при 2+).
2. Private: создать комнату → поделиться кодом → нажать “Start”.
3. Tournament: создать → участники входят → старт → раунды до победителя.
