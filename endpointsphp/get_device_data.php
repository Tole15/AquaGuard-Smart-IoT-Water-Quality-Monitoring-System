<?php
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);
// get_device_data.php
header('Content-Type: application/json');
require_once 'config.php';

// Obtener datos del POST
$data = json_decode(file_get_contents('php://input'), true);
$etiqueta_dispositivo = isset($data['etiqueta_dispositivo']) ? trim($data['etiqueta_dispositivo']) : '';

if (empty($etiqueta_dispositivo)) {
    echo json_encode(['status' => 'error', 'message' => 'Etiqueta del dispositivo es requerida.']);
    exit;
}

// Consultar datos del dispositivo
$stmt = $conn_dispositivos->prepare("
    SELECT ntu, ph, temperatura, timestamp 
    FROM dispositivos 
    WHERE etiqueta_dispositivo = ?
");
$stmt->bind_param("s", $etiqueta_dispositivo);
$stmt->execute();
$result = $stmt->get_result();

if ($result->num_rows === 0) {
    echo json_encode(['status' => 'error', 'message' => 'Dispositivo no encontrado.']);
    $stmt->close();
    $conn_dispositivos->close();
    exit;
}

$data_device = $result->fetch_assoc();

echo json_encode([
    'status' => 'success',
    'data' => [
        'ntu' => $data_device['ntu'],
        'ph' => $data_device['ph'],
        'temperatura' => $data_device['temperatura'],
        'timestamp' => $data_device['timestamp']
    ]
]);

$stmt->close();
$conn_dispositivos->close();
?>
