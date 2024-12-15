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



 
 //添加数据
 function JsonAdd()
 {
    // 创建XMLHttpRequest对象
    const xhr = new XMLHttpRequest();
    // 设置POST请求，并指定后端接收数据的URL
    xhr.open("POST", "http://127.0.0.1:5500/%E6%B8%B8%E5%AE%A2%E7%AE%A1%E7%90%86%E7%B3%BB%E7%BB%9F.c", true);
    // 设置请求头，指定发送的数据类型为JSON
    xhr.setRequestHeader("Content-Type", "application/json");

    // 获取输入框中的数据
    const name = document.getElementById('Name').value;
    const phonenumber = document.getElementById('PhoneNumber').value;
    const password = document.getElementById('Password').value;

    // 将数据转换为json格式
    const jsonData = JSON.stringify({"name": name, "phonenumber": phonenumber, "password": password, "task": 'add'});
    console.log("本次提交数据：", jsonData);

    // 处理响应数据
    xhr.onreadystatechange = function () {
        if (xhr.readyState === 4) { // 请求完成
            if (xhr.status === 200) {
                // 后端返回200状态码，处理成功情况
                console.log("请求成功，服务器响应：", xhr.responseText);
                // 这里可以添加更多的成功处理逻辑
            } else {
                // 后端返回非200状态码，处理失败情况
                console.error('请求失败，状态码：', xhr.status, '，响应：', xhr.responseText);
                // 这里可以添加更多的失败处理逻辑
            }
        }
    };

    // 发送请求，并将JSON数据作为请求体发送给后端
    xhr.send(jsonData);
}

 //注销账户
 function JsonDelete() {
    // 创建XMLHttpRequest对象
    const xhr = new XMLHttpRequest();
    // 设置POST请求，并指定后端接收数据的URL
    xhr.open("POST", "../../游客管理系统.c", true);
    // 设置请求头，指定发送的数据类型为JSON
    xhr.setRequestHeader("Content-Type", "application/json");

    // 获取输入框中的数据
    const name = document.getElementById('Name').value;
    const phonenumber = document.getElementById('PhoneNumber').value;
    const password = document.getElementById('Password').value;

    // 将数据转换为json格式
    const jsonData = JSON.stringify({"name": name, "phonenumber": phonenumber, "password": password, "task": 'delete'});
    console.log("本次提交数据：", jsonData);

    // 处理响应数据
    xhr.onreadystatechange = function () {
        if (xhr.readyState === 4) { // 请求完成
            if (xhr.status === 200) {
                // 后端返回200状态码，处理成功情况
                console.log("请求成功，服务器响应：", xhr.responseText);
                // 这里可以添加更多的成功处理逻辑
            } else {
                // 后端返回非200状态码，处理失败情况
                console.error('请求失败，状态码：', xhr.status, '，响应：', xhr.responseText);
                // 这里可以添加更多的失败处理逻辑
            }
        }
    };

    // 发送请求，并将JSON数据作为请求体发送给后端
    xhr.send(jsonData);
}



// 这是一个get请求，用来将用户输入的数据传回后端并让后端判断是否输入了正确的账号密码
    function JsonCheck(){
        // 创建XMLHttpRequest对象
        const xhr = new XMLHttpRequest();
        // 设置POST请求，并指定后端接收数据的URL
        xhr.open("POST", "../../游客管理系统.c", true);
        // 设置请求头，指定发送的数据类型为JSON
        xhr.setRequestHeader("Content-Type", "application/json");
    
        // 获取输入框中的数据
        const name = document.getElementById('Name').value;
        const phonenumber = document.getElementById('PhoneNumber').value;
        const password = document.getElementById('Password').value;
    
        // 将数据转换为json格式
        const jsonData = JSON.stringify({"name": name, "phonenumber": phonenumber, "password": password, "task": 'check'});
        console.log("本次提交数据：", jsonData);
    
        // 处理响应数据
        xhr.onreadystatechange = function () {
            if (xhr.readyState === 4) { // 请求完成
                if (xhr.status === 200) {
                    // 后端返回200状态码，处理成功情况
                    console.log("请求成功，服务器响应：", xhr.responseText);
                    // 这里可以添加更多的成功处理逻辑
                } else {
                    // 后端返回非200状态码，处理失败情况
                    console.error('请求失败，状态码：', xhr.status, '，响应：', xhr.responseText);
                    // 这里可以添加更多的失败处理逻辑
                }
            }
        };
    
        // 发送请求，并将JSON数据作为请求体发送给后端
        xhr.send(jsonData);
    }
