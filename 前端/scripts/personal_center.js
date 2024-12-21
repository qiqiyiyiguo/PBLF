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

// 退出登录逻辑
function logout() {
    const jsonData = JSON.stringify({
        "task": 'logout'
    });

    var ws = new WebSocket('ws://localhost:8080/echo'); // 使用 ws 协议

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
        const response = event.data;
        // 假设响应格式为纯文本，每行一个预约信息
        const appointments = response.split('\n').map(line => {
            const parts = line.split(' ');
            return { date: parts[0], time: parts[1], campus: parts.slice(2).join(' ') };
        });
        showAppointments(appointments);
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
    // 假设登录信息已经保存在本地存储中
    const username = localStorage.getItem('username');
    const number = localStorage.getItem('number');
    showUserInfo(username, number);
    getAppointments();
});