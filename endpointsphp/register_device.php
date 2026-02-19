<?php
// register_device.php
header('Content-Type: application/json');
require_once 'config.php';

// Obtener datos del POST
$data = json_decode(file_get_contents('php://input'), true);
$etiqueta_dispositivo = isset($data['etiqueta_dispositivo']) ? trim($data['etiqueta_dispositivo']) : '';
$nombre_espacio = isset($data['nombre_espacio']) ? trim($data['nombre_espacio']) : '';
$tipo_espacio = isset($data['tipo_espacio']) ? trim($data['tipo_espacio']) : '';
$espacio_publico = isset($data['espacio_publico']) ? (bool)$data['espacio_publico'] : false;
$id_usuario = isset($data['id_usuario']) ? (int)$data['id_usuario'] : 0; // Asegúrate de recibir el ID del usuario autenticado

if (empty($etiqueta_dispositivo) || $id_usuario === 0) {
    echo json_encode(['status' => 'error', 'message' => 'Etiqueta del dispositivo y ID de usuario son requeridos.']);
    exit;
}

// Verificar que la etiqueta sea única
$stmt = $conn_dispositivos->prepare("SELECT id_dispositivo FROM dispositivos WHERE etiqueta_dispositivo = ?");
$stmt->bind_param("s", $etiqueta_dispositivo);
$stmt->execute();
$stmt->store_result();

if ($stmt->num_rows > 0) {
    echo json_encode(['status' => 'error', 'message' => 'La etiqueta del dispositivo ya está registrada.']);
    $stmt->close();
    $conn_dispositivos->close();
    exit;
}
$stmt->close();

// Insertar el nuevo dispositivo
$stmt = $conn_dispositivos->prepare("INSERT INTO dispositivos (etiqueta_dispositivo, nombre_espacio, tipo_espacio, espacio_publico) VALUES (?, ?, ?, ?)");
$stmt->bind_param("sssi", $etiqueta_dispositivo, $nombre_espacio, $tipo_espacio, $espacio_publico);

if ($stmt->execute()) {
    $id_dispositivo = $stmt->insert_id;
    $stmt->close();

    // Actualizar la lista de dispositivos del usuario
    $stmt = $conn_usuarios->prepare("SELECT dispositivos_asociados FROM usuarios WHERE id_usuario = ?");
    $stmt->bind_param("i", $id_usuario);
    $stmt->execute();
    $stmt->bind_result($dispositivos_asociados);
    $stmt->fetch();
    $stmt->close();

    $dispositivos_array = !empty($dispositivos_asociados) ? explode(',', $dispositivos_asociados) : [];
    $dispositivos_array[] = $id_dispositivo;
    $dispositivos_actualizados = implode(',', $dispositivos_array);

    $stmt = $conn_usuarios->prepare("UPDATE usuarios SET dispositivos_asociados = ? WHERE id_usuario = ?");
    $stmt->bind_param("si", $dispositivos_actualizados, $id_usuario);

    if ($stmt->execute()) {
        echo json_encode(['status' => 'success', 'message' => 'Dispositivo registrado y asociado al usuario.']);
    } else {
        echo json_encode(['status' => 'error', 'message' => 'Dispositivo registrado pero no se pudo asociar al usuario.']);
    }

    $stmt->close();
} else {
    echo json_encode(['status' => 'error', 'message' => 'Error al registrar el dispositivo.']);
}

$conn_dispositivos->close();
$conn_usuarios->close();
?>
