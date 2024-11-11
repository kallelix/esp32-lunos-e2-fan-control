# Lunos e2 Fan Control
This project provides an API to control Lunos e2 fan pairs on an esp32 without any hardware other than a small driver circuit. The fan is working like this one in this dataseet: https://www.farnell.com/datasheets/2634798.pdf

## Circuit
Circuit for one fan pair. Pin J1 3 and 2 are the input pwm signales from the esp32.
![circuit](https://github.com/kallelix/Fan-Control/blob/main/circuit_for_one_fan_pair.png?raw=true)

## Usage
### Setup
1. Flash Firmeware
2. WIFI setup with Wifi manager
3. Config pins with /config endpoint

### API Endpoints
API definition in api.yaml

#### Get Fan Data
- **Endpoint:** `/info`
- **Method:** `GET`
- **Response:**
  ```json
  {
    "fanpairs": [
      {
        "fanpair": "living_room",
        "power": 50,
        "cycletime": 30000
      }
    ]
  }
  ```

#### Set Fan Data
- **Endpoint:** `/set`
- **Method:** `POST`
- **Request Body:**
  ```json
  {
    "fanpair": "living_room",
    "scenario": "default | off | low | medium | high | highest | summer | out",
    "power": 100,
    "cycletime": 30000
  }
  ```
- **Response:**
  ```json
  {
  }
  ```

#### Set Configuration Data
- **Endpoint:** `/config`
- **Method:** `POST`
- **Request Body:**
  ```json
  {
    "hostname": "fan-control",
    "fanpairs": [
      {
        "name": "living_room",
        "fanpin1": 3,
        "fanpin2": 4,
        "defaultscenario": "default"
      }
    ]
  }
  ```
- **Response:**
  ```json
  {
  }
  ```

#### Clear Configuration Data
- **Endpoint:** `/clear`
- **Method:** `POST`
- **Response:**
  ```json
  {
  }
  ```

## License

This project is licensed under the MIT License.
