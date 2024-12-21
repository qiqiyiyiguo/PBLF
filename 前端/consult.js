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
const messageInput = document.getElementById('messageInput');
const sendButton = document.getElementById('sendButton');
const chatMessages = document.getElementById('chatMessages');
const welcomeMessage = document.getElementById('welcomeMessage'); 

var ws = new WebSocket('ws://localhost:7777'); 

ws.onopen = function () {
    console.log('WebSocket 连接已经建立。');
};

ws.onmessage = function (event) {
    console.log('收到服务器消息：', event.data);
    const replyMessage = document.createElement('div');
    replyMessage.textContent = event.data;
    replyMessage.classList.add('message-received');
    chatMessages.appendChild(replyMessage);
};

sendButton.addEventListener('click', (e) => {
    e.preventDefault();
    const message = messageInput.value;
    if (message.trim() !== '') {
        
        const sentMessage = document.createElement('div');
        sentMessage.textContent = message;
        sentMessage.classList.add('message-sent');
        chatMessages.appendChild(sentMessage);

        // 发送消息到服务器
        ws.send(message);

        // 清空输入框
        messageInput.value = '';

        // 隐藏欢迎消息
        welcomeMessage.style.display = 'none';
    }
});

ws.onerror = function (event) {
    console.error('WebSocket 连接出现错误：', event);
};

ws.onclose = function () {
    console.log('WebSocket 连接已经关闭。');
};
