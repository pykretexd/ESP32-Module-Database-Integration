<?php 
    // Connect to database
    $db_host = "127.0.0.1";
    $db_name = "agrilog";
    $db_port = "5432";
    $db_user = "postgres";
    $db_pass = "ntig123!";

    $con = pg_connect("hostaddr=$db_host port=$db_port dbname=$db_name user=$db_user password=$db_pass");
    if (!$con)
    {
        printf("<p>No database connection</p>");
    }

    // Function for logging
    function rx_log($message)
    {
    error_log(date('Y-m-d H:i:s') .  " : " . $message . "\n", 3, "./insert-temp.log");
    }

    // Access sent packet from module
    $method = $_SERVER['REQUEST_METHOD'];
    $request = explode('/', trim($_SERVER['PATH_INFO'],'/'));

    // If packet comes from weather station
    if ($_REQUEST['device'] == "d4") {
        // SQL query
        $sql = sprintf("INSERT INTO public.measurements (average_wind_speed, gust_wind_speed, wind_direction, rainfall, temperature, humidity, light, uvi, station_name) VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9);");

        // Parameterized query to avoid SQLi
        $result = pg_query_params($con, $sql, array($_REQUEST['average_wind_speed'],
                                                    $_REQUEST['gust_wind_speed'],
                                                    $_REQUEST['wind_direction'],
                                                    $_REQUEST['rainfall'],
                                                    $_REQUEST['temperature'],
                                                    $_REQUEST['humidity'],
                                                    $_REQUEST['light'],
                                                    $_REQUEST['uvi'],
                                                    $_REQUEST['station_name']))
    }