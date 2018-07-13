var http = require('http');
var url = require('url');
var util = require('util');
//
http.createServer(function(req, res){
    res.writeHead(200, {'Content-Type': 'text/plain'});
	if("/favicon.ico" == req.url)
	{
		res.end();
		return;
	}
 	//解析 url 参数
	var _params = url.parse(req.url, true).query;
	var _hid = _params.hid;
	var _longitude = _params.longitude;
	var _latitude = _params.latitude;
	var _locatetime = _params.locatetime;
	console.log(req.url);
	//console.log(_hid);
	//console.log(_longitude);
	//console.log(_latitude);
	//console.log(_locatetime);
	//console.log("------------");
	if(typeof(_hid)=="undefined" || typeof(_longitude)=="undefined" || typeof(_latitude)=="undefined" || typeof(_locatetime)=="undefined")
	{
		var _result = '{"status":"404","reason":"param error!"}'
		res.write(_result);
		res.end();
		return;
	}
	
	if(_hid=="" || _longitude=="" || _latitude=="" || _locatetime=="" )
	{
		var _result = '{"status":"404","reason":"error param"}'
		res.write(_result);
		res.end();
		return;
	}
	//
	//c84416000034
	//var _gpsid = _hid.charAt(_hid.length - 1);
		var ssss1 = _hid.substr(_hid.length-5);
	var ssss2 = parseInt(ssss1, 16);
	var _gpsid = ssss2%10;
	
	//console.log(_gpsid);
	var  sql = "insert into gps_" + _gpsid + "(hid,longitude,latitude,locatetime,speed) values('"+_hid +"','" +_longitude + "','"+ _latitude + "',"+ _locatetime +",'0.000')";
	//console.log(sql);
	//
	var query = require("./mysqlpool.js");
	query(sql, function (err, results, fields) {
		if(err)
		{
			//console.log('[SELECT ERROR] - ',err.message);
			//var _result = '{"status":"404","reason":"'+err.message+'"}'
			var _result = '{"status":"404","reason":"db error!"}'
			res.write(_result);
			res.end();
			return;
		}
		//console.log('--------------------------SELECT----------------------------');
		//console.log(result);
		//console.log('------------------------------------------------------------\n\n');  
		var _result = '{"status":"200","reason":"ok"}'
		res.write(_result);
		res.end();
	});

 
}).listen(3001);

// 终端打印如下信息
console.log('Server running at http://127.0.0.1:3001/');