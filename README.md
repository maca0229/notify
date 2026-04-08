# Claude Notifier

A haptic notification device for Claude Code — vibrates when Claude is waiting for your approval, stops when you respond.

## How It Works

Claude Code hooks (`PreToolUse` / `PostToolUse`) send HTTP requests to an ESP32 running a web server. The ESP32 drives a vibration motor via an NPN transistor, pulsing until you approve the action or a 10-second timeout kicks in.

```
Claude Code → HTTP /on  → ESP32 → vibration motor starts
             HTTP /off → ESP32 → vibration motor stops
```

## Hardware

| Component | Notes |
|-----------|-------|
| XIAO ESP32-C3 | Main controller |
| Coin vibration motor | 3V, driven via transistor |
| C1815 NPN transistor | E=left, C=middle, B=right (flat face toward you) |
| 1kΩ resistor | Between GPIO and base |

**Wiring:** 3.3V → motor → C (collector) · GPIO D10 → 1kΩ → B (base) · E (emitter) → GND

## Motor Behavior

- Pulse pattern: ON 300ms / OFF 200ms
- Auto-stop after 10 seconds if no `/off` received

## Claude Code Hook Config (`~/.claude/settings.json`)

```json
{
  "hooks": {
    "PreToolUse": [
      {
        "matcher": "",
        "hooks": [{"type": "command", "command": "curl -s http://<ESP32_IP>/on > /dev/null"}]
      }
    ],
    "PostToolUse": [
      {
        "matcher": "",
        "hooks": [{"type": "command", "command": "curl -s http://<ESP32_IP>/off > /dev/null"}]
      }
    ]
  }
}
```

Replace `<ESP32_IP>` with your device's IP (check Serial Monitor on first boot).

## API

| Endpoint | Action |
|----------|--------|
| `GET /on` | Start vibration |
| `GET /off` | Stop vibration |
| `GET /` | Health check |
