# Lunos e2 Fan Control

This project provides an API for controlling Lunos e2 fan pairs on a esp32.

## Installation

1. Clone the repository:
   ```sh
   git clone https://github.com/kallelix/fan-control.git
   ```
2. Navigate to the project directory:
   ```sh
   cd lunos-e2-fan-control
   ```
3. Install the required dependencies:
   ```sh
   # Assuming you are using PlatformIO
   pio run
   ```

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