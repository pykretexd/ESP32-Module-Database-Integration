<?php
    // Connects to the database
    // IP: 10.209.1.138, Username: 198526_xo18147, Password: Simon_19, DB: 198526-agrilog
    $con = mysqli_connect("10.209.1.138", "198526_xo18147", "Simon_19", "198526-agrilog"); 

    // Fetches values
    $method = $_SERVER['REQUEST_METHOD'];
    $request = explode('/', trim($_SERVER['PATH_INFO'],'/'));

    // Values
    $user = (int) $_REQUEST['user'];
    $station = (int) $_REQUEST['station'];
    $average_wind_speed = (int) $_REQUEST['average_wind_speed'];
    $gust_wind_speed = (int) $_REQUEST['gust_wind_speed'];
    $wind_direction = (int) $_REQUEST['wind_direction'];
    $rainfall = (int) $_REQUEST['rainfall'];
    $temperature = (int) $_REQUEST['temperature'];
    $humidity = (int) $_REQUEST['humidity'];
    $light = (int) $_REQUEST['light'];
    $uvi = (int) $_REQUEST['uvi'];

    // Inputs values
    $query = "INSERT INTO `measurements` (average_wind_speed, gust_wind_speed, wind_direction, rainfall, temperature, humidity, light, UVI)
    VALUES ($user, $station, $average_wind_speed, $gust_wind_speed, $wind_direction, $rainfall, $temperature, $humidity, $light, $uvi)";
?>