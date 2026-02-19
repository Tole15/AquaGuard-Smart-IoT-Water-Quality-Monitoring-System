<?php
// config.php

$servername = "localhost"; // Cambia esto si tu servidor de base de datos es diferente
$username = "app_user"; // Usuario de la base de datos que creaste
$password = "Tolentino1"; // Contraseña que asignaste al usuario
$dbname_usuarios = "usuarios_db"; // Nombre correcto de la base de datos de usuarios
$dbname_dispositivos = "dispositivos_db"; // Nombre correcto de la base de datos de dispositivos

// Crear conexión para la DB de Usuarios
$conn_usuarios = new mysqli($servername, $username, $password, $dbname_usuarios);

// Verificar conexión
if ($conn_usuarios->connect_error) {
    die("Conexión fallida a la DB de Usuarios: " . $conn_usuarios->connect_error);
}

// Crear conexión para la DB de Dispositivos
$conn_dispositivos = new mysqli($servername, $username, $password, $dbname_dispositivos);

// Verificar conexión
if ($conn_dispositivos->connect_error) {
    die("Conexión fallida a la DB de Dispositivos: " . $conn_dispositivos->connect_error);
}

// Establecer el conjunto de caracteres a UTF-8
$conn_usuarios->set_charset("utf8");
$conn_dispositivos->set_charset("utf8");
?>
