function clickAfter(who) {
    if(who==='个人中心'){
        checkLogin();
    }
    else if(who==='文创产品'){
        window.location.href = "./goods.html";
    }
    else if(who==='校园风景'){
        window.location.href = "./view.html";
    }
    else if(who==='预约管理'){
        window.location.href = "./book.html";
    }
    else if(who==='首页'){
        window.location.href = "./index.html";
    }
}

// 检查登录逻辑
function checkLogin() {
    const jsonData = JSON.stringify({
        "task": 'checkLogin'
    });

    var ws = new WebSocket('ws://localhost:8080/echo'); // 使用 ws 协议

    ws.onopen = function () {
        console.log('WebSocket 连接已经建立。');
        ws.send(jsonData);
    };

    ws.onmessage = function (event) {
        console.log('收到服务器消息：', event.data);
        const receiverdData = event.data;
        if (receiverdData=="success") {
            // 登录成功，跳转到个人中心页面
            window.location.href = './personal_center.html';
        } else {
            window.location.href = './login.html';
        }
    };

    ws.onerror = function (event) {
        console.error('WebSocket 连接出现错误：', event);
    };

    ws.onclose = function () {
        console.log('WebSocket 连接已经关闭。');
    };

    console.log("本次提交数据：", jsonData);
}