var express = require('express');
var router = express.Router();

var mysql = require('mysql');
var dbconfig = require('./config/database.js');
var connection = mysql.createConnection(dbconfig);

connection.connect();

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index');
});

router.post ('/attend', function (req, res){
  var id = req.param('ID');
  var pw = req.param('PW');

  connection.query('SELECT * FROM User where ID=' + '"' + id + '"' + ' AND PW=' + '"' + pw + '"', function (err, rows){
    if (err) throw err;

    if (rows.length != 0){
      req.session.ID = rows[0].ID;
      req.session.PW = rows[0].PW;
      req.session.Name = rows[0].Name;
      req.session.Major = rows[0].Major;
      req.session.Type = rows[0].Type;
      req.session.LectureCode = rows[0].LectureCode;
      req.session.NFCID = rows[0].NFCID;
      /*
      var ID = req.session.ID;
      var PW = req.session.PW;
      var Name = req.session.Name;
      var Major = req.session.Major;
      var Type = req.session.Type;
      var LectureCode = req.session.LectureCode;
      var NFCID = req.session.NFCID;
    */
      //console.log ('The solution is: ', rows);
      //res.send (rows);
      //console.log(req.session);
      //console.log(rows[0].Name);
      if (rows[0].Type == 0){
        connection.query('SELECT DISTINCT Lecture.Name, Attendance.Attend FROM Lecture, Attendance WHERE Attendance.UserID =' + '"' + id + '" AND Lecture.Code = Attendance.LectureCode', function (err, rowss){
          if (err) throw err;

          res.redirect('/attend')
        });
      } else{
        connection.query('SELECT DISTINCT Lecture.Name, Attendance.Attend FROM Lecture, Attendance WHERE Attendance.UserID =' + '"' + id + '" AND Lecture.Code = Attendance.LectureCode', function (err, rowss){
          if (err) throw err;

          res.redirect('/prof')
        });
      }
    } else {
      res.send ('<script>alert("존재하지 않는 계정입니다!"); location.href="/";</script>');
    }
  });
});

module.exports = router;
