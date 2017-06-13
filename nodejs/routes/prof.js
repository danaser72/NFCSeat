var express = require('express');
var router = express.Router();

var mysql = require('mysql');
var dbconfig = require('./config/database.js');
var connection = mysql.createConnection(dbconfig);

/*

var ID = req.session.ID;
var PW = req.session.PW;
var Name = req.session.Name;
var Major = req.session.Major;
var Type = req.session.Type;
var LectureCode = req.session.LectureCode;
var NFCID = req.session.NFCID;
*/

var temp;
var arr;

connection.connect();

/* GET users listing. */
router.get('/', function(req, res, next) {
  var Name = req.session.Name;
  connection.query('SELECT DISTINCT Lecture.Name FROM Lecture, Attendance WHERE Attendance.profName =' + '"' + Name + '" AND Lecture.Code = Attendance.LectureCode', function (err, rowss){
    if (err) throw err;

    res.render('prof', {loginedName: Name, num: rowss});
  });
});

router.get ('/logout', function (req, res){
  req.session.destroy(function(err){
    if (err) console.error('error', err);
    res.send('<script>alert("로그아웃 되었습니다!"); location.href="/";</script>');
  })
});

router.get('/get*', function (req, res){
  var ID = req.session.ID;
  var str = decodeURI(req.originalUrl).substring("9");
  req.session.sub = str;

  res.redirect('student');
});

router.get('/student', function(req, res, next) {
  var Name = req.session.Name;
  var ID = req.session.ID;
  var str = req.session.sub;

  console.log(str);
  connection.query('SELECT DISTINCT User.Name, User.ID FROM Attendance, Lecture, User WHERE User.ID=Attendance.UserID AND Lecture.Code = Attendance.LectureCode AND Lecture.Name='+ '"' + str + '"' + ' order by User.ID', function (err, rowss){
    if (err) throw err;

  connection.query('SELECT Attendance.Index, Attendance.Attend FROM Attendance, Lecture WHERE Lecture.Code = Attendance.LectureCode AND Lecture.Name='+ '"' + str + '"' + ' order by Attendance.UserID, Attendance.Date', function (err, rows){
    if (err) throw err;

    temp = new Array(16);

    for (var i = 0 ; i < 16 ; i++){
      temp [i] = new Array(rows.length/16);
    }

    for (var i = 0 ; i < 16 ; i++){
      for (var j = 0 ; j < rows.length/16 ; j++){
        temp[i][j] = rows[i+16*j];
      }
    }
    res.render('student', {usernames: rowss, num: temp});
  });
  });
});

router.post('/student', function(req,res, next){
  arr = req.param('attend');
  res.send("<script>if(confirm('수정하시겠습니까?')){alert('수정되었습니다.'); location.href='student/success'}else{location.href='student'};</script>");
});

router.get('/student/success', function (req,res,next){
  for (var i = 0 ; i < arr.length/temp.length ; i++){
    for (var j = 0 ; j < temp.length; j++){
      temp[j][i].Attend = arr[i*temp.length+j];
    }
  }

  var rowss = new Array(arr.length);

  for (var i = 0 ; i < temp.length; i++){
    for (var j= 0 ; j < arr.length/temp.length ;j++){
      rowss[i+16*j] = temp[i][j];
    }
  }

  for (var i = 0 ; i < rowss.length ; i++){
    connection.query('UPDATE Attendance SET Attend="' + rowss[i].Attend + '" WHERE Attendance.Index="' + rowss[i].Index + '"', function(err, rows){
      if (err) throw err;
    });
  }
  res.send("<script>location.href='../student'</script>");
});

module.exports = router;
