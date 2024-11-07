# Lunos e2 Fan Control

This project provides an API for controlling Lunos e2 fan pairs on a esp32 without further hardware than a little driver circuit.

## Circuit

![circuit](https://github.com/kallelix/Fan-Control/blob/main/circuit_for_one_fan_pair.png?raw=true)

Pin J1 3 and 2 are the input pwm signales from the esp32.

## Usage

### API Endpoints

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
    "message": "Successful response"
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
    "message": "Successful response"
  }
  ```

#### Clear Configuration Data
- **Endpoint:** `/clear`
- **Method:** `POST`
- **Response:**
  ```json
  {
    "message": "Configuration cleared successfully"
  }
  ```

## License

This project is licensed under the MIT License.
```
