<?php
// register_user.php
header('Content-Type: application/json');
require_once 'config.php';

// Función para enviar respuestas JSON
function jsonResponse($status, $message) {
    echo json_encode(['status' => $status, 'message' => $message]);
    exit;
}

// Obtener datos del POST
$data = json_decode(file_get_contents('php://input'), true);
$username = isset($data['username']) ? trim($data['username']) : '';
$password = isset($data['password']) ? $data['password'] : '';

if (empty($username) || empty($password)) {
    jsonResponse('error', 'Username y password son requeridos.');
}

// Validar longitud y formato de las entradas
if (strlen($username) < 3 || strlen($password) < 6) {
    jsonResponse('error', 'El username debe tener al menos 3 caracteres y la contraseña 6.');
}

// Verificar si el usuario ya existe
$stmt = $conn_usuarios->prepare("SELECT id_usuario FROM usuarios WHERE username = ?");
$stmt->bind_param("s", $username);
$stmt->execute();
$stmt->store_result();

if ($stmt->num_rows > 0) {
    $stmt->close();
    $conn_usuarios->close();
    jsonResponse('error', 'El username ya está en uso.');
}
$stmt->close();

// Hashear la contraseña
$password_hashed = password_hash($password, PASSWORD_BCRYPT);

// Insertar el nuevo usuario
$stmt = $conn_usuarios->prepare("INSERT INTO usuarios (username, password_hashed, dispositivos_asociados) VALUES (?, ?, '')");
$stmt->bind_param("ss", $username, $password_hashed);

if ($stmt->execute()) {
    jsonResponse('success', 'Usuario registrado exitosamente.');
} else {
    jsonResponse('error', 'Error al registrar el usuario: ' . $conn_usuarios->error);
}

$stmt->close();
$conn_usuarios->close();
?>
