//
var http = require('http');
var url = require('url');
var util = require('util');
var mysql = require("mysql");
var pool = mysql.createPool({
	host: '47.88.0.145',
	user: 'gvap',
	password: 'ulifedb@hddvs2013$',
	port: '63306',
	database: 'gvap',
});

var query = function (sql, callback) 
{
	pool.getConnection(function (err, conn) 
	{
		if (err) {
			//console.log('[ERROR] - ', err.message);
			callback(err,'','');
		} else {
			conn.query(sql, function (err, results, fields) {
				//事件驱动回调
				callback(err, results, fields);
			});
			//释放连接，需要注意的是连接释放需要在此处释放，而不是在查询回调里面释放
			conn.release();
		}
	});
};

module.exports = query;