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

    function rx_log($message)
    {
    error_log(date('Y-m-d H:i:s') .  " : " . $message . "\n", 3, "./insert-temp.log");
    }

    $method = $_SERVER['REQUEST_METHOD'];
    $request = explode('/', trim($_SERVER['PATH_INFO'],'/'));

    $sql = sprintf("INSERT INTO public.bytes (byte, device_uuid) VALUES ($1, $2);");
    
    $result = pg_query_params($con, $sql, array($_REQUEST['byte'], $_REQUEST['device_uuid']));

    rx_log("TEST: " . $sql . $_REQUEST['byte']);
?>