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

connection.connect();

/* GET users listing. */
router.get('/', function(req, res, next) {
  var ID = req.session.ID;
  var Name = req.session.Name;

  connection.query('SELECT DISTINCT Lecture.Name FROM Lecture, Attendance WHERE Attendance.UserID =' + '"' + ID + '" AND Lecture.Code = Attendance.LectureCode', function (err, rowss){
    if (err) throw err;

    res.render('attend', {loginedName: Name, num: rowss});
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
  var str = decodeURI(req.originalUrl).substring("11");
  req.session.sub = str;

  res.redirect('subject');
});

router.get('/subject', function(req, res, next) {
  var ID = req.session.ID;
  var str = req.session.sub;
  connection.query('SELECT Lecture.Name, Attendance.Attend FROM Lecture, Attendance WHERE Attendance.UserID =' + '"' + ID + '" AND Lecture.Code = Attendance.LectureCode AND Lecture.Name=' + '"'+ str +'"', function (err, rows){
    if (err) throw err;
    res.render('subject', {names: str, num: rows});
  });
});

module.exports = router;
