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

var ws = new WebSocket('ws://localhost:8082/echo'); 

const encoder=new TextEncoder();
const decoder=new TextDecoder('UTF-8');
ws.binaryType='arraybuffer';
// 检查 WebSocket 是否处于可以发送消息的状态
function canSend(ws) {
    return ws.readyState === WebSocket.OPEN;
}

// 发送消息的函数
function sendMessage(message) {
    if (canSend(ws)) {
        let encodedData=encoder.encode(message);
        ws.send(encodedData);
        console.log('消息已发送:', message);
    } else {
        console.error('WebSocket 连接未建立或已关闭，无法发送消息。');
        // 这里可以添加重试逻辑或提示用户
    }
}

// 为发送按钮添加事件监听器
sendButton.addEventListener('click', (e) => {
    e.preventDefault();
    const message = messageInput.value.trim();
    if (message) {
        // 发送消息前，先检查 WebSocket 状态
        sendMessage(message);

        // 更新聊天界面
        const sentMessage = document.createElement('div');
        sentMessage.textContent = message;
        sentMessage.classList.add('message-sent');
        chatMessages.appendChild(sentMessage);

        // 清空输入框和隐藏欢迎消息
        messageInput.value = '';
        welcomeMessage.style.display = 'none';
    } else {
        console.log('未输入消息或输入内容为空。');
    }
});
    

ws.onopen = function () {
    console.log('WebSocket 连接已经建立。');
};

ws.onmessage = function (event) {
    if (event.data instanceof Blob) {
        const reader = new FileReader();
        reader.onload = function() {
            const text = reader.result;
            console.log('Blob as text:', text);
            const replyMessage = document.createElement('div');
            replyMessage.textContent = text;
            replyMessage.classList.add('message-received');
            chatMessages.appendChild(replyMessage);
        };
        reader.readAsText(event.data); // 默认使用UTF-8编码读取
    } else {
        try {
            let decodedMessage = decoder.decode(event.data, {stream: true}); // 使用stream:true参数
            const replyMessage = document.createElement('div');
            replyMessage.textContent = decodedMessage;
            replyMessage.classList.add('message-received');
            chatMessages.appendChild(replyMessage);
        } catch (error) {
            console.error('Error decoding message:', error);
        }
    }
};
ws.onclose = function () {
    console.log('WebSocket 连接已经关闭。正在尝试重新连接...');
    // 重新设置事件监听器
    ws.onopen = function () { /* ... */ };
    ws.onmessage = function (event) { /* ... */ };
    ws.onerror = function (event) { /* ... */ };
    setTimeout(function() {
        ws = new WebSocket('ws://localhost:8082/echo');
    }, 1000); // 例如，1秒后重连
};

ws.onerror = function (event) {
    console.error('WebSocket 连接出现错误：', event);
};

ws.onclose = function () {
    console.log('WebSocket 连接已经关闭。');
};

