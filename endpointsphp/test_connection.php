<?php
// test_connection.php

// Habilitar la visualización de errores
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

require 'config.php';

if ($conn_usuarios->ping()) {
    echo "Conexión exitosa a la DB de Usuarios.<br>";
} else {
    echo "Error en la conexión a la DB de Usuarios.<br>";
}

if ($conn_dispositivos->ping()) {
    echo "Conexión exitosa a la DB de Dispositivos.<br>";
} else {
    echo "Error en la conexión a la DB de Dispositivos.<br>";
}

$conn_usuarios->close();
$conn_dispositivos->close();
?>
