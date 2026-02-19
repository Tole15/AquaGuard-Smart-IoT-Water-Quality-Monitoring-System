#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// --------------------- Configuración de WiFi ---------------------
const char* ssid = "INFINITUM8SH5_2.4";
const char* password = "W7nZn9p3SD";

// URLs del servidor
const char* validateUrl = "http://192.168.1.84/aquaguard_api/validate_device.php";
const char* sendDataUrl = "http://192.168.1.84/aquaguard_api/receive_data.php";
const char* urlMatlab = "http://192.168.1.84/aquaguard_api/matlab_receive_data.php"; // Actualiza esta línea si es necesario


// Etiqueta única del dispositivo
const char* etiquetaDispositivo = "AQUAGUARD001";

// Configuración del puerto UART para comunicación con ATmega328P
HardwareSerial SerialATmega(2); // UART2 (GPIO16=RX2, GPIO17=TX2)

// --------------------- Macros de Logging ---------------------
#define LOG_INFO(msg) Serial.println("[INFO] " + String(msg))
#define LOG_ERROR(msg) Serial.println("[ERROR] " + String(msg))

// --------------------- Funciones Prototipo ---------------------
void reconnectWiFi();
bool validateDevice();
bool parseData(String data, float &ntu, float &ph, float &temperatura);
bool sendReading(float ntu, float ph, float temperatura);
bool sendDatosMatlab(float ntu, float ph); // Actualizar la declaración

// --------------------- Setup ---------------------
void setup() {
    Serial.begin(115200);
    delay(1000);
    LOG_INFO("Iniciando ESP32...");

    // Configurar UART2 para comunicación con ATmega328P
    SerialATmega.begin(9600, SERIAL_8N1, 16, 17); // Baud rate 9600, RX=GPIO16, TX=GPIO17
    SerialATmega.setTimeout(2000); // Timeout de 2 segundos

    LOG_INFO("Configurando conexión WiFi...");
    reconnectWiFi();

    // Validar el dispositivo al iniciar
    LOG_INFO("Validando etiqueta del dispositivo...");
    if (!validateDevice()) {
        LOG_ERROR("Etiqueta no registrada después de múltiples intentos. No se enviarán datos.");
    }
}

// --------------------- Loop ---------------------
void loop() {
    // Verificar si la conexión WiFi está activa
    if (WiFi.status() != WL_CONNECTED) {
        LOG_ERROR("Conexión WiFi perdida. Intentando reconectar...");
        reconnectWiFi();
    }

    // Leer datos desde UART si están disponibles
    if (SerialATmega.available()) {
        String data = SerialATmega.readStringUntil('\n'); // Leer hasta el carácter de nueva línea
        data.trim(); // Eliminar espacios en blanco al inicio y final
        LOG_INFO("Datos recibidos: '" + data + "'");

        float ntu = 0.0, ph = 0.0, temperatura = 0.0;
        if (parseData(data, ntu, ph, temperatura)) {
            LOG_INFO("Datos parseados correctamente. Enviando al servidor...");

            // Enviar datos a la base de datos
            if (sendReading(ntu, ph, temperatura)) {
                LOG_INFO("Datos enviados a la base de datos exitosamente.");
            } else {
                LOG_ERROR("Error al enviar datos a la base de datos.");
            }

            // Enviar datos individuales a MATLAB
            if (sendDatosMatlab(ntu, ph)) {
                LOG_INFO("Datos enviados a MATLAB exitosamente.");
            } else {
                LOG_ERROR("Error al enviar datos a MATLAB.");
            }
        } else {
            LOG_ERROR("Error al parsear los datos.");
        }
    }

    delay(100); // Pausa antes de la siguiente iteración
}

// --------------------- Funciones Auxiliares ---------------------
void reconnectWiFi() {
    while (WiFi.status() != WL_CONNECTED) {
        LOG_INFO("Intentando reconectar a WiFi...");
        WiFi.disconnect();
        WiFi.begin(ssid, password);
        delay(3000); // Esperar 3 segundos antes de reintentar
    }
    LOG_INFO("Reconectado a la red WiFi");
}

bool validateDevice() {
    int attempts = 0;
    while (attempts < 3) {
        if (WiFi.status() != WL_CONNECTED) {
            reconnectWiFi();
        }

        HTTPClient http;
        http.begin(validateUrl);
        http.addHeader("Content-Type", "application/json");

        // Crear un documento JSON con la etiqueta del dispositivo
        DynamicJsonDocument doc(128);
        doc["etiqueta_dispositivo"] = etiquetaDispositivo;

        String jsonData;
        serializeJson(doc, jsonData);
        LOG_INFO("Intento #" + String(attempts + 1) + " - Validando etiqueta: " + jsonData);

        int httpResponseCode = http.POST(jsonData);
        if (httpResponseCode > 0) {
            String response = http.getString();
            LOG_INFO("Respuesta del servidor: " + response);

            DynamicJsonDocument responseDoc(128);
            DeserializationError error = deserializeJson(responseDoc, response);
            if (!error) {
                String status = responseDoc["status"];
                if (status == "success") {
                    http.end();
                    return true;
                } else {
                    String message = responseDoc["message"] | "Etiqueta no registrada.";
                    LOG_ERROR("Error del servidor: " + message);
                }
            } else {
                LOG_ERROR("Error al parsear la respuesta del servidor.");
            }
        } else {
            LOG_ERROR("Error al consultar el servidor: Código " + String(httpResponseCode));
        }

        http.end();
        attempts++;
        delay(2000); // Esperar antes de intentar nuevamente
    }
    return false; // Etiqueta no validada
}

bool parseData(String data, float &ntu, float &ph, float &temperatura) {
    int indexNTU = data.indexOf("NTU:");
    int indexPH = data.indexOf("pH:");
    int indexTemp = data.indexOf("Temp:");

    if (indexNTU == -1 || indexPH == -1 || indexTemp == -1) {
        return false;
    }

    String ntuStr = data.substring(indexNTU + 4, data.indexOf(',', indexNTU));
    String phStr = data.substring(indexPH + 3, data.indexOf(',', indexPH));
    String tempStr = data.substring(indexTemp + 5);
    tempStr.replace("C", "");
    tempStr.trim();

    if (!isFloat(ntuStr) || !isFloat(phStr) || !isFloat(tempStr)) {
        return false;
    }

    ntu = ntuStr.toFloat();
    ph = phStr.toFloat();
    temperatura = tempStr.toFloat();
    return true;
}

bool isFloat(String str) {
    float val = str.toFloat();
    return !isnan(val) && !isinf(val);
}

bool sendReading(float ntu, float ph, float temperatura) {
    int attempts = 0;
    while (attempts < 3) {
        if (WiFi.status() != WL_CONNECTED) {
            reconnectWiFi();
        }

        HTTPClient http;
        http.begin(sendDataUrl);
        http.addHeader("Content-Type", "application/json");

        DynamicJsonDocument doc(256);
        doc["etiqueta_dispositivo"] = etiquetaDispositivo;
        doc["ntu"] = ntu;
        doc["ph"] = ph;
        doc["temperatura"] = temperatura;

        String jsonData;
        serializeJson(doc, jsonData);
        LOG_INFO("Intento #" + String(attempts + 1) + " - Enviando datos a la base de datos: " + jsonData);

        int httpResponseCode = http.POST(jsonData);
        if (httpResponseCode > 0) {
            String response = http.getString();
            LOG_INFO("Respuesta del servidor: " + response);
            http.end();
            // Verificar si la respuesta indica éxito
            DynamicJsonDocument responseDoc(128);
            DeserializationError error = deserializeJson(responseDoc, response);
            if (!error && String(responseDoc["status"]) == "success") {
                return true;
            } else {
                LOG_ERROR("Error en la respuesta del servidor: " + String(responseDoc["message"]));
            }
        } else {
            LOG_ERROR("Error al enviar datos a la base de datos: Código " + String(httpResponseCode));
        }

        http.end();
        attempts++;
        delay(2000); // Esperar antes de intentar nuevamente
    }
    return false;
}

bool sendDatosMatlab(float ntu, float ph) {
    if (WiFi.status() != WL_CONNECTED) {
        LOG_ERROR("Conexión WiFi perdida. Intentando reconectar...");
        reconnectWiFi();
    }

    HTTPClient http;
    http.begin(urlMatlab);
    http.addHeader("Content-Type", "application/json");

    // Crear un documento JSON con los datos individuales
    DynamicJsonDocument doc(256);
    JsonObject datos = doc.createNestedObject("datos");
    datos["ntu"] = ntu;
    datos["ph"] = ph;

    String jsonData;
    if (serializeJson(doc, jsonData) == 0) {
        LOG_ERROR("Error al serializar el JSON para MATLAB.");
        http.end();
        return false;
    }
    LOG_INFO("Enviando datos a MATLAB: " + jsonData);

    int httpResponseCode = http.POST(jsonData);
    if (httpResponseCode > 0) {
        String response = http.getString();
        LOG_INFO("Respuesta de MATLAB: " + response);
        http.end();
        // Verificar si la respuesta indica éxito
        DynamicJsonDocument responseDoc(128);
        DeserializationError error = deserializeJson(responseDoc, response);
        if (!error && String(responseDoc["status"]) == "success") {
            return true;
        } else {
            LOG_ERROR("Error en la respuesta de MATLAB: " + String(responseDoc["message"]));
        }
    } else {
        LOG_ERROR("Error al enviar datos a MATLAB: Código " + String(httpResponseCode));
        LOG_ERROR("Mensaje de error: " + http.errorToString(httpResponseCode));
    }

    http.end();
    return false;
}
