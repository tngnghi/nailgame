# Nail Salon Simulator 💅

A terminal-based management simulation game written in C++. Run your own nail salon: hire technicians, serve customers, track satisfaction, and keep the money flowing.

## Gameplay

Each in-game day lasts 600 minutes (10 hours). You control the salon turn by turn:

- **Spawn customers** — each arrives with a requested service, patience timer, and minimum skill/speed requirements
- **Assign jobs** — match technicians to customers based on skill fit; the game auto-picks the best match or asks you to confirm a partial one
- **Skip time** — advance minutes to complete ongoing services and let impatient customers leave
- **End the day** — collect earnings, see satisfaction scores, and possibly hire new staff

## Features

- 9 service types (Manicure, Pedicure, Gel, Acrylic, Dip, Hard Gel, Fill, Design, GelX) with different base durations
- Technicians have randomized **skill**, **service quality**, and **speed** stats — and improve with every job
- Customer **patience** degrades each minute; they walk out if kept waiting too long
- Design levels (1–3) add extra service time and complexity
- Money earned scales with service duration and type

## Build & Run

```bash
cmake -B build
cmake --build build
./build/NailGame
```

Requires C++20 and CMake 3.29+.

## Controls

| Input | Action |
|---|---|
| `1` | Add a random customer |
| `2` | Assign pending jobs |
| `3` | Skip N minutes |
| `4` | End the day |
| `5` | Refuse a waiting customer |
| `0` | Quit |
