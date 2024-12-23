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
        recheckLogin();
    }
    else if(who==='首页'){
        window.location.href = "./index.html";
    }
    else if(who==='consult'){
        window.location.href = "./consult.html";
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

// 预约管理检查登录逻辑
function recheckLogin() {
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
            window.location.href = './book.html';
        } else {
            alert('请先登录！');
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


// 退出登录逻辑
function logout() {
    const jsonData = JSON.stringify({
        "task": 'logout'
    });

    var ws = new WebSocket('ws://localhost:8080/echo'); 

    ws.onopen = function () {
        console.log('WebSocket 连接已经建立。');
        ws.send(jsonData);
    };

    ws.onmessage = function (event) {
        console.log('收到服务器消息：', event.data);
        const receiverdData = event.data;
        if (receiverdData=="logged out") {
            // 退出成功，跳转到注册页面
            window.location.href = './login.html';
        } else {
            alert(response.message || 'Logout failed');
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


// 显示用户信息和预约信息
function showUserInfo(name, number) {
    document.getElementById('user-name').textContent = name;
    document.getElementById('user-number').textContent = number;
}

function showAppointments(appointments) {
    const appointmentsElement = document.getElementById('appointments');
    appointmentsElement.innerHTML = ''; // 清空现有预约信息
    appointments.forEach(appointment => {
        const appointmentElement = document.createElement('div');
        appointmentElement.textContent = `Date: ${appointment.date}, Time: ${appointment.time}, Campus: ${appointment.campus}`;
        appointmentsElement.appendChild(appointmentElement);
    });
}

// 获取预约信息
function getAppointments() {
    const ws = new WebSocket('ws://localhost:8080');
    ws.onopen = function() {
        const data = JSON.stringify({ task: 'getAppointments' });
        ws.send(data);
    };
    ws.onmessage = function(event) {
        const response = event.data.trim();  // 去除响应数据开头和结尾的空行
        // 假设响应格式为纯文本，每行一个预约信息
        const appointments = response.split('\n').map(line => {
            const trimmedLine = line.trim();  // 去除每行的前后空白字符
            if (trimmedLine) {  // 只处理非空行
                const parts = trimmedLine.split(' ');
                return { 
                    date: parts.slice(1, 4).join(' '), 
                    time: parts.slice(4, 6).join(' '), 
                    campus: parts.slice(6, 9).join(' ') 
                };
            }
            return null;  // 空行返回 null
        }).filter(appointment => appointment !== null);  // 过滤掉 null
    
        showAppointments(appointments);
    };
    ws.onerror = function(event) {
        console.error('WebSocket error:', event);
    };
    ws.onclose = function() {
        console.log('WebSocket connection closed');
    };
}






// 获取用户信息
function getUsers() {
    const ws = new WebSocket('ws://localhost:8080');
    ws.onopen = function() {
        const data = JSON.stringify({ task: 'getUser' });
        ws.send(data);
    };
    ws.onmessage = function(event) {
        const response = event.data;
        console.log('Received user data:', response);  // 调试日志
        const parts = response.split(' ');
        const name = parts[0];  // 获取用户名
        const number = parts[1];  // 获取号码
        showUserInfo(name, number);  // 显示用户信息
    };
    ws.onerror = function(event) {
        console.error('WebSocket error:', event);
    };
    ws.onclose = function() {
        console.log('WebSocket connection closed');
    };
}


// 在页面加载完成后获取用户信息和预约信息
document.addEventListener('DOMContentLoaded', function() {
    getUsers();
    getAppointments();
});