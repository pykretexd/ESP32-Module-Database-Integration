<?PHP
    include_once("./lib/rx-lib-database.php");

    function rx_log($message)
    {
    error_log(date('Y-m-d H:i:s') .  " : " . $message . "\n", 3, "./insert-temp.log");
    }

    $method = $_SERVER['REQUEST_METHOD'];
    $request = explode('/', trim($_SERVER['PATH_INFO'],'/'));

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
    
    $query = sprintf("INSERT INTO public.measurements (user, station, average_wind_speed, gust_wind_speed, wind_direction, rainfall, temperature, humidity, light, uvi) 
    VALUES ($user, $station, $average_wind_speed, $gust_wind_speed, $wind_direction, $rainfall, $temperature, $humidity, $light, $uvi);");

    rx_log("Query: " . $query);

    rx_db_insert($query);
?>