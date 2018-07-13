npm -v
node -v
node node.js &

http://47.254.38.236:3000/?hid=c8441c000001
http://47.254.38.236:3001/?hid=c8441c000001&longitude=113.862457&latitude=22.562511&locatetime=1524015009
http://127.0.0.1:3001/?hid=c8441c000001&longitude=113.862457&latitude=22.562511&locatetime=1524015009
http://47.254.38.236:3002/?hid=c8441c000001&locatetime=1524030695


http://127.0.0.1:3000/?hid=c8441c000001




Window 上安装Node.js


Ubuntu apt-get命令安装
sudo apt-get install nodejs
sudo apt-get install npm


CentOS 下安装 Node.js
推荐使用nvm来管理node版本问题，不然很头疼。
NVM
curl -o- https://raw.githubusercontent.com/creationix/nvm/v0.29.0/install.sh | bash
一定要重新启动shell,,开启一个新的shell
nvm ls-remote
nvm install v9.9.0
nvm use v9.9.0
nvm alias default 9.9.0
which node
将node链接至/usr/bin目录
ln -s /usr/bin/node /root/.nvm/versions/node/v9.9.0/bin/node  (替换为您node的位置)


使用淘宝 NPM 镜像
npm install -g cnpm --registry=https://registry.npm.taobao.org
cd /usr/lib
cnpm install mysql
cnpm install forever -g

js放到下
 /usr/lib/node_modules/


forever start app.js          #启动
$ forever stop app.js           #关闭



forever start node.js
forever start selectteen.js
forever start insert.js


ps aux|grep node


SELECT * FROM gps_0 where hid = 'c8441c00017c' order by locatetime desc LIMIT 0,10

<script>
var myDate = new Date();
myDate.getYear();        //获取当前年份(2位)
myDate.getFullYear();    //获取完整的年份(4位,1970-????)
myDate.getMonth();       //获取当前月份(0-11,0代表1月)
myDate.getDate();        //获取当前日(1-31)
myDate.getDay();         //获取当前星期X(0-6,0代表星期天)
myDate.getTime();        //获取当前时间(从1970.1.1开始的毫秒数)
myDate.getHours();       //获取当前小时数(0-23)
myDate.getMinutes();     //获取当前分钟数(0-59)
myDate.getSeconds();     //获取当前秒数(0-59)
myDate.getMilliseconds();    //获取当前毫秒数(0-999)
myDate.toLocaleDateString();     //获取当前日期
var mytime=myDate.toLocaleTimeString();     //获取当前时间
myDate.toLocaleString();        //获取日期与时间

document.write(myDate.getFullYear());
document.write("<br/>")
var d = Date.UTC(myDate.getUTCFullYear(),myDate.getUTCMonth(),myDate.getUTCDate(),myDate.getUTCHours(),myDate.getUTCMinutes(),myDate.getUTCSeconds())
document.write(d/1000)
document.write("<br/>")
document.write( d/1000  - 1528316578);
//1528352005
</script>

http://www.w3school.com.cn/tiy/t.asp?f=jseg_utc

