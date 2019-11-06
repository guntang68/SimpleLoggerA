
#ifndef PAGES_H_
#define PAGES_H_

const char inputB[] PROGMEM = R"=====(
	<!DOCTYPE html> <html>
	<head><meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
	<title>Configure DDMS</title>
	<style>
	html { 
		font-family: Helvetica; 
		display: inline-block; 
		margin: 0px auto; 
		text-align: center;}
	body{
		margin-top: 50px;} 
	h1 {
		color: #444444;
		margin: 50px auto 30px;} 
	h3 {
		color: #444444;
		margin-bottom: 50px;}
	h5 {
		color: #ff0000;
		margin: 50px auto 30px;}";
	input.html-text-box {
		display: block;
		width: 80px;
		background-color: #3498db;
		border: none;
		color: white;
		padding: 13px 30px;
		text-decoration: none;
		font-size: 25px;
		margin: 0px auto 35px; 
		cursor: pointer;
		border-radius: 4px;}
	.button-on {
		background-color: #3498db;}
	.button-on:active {
		background-color: #2980b9;}
	.button-off {
		background-color: #34495e;}
	.button-off:active {
		background-color: #2c3e50;}
	p {
		font-size: 14px;
		color: #888;
		margin-bottom: 10px;}
	</style>
	</head>
	<body>
	<h1>WELCOME TO GREENFINDER DDMS</h1>
	<h3>Configure DDMS Sonar depth</h3>

	<h5>[note]</h5>

	<form action="/config" method="POST">
	Min:
	<br/>
	<input type="number" name="min" value="[minn]" maxlength="100" />
	<br/>
	Max:
	<br/>
	<input type="number" name="max" value="[maxx]" maxlength="100" />
	<br/>
	<input type="submit" value="Submit" />
	</form>

	</body>
	</html>)=====";

const char inputA[] PROGMEM = R"=====(
	<!DOCTYPE html> <html>
	<head><meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
	<title>Configure NiNe Machine</title>
	<style>
	html { 
		font-family: Helvetica; 
		display: inline-block; 
		margin: 0px auto; 
		text-align: center;}
	body{
		margin-top: 50px;} 
	h1 {
		color: #444444;
		margin: 50px auto 30px;} 
	h3 {
		color: #444444;
		margin-bottom: 50px;}
	h5 {
		color: #ff0000;
		margin: 50px auto 30px;}";
	input.html-text-box {
		display: block;
		width: 80px;
		background-color: #3498db;
		border: none;
		color: white;
		padding: 13px 30px;
		text-decoration: none;
		font-size: 25px;
		margin: 0px auto 35px; 
		cursor: pointer;
		border-radius: 4px;}
	.button-on {
		background-color: #3498db;}
	.button-on:active {
		background-color: #2980b9;}
	.button-off {
		background-color: #34495e;}
	.button-off:active {
		background-color: #2c3e50;}
	p {
		font-size: 14px;
		color: #888;
		margin-bottom: 10px;}
	</style>
	</head>
	<body>
	<h1>WELCOME TO NiNe MACHINE</h1>
	<h3>Configure NiNe Machine ID</h3>

	
	
	<h5>[note]</h5>

	<form action="/adTech" method="POST">
	Machine Mac Address:
	<br/>
	[mac]
	<br/><br/>
	Machine ID:
	<br/>
	<input type="text" name="mId" value="[mID]" maxlength="6" />
	<input type="submit" value="Submit" />
	</form>

	<form action="/adTech" method="POST">
	<br/><br/>
	<input type="hidden" name="clear" value=true />
	<input type="submit" value="Clear FS" />
	</form>

	</body>
	</html>)=====";

const char ota[] PROGMEM = R"=====(
	<style>
	.example1 {
	  font: 12px/22px Courier;
	  color: DarkRed;
	  }
	.bold {
	  font-weight: bold;
	  }
	.italic {
	  font-style: italic;
	  }
	</style>

	<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>
	<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>
		<input type='file' name="update">
		<input type='submit' value='Update'>
		<a href="/reboot">Reboot</a>
	</form>
	<div id='prg'></div>
	<div id="dd" class="example1"></div>
	<script>
	var busy = 0;
	var timer;
	$('form').submit(function(e){
		e.preventDefault();
		var form = $('#upload_form')[0];
		var data = new FormData(form);
		$.ajax({
			url: '/update',
			type: 'POST',
			data: data,
			contentType: false,
			processData:false,
			xhr: function() {
				var xhr = new window.XMLHttpRequest();
				xhr.upload.addEventListener('progress', function(evt) {
				if (evt.lengthComputable) {
					var per = evt.loaded / evt.total;
					busy = 1;
					$('#prg').html('progress: ' + Math.round(per*100) + '%   &nbsp;&nbsp;&nbsp;&nbsp;' + evt.loaded + '/' + evt.total);
				}
	  		}, false);
	  		return xhr;
	  		},
	  		success:function(d, s) {
	  			console.log('success!');
				busy = 0;
				tengokData();
				$('#prg').html('progress: <b>done</b>');
	 		},
	 		error: function (a, b, c) {
				$('#prg').html('progress: <b>fail</b>');
	 		}
	 	});
	 });
	function tengokData(){
		if(busy == 0){

			$.ajax({
				type: "GET",
				url: "/stat",
				data: "",
				dataType: "text",
				async:   true,
				timeout: 600,
				success: function(data) {
					timer = new Timer(function() {	tengokData();}, 5000);
					$('#dd').html(data);
				},
				error: function() {
					timer = new Timer(function() {	tengokData();}, 5000);
	//				$('#dd').html("n/a");
				}
			});

		}



	}

	$(document).ready(function(){
		tengokData();
	});


	function Timer(callback, delay) {
		var timerId, start, remaining = delay;
	
		this.pause = function() {
			window.clearTimeout(timerId);
			remaining -= new Date() - start;
		};
		
		this.resume = function() {
			start = new Date();
			window.clearTimeout(timerId);
			timerId = window.setTimeout(callback, remaining);
		};
		
		this.resume();
	}
	 </script>)=====";



//const char loginIndex[] PROGMEM = R"=====(
// <form name='loginForm'>
//    <table width='20%' bgcolor='A09F9F' align='center'>
//        <tr>
//            <td colspan=2>
//                <center><font size=4><b>AKU Login Page</b></font></center>
//                <br>
//            </td>
//            <br>
//            <br>
//        </tr>
//        <td>Username:</td>
//        <td><input type='text' size=25 name='userid'><br></td>
//        </tr>
//        <br>
//        <br>
//        <tr>
//            <td>Password:</td>
//            <td><input type='Password' size=25 name='pwd'><br></td>
//            <br>
//            <br>
//        </tr>
//        <tr>
//            <td><input type='submit' onclick='check(this.form)' value='Login'></td>
//        </tr>
//    </table>
//</form>
//<script>
//    function check(form)
//    {
//    if(form.userid.value=='admin' && form.pwd.value=='l')
//    {
//		window.open('/ota')
//    }
//    else
//    {
//     	alert('Error Password or Username')/*displays error message*/
//    }
//    }
//</script>)=====";
//
//
//
//
//const char GAME_page[] PROGMEM = R"=====(
//<!DOCTYPE html>
//<html>
//<style>
//body {
//  text-align:center;
//  font-family: helvetica;
//}
//canvas {
//  border: 2px solid rgb(151, 149, 149);
//}
//</style>
//<body>
//ESP32 web server demo
//<h1>circuits4you.com</h1>
//<h3>Simple Snake Game</h3>
//<canvas id="stage" height="400" width="520"></canvas><br>
//use arraow keys to play
//</body>
//</html>
//<script>
///**
// * Namespace
// */
//var Game      = Game      || {};
//var Keyboard  = Keyboard  || {};
//var Component = Component || {};
//
///**
// * Keyboard Map
// */
//Keyboard.Keymap = {
//  37: 'left',
//  38: 'up',
//  39: 'right',
//  40: 'down'
//};
//
///**
// * Keyboard Events
// */
//Keyboard.ControllerEvents = function() {
//
//  // Setts
//  var self      = this;
//  this.pressKey = null;
//  this.keymap   = Keyboard.Keymap;
//
//  // Keydown Event
//  document.onkeydown = function(event) {
//    self.pressKey = event.which;
//  };
//
//  // Get Key
//  this.getKey = function() {
//    return this.keymap[this.pressKey];
//  };
//};
//
///**
// * Game Component Stage
// */
//Component.Stage = function(canvas, conf) {
//
//  // Sets
//  this.keyEvent  = new Keyboard.ControllerEvents();
//  this.width     = canvas.width;
//  this.height    = canvas.height;
//  this.length    = [];
//  this.food      = {};
//  this.score     = 0;
//  this.direction = 'right';
//  this.conf      = {
//    cw   : 10,
//    size : 5,
//    fps  : 1000
//  };
//
//  // Merge Conf
//  if (typeof conf == 'object') {
//    for (var key in conf) {
//      if (conf.hasOwnProperty(key)) {
//        this.conf[key] = conf[key];
//      }
//    }
//  }
//
//};
//
///**
// * Game Component Snake
// */
//Component.Snake = function(canvas, conf) {
//
//  // Game Stage
//  this.stage = new Component.Stage(canvas, conf);
//
//  // Init Snake
//  this.initSnake = function() {
//
//    // Itaration in Snake Conf Size
//    for (var i = 0; i < this.stage.conf.size; i++) {
//
//      // Add Snake Cells
//      this.stage.length.push({x: i, y:0});
//    }
//  };
//
//  // Call init Snake
//  this.initSnake();
//
//  // Init Food
//  this.initFood = function() {
//
//    // Add food on stage
//    this.stage.food = {
//      x: Math.round(Math.random() * (this.stage.width - this.stage.conf.cw) / this.stage.conf.cw),
//      y: Math.round(Math.random() * (this.stage.height - this.stage.conf.cw) / this.stage.conf.cw),
//    };
//  };
//
//  // Init Food
//  this.initFood();
//
//  // Restart Stage
//  this.restart = function() {
//    this.stage.length            = [];
//    this.stage.food              = {};
//    this.stage.score             = 0;
//    this.stage.direction         = 'right';
//    this.stage.keyEvent.pressKey = null;
//    this.initSnake();
//    this.initFood();
//  };
//};
//
///**
// * Game Draw
// */
//Game.Draw = function(context, snake) {
//
//  // Draw Stage
//  this.drawStage = function() {
//
//    // Check Keypress And Set Stage direction
//    var keyPress = snake.stage.keyEvent.getKey();
//    if (typeof(keyPress) != 'undefined') {
//      snake.stage.direction = keyPress;
//    }
//
//    // Draw White Stage
//    context.fillStyle = "white";
//    context.fillRect(0, 0, snake.stage.width, snake.stage.height);
//
//    // Snake Position
//    var nx = snake.stage.length[0].x;
//    var ny = snake.stage.length[0].y;
//
//    // Add position by stage direction
//    switch (snake.stage.direction) {
//      case 'right':
//        nx++;
//        break;
//      case 'left':
//        nx--;
//        break;
//      case 'up':
//        ny--;
//        break;
//      case 'down':
//        ny++;
//        break;
//    }
//
//    // Check Collision
//    if (this.collision(nx, ny) == true) {
//      snake.restart();
//      return;
//    }
//
//    // Logic of Snake food
//    if (nx == snake.stage.food.x && ny == snake.stage.food.y) {
//      var tail = {x: nx, y: ny};
//      snake.stage.score++;
//      snake.initFood();
//    } else {
//      var tail = snake.stage.length.pop();
//      tail.x   = nx;
//      tail.y   = ny;
//    }
//    snake.stage.length.unshift(tail);
//
//    // Draw Snake
//    for (var i = 0; i < snake.stage.length.length; i++) {
//      var cell = snake.stage.length[i];
//      this.drawCell(cell.x, cell.y);
//    }
//
//    // Draw Food
//    this.drawCell(snake.stage.food.x, snake.stage.food.y);
//
//    // Draw Score
//    context.fillText('Score: ' + snake.stage.score, 5, (snake.stage.height - 5));
//  };
//
//  // Draw Cell
//  this.drawCell = function(x, y) {
//    context.fillStyle = 'rgb(170, 170, 170)';
//    context.beginPath();
//    context.arc((x * snake.stage.conf.cw + 6), (y * snake.stage.conf.cw + 6), 4, 0, 2*Math.PI, false);
//    context.fill();
//  };
//
//  // Check Collision with walls
//  this.collision = function(nx, ny) {
//    if (nx == -1 || nx == (snake.stage.width / snake.stage.conf.cw) || ny == -1 || ny == (snake.stage.height / snake.stage.conf.cw)) {
//      return true;
//    }
//    return false;
//  }
//};
//
//
///**
// * Game Snake
// */
//Game.Snake = function(elementId, conf) {
//
//  // Sets
//  var canvas   = document.getElementById(elementId);
//  var context  = canvas.getContext("2d");
//  var snake    = new Component.Snake(canvas, conf);
//  var gameDraw = new Game.Draw(context, snake);
//
//  // Game Interval
//  setInterval(function() {gameDraw.drawStage();}, snake.stage.conf.fps);
//};
//
//
///**
// * Window Load
// */
//window.onload = function() {
//  var snake = new Game.Snake('stage', {fps: 100, size: 4});
//};
//</script>
//)=====";

#endif

