function clickAfter(who) {
    if(who==='个人中心'){
        window.location.href = "./login.html"
    }
    else if(who==='文创产品'){
        window.location.href = "./goods.html"
    }
    else if(who==='校园风景'){
        window.location.href = "./view.html"
    }
    else if(who==='预约管理'){
        window.location.href = "./book.html"
    }
    else if(who==='首页'){
        window.location.href = "./index.html"
    }
}

let switchCtn = document.querySelector("#switch-cnt");
    let switchC1 = document.querySelector("#switch-c1");
    let switchC2 = document.querySelector("#switch-c2");
    let switchCircle = document.querySelectorAll(".switch_circle");
    let switchBtn = document.querySelectorAll(".switch-btn");
    let aContainer = document.querySelector("#a-container");
    let bContainer = document.querySelector("#b-container");
    let allButtons = document.querySelectorAll(".submit");

    let getButtons = (e) => e.preventDefault()
    let changeForm = (e) => {
        // 修改类名
        switchCtn.classList.add("is-gx");
        setTimeout(function () {
            switchCtn.classList.remove("is-gx");
        }, 1500)
        switchCtn.classList.toggle("is-txr");
        switchCircle[0].classList.toggle("is-txr");
        switchCircle[1].classList.toggle("is-txr");

        switchC1.classList.toggle("is-hidden");
        switchC2.classList.toggle("is-hidden");
        aContainer.classList.toggle("is-txl");
        bContainer.classList.toggle("is-txl");
        bContainer.classList.toggle("is-z");
    }
    // 点击切换
    let shell = (e) => {
        for (var i = 0; i < allButtons.length; i++)
            allButtons[i].addEventListener("click", getButtons);
        for (var i = 0; i < switchBtn.length; i++)
            switchBtn[i].addEventListener("click", changeForm)
    }
    window.addEventListener("load", shell);



function addData(){
     // 获取输入框中的数据
     const name = document.getElementById('Name').value;
     const phonenumber = document.getElementById('PhoneNumber').value;
     const password = document.getElementById('Password').value;
     console.log(name,phonenumber,password);
     // 将数据转换为json格式
     const jsonData = JSON.stringify({"name":name,"phonenumber":phonenumber,"password":password,"task":'register'})
     var ws = new WebSocket('http://localhost:8080/echo');
    ws.onopen = function() {
        console.log('WebSocket 连接已经建立。');
        ws.send(jsonData);
    };
    ws.onmessage = function(event) {
        console.log('收到服务器消息：', event.data);
        const receiverdData = event.data;
    };
    ws.onerror = function(event) {
        console.error('WebSocket 连接出现错误：', event);
    };
    ws.onclose = function() {
        console.log('WebSocket 连接已经关闭。');
    };

     
     console.log("本次提交数据：",jsonData);
}
 
function checkData(){
    // 获取输入框中的数据
    const name = document.getElementById('Name').value;
    const phonenumber = document.getElementById('PhoneNumber').value;
    const password = document.getElementById('Password').value;
    console.log(name,phonenumber,password);
    // 将数据转换为json格式
    const jsonData = JSON.stringify({"name":name,"phonenumber":phonenumber,"password":password,"task":'login'})
    var ws = new WebSocket('http://localhost:8080/echo');
   ws.onopen = function() {
       console.log('WebSocket 连接已经建立。');
       ws.send(jsonData);
   };
   ws.onmessage = function(event) {
       console.log('收到服务器消息：', event.data);
       const receiverdData = event.data;
   };
   ws.onerror = function(event) {
       console.error('WebSocket 连接出现错误：', event);
   };
   ws.onclose = function() {
       console.log('WebSocket 连接已经关闭。');
   };

    
    console.log("本次提交数据：",jsonData);
}

function deleteData(){
    // 获取输入框中的数据
    const name = document.getElementById('Name').value;
    const phonenumber = document.getElementById('PhoneNumber').value;
    const password = document.getElementById('Password').value;
    console.log(name,phonenumber,password);
    // 将数据转换为json格式
    const jsonData = JSON.stringify({"name":name,"phonenumber":phonenumber,"password":password,"task":'delete'})
    var ws = new WebSocket('http://localhost:8080/echo');
   ws.onopen = function() {
       console.log('WebSocket 连接已经建立。');
       ws.send(jsonData);
   };
   ws.onmessage = function(event) {
       console.log('收到服务器消息：', event.data);
       const receiverdData = event.data;
   };
   ws.onerror = function(event) {
       console.error('WebSocket 连接出现错误：', event);
   };
   ws.onclose = function() {
       console.log('WebSocket 连接已经关闭。');
   };

    
    console.log("本次提交数据：",jsonData);
}

    