npm -v
node -v
node node.js &

http://47.254.38.236:3000/?hid=c8441c000001
http://47.254.38.236:3001/?hid=c8441c000001&longitude=113.862457&latitude=22.562511&locatetime=1524015009
http://127.0.0.1:3001/?hid=c8441c000001&longitude=113.862457&latitude=22.562511&locatetime=1524015009
http://47.254.38.236:3002/?hid=c8441c000001&locatetime=1524030695


http://127.0.0.1:3000/?hid=c8441c000001




Window �ϰ�װNode.js


Ubuntu apt-get���װ
sudo apt-get install nodejs
sudo apt-get install npm


CentOS �°�װ Node.js
�Ƽ�ʹ��nvm������node�汾���⣬��Ȼ��ͷ�ۡ�
NVM
curl -o- https://raw.githubusercontent.com/creationix/nvm/v0.29.0/install.sh | bash
һ��Ҫ��������shell,,����һ���µ�shell
nvm ls-remote
nvm install v9.9.0
nvm use v9.9.0
nvm alias default 9.9.0
which node
��node������/usr/binĿ¼
ln -s /usr/bin/node /root/.nvm/versions/node/v9.9.0/bin/node  (�滻Ϊ��node��λ��)


ʹ���Ա� NPM ����
npm install -g cnpm --registry=https://registry.npm.taobao.org
cd /usr/lib
cnpm install mysql
cnpm install forever -g

js�ŵ���
 /usr/lib/node_modules/


forever start app.js          #����
$ forever stop app.js           #�ر�



forever start node.js
forever start selectteen.js
forever start insert.js


ps aux|grep node


SELECT * FROM gps_0 where hid = 'c8441c00017c' order by locatetime desc LIMIT 0,10

<script>
var myDate = new Date();
myDate.getYear();        //��ȡ��ǰ���(2λ)
myDate.getFullYear();    //��ȡ���������(4λ,1970-????)
myDate.getMonth();       //��ȡ��ǰ�·�(0-11,0����1��)
myDate.getDate();        //��ȡ��ǰ��(1-31)
myDate.getDay();         //��ȡ��ǰ����X(0-6,0����������)
myDate.getTime();        //��ȡ��ǰʱ��(��1970.1.1��ʼ�ĺ�����)
myDate.getHours();       //��ȡ��ǰСʱ��(0-23)
myDate.getMinutes();     //��ȡ��ǰ������(0-59)
myDate.getSeconds();     //��ȡ��ǰ����(0-59)
myDate.getMilliseconds();    //��ȡ��ǰ������(0-999)
myDate.toLocaleDateString();     //��ȡ��ǰ����
var mytime=myDate.toLocaleTimeString();     //��ȡ��ǰʱ��
myDate.toLocaleString();        //��ȡ������ʱ��

document.write(myDate.getFullYear());
document.write("<br/>")
var d = Date.UTC(myDate.getUTCFullYear(),myDate.getUTCMonth(),myDate.getUTCDate(),myDate.getUTCHours(),myDate.getUTCMinutes(),myDate.getUTCSeconds())
document.write(d/1000)
document.write("<br/>")
document.write( d/1000  - 1528316578);
//1528352005
</script>

http://www.w3school.com.cn/tiy/t.asp?f=jseg_utc

