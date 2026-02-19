<?php
// receive_data.php

header('Content-Type: application/json');
require_once 'config.php';

// Obtener los datos enviados en el cuerpo de la solicitud
$data = json_decode(file_get_contents('php://input'), true);

// Extraer y limpiar los datos recibidos
$etiqueta_dispositivo = isset($data['etiqueta_dispositivo']) ? trim($data['etiqueta_dispositivo']) : '';
$ntu = isset($data['ntu']) ? (float)$data['ntu'] : null;
$ph = isset($data['ph']) ? (float)$data['ph'] : null;
$temperatura = isset($data['temperatura']) ? (float)$data['temperatura'] : null;

// Validar que se proporcionó la etiqueta del dispositivo
if (empty($etiqueta_dispositivo)) {
    echo json_encode(['status' => 'error', 'message' => 'Etiqueta del dispositivo es requerida.']);
    exit;
}

// Validar que el dispositivo exista en la base de datos
$stmt = $conn_dispositivos->prepare("
    SELECT id_dispositivo 
    FROM dispositivos 
    WHERE etiqueta_dispositivo = ?
");
$stmt->bind_param("s", $etiqueta_dispositivo);
$stmt->execute();
$stmt->store_result();

if ($stmt->num_rows === 0) {
    echo json_encode(['status' => 'error', 'message' => 'Dispositivo no encontrado.']);
    $stmt->close();
    $conn_dispositivos->close();
    exit;
}

$stmt->close();

// Actualizar los datos del dispositivo
$stmt = $conn_dispositivos->prepare("
    UPDATE dispositivos 
    SET ntu = ?, ph = ?, temperatura = ?, timestamp = CURRENT_TIMESTAMP 
    WHERE etiqueta_dispositivo = ?
");
$stmt->bind_param("ddds", $ntu, $ph, $temperatura, $etiqueta_dispositivo);

if ($stmt->execute()) {
    echo json_encode(['status' => 'success', 'message' => 'Datos actualizados correctamente.']);
} else {
    echo json_encode(['status' => 'error', 'message' => 'Error al actualizar los datos.']);
}

// Cerrar recursos
$stmt->close();
$conn_dispositivos->close();
?>
