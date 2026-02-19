<?php
// Definir una función para escribir logs
function writeLog($message) {
    $logFile = 'debug_log.txt';
    $currentTime = date('Y-m-d H:i:s');
    file_put_contents($logFile, "[$currentTime] $message\n", FILE_APPEND);
}

// Permitir solicitudes desde cualquier origen (CORS)
header("Access-Control-Allow-Origin: *");
header("Content-Type: application/json; charset=UTF-8");

// Obtener el cuerpo de la solicitud
$json = file_get_contents('php://input');
writeLog("Datos recibidos: " . $json);

// Decodificar el JSON recibido
$data = json_decode($json, true);

// Verificar si la decodificación fue exitosa
if ($data === null) {
    http_response_code(400);
    $errorMsg = json_encode(["status" => "error", "message" => "JSON inválido"]);
    echo $errorMsg;
    writeLog("Error: JSON inválido");
    exit();
}

// Verificar si el campo 'datos' existe y es un array u objeto
if (!isset($data['datos']) || !is_array($data['datos'])) {
    http_response_code(400);
    $errorMsg = json_encode(["status" => "error", "message" => "Formato de datos incorrecto"]);
    echo $errorMsg;
    writeLog("Error: Formato de datos incorrecto");
    exit();
}

// Extraer los valores de 'ntu' y 'ph'
$registro = $data['datos'];

if (!isset($registro['ntu'], $registro['ph'])) {
    http_response_code(400);
    $errorMsg = json_encode(["status" => "error", "message" => "'ntu' o 'ph' no están definidos"]);
    echo $errorMsg;
    writeLog("Error: 'ntu' o 'ph' no están definidos en los datos recibidos");
    exit();
}

// Ruta al archivo donde se almacenarán los datos
$archivo = 'datos_matlab.csv';

// Verificar si el archivo existe, si no, crear con encabezados
if (!file_exists($archivo)) {
    $handle = fopen($archivo, 'w');
    if ($handle === false) {
        http_response_code(500);
        $errorMsg = json_encode(["status" => "error", "message" => "No se pudo crear el archivo"]);
        echo $errorMsg;
        writeLog("Error: No se pudo crear el archivo CSV");
        exit();
    }
    // Escribir encabezados
    fputcsv($handle, ['Timestamp', 'NTU', 'pH']);
    fclose($handle);
    writeLog("Archivo CSV creado con encabezados");
}

// Abrir el archivo en modo append
$handle = fopen($archivo, 'a');

if ($handle === false) {
    http_response_code(500);
    $errorMsg = json_encode(["status" => "error", "message" => "No se pudo abrir el archivo"]);
    echo $errorMsg;
    writeLog("Error: No se pudo abrir el archivo CSV para escritura");
    exit();
}

// Escribir el registro en el archivo CSV
fputcsv($handle, [
    date('Y-m-d H:i:s'), // Timestamp
    $registro['ntu'],
    $registro['ph']
]);
writeLog("Registro escrito en CSV: NTU={$registro['ntu']}, pH={$registro['ph']}");

fclose($handle);

// Responder con éxito
echo json_encode(["status" => "success", "message" => "Datos recibidos y almacenados"]);
writeLog("Datos almacenados exitosamente en CSV");
?>
