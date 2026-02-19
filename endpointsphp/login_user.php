<?php
// login_user.php
header('Content-Type: application/json');
require_once 'config.php';

// Iniciar sesión (opcional, si usas sesiones)
session_start();

// Obtener datos del POST
$data = json_decode(file_get_contents('php://input'), true);
$username = isset($data['username']) ? trim($data['username']) : '';
$password = isset($data['password']) ? $data['password'] : '';

if (empty($username) || empty($password)) {
    echo json_encode(['status' => 'error', 'message' => 'Username y password son requeridos.']);
    exit;
}

// Obtener el usuario
$stmt = $conn_usuarios->prepare("SELECT id_usuario, password_hashed, dispositivos_asociados FROM usuarios WHERE username = ?");
$stmt->bind_param("s", $username);
$stmt->execute();
$stmt->store_result();

if ($stmt->num_rows === 0) {
    echo json_encode(['status' => 'error', 'message' => 'Usuario no encontrado.']);
    $stmt->close();
    $conn_usuarios->close();
    exit;
}

$stmt->bind_result($id_usuario, $password_hashed, $dispositivos_asociados);
$stmt->fetch();

// Verificar la contraseña
if (!password_verify($password, $password_hashed)) {
    echo json_encode(['status' => 'error', 'message' => 'Contraseña incorrecta.']);
    $stmt->close();
    $conn_usuarios->close();
    exit;
}

$stmt->close();

// Opcional: establecer sesión
// $_SESSION['id_usuario'] = $id_usuario;

// Obtener la lista de dispositivos asociados
$dispositivos = [];
if (!empty($dispositivos_asociados)) {
    $dispositivos_ids = explode(',', $dispositivos_asociados);
    $placeholders = implode(',', array_fill(0, count($dispositivos_ids), '?'));
    $types = str_repeat('i', count($dispositivos_ids));
    
    $stmt = $conn_dispositivos->prepare("SELECT etiqueta_dispositivo FROM dispositivos WHERE id_dispositivo IN ($placeholders)");
    $stmt->bind_param($types, ...$dispositivos_ids);
    $stmt->execute();
    $result = $stmt->get_result();
    
    while ($row = $result->fetch_assoc()) {
        $dispositivos[] = $row['etiqueta_dispositivo'];
    }
    $stmt->close();
}

// Respuesta JSON incluyendo id_usuario
echo json_encode([
    'status' => 'success',
    'id_usuario' => $id_usuario,
    'dispositivos_asociados' => $dispositivos
]);

$conn_usuarios->close();
$conn_dispositivos->close();
?>
