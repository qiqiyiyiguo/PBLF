function clickAfter(who) { 
    if(who === '个人中心'){ 
        window.location.href = "./login.html";
    } else if(who === '文创产品') {
        window.location.href = "./goods.html";
    } else if(who === '校园风景') {
        window.location.href = "./view.html";
    } else if(who === '预约管理') {
        window.location.href = "./book.html";
    } else if(who === '首页') {
        window.location.href = "./index.html";
    }
}

//获取输入
const chatmessages = document.getElementById('chatmessages').value;
console.log(chatmessages);

var ws = new WebSocket('ws://localhost:808/echo'); // 使用 ws 协议

ws.onopen = function () {
    console.log('WebSocket 连接已经建立。');
    ws.send(jsonData);
};


 ws.onmessage = function (event) {
        console.log('收到服务器消息：', event.data);
        const receiverdData = event.data;

        if (receiverdData === 'success') {
            alert("您已成功注册，点击确定跳转预约界面");
            window.location.href = './book.html';
        } else {
            alert(receiverdData); // 如果是用户已存在，返回相应的错误信息
        }
    };

    ws.onerror = function (event) {
        console.error('WebSocket 连接出现错误：', event);
    };

    ws.onclose = function () {
        console.log('WebSocket 连接已经关闭。');
    };

    console.log("本次提交数据：", jsonData);

