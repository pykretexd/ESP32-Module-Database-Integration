<?PHP
   // Connect to database
   $db_host = "127.0.0.1";
   $db_name = "agrilog";
   $db_port = "5432";
   $db_user = "postgres";
   $db_pass = "ntig123!";

   $dbconn = pg_connect("hostaddr=$db_host port=$db_port dbname=$db_name user=$db_user password=$db_pass");

   if (!$dbconn)
   {
      printf("<p>No database connection</p>");
   }

   function rx_db_insert($query)
   {
      global $dbconn;
      $result = pg_query($dbconn, $query);
   }

   function rx_db_query($query)
   {
      global $dbconn;
      
      // Try to prevent sql injection
      $safe_query = pg_escape_string($query);
      
      $result = pg_query($dbconn, $safe_query);
   }
?>
