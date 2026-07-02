import{c as ut,o as yt,r as mt,t as pt}from"./main-DufM469s.js";var i=ut(mt(),1);function et(t){var e,o,a="";if(typeof t=="string"||typeof t=="number")a+=t;else if(typeof t=="object")if(Array.isArray(t)){var r=t.length;for(e=0;e<r;e++)t[e]&&(o=et(t[e]))&&(a&&(a+=" "),a+=o)}else for(o in t)t[o]&&(a&&(a+=" "),a+=o);return a}function $(){for(var t,e,o=0,a="",r=arguments.length;o<r;o++)(t=arguments[o])&&(e=et(t))&&(a&&(a+=" "),a+=e);return a}var D=t=>typeof t=="number"&&!isNaN(t),A=t=>typeof t=="string",N=t=>typeof t=="function",_t=t=>A(t)||D(t),K=t=>A(t)||N(t)?t:null,gt=(t,e)=>t===!1||D(t)&&t>0?t:e,Y=t=>(0,i.isValidElement)(t)||A(t)||N(t)||D(t);function vt(t,e,o=300){let{scrollHeight:a,style:r}=t;requestAnimationFrame(()=>{r.minHeight="initial",r.height=a+"px",r.transition=`all ${o}ms`,requestAnimationFrame(()=>{r.height="0",r.padding="0",r.margin="0",setTimeout(e,o)})})}function X({enter:t,exit:e,appendPosition:o=!1,collapse:a=!0,collapseDuration:r=300}){return function({children:f,position:s,preventExitTransition:u,done:c,nodeRef:m,isIn:v,playToast:x}){let w=o?`${t}--${s}`:t,k=o?`${e}--${s}`:e,I=(0,i.useRef)(0);return(0,i.useLayoutEffect)(()=>{let T=m.current,g=w.split(" "),p=n=>{n.target===m.current&&(x(),T.removeEventListener("animationend",p),T.removeEventListener("animationcancel",p),I.current===0&&n.type!=="animationcancel"&&T.classList.remove(...g))};T.classList.add(...g),T.addEventListener("animationend",p),T.addEventListener("animationcancel",p)},[]),(0,i.useEffect)(()=>{let T=m.current,g=()=>{T.removeEventListener("animationend",g),a?vt(T,c,r):c()};v||(u?g():(I.current=1,T.className+=` ${k}`,T.addEventListener("animationend",g)))},[v]),i.createElement(i.Fragment,null,f)}}function Z(t,e){return{content:ot(t.content,t.props),containerId:t.props.containerId,id:t.props.toastId,theme:t.props.theme,type:t.props.type,data:t.props.data||{},isLoading:t.props.isLoading,icon:t.props.icon,reason:t.removalReason,status:e}}function ot(t,e,o=!1){return(0,i.isValidElement)(t)&&!A(t.type)?(0,i.cloneElement)(t,{closeToast:e.closeToast,toastProps:e,data:e.data,isPaused:o}):N(t)?t({closeToast:e.closeToast,toastProps:e,data:e.data,isPaused:o}):t}function bt({closeToast:t,theme:e,ariaLabel:o="close"}){return i.createElement("button",{className:`Toastify__close-button Toastify__close-button--${e}`,type:"button",onClick:a=>{a.stopPropagation(),t(!0)},"aria-label":o},i.createElement("svg",{"aria-hidden":"true",viewBox:"0 0 14 16"},i.createElement("path",{fillRule:"evenodd",d:"M7.71 8.23l3.75 3.75-1.48 1.48-3.75-3.75-3.75 3.75L1 11.98l3.75-3.75L1 4.48 2.48 3l3.75 3.75L9.98 3l1.48 1.48-3.75 3.75z"})))}function Tt({delay:t,isRunning:e,closeToast:o,type:a="default",hide:r,className:f,controlledProgress:s,progress:u,rtl:c,isIn:m,theme:v}){let x=r||s&&u===0,w={animationDuration:`${t}ms`,animationPlayState:e?"running":"paused"};s&&(w.transform=`scaleX(${u})`);let k=$("Toastify__progress-bar",s?"Toastify__progress-bar--controlled":"Toastify__progress-bar--animated",`Toastify__progress-bar-theme--${v}`,`Toastify__progress-bar--${a}`,{"Toastify__progress-bar--rtl":c}),I=N(f)?f({rtl:c,type:a,defaultClassName:k}):$(k,f),T={[s&&u>=1?"onTransitionEnd":"onAnimationEnd"]:s&&u<1?null:()=>{m&&o()}};return i.createElement("div",{className:"Toastify__progress-bar--wrp","data-hidden":x},i.createElement("div",{className:`Toastify__progress-bar--bg Toastify__progress-bar-theme--${v} Toastify__progress-bar--${a}`}),i.createElement("div",{role:"progressbar","aria-hidden":x?"true":"false","aria-label":"notification timer","aria-valuenow":s?Math.round(u*100):void 0,"aria-valuemin":0,"aria-valuemax":100,className:I,style:w,...T}))}var ht=1,at=()=>`${ht++}`;function xt(t,e,o){let a=1,r=0,f=[],s=[],u=e,c=new Map,m=new Set,v=n=>(m.add(n),()=>m.delete(n)),x=()=>{s=Array.from(c.values()),m.forEach(n=>n())},w=({containerId:n,toastId:l,updateId:d})=>{let h=n?n!==t:t!==1,C=c.has(l)&&d==null;return h||C},k=(n,l)=>{c.forEach(d=>{var h;(l==null||l===d.props.toastId)&&((h=d.toggle)==null||h.call(d,n))})},I=n=>{var l,d;n.isActive&&((d=(l=n.props)==null?void 0:l.onClose)==null||d.call(l,n.removalReason),n.isActive=!1,o(Z(n,"removed")))},T=n=>{if(n==null)c.forEach(I);else{let l=c.get(n);l&&I(l)}x()},g=()=>{r-=f.length,f=[]},p=n=>{var l,d;let{toastId:h,updateId:C}=n.props,y=C==null;n.staleId&&c.delete(n.staleId),n.isActive=!0,c.set(h,n),x(),o(Z(n,y?"added":"updated")),y&&((d=(l=n.props).onOpen)==null||d.call(l))};return{id:t,props:u,observe:v,toggle:k,removeToast:T,toasts:c,clearQueue:g,buildToast:(n,l)=>{if(w(l))return;let{toastId:d,updateId:h,data:C,staleId:y,delay:b}=l,L=h==null;L&&r++;let O={...u,style:u.toastStyle,key:a++,...Object.fromEntries(Object.entries(l).filter(([S,P])=>P!=null)),toastId:d,updateId:h,data:C,isIn:!1,className:K(l.className||u.toastClassName),progressClassName:K(l.progressClassName||u.progressClassName),autoClose:l.isLoading?!1:gt(l.autoClose,u.autoClose),closeToast(S){let P=c.get(d);P&&(P.removalReason=S,T(d))},deleteToast(){if(c.get(d)!=null){if(c.delete(d),r--,r<0&&(r=0),f.length>0){p(f.shift());return}x()}}};O.closeButton=u.closeButton,l.closeButton===!1||Y(l.closeButton)?O.closeButton=l.closeButton:l.closeButton===!0&&(O.closeButton=Y(u.closeButton)?u.closeButton:!0);let z={content:n,props:O,staleId:y};u.limit&&u.limit>0&&r>u.limit&&L?f.push(z):D(b)?setTimeout(()=>{p(z)},b):p(z)},setProps(n){u=n},setToggle:(n,l)=>{let d=c.get(n);d&&(d.toggle=l)},isToastActive:n=>{var l;return(l=c.get(n))==null?void 0:l.isActive},getSnapshot:()=>s}}var E=new Map,R=[],Q=new Set,kt=t=>Q.forEach(e=>e(t)),rt=()=>E.size>0;function wt(){R.forEach(t=>nt(t.content,t.options)),R=[]}var Et=(t,{containerId:e})=>{var o;return(o=E.get(e||1))==null?void 0:o.toasts.get(t)};function st(t,e){var o;if(e)return!!((o=E.get(e))!=null&&o.isToastActive(t));let a=!1;return E.forEach(r=>{r.isToastActive(t)&&(a=!0)}),a}function It(t){if(!rt()){R=R.filter(e=>t!=null&&e.options.toastId!==t);return}if(t==null||_t(t))E.forEach(e=>{e.removeToast(t)});else if(t&&("containerId"in t||"id"in t)){let e=E.get(t.containerId);e?e.removeToast(t.id):E.forEach(o=>{o.removeToast(t.id)})}}var Ct=(t={})=>{E.forEach(e=>{e.props.limit&&(!t.containerId||e.id===t.containerId)&&e.clearQueue()})};function nt(t,e){Y(t)&&(rt()||R.push({content:t,options:e}),E.forEach(o=>{o.buildToast(t,e)}))}function Lt(t){var e;(e=E.get(t.containerId||1))==null||e.setToggle(t.id,t.fn)}function it(t,e){E.forEach(o=>{(e==null||!(e!=null&&e.containerId)||e?.containerId===o.id)&&o.toggle(t,e?.id)})}function Ot(t){let e=t.containerId||1;return{subscribe(o){let a=xt(e,t,kt);E.set(e,a);let r=a.observe(o);return wt(),()=>{r(),E.delete(e)}},setProps(o){var a;(a=E.get(e))==null||a.setProps(o)},getSnapshot(){var o;return(o=E.get(e))==null?void 0:o.getSnapshot()}}}function zt(t){return Q.add(t),()=>{Q.delete(t)}}function Nt(t){return t&&(A(t.toastId)||D(t.toastId))?t.toastId:at()}function M(t,e){return nt(t,e),e.toastId}function H(t,e){return{...e,type:e&&e.type||t,toastId:Nt(e)}}function U(t){return(e,o)=>M(e,H(t,o))}function _(t,e){return M(t,H("default",e))}_.loading=(t,e)=>M(t,H("default",{isLoading:!0,autoClose:!1,closeOnClick:!1,closeButton:!1,draggable:!1,...e}));function Pt(t,{pending:e,error:o,success:a},r){let f;e&&(f=A(e)?_.loading(e,r):_.loading(e.render,{...r,...e}));let s={isLoading:null,autoClose:null,closeOnClick:null,closeButton:null,draggable:null},u=(m,v,x)=>{if(v==null){_.dismiss(f);return}let w={type:m,...s,...r,data:x},k=A(v)?{render:v}:v;return f?_.update(f,{...w,...k}):_(k.render,{...w,...k}),x},c=N(t)?t():t;return c.then(m=>u("success",a,m)).catch(m=>u("error",o,m)),c}_.promise=Pt;_.success=U("success");_.info=U("info");_.error=U("error");_.warning=U("warning");_.warn=_.warning;_.dark=(t,e)=>M(t,H("default",{theme:"dark",...e}));function $t(t){It(t)}_.dismiss=$t;_.clearWaitingQueue=Ct;_.isActive=st;_.update=(t,e={})=>{let o=Et(t,e);if(o){let{props:a,content:r}=o,f={delay:100,...a,...e,toastId:e.toastId||t,updateId:at()};f.toastId!==t&&(f.staleId=t);let s=f.render||r;delete f.render,M(s,f)}};_.done=t=>{_.update(t,{progress:1})};_.onChange=zt;_.play=t=>it(!0,t);_.pause=t=>it(!1,t);function At(t){var e;let{subscribe:o,getSnapshot:a,setProps:r}=(0,i.useRef)(Ot(t)).current;r(t);let f=(e=(0,i.useSyncExternalStore)(o,a,a))==null?void 0:e.slice();function s(u){if(!f)return[];let c=new Map;return t.newestOnTop&&f.reverse(),f.forEach(m=>{let{position:v}=m.props;c.has(v)||c.set(v,[]),c.get(v).push(m)}),Array.from(c,m=>u(m[0],m[1]))}return{getToastToRender:s,isToastActive:st,count:f?.length}}function Rt(t){let[e,o]=(0,i.useState)(!1),[a,r]=(0,i.useState)(!1),f=(0,i.useRef)(null),s=(0,i.useRef)({start:0,delta:0,removalDistance:0,canCloseOnClick:!0,canDrag:!1,didMove:!1}).current,{autoClose:u,pauseOnHover:c,closeToast:m,onClick:v,closeOnClick:x}=t;Lt({id:t.toastId,containerId:t.containerId,fn:o}),(0,i.useEffect)(()=>{if(t.pauseOnFocusLoss)return w(),()=>{k()}},[t.pauseOnFocusLoss]);function w(){document.hasFocus()||p(),window.addEventListener("focus",g),window.addEventListener("blur",p)}function k(){window.removeEventListener("focus",g),window.removeEventListener("blur",p)}function I(y){if(t.draggable===!0||t.draggable===y.pointerType){n();let b=f.current;s.canCloseOnClick=!0,s.canDrag=!0,b.style.transition="none",t.draggableDirection==="x"?(s.start=y.clientX,s.removalDistance=b.offsetWidth*(t.draggablePercent/100)):(s.start=y.clientY,s.removalDistance=b.offsetHeight*(t.draggablePercent===80?t.draggablePercent*1.5:t.draggablePercent)/100)}}function T(y){let{top:b,bottom:L,left:O,right:z}=f.current.getBoundingClientRect();y.pointerType==="mouse"&&t.pauseOnHover&&y.clientX>=O&&y.clientX<=z&&y.clientY>=b&&y.clientY<=L?p():g()}function g(){o(!0)}function p(){o(!1)}function n(){s.didMove=!1,document.addEventListener("pointermove",d),document.addEventListener("pointerup",h)}function l(){document.removeEventListener("pointermove",d),document.removeEventListener("pointerup",h)}function d(y){let b=f.current;if(s.canDrag&&b){s.didMove=!0,e&&p(),t.draggableDirection==="x"?s.delta=y.clientX-s.start:s.delta=y.clientY-s.start,s.start!==y.clientX&&(s.canCloseOnClick=!1);let L=t.draggableDirection==="x"?`${s.delta}px, var(--y)`:`0, calc(${s.delta}px + var(--y))`;b.style.transform=`translate3d(${L},0)`,b.style.opacity=`${1-Math.abs(s.delta/s.removalDistance)}`}}function h(){l();let y=f.current;if(s.canDrag&&s.didMove&&y){if(s.canDrag=!1,Math.abs(s.delta)>s.removalDistance){r(!0),t.closeToast(!0),t.collapseAll();return}y.style.transition="transform 0.2s, opacity 0.2s",y.style.removeProperty("transform"),y.style.removeProperty("opacity")}}let C={onPointerDown:I,onPointerUp:T};return u&&c&&(C.onMouseEnter=p,t.stacked||(C.onMouseLeave=g)),x&&(C.onClick=y=>{v&&v(y),s.canCloseOnClick&&m(!0)}),{playToast:g,pauseToast:p,isRunning:e,preventExitTransition:a,toastRef:f,eventHandlers:C}}var lt=typeof window<"u"?i.useLayoutEffect:i.useEffect,j=({theme:t,type:e,isLoading:o,...a})=>i.createElement("svg",{viewBox:"0 0 24 24",width:"100%",height:"100%",fill:t==="colored"?"currentColor":`var(--toastify-icon-color-${e})`,...a});function Dt(t){return i.createElement(j,{...t},i.createElement("path",{d:"M23.32 17.191L15.438 2.184C14.728.833 13.416 0 11.996 0c-1.42 0-2.733.833-3.443 2.184L.533 17.448a4.744 4.744 0 000 4.368C1.243 23.167 2.555 24 3.975 24h16.05C22.22 24 24 22.044 24 19.632c0-.904-.251-1.746-.68-2.44zm-9.622 1.46c0 1.033-.724 1.823-1.698 1.823s-1.698-.79-1.698-1.822v-.043c0-1.028.724-1.822 1.698-1.822s1.698.79 1.698 1.822v.043zm.039-12.285l-.84 8.06c-.057.581-.408.943-.897.943-.49 0-.84-.367-.896-.942l-.84-8.065c-.057-.624.25-1.095.779-1.095h1.91c.528.005.84.476.784 1.1z"}))}function Mt(t){return i.createElement(j,{...t},i.createElement("path",{d:"M12 0a12 12 0 1012 12A12.013 12.013 0 0012 0zm.25 5a1.5 1.5 0 11-1.5 1.5 1.5 1.5 0 011.5-1.5zm2.25 13.5h-4a1 1 0 010-2h.75a.25.25 0 00.25-.25v-4.5a.25.25 0 00-.25-.25h-.75a1 1 0 010-2h1a2 2 0 012 2v4.75a.25.25 0 00.25.25h.75a1 1 0 110 2z"}))}function St(t){return i.createElement(j,{...t},i.createElement("path",{d:"M12 0a12 12 0 1012 12A12.014 12.014 0 0012 0zm6.927 8.2l-6.845 9.289a1.011 1.011 0 01-1.43.188l-4.888-3.908a1 1 0 111.25-1.562l4.076 3.261 6.227-8.451a1 1 0 111.61 1.183z"}))}function Bt(t){return i.createElement(j,{...t},i.createElement("path",{d:"M11.983 0a12.206 12.206 0 00-8.51 3.653A11.8 11.8 0 000 12.207 11.779 11.779 0 0011.8 24h.214A12.111 12.111 0 0024 11.791 11.766 11.766 0 0011.983 0zM10.5 16.542a1.476 1.476 0 011.449-1.53h.027a1.527 1.527 0 011.523 1.47 1.475 1.475 0 01-1.449 1.53h-.027a1.529 1.529 0 01-1.523-1.47zM11 12.5v-6a1 1 0 012 0v6a1 1 0 11-2 0z"}))}function Xt(){return i.createElement("div",{className:"Toastify__spinner"})}var V={info:Mt,warning:Dt,success:St,error:Bt,spinner:Xt},Ht=t=>t in V;function Ut({theme:t,type:e,isLoading:o,icon:a}){let r=null,f={theme:t,type:e};return a===!1||(N(a)?r=a({...f,isLoading:o}):(0,i.isValidElement)(a)?r=(0,i.cloneElement)(a,f):o?r=V.spinner():Ht(e)&&(r=V[e](f))),r}var jt=t=>{let{isRunning:e,preventExitTransition:o,toastRef:a,eventHandlers:r,playToast:f}=Rt(t),{closeButton:s,children:u,autoClose:c,onClick:m,type:v,hideProgressBar:x,closeToast:w,transition:k,position:I,className:T,style:g,progressClassName:p,updateId:n,role:l,progress:d,rtl:h,toastId:C,deleteToast:y,isIn:b,isLoading:L,closeOnClick:O,theme:z,ariaLabel:S}=t,P=$("Toastify__toast",`Toastify__toast-theme--${z}`,`Toastify__toast--${v}`,{"Toastify__toast--rtl":h},{"Toastify__toast--close-on-click":O}),dt=N(T)?T({rtl:h,position:I,type:v,defaultClassName:P}):$(P,T),G=Ut(t),J=!!d||!c,q={closeToast:w,type:v,theme:z},B=null;return s===!1||(N(s)?B=s(q):(0,i.isValidElement)(s)?B=(0,i.cloneElement)(s,q):B=bt(q)),i.createElement(k,{isIn:b,done:y,position:I,preventExitTransition:o,nodeRef:a,playToast:f},i.createElement("div",{id:C,tabIndex:0,onClick:m,"data-in":b,className:dt,...r,style:g,ref:a,...b&&{role:l,"aria-label":S}},G!=null&&i.createElement("div",{className:$("Toastify__toast-icon",{"Toastify--animate-icon Toastify__zoom-enter":!L})},G),ot(u,t,!e),B,!t.customProgressBar&&i.createElement(Tt,{...n&&!J?{key:`p-${n}`}:{},rtl:h,theme:z,delay:c,isRunning:e,isIn:b,closeToast:w,hide:x,type:v,className:p,controlledProgress:J,progress:d||0})))},F=(t,e=!1)=>({enter:`Toastify--animate Toastify__${t}-enter`,exit:`Toastify--animate Toastify__${t}-exit`,appendPosition:e}),ft=X(F("bounce",!0)),te=X(F("slide",!0)),ee=X(F("zoom")),oe=X(F("flip")),Ft={position:"top-right",transition:ft,autoClose:5e3,closeButton:!0,pauseOnHover:!0,pauseOnFocusLoss:!0,draggable:"touch",draggablePercent:80,draggableDirection:"x",role:"alert",theme:"light","aria-label":"Notifications Alt+T",hotKeys:t=>t.altKey&&t.code==="KeyT"};function qt(t){let e={...Ft,...t},o=t.stacked,[a,r]=(0,i.useState)(!0),f=(0,i.useRef)(null),{getToastToRender:s,isToastActive:u,count:c}=At(e),{className:m,style:v,rtl:x,containerId:w,hotKeys:k}=e;function I(g){let p=$("Toastify__toast-container",`Toastify__toast-container--${g}`,{"Toastify__toast-container--rtl":x});return N(m)?m({position:g,rtl:x,defaultClassName:p}):$(p,K(m))}function T(){o&&(r(!0),_.play())}return lt(()=>{var g;if(o){let p=f.current.querySelectorAll('[data-in="true"]'),n=12,l=(g=e.position)==null?void 0:g.includes("top"),d=0,h=0;Array.from(p).reverse().forEach((C,y)=>{let b=C;b.classList.add("Toastify__toast--stacked"),y>0&&(b.dataset.collapsed=`${a}`),b.dataset.pos||(b.dataset.pos=l?"top":"bot");let L=d*(a?.2:1)+(a?0:n*y),O=Math.max(.5,1-(a?h:0));b.style.setProperty("--y",`${l?L:L*-1}px`),b.style.setProperty("--g",`${n}`),b.style.setProperty("--s",`${O}`),d+=b.offsetHeight,h+=.025})}},[a,c,o]),(0,i.useEffect)(()=>{function g(p){var n;let l=f.current;k(p)&&((n=l?.querySelector('[tabIndex="0"]'))==null||n.focus(),r(!1),_.pause()),p.key==="Escape"&&(document.activeElement===l||l!=null&&l.contains(document.activeElement))&&(r(!0),_.play())}return document.addEventListener("keydown",g),()=>{document.removeEventListener("keydown",g)}},[k]),i.createElement("section",{ref:f,className:"Toastify",id:w,onMouseEnter:()=>{o&&(r(!1),_.pause())},onMouseLeave:T,"aria-live":"polite","aria-atomic":"false","aria-relevant":"additions text","aria-label":e["aria-label"]},s((g,p)=>{let n=p.length?{...v}:{...v,pointerEvents:"none"};return i.createElement("div",{tabIndex:-1,className:I(g),"data-stacked":o,style:n,key:`c-${g}`},p.map(({content:l,props:d})=>i.createElement(jt,{...d,stacked:o,collapseAll:T,isIn:u(d.toastId,d.containerId),key:`t-${d.key}`},l)))}))}var Kt=`:root {
  --toastify-color-light: #fff;
  --toastify-color-dark: #121212;
  --toastify-color-info: #3498db;
  --toastify-color-success: #07bc0c;
  --toastify-color-warning: #f1c40f;
  --toastify-color-error: hsl(6, 78%, 57%);
  --toastify-color-transparent: rgba(255, 255, 255, 0.7);

  --toastify-icon-color-info: var(--toastify-color-info);
  --toastify-icon-color-success: var(--toastify-color-success);
  --toastify-icon-color-warning: var(--toastify-color-warning);
  --toastify-icon-color-error: var(--toastify-color-error);

  --toastify-container-width: fit-content;
  --toastify-toast-width: 320px;
  --toastify-toast-offset: 16px;
  --toastify-toast-top: max(var(--toastify-toast-offset), env(safe-area-inset-top));
  --toastify-toast-right: max(var(--toastify-toast-offset), env(safe-area-inset-right));
  --toastify-toast-left: max(var(--toastify-toast-offset), env(safe-area-inset-left));
  --toastify-toast-bottom: max(var(--toastify-toast-offset), env(safe-area-inset-bottom));
  --toastify-toast-background: #fff;
  --toastify-toast-padding: 14px;
  --toastify-toast-min-height: 64px;
  --toastify-toast-max-height: 800px;
  --toastify-toast-bd-radius: 6px;
  --toastify-toast-shadow: 0px 4px 12px rgba(0, 0, 0, 0.1);
  --toastify-font-family: sans-serif;
  --toastify-z-index: 9999;
  --toastify-text-color-light: #757575;
  --toastify-text-color-dark: #fff;

  /* Used only for colored theme */
  --toastify-text-color-info: #fff;
  --toastify-text-color-success: #fff;
  --toastify-text-color-warning: #fff;
  --toastify-text-color-error: #fff;

  --toastify-spinner-color: #616161;
  --toastify-spinner-color-empty-area: #e0e0e0;
  --toastify-color-progress-light: linear-gradient(to right, #4cd964, #5ac8fa, #007aff, #34aadc, #5856d6, #ff2d55);
  --toastify-color-progress-dark: #bb86fc;
  --toastify-color-progress-info: var(--toastify-color-info);
  --toastify-color-progress-success: var(--toastify-color-success);
  --toastify-color-progress-warning: var(--toastify-color-warning);
  --toastify-color-progress-error: var(--toastify-color-error);
  /* used to control the opacity of the progress trail */
  --toastify-color-progress-bgo: 0.2;
}

.Toastify__toast-container {
  z-index: var(--toastify-z-index);
  -webkit-transform: translate3d(0, 0, var(--toastify-z-index));
  position: fixed;
  width: var(--toastify-container-width);
  box-sizing: border-box;
  color: #fff;
  display: flex;
  flex-direction: column;
}

.Toastify__toast-container--top-left {
  top: var(--toastify-toast-top);
  left: var(--toastify-toast-left);
}
.Toastify__toast-container--top-center {
  top: var(--toastify-toast-top);
  left: 50%;
  transform: translateX(-50%);
  align-items: center;
}
.Toastify__toast-container--top-right {
  top: var(--toastify-toast-top);
  right: var(--toastify-toast-right);
  align-items: end;
}
.Toastify__toast-container--bottom-left {
  bottom: var(--toastify-toast-bottom);
  left: var(--toastify-toast-left);
}
.Toastify__toast-container--bottom-center {
  bottom: var(--toastify-toast-bottom);
  left: 50%;
  transform: translateX(-50%);
  align-items: center;
}
.Toastify__toast-container--bottom-right {
  bottom: var(--toastify-toast-bottom);
  right: var(--toastify-toast-right);
  align-items: end;
}

.Toastify__toast {
  --y: 0px;
  position: relative;
  touch-action: none;
  width: var(--toastify-toast-width);
  min-height: var(--toastify-toast-min-height);
  box-sizing: border-box;
  margin-bottom: 1rem;
  padding: var(--toastify-toast-padding);
  border-radius: var(--toastify-toast-bd-radius);
  box-shadow: var(--toastify-toast-shadow);
  max-height: var(--toastify-toast-max-height);
  font-family: var(--toastify-font-family);
  /* webkit only issue #791 */
  z-index: 0;
  /* inner swag */
  display: flex;
  flex: 1 auto;
  align-items: center;
  word-break: break-word;
}

@media only screen and (max-width: 480px) {
  .Toastify__toast-container {
    width: 100vw;
    left: env(safe-area-inset-left);
    margin: 0;
  }
  .Toastify__toast-container--top-left,
  .Toastify__toast-container--top-center,
  .Toastify__toast-container--top-right {
    top: env(safe-area-inset-top);
    transform: translateX(0);
  }
  .Toastify__toast-container--bottom-left,
  .Toastify__toast-container--bottom-center,
  .Toastify__toast-container--bottom-right {
    bottom: env(safe-area-inset-bottom);
    transform: translateX(0);
  }
  .Toastify__toast-container--rtl {
    right: env(safe-area-inset-right);
    left: initial;
  }
  .Toastify__toast {
    --toastify-toast-width: 100%;
    margin-bottom: 0;
    border-radius: 0;
  }
}

.Toastify__toast-container[data-stacked='true'] {
  width: var(--toastify-toast-width);
}

@media only screen and (max-width: 480px) {
  .Toastify__toast-container[data-stacked='true'] {
    width: 100vw;
  }
}

.Toastify__toast--stacked {
  position: absolute;
  width: 100%;
  transform: translate3d(0, var(--y), 0) scale(var(--s));
  transition: transform 0.3s;
}

.Toastify__toast--stacked[data-collapsed] .Toastify__toast-body,
.Toastify__toast--stacked[data-collapsed] .Toastify__close-button {
  transition: opacity 0.1s;
}

.Toastify__toast--stacked[data-collapsed='false'] {
  overflow: visible;
}

.Toastify__toast--stacked[data-collapsed='true']:not(:last-child) > * {
  opacity: 0;
}

.Toastify__toast--stacked:after {
  content: '';
  position: absolute;
  left: 0;
  right: 0;
  height: calc(var(--g) * 1px);
  bottom: 100%;
}

.Toastify__toast--stacked[data-pos='top'] {
  top: 0;
}

.Toastify__toast--stacked[data-pos='bot'] {
  bottom: 0;
}

.Toastify__toast--stacked[data-pos='bot'].Toastify__toast--stacked:before {
  transform-origin: top;
}

.Toastify__toast--stacked[data-pos='top'].Toastify__toast--stacked:before {
  transform-origin: bottom;
}

.Toastify__toast--stacked:before {
  content: '';
  position: absolute;
  left: 0;
  right: 0;
  bottom: 0;
  height: 100%;
  transform: scaleY(3);
  z-index: -1;
}

.Toastify__toast--rtl {
  direction: rtl;
}

.Toastify__toast--close-on-click {
  cursor: pointer;
}

.Toastify__toast-icon {
  margin-inline-end: 10px;
  width: 22px;
  flex-shrink: 0;
  display: flex;
}

.Toastify--animate {
  animation-fill-mode: both;
  animation-duration: 0.5s;
}

.Toastify--animate-icon {
  animation-fill-mode: both;
  animation-duration: 0.3s;
}

.Toastify__toast-theme--dark {
  background: var(--toastify-color-dark);
  color: var(--toastify-text-color-dark);
}

.Toastify__toast-theme--light {
  background: var(--toastify-color-light);
  color: var(--toastify-text-color-light);
}

.Toastify__toast-theme--colored.Toastify__toast--default {
  background: var(--toastify-color-light);
  color: var(--toastify-text-color-light);
}

.Toastify__toast-theme--colored.Toastify__toast--info {
  color: var(--toastify-text-color-info);
  background: var(--toastify-color-info);
}

.Toastify__toast-theme--colored.Toastify__toast--success {
  color: var(--toastify-text-color-success);
  background: var(--toastify-color-success);
}

.Toastify__toast-theme--colored.Toastify__toast--warning {
  color: var(--toastify-text-color-warning);
  background: var(--toastify-color-warning);
}

.Toastify__toast-theme--colored.Toastify__toast--error {
  color: var(--toastify-text-color-error);
  background: var(--toastify-color-error);
}

.Toastify__progress-bar-theme--light {
  background: var(--toastify-color-progress-light);
}

.Toastify__progress-bar-theme--dark {
  background: var(--toastify-color-progress-dark);
}

.Toastify__progress-bar--info {
  background: var(--toastify-color-progress-info);
}

.Toastify__progress-bar--success {
  background: var(--toastify-color-progress-success);
}

.Toastify__progress-bar--warning {
  background: var(--toastify-color-progress-warning);
}

.Toastify__progress-bar--error {
  background: var(--toastify-color-progress-error);
}

.Toastify__progress-bar-theme--colored.Toastify__progress-bar--info,
.Toastify__progress-bar-theme--colored.Toastify__progress-bar--success,
.Toastify__progress-bar-theme--colored.Toastify__progress-bar--warning,
.Toastify__progress-bar-theme--colored.Toastify__progress-bar--error {
  background: var(--toastify-color-transparent);
}

.Toastify__close-button {
  color: #fff;
  position: absolute;
  top: 6px;
  right: 6px;
  background: transparent;
  outline: none;
  border: none;
  padding: 0;
  cursor: pointer;
  opacity: 0.7;
  transition: 0.3s ease;
  z-index: 1;
}

.Toastify__toast--rtl .Toastify__close-button {
  left: 6px;
  right: unset;
}

.Toastify__close-button--light {
  color: #000;
  opacity: 0.3;
}

.Toastify__close-button > svg {
  fill: currentColor;
  height: 16px;
  width: 14px;
}

.Toastify__close-button:hover,
.Toastify__close-button:focus {
  opacity: 1;
}

@keyframes Toastify__trackProgress {
  0% {
    transform: scaleX(1);
  }
  100% {
    transform: scaleX(0);
  }
}

.Toastify__progress-bar {
  position: absolute;
  bottom: 0;
  left: 0;
  width: 100%;
  height: 100%;
  z-index: 1;
  opacity: 0.7;
  transform-origin: left;
}

.Toastify__progress-bar--animated {
  animation: Toastify__trackProgress linear 1 forwards;
}

.Toastify__progress-bar--controlled {
  transition: transform 0.2s;
}

.Toastify__progress-bar--rtl {
  right: 0;
  left: initial;
  transform-origin: right;
  border-bottom-left-radius: initial;
}

.Toastify__progress-bar--wrp {
  position: absolute;
  overflow: hidden;
  bottom: 0;
  left: 0;
  width: 100%;
  height: 5px;
  border-bottom-left-radius: var(--toastify-toast-bd-radius);
  border-bottom-right-radius: var(--toastify-toast-bd-radius);
}

.Toastify__progress-bar--wrp[data-hidden='true'] {
  opacity: 0;
}

.Toastify__progress-bar--bg {
  opacity: var(--toastify-color-progress-bgo);
  width: 100%;
  height: 100%;
}

.Toastify__spinner {
  width: 20px;
  height: 20px;
  box-sizing: border-box;
  border: 2px solid;
  border-radius: 100%;
  border-color: var(--toastify-spinner-color-empty-area);
  border-right-color: var(--toastify-spinner-color);
  animation: Toastify__spin 0.65s linear infinite;
}

@keyframes Toastify__bounceInRight {
  from,
  60%,
  75%,
  90%,
  to {
    animation-timing-function: cubic-bezier(0.215, 0.61, 0.355, 1);
  }
  from {
    opacity: 0;
    transform: translate3d(3000px, 0, 0);
  }
  60% {
    opacity: 1;
    transform: translate3d(-25px, 0, 0);
  }
  75% {
    transform: translate3d(10px, 0, 0);
  }
  90% {
    transform: translate3d(-5px, 0, 0);
  }
  to {
    transform: none;
  }
}

@keyframes Toastify__bounceOutRight {
  20% {
    opacity: 1;
    transform: translate3d(-20px, var(--y), 0);
  }
  to {
    opacity: 0;
    transform: translate3d(2000px, var(--y), 0);
  }
}

@keyframes Toastify__bounceInLeft {
  from,
  60%,
  75%,
  90%,
  to {
    animation-timing-function: cubic-bezier(0.215, 0.61, 0.355, 1);
  }
  0% {
    opacity: 0;
    transform: translate3d(-3000px, 0, 0);
  }
  60% {
    opacity: 1;
    transform: translate3d(25px, 0, 0);
  }
  75% {
    transform: translate3d(-10px, 0, 0);
  }
  90% {
    transform: translate3d(5px, 0, 0);
  }
  to {
    transform: none;
  }
}

@keyframes Toastify__bounceOutLeft {
  20% {
    opacity: 1;
    transform: translate3d(20px, var(--y), 0);
  }
  to {
    opacity: 0;
    transform: translate3d(-2000px, var(--y), 0);
  }
}

@keyframes Toastify__bounceInUp {
  from,
  60%,
  75%,
  90%,
  to {
    animation-timing-function: cubic-bezier(0.215, 0.61, 0.355, 1);
  }
  from {
    opacity: 0;
    transform: translate3d(0, 3000px, 0);
  }
  60% {
    opacity: 1;
    transform: translate3d(0, -20px, 0);
  }
  75% {
    transform: translate3d(0, 10px, 0);
  }
  90% {
    transform: translate3d(0, -5px, 0);
  }
  to {
    transform: translate3d(0, 0, 0);
  }
}

@keyframes Toastify__bounceOutUp {
  20% {
    transform: translate3d(0, calc(var(--y) - 10px), 0);
  }
  40%,
  45% {
    opacity: 1;
    transform: translate3d(0, calc(var(--y) + 20px), 0);
  }
  to {
    opacity: 0;
    transform: translate3d(0, -2000px, 0);
  }
}

@keyframes Toastify__bounceInDown {
  from,
  60%,
  75%,
  90%,
  to {
    animation-timing-function: cubic-bezier(0.215, 0.61, 0.355, 1);
  }
  0% {
    opacity: 0;
    transform: translate3d(0, -3000px, 0);
  }
  60% {
    opacity: 1;
    transform: translate3d(0, 25px, 0);
  }
  75% {
    transform: translate3d(0, -10px, 0);
  }
  90% {
    transform: translate3d(0, 5px, 0);
  }
  to {
    transform: none;
  }
}

@keyframes Toastify__bounceOutDown {
  20% {
    transform: translate3d(0, calc(var(--y) - 10px), 0);
  }
  40%,
  45% {
    opacity: 1;
    transform: translate3d(0, calc(var(--y) + 20px), 0);
  }
  to {
    opacity: 0;
    transform: translate3d(0, 2000px, 0);
  }
}

.Toastify__bounce-enter--top-left,
.Toastify__bounce-enter--bottom-left {
  animation-name: Toastify__bounceInLeft;
}

.Toastify__bounce-enter--top-right,
.Toastify__bounce-enter--bottom-right {
  animation-name: Toastify__bounceInRight;
}

.Toastify__bounce-enter--top-center {
  animation-name: Toastify__bounceInDown;
}

.Toastify__bounce-enter--bottom-center {
  animation-name: Toastify__bounceInUp;
}

.Toastify__bounce-exit--top-left,
.Toastify__bounce-exit--bottom-left {
  animation-name: Toastify__bounceOutLeft;
}

.Toastify__bounce-exit--top-right,
.Toastify__bounce-exit--bottom-right {
  animation-name: Toastify__bounceOutRight;
}

.Toastify__bounce-exit--top-center {
  animation-name: Toastify__bounceOutUp;
}

.Toastify__bounce-exit--bottom-center {
  animation-name: Toastify__bounceOutDown;
}

@keyframes Toastify__zoomIn {
  from {
    opacity: 0;
    transform: scale3d(0.3, 0.3, 0.3);
  }
  50% {
    opacity: 1;
  }
}

@keyframes Toastify__zoomOut {
  from {
    opacity: 1;
  }
  50% {
    opacity: 0;
    transform: translate3d(0, var(--y), 0) scale3d(0.3, 0.3, 0.3);
  }
  to {
    opacity: 0;
  }
}

.Toastify__zoom-enter {
  animation-name: Toastify__zoomIn;
}

.Toastify__zoom-exit {
  animation-name: Toastify__zoomOut;
}

@keyframes Toastify__flipIn {
  from {
    transform: perspective(400px) rotate3d(1, 0, 0, 90deg);
    animation-timing-function: ease-in;
    opacity: 0;
  }
  40% {
    transform: perspective(400px) rotate3d(1, 0, 0, -20deg);
    animation-timing-function: ease-in;
  }
  60% {
    transform: perspective(400px) rotate3d(1, 0, 0, 10deg);
    opacity: 1;
  }
  80% {
    transform: perspective(400px) rotate3d(1, 0, 0, -5deg);
  }
  to {
    transform: perspective(400px);
  }
}

@keyframes Toastify__flipOut {
  from {
    transform: translate3d(0, var(--y), 0) perspective(400px);
  }
  30% {
    transform: translate3d(0, var(--y), 0) perspective(400px) rotate3d(1, 0, 0, -20deg);
    opacity: 1;
  }
  to {
    transform: translate3d(0, var(--y), 0) perspective(400px) rotate3d(1, 0, 0, 90deg);
    opacity: 0;
  }
}

.Toastify__flip-enter {
  animation-name: Toastify__flipIn;
}

.Toastify__flip-exit {
  animation-name: Toastify__flipOut;
}

@keyframes Toastify__slideInRight {
  from {
    transform: translate3d(110%, 0, 0);
    visibility: visible;
  }
  to {
    transform: translate3d(0, var(--y), 0);
  }
}

@keyframes Toastify__slideInLeft {
  from {
    transform: translate3d(-110%, 0, 0);
    visibility: visible;
  }
  to {
    transform: translate3d(0, var(--y), 0);
  }
}

@keyframes Toastify__slideInUp {
  from {
    transform: translate3d(0, 110%, 0);
    visibility: visible;
  }
  to {
    transform: translate3d(0, var(--y), 0);
  }
}

@keyframes Toastify__slideInDown {
  from {
    transform: translate3d(0, -110%, 0);
    visibility: visible;
  }
  to {
    transform: translate3d(0, var(--y), 0);
  }
}

@keyframes Toastify__slideOutRight {
  from {
    transform: translate3d(0, var(--y), 0);
  }
  to {
    visibility: hidden;
    transform: translate3d(110%, var(--y), 0);
  }
}

@keyframes Toastify__slideOutLeft {
  from {
    transform: translate3d(0, var(--y), 0);
  }
  to {
    visibility: hidden;
    transform: translate3d(-110%, var(--y), 0);
  }
}

@keyframes Toastify__slideOutDown {
  from {
    transform: translate3d(0, var(--y), 0);
  }
  to {
    visibility: hidden;
    transform: translate3d(0, 500px, 0);
  }
}

@keyframes Toastify__slideOutUp {
  from {
    transform: translate3d(0, var(--y), 0);
  }
  to {
    visibility: hidden;
    transform: translate3d(0, -500px, 0);
  }
}

.Toastify__slide-enter--top-left,
.Toastify__slide-enter--bottom-left {
  animation-name: Toastify__slideInLeft;
}

.Toastify__slide-enter--top-right,
.Toastify__slide-enter--bottom-right {
  animation-name: Toastify__slideInRight;
}

.Toastify__slide-enter--top-center {
  animation-name: Toastify__slideInDown;
}

.Toastify__slide-enter--bottom-center {
  animation-name: Toastify__slideInUp;
}

.Toastify__slide-exit--top-left,
.Toastify__slide-exit--bottom-left {
  animation-name: Toastify__slideOutLeft;
  animation-timing-function: ease-in;
  animation-duration: 0.3s;
}

.Toastify__slide-exit--top-right,
.Toastify__slide-exit--bottom-right {
  animation-name: Toastify__slideOutRight;
  animation-timing-function: ease-in;
  animation-duration: 0.3s;
}

.Toastify__slide-exit--top-center {
  animation-name: Toastify__slideOutUp;
  animation-timing-function: ease-in;
  animation-duration: 0.3s;
}

.Toastify__slide-exit--bottom-center {
  animation-name: Toastify__slideOutDown;
  animation-timing-function: ease-in;
  animation-duration: 0.3s;
}

@keyframes Toastify__spin {
  from {
    transform: rotate(0deg);
  }
  to {
    transform: rotate(360deg);
  }
}
`,tt=new Map,Yt=(t,e)=>{lt(()=>{if(!t||typeof document>"u")return;let o=document,a=tt.get(o);if(a){e&&a.setAttribute("nonce",e);return}let r=o.createElement("style");r.textContent=t,e&&r.setAttribute("nonce",e),o.head.appendChild(r),tt.set(o,r)},[e])};function Qt(t){return Yt(Kt,t.nonce),i.createElement(qt,{...t})}var ae=yt({CustomToast:()=>ct,Notify:()=>Wt,RemoveToast:()=>Vt,default:()=>Jt}),W=pt(),Vt=t=>{_.dismiss(t)},Wt=({message:t,level:e,duration:o,toast_id:a})=>{let r="default";e==="error"&&(r="error"),e==="warning"&&(r="warning"),e==="info"&&(r="info"),e==="debug"&&(r="default"),e==="success"&&(r="success"),_(ct,{autoClose:o??!1,type:r,theme:"colored",icon:!1,data:{message:t,level:e},toastId:a})},ct=({data:t,toastProps:e})=>(0,W.jsx)("div",{className:"pl-1 py-2 me-6",children:(0,W.jsx)("p",{children:t.message})}),Gt={success:"bg-success text-success-content",error:"bg-error text-error-content",info:"bg-info text-info-content",warning:"bg-warning text-warning-content",default:"bg-neutral text-neutral-content"};function Jt(){return(0,W.jsx)(Qt,{position:"bottom-right",autoClose:5e3,hideProgressBar:!1,newestOnTop:!1,closeOnClick:!1,rtl:!1,pauseOnFocusLoss:!0,draggable:!0,pauseOnHover:!0,theme:"light",transition:ft,toastClassName:t=>Gt[t?.type||"default"]+" relative flex min-w-84 p-1 min-h-10 rounded-md justify-between overflow-hidden cursor-pointer my-2",progressClassName:"progress",className:""})}export{Wt as n,ae as t};
