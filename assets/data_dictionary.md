# Data Dictionary

This document summarizes key data fields used by the project’s database and API. Each table lists the **data field**, its **purpose**, **measurement units**, and an **example** value.

---

## Database: `sensors`

| Data field | Purpose | Units | Example |
|---|---|---|---|
| `sensor_id` | Unique sensor identifier (primary key); referenced by `temperature_log.sensor_id`. | n/a | `0` |
| `mac_addr` | Device MAC address that publishes readings. | MAC address | `xx:xx:xx:xx:xx:xx` |
| `device_id` | Device label or firmware identifier. | n/a | `00` |
| `device_location` | Human‑readable location of the device. | n/a | `Mike C office` |

---

## Database: `temperature_log`

| Data field | Purpose | Units | Example |
|---|---|---|---|
| `log_id` | Unique log entry id (auto‑increment primary key). | n/a | `1` |
| `read_time` | Timestamp when the reading was captured (UTC recommended). | datetime (ISO‑like) | `2025-09-24 13:05:00` |
| `sensor_id` | Foreign key to `sensors.sensor_id`. | n/a | `0` |
| `temperature_f` | Temperature reading in Fahrenheit. | °F | `72.50` |
| `humidity` | Relative humidity. | % RH | `44.30` |
| `pressure` | Barometric pressure. | hPa | `996.30` |

---

## API JSON payload (create temperature reading) — `POST /api/v1/temperatures`

| Data field | Purpose | Units | Example |
|---|---|---|---|
| `sensor_id` | Sensor that produced the reading. **Required**. | n/a | `0` |
| `temperature_f` | Temperature in Fahrenheit. **Required**. | °F | `72.5` |
| `humidity` | Relative humidity. **Required**. | % RH | `44.3` |
| `pressure` | Barometric pressure. **Required**. | hPa | `996.3` |
| `read_time` | Reading time; if omitted, server uses current UTC. | datetime (ISO‑like) | `2025-09-24 13:05:00` |

---

## API JSON payload (create sensor) — `POST /api/v1/sensors`

| Data field | Purpose | Units | Example |
|---|---|---|---|
| `sensor_id` | Unique sensor identifier (no auto‑increment). **Required**. | n/a | `1` |
| `mac_addr` | Device MAC address. **Required**. | MAC address | `aa:bb:cc:dd:ee:ff` |
| `device_id` | Device label or firmware identifier. **Required**. | n/a | `01` |
| `device_location` | Human‑readable location. **Required**. | n/a | `Lab A` |

