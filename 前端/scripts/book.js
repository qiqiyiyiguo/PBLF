function clickAfter(who) { 
    if (who === '个人中心') {
        checkLogin();
    } else if (who === '文创产品') {
        window.location.href = "./goods.html";
    } else if (who === '校园风景') {
        window.location.href = "./view.html";
    } else if (who === '预约管理') {
        recheckLogin();
    } else if (who === '首页') {
        window.location.href = "./index.html";
    }
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


const isLeapYear = (year) => {
    return (
        (year % 4 === 0 && year % 100 !== 0 && year % 400 !== 0) ||
        (year % 100 === 0 && year % 400 === 0)
    );
};

const getFebDays = (year) => {
    return isLeapYear(year) ? 29 : 28;
};

let calendar = document.querySelector('.calendar');

const month_names = [
    'January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December',
];

let month_picker = document.querySelector('#month-picker');
const dayTextFormate = document.querySelector('.day-text-formate');
const timeFormate = document.querySelector('.campus-choose');
const dateFormate = document.querySelector('.submitbutton');

month_picker.onclick = () => {
    month_list.classList.remove('hideonce');
    month_list.classList.remove('hide');
    month_list.classList.add('show');
    dayTextFormate.classList.remove('showtime');
    dayTextFormate.classList.add('hidetime');
    timeFormate.classList.remove('showtime');
    timeFormate.classList.add('hideTime');
    dateFormate.classList.remove('showtime');
    dateFormate.classList.add('hideTime');
};

// 定义一个全局变量来存储选中的日期
let selectedDateString = '';

const generateCalendar = (month, year) => {
    let calendar_days = document.querySelector('.calendar-days');
    calendar_days.innerHTML = '';

    let calendar_header_year = document.querySelector('#year');
    let days_of_month = [
        31, getFebDays(year), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
    ];

    let currentDate = new Date();
    let today = new Date();
    today.setHours(0, 0, 0, 0); // 设置为今天的0点，以便进行日期比较
    const disableDate = new Date(2026, 0, 1); // 禁用2026年1月1日及之后的日期

    month_picker.innerHTML = month_names[month];
    calendar_header_year.innerHTML = year;

    let first_day = new Date(year, month);

    let selectedDay = null; // 跟踪选中的日期
    let selectedDayOriginalColor = ''; // 保存选中日期的原始颜色

    for (let i = 0; i < days_of_month[month] + first_day.getDay(); i++) {
        let day = document.createElement('div');

        if (i >= first_day.getDay()) {
            let dayOfMonth = i - first_day.getDay() + 1;
            day.innerHTML = dayOfMonth;

            let currentDay = new Date(year, month, dayOfMonth);

            // 禁用今天之前的日期，并将其颜色设置为灰色
            if (currentDay < today || currentDay >= disableDate) {
                day.classList.add('disabled');
                day.style.color = 'gray'; // 设置日期为灰色
            }

            // 高亮今天的日期
            if (dayOfMonth === currentDate.getDate() && year === currentDate.getFullYear() && month === currentDate.getMonth()) {
                day.classList.add('current-date');
            }

            // 添加点击事件，选择日期
            day.onclick = () => {
                if (!day.classList.contains('disabled')) {
                    // 如果点击的是已经选中的日期，取消选中
                    if (selectedDay === day) {
                        // 恢复原来的背景颜色和文字颜色
                        day.style.backgroundColor = ''; // 恢复背景颜色
                        day.style.color = selectedDayOriginalColor; // 恢复文字颜色
                        selectedDay = null; // 清空选中日期
                    } else {
                        // 如果已有日期被选中，恢复背景颜色和文字颜色
                        if (selectedDay) {
                            selectedDay.style.backgroundColor = ''; // 恢复背景颜色
                            selectedDay.style.color = selectedDayOriginalColor; // 恢复文字颜色
                        }

                        // 保存选中日期的原始文字颜色
                        selectedDayOriginalColor = day.style.color || '';

                        // 设置选中日期的背景颜色为紫色
                        day.style.backgroundColor = 'purple';
                        day.style.color = 'white'; // 改变文字颜色为白色
                        day.style.borderRadius='20%';
                        // 更新选中的日期
                        selectedDay = day;
                    }

                    // 将选中的日期信息赋值给全局变量
                    selectedDateString = `${month + 1} ${dayOfMonth} ${year}`;

                }
            };
        }

        calendar_days.appendChild(day);
    }
};



let month_list = calendar.querySelector('.month-list');
month_names.forEach((e, index) => {
    let month = document.createElement('div');
    month.innerHTML = `<div class="${e}">${e}</div>`;
    month_list.append(month);
    month.onclick = () => {
        currentMonth.value = index;
        generateCalendar(currentMonth.value, currentYear.value);
        month_list.classList.replace('show', 'hide');
        dayTextFormate.classList.remove('hideTime');
        dayTextFormate.classList.add('showtime');
        timeFormate.classList.remove('hideTime');
        timeFormate.classList.add('showtime');
        dateFormate.classList.remove('hideTime');
        dateFormate.classList.add('showtime');
    };
});

(function () {
    month_list.classList.add('hideonce');
})();

document.querySelector('#pre-year').onclick = () => {
    --currentYear.value;
    generateCalendar(currentMonth.value, currentYear.value);
};

document.querySelector('#next-year').onclick = () => {
    ++currentYear.value;
    generateCalendar(currentMonth.value, currentYear.value);
};

let currentDate = new Date();
let currentMonth = { value: currentDate.getMonth() };
let currentYear = { value: currentDate.getFullYear() };

generateCalendar(currentMonth.value, currentYear.value);

const todayShowTime = document.querySelector('.time-formate');
const todayShowDate = document.querySelector('.date-formate');

const currshowDate = new Date();
const showCurrentDateOption = {
    year: 'numeric',
    month: 'long',
    day: 'numeric',
    weekday: 'long',
};

const currentDateFormate = new Intl.DateTimeFormat(
    'en-US',
    showCurrentDateOption
).format(currshowDate);



setInterval(() => {
    const timer = new Date();
    const option = {
        hour: 'numeric',
        minute: 'numeric',
        second: 'numeric',
    };
    const formateTimer = new Intl.DateTimeFormat('en-us', option).format(timer);
    let time = `${`${timer.getHours()}`.padStart(
        2,
        '0'
    )}:${`${timer.getMinutes()}`.padStart(
        2,
        '0'
    )}: ${`${timer.getSeconds()}`.padStart(2, '0')}`;
    
}, 1000);

// 全局变量用于存储起始时间和终止时间
let startTime = null;
let endTime = null;
let time ='';

// 获取所有时间选择元素
const timeElements = document.querySelectorAll('.time');

// 绑定点击事件，选择时间并高亮显示
timeElements.forEach(timeElement => {
    timeElement.addEventListener('click', (e) => {
        const selectedTime = e.target.innerText;  // 获取选中的时间

        if (!startTime) {
            // 选择起始时间
            startTime = selectedTime;
            e.target.classList.add('timeselected');
        } else if (!endTime && selectedTime >= startTime) {
            // 选择终止时间
            endTime = selectedTime;
            e.target.classList.add('timeselected');
            highlightTimeRange(startTime, endTime);
        } else {
            // 如果已选择终止时间或选择的时间无效（如终止时间早于起始时间），则重置
            resetSelection();
            startTime = selectedTime;
            e.target.classList.add('timeselected');
        }
    });
});

// 高亮显示起始时间和终止时间之间的时间段
function highlightTimeRange(startTime, endTime) {
    let startIndex = getTimeIndex(startTime);
    let endIndex = getTimeIndex(endTime);

    // 确保起始时间在终止时间之前
    if (startIndex > endIndex) {
        [startIndex, endIndex] = [endIndex, startIndex];
    }

    // 遍历并高亮显示时间段
    for (let i = startIndex; i <= endIndex; i++) {
        timeElements[i].classList.add('highlighted');
    }
}

// 获取时间的索引位置（09:00 => 0, 10:00 => 1, ..., 18:00 => 9）
function getTimeIndex(time) {
    const times = ["09:00", "10:00", "11:00", "12:00", "13:00", "14:00", "15:00", "16:00", "17:00", "18:00"];
    return times.indexOf(time);
}

// 重置选中状态
function resetSelection() {
    timeElements.forEach(time => {
        time.classList.remove('timeselected', 'highlighted');
    });
    startTime = endTime = null;
}

// 定义一个全局变量储存校区信息
// 获取按钮
const btn1 = document.getElementById('btn1');
const btn2 = document.getElementById('btn2');

// 全局变量记录最后选择的按钮类名
let campus = '';

// 添加事件监听器
btn1.addEventListener('click', () => handleButtonClick(btn1));
btn2.addEventListener('click', () => handleButtonClick(btn2));

function handleButtonClick(button) {
  // 检查当前按钮是否已经被选中
  const isSelected = button.classList.contains('selectedCampus');
  btn1.classList.remove('selectedCampus');
btn2.classList.remove('selectedCampus');
  
  if (isSelected) {
    // 如果当前按钮已经选中，取消选中状态
    button.classList.remove('selectedCampus');
    campus = ''; // 清除记录
  } else {
    // 如果当前按钮未选中，设置为选中状态
    button.classList.add('selectedCampus');
    campus = button.className; // 记录当前按钮的类名
  }
}




function upData(){
    time = `${startTime} ${endTime}`
    console.log(campus,time,selectedDateString);
    if(campus === '' || time === '' || startTime=== null||endTime===null|| selectedDateString === ''){
        alert("请完整输入预约数据");
    }else{
// 将数据转换为json格式
const jsonData = JSON.stringify({"campus":campus,"time":time,"date":selectedDateString,"task":'appoint'})
var ws = new WebSocket('http://localhost:8080/echo');
ws.onopen = function() {
   console.log('WebSocket 连接已经建立。');
   ws.send(jsonData);
};
ws.onmessage = function(event) {
   console.log('收到服务器消息：', event.data);
   const receiverdData = event.data;
   if(receiverdData==='success'){
    alert("您已成功预约，点击确定跳转主页");
    window.location.href = './index.html';
}else{
    alert(receiverdData);
}
};
ws.onerror = function(event) {
   console.error('WebSocket 连接出现错误：', event);
};
ws.onclose = function() {
   console.log('WebSocket 连接已经关闭。');
};


console.log("本次提交数据：",jsonData);
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