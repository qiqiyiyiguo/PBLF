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
    else if(who==='play'){
        startMove();
    }
}



function setAnimationDuration(element, duration) {
    element.style.animationDuration = `${duration}s`;
  }
  const element = document.querySelector('.content');
  setAnimationDuration(element, 22); 



// 控制动画开始和暂停
function startMove() {
    let elem = document.querySelector('.content');

    // 如果动画是暂停的，那么开始动画
    if (elem.style['animationPlayState'] === 'paused' || elem.style['animationPlayState'] === '') {
        elem.style['animationPlayState'] = 'running';
        
        // 在开始动画后2秒自动暂停动画
        setTimeout(() => {
            elem.style['animationPlayState'] = 'paused';
        }, 2000); // 2秒后暂停动画
    }
}

