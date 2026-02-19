<?php
// validate_device.php
header('Content-Type: application/json');
require_once 'config.php';

// Obtener datos del POST
$data = json_decode(file_get_contents('php://input'), true);
$etiqueta_dispositivo = isset($data['etiqueta_dispositivo']) ? trim($data['etiqueta_dispositivo']) : '';

if (empty($etiqueta_dispositivo)) {
    echo json_encode(['status' => 'error', 'message' => 'Etiqueta del dispositivo es requerida.']);
    exit;
}

// Verificar si la etiqueta existe
$stmt = $conn_dispositivos->prepare("SELECT id_dispositivo FROM dispositivos WHERE etiqueta_dispositivo = ?");
$stmt->bind_param("s", $etiqueta_dispositivo);
$stmt->execute();
$stmt->store_result();

if ($stmt->num_rows > 0) {
    // Si la etiqueta está registrada (respuesta esperada para el ESP32)
    echo json_encode(['status' => 'success', 'message' => 'Etiqueta válida.']);
} else {
    // Si la etiqueta no está registrada
    echo json_encode(['status' => 'error', 'message' => 'Etiqueta no registrada.']);
}

$stmt->close();
$conn_dispositivos->close();
?>
