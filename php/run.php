<?php header("Content-Type: text/html; charset=utf8");
 $conn = mysql_connect("mydbinstance.ctntuub9tahf.us-west-2.rds.amazonaws.com", "root", "12345678");
 $dbconn = mysql_select_db("CreativeDesignDB", $conn);
 $nfcid = $_GET["NFCID"];
 $att = $_GET["att"];
 $date = date("ymd");
 $time = date("His");
 $yoil_temp = array("일","월","화","수","목","금","토");
 $yoil = $yoil_temp[date('w', strtotime(date("Y-m-d")))];

 $sql = "SELECT * FROM User WHERE NFCID='$nfcid'";
 $ret = mysql_query($sql, $conn);
 $row1 = mysql_fetch_array($ret);

 if ($row1[att] != '0'){
   $sql="UPDATE Attendance set attend='$att' WHERE UserID='$row1[ID]' AND Attendance.Date='$date'";
   if (!mysql_query($sql, $conn)){
     die('Error: '.mysql_error());
   }
  echo "1 record modified.";
 }
 else{
   $sql = "SELECT MAX(Attendance.Index) AS Max_index FROM Attendance order by Attendance.Index";
   $ret = mysql_query($sql, $conn);
   $row = mysql_fetch_array($ret);
   $Index = $ret[Max_index]+1;
   $sql ="INSERT INTO Attendance (Attendance.Index, UserID, LectureCode, Date, profName, Attend) VALUES ('$Index', '$row1[ID]', 'CSE203-01', '$date', '관리자', 0)";

  if (!mysql_query($sql, $conn)){
    die('Error: '.mysql_error());
  }
  echo "1 record added.";
  }
 mysql_close($conn);
?>
