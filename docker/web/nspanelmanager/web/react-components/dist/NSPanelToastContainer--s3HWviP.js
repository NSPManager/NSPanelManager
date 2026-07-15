import { c as __toESM, o as __exportAll, r as require_react, t as require_jsx_runtime } from "./main-GZAbzCI4.js";
//#region node_modules/clsx/dist/clsx.mjs
var import_react = /* @__PURE__ */ __toESM(require_react(), 1);
function r(e) {
	var t, f, n = "";
	if ("string" == typeof e || "number" == typeof e) n += e;
	else if ("object" == typeof e) if (Array.isArray(e)) {
		var o = e.length;
		for (t = 0; t < o; t++) e[t] && (f = r(e[t])) && (n && (n += " "), n += f);
	} else for (f in e) e[f] && (n && (n += " "), n += f);
	return n;
}
function clsx() {
	for (var e, t, f = 0, n = "", o = arguments.length; f < o; f++) (e = arguments[f]) && (t = r(e)) && (n && (n += " "), n += t);
	return n;
}
//#endregion
//#region node_modules/react-toastify/dist/index.mjs
var L = (t) => typeof t == "number" && !isNaN(t), N = (t) => typeof t == "string", P = (t) => typeof t == "function", ut = (t) => N(t) || L(t), z = (t) => N(t) || P(t) ? t : null, yt = (t, o) => t === !1 || L(t) && t > 0 ? t : o, F = (t) => (0, import_react.isValidElement)(t) || N(t) || P(t) || L(t);
function J(t, o, e = 300) {
	let { scrollHeight: r, style: a } = t;
	requestAnimationFrame(() => {
		a.minHeight = "initial", a.height = r + "px", a.transition = `all ${e}ms`, requestAnimationFrame(() => {
			a.height = "0", a.padding = "0", a.margin = "0", setTimeout(o, e);
		});
	});
}
function R({ enter: t, exit: o, appendPosition: e = !1, collapse: r = !0, collapseDuration: a = 300 }) {
	return function({ children: s, position: f, preventExitTransition: c, done: T, nodeRef: g, isIn: v, playToast: x }) {
		let C = e ? `${t}--${f}` : t, S = e ? `${o}--${f}` : o, E = (0, import_react.useRef)(0);
		return (0, import_react.useLayoutEffect)(() => {
			let d = g.current, p = C.split(" "), b = (n) => {
				n.target === g.current && (x(), d.removeEventListener("animationend", b), d.removeEventListener("animationcancel", b), E.current === 0 && n.type !== "animationcancel" && d.classList.remove(...p));
			};
			d.classList.add(...p), d.addEventListener("animationend", b), d.addEventListener("animationcancel", b);
		}, []), (0, import_react.useEffect)(() => {
			let d = g.current, p = () => {
				d.removeEventListener("animationend", p), r ? J(d, T, a) : T();
			};
			v || (c ? p() : (E.current = 1, d.className += ` ${S}`, d.addEventListener("animationend", p)));
		}, [v]), import_react.createElement(import_react.Fragment, null, s);
	};
}
function tt(t, o) {
	return {
		content: ot(t.content, t.props),
		containerId: t.props.containerId,
		id: t.props.toastId,
		theme: t.props.theme,
		type: t.props.type,
		data: t.props.data || {},
		isLoading: t.props.isLoading,
		icon: t.props.icon,
		reason: t.removalReason,
		status: o
	};
}
function ot(t, o, e = !1) {
	return (0, import_react.isValidElement)(t) && !N(t.type) ? (0, import_react.cloneElement)(t, {
		closeToast: o.closeToast,
		toastProps: o,
		data: o.data,
		isPaused: e
	}) : P(t) ? t({
		closeToast: o.closeToast,
		toastProps: o,
		data: o.data,
		isPaused: e
	}) : t;
}
function gt({ closeToast: t, theme: o, ariaLabel: e = "close" }) {
	return import_react.createElement("button", {
		className: `Toastify__close-button Toastify__close-button--${o}`,
		type: "button",
		onClick: (r) => {
			r.stopPropagation(), t(!0);
		},
		"aria-label": e
	}, import_react.createElement("svg", {
		"aria-hidden": "true",
		viewBox: "0 0 14 16"
	}, import_react.createElement("path", {
		fillRule: "evenodd",
		d: "M7.71 8.23l3.75 3.75-1.48 1.48-3.75-3.75-3.75 3.75L1 11.98l3.75-3.75L1 4.48 2.48 3l3.75 3.75L9.98 3l1.48 1.48-3.75 3.75z"
	})));
}
function bt({ delay: t, isRunning: o, closeToast: e, type: r = "default", hide: a, className: l, controlledProgress: s, progress: f, rtl: c, isIn: T, theme: g }) {
	let v = a || s && f === 0, x = {
		animationDuration: `${t}ms`,
		animationPlayState: o ? "running" : "paused"
	};
	s && (x.transform = `scaleX(${f})`);
	let C = clsx("Toastify__progress-bar", s ? "Toastify__progress-bar--controlled" : "Toastify__progress-bar--animated", `Toastify__progress-bar-theme--${g}`, `Toastify__progress-bar--${r}`, { ["Toastify__progress-bar--rtl"]: c }), S = P(l) ? l({
		rtl: c,
		type: r,
		defaultClassName: C
	}) : clsx(C, l), E = { [s && f >= 1 ? "onTransitionEnd" : "onAnimationEnd"]: s && f < 1 ? null : () => {
		T && e();
	} };
	return import_react.createElement("div", {
		className: "Toastify__progress-bar--wrp",
		"data-hidden": v
	}, import_react.createElement("div", { className: `Toastify__progress-bar--bg Toastify__progress-bar-theme--${g} Toastify__progress-bar--${r}` }), import_react.createElement("div", {
		role: "progressbar",
		"aria-hidden": v ? "true" : "false",
		"aria-label": "notification timer",
		"aria-valuenow": s ? Math.round(f * 100) : void 0,
		"aria-valuemin": 0,
		"aria-valuemax": 100,
		className: S,
		style: x,
		...E
	}));
}
var Qt = 1, st = () => `${Qt++}`;
function vt(t, o, e) {
	let r = 1, a = 0, l = [], s = [], f = o, c = /* @__PURE__ */ new Map(), T = /* @__PURE__ */ new Set(), g = (i) => (T.add(i), () => T.delete(i)), v = () => {
		s = Array.from(c.values()), T.forEach((i) => i());
	}, x = ({ containerId: i, toastId: n, updateId: u }) => {
		let h = i ? i !== t : t !== 1, m = c.has(n) && u == null;
		return h || m;
	}, C = (i, n) => {
		c.forEach((u) => {
			var h;
			(n == null || n === u.props.toastId) && ((h = u.toggle) == null || h.call(u, i));
		});
	}, S = (i) => {
		var n, u;
		i.isActive && ((u = (n = i.props) == null ? void 0 : n.onClose) == null || u.call(n, i.removalReason), i.isActive = !1, e(tt(i, "removed")));
	}, E = (i) => {
		if (i == null) c.forEach(S);
		else {
			let n = c.get(i);
			n && S(n);
		}
		v();
	}, d = () => {
		a -= l.length, l = [];
	}, p = (i) => {
		var m, _;
		let { toastId: n, updateId: u } = i.props, h = u == null;
		i.staleId && c.delete(i.staleId), i.isActive = !0, c.set(n, i), v(), e(tt(i, h ? "added" : "updated")), h && ((_ = (m = i.props).onOpen) == null || _.call(m));
	};
	return {
		id: t,
		props: f,
		observe: g,
		toggle: C,
		removeToast: E,
		toasts: c,
		clearQueue: d,
		buildToast: (i, n) => {
			if (x(n)) return;
			let { toastId: u, updateId: h, data: m, staleId: _, delay: k } = n, D = h == null;
			D && a++;
			let A = {
				...f,
				style: f.toastStyle,
				key: r++,
				...Object.fromEntries(Object.entries(n).filter(([M, $]) => $ != null)),
				toastId: u,
				updateId: h,
				data: m,
				isIn: !1,
				className: z(n.className || f.toastClassName),
				progressClassName: z(n.progressClassName || f.progressClassName),
				autoClose: n.isLoading ? !1 : yt(n.autoClose, f.autoClose),
				closeToast(M) {
					let $ = c.get(u);
					$ && ($.removalReason = M, E(u));
				},
				deleteToast() {
					if (c.get(u) != null) {
						if (c.delete(u), a--, a < 0 && (a = 0), l.length > 0) {
							p(l.shift());
							return;
						}
						v();
					}
				}
			};
			A.closeButton = f.closeButton, n.closeButton === !1 || F(n.closeButton) ? A.closeButton = n.closeButton : n.closeButton === !0 && (A.closeButton = F(f.closeButton) ? f.closeButton : !0);
			let B = {
				content: i,
				props: A,
				staleId: _
			};
			f.limit && f.limit > 0 && a > f.limit && D ? l.push(B) : L(k) ? setTimeout(() => {
				p(B);
			}, k) : p(B);
		},
		setProps(i) {
			f = i;
		},
		setToggle: (i, n) => {
			let u = c.get(i);
			u && (u.toggle = n);
		},
		isToastActive: (i) => {
			var n;
			return (n = c.get(i)) == null ? void 0 : n.isActive;
		},
		getSnapshot: () => s
	};
}
var I = /* @__PURE__ */ new Map(), U = [], rt = /* @__PURE__ */ new Set(), Wt = (t) => rt.forEach((o) => o(t)), ht = () => I.size > 0;
function Gt() {
	U.forEach((t) => nt(t.content, t.options)), U = [];
}
var Ct = (t, { containerId: o }) => {
	var e;
	return (e = I.get(o || 1)) == null ? void 0 : e.toasts.get(t);
};
function X(t, o) {
	var r;
	if (o) return !!((r = I.get(o)) != null && r.isToastActive(t));
	let e = !1;
	return I.forEach((a) => {
		a.isToastActive(t) && (e = !0);
	}), e;
}
function xt(t) {
	if (!ht()) {
		U = U.filter((o) => t != null && o.options.toastId !== t);
		return;
	}
	if (t == null || ut(t)) I.forEach((o) => {
		o.removeToast(t);
	});
	else if (t && ("containerId" in t || "id" in t)) {
		let o = I.get(t.containerId);
		o ? o.removeToast(t.id) : I.forEach((e) => {
			e.removeToast(t.id);
		});
	}
}
var Et = (t = {}) => {
	I.forEach((o) => {
		o.props.limit && (!t.containerId || o.id === t.containerId) && o.clearQueue();
	});
};
function nt(t, o) {
	F(t) && (ht() || U.push({
		content: t,
		options: o
	}), I.forEach((e) => {
		e.buildToast(t, o);
	}));
}
function Pt(t) {
	var o;
	(o = I.get(t.containerId || 1)) == null || o.setToggle(t.id, t.fn);
}
function it(t, o) {
	I.forEach((e) => {
		(o == null || !(o != null && o.containerId) || (o == null ? void 0 : o.containerId) === e.id) && e.toggle(t, o == null ? void 0 : o.id);
	});
}
function It(t) {
	let o = t.containerId || 1;
	return {
		subscribe(e) {
			let r = vt(o, t, Wt);
			I.set(o, r);
			let a = r.observe(e);
			return Gt(), () => {
				a(), I.delete(o);
			};
		},
		setProps(e) {
			var r;
			(r = I.get(o)) == null || r.setProps(e);
		},
		getSnapshot() {
			var e;
			return (e = I.get(o)) == null ? void 0 : e.getSnapshot();
		}
	};
}
function St(t) {
	return rt.add(t), () => {
		rt.delete(t);
	};
}
function qt(t) {
	return t && (N(t.toastId) || L(t.toastId)) ? t.toastId : st();
}
function H(t, o) {
	return nt(t, o), o.toastId;
}
function Q(t, o) {
	return {
		...o,
		type: o && o.type || t,
		toastId: qt(o)
	};
}
function W(t) {
	return (o, e) => H(o, Q(t, e));
}
function y(t, o) {
	return H(t, Q("default", o));
}
y.loading = (t, o) => H(t, Q("default", {
	isLoading: !0,
	autoClose: !1,
	closeOnClick: !1,
	closeButton: !1,
	draggable: !1,
	...o
}));
function Kt(t, { pending: o, error: e, success: r }, a) {
	let l;
	o && (l = N(o) ? y.loading(o, a) : y.loading(o.render, {
		...a,
		...o
	}));
	let s = {
		isLoading: null,
		autoClose: null,
		closeOnClick: null,
		closeButton: null,
		draggable: null
	}, f = (T, g, v) => {
		if (g == null) {
			y.dismiss(l);
			return;
		}
		let x = {
			type: T,
			...s,
			...a,
			data: v
		}, C = N(g) ? { render: g } : g;
		return l ? y.update(l, {
			...x,
			...C
		}) : y(C.render, {
			...x,
			...C
		}), v;
	}, c = P(t) ? t() : t;
	return c.then((T) => f("success", r, T)).catch((T) => f("error", e, T)), c;
}
y.promise = Kt;
y.success = W("success");
y.info = W("info");
y.error = W("error");
y.warning = W("warning");
y.warn = y.warning;
y.dark = (t, o) => H(t, Q("default", {
	theme: "dark",
	...o
}));
function Yt(t) {
	xt(t);
}
y.dismiss = Yt;
y.clearWaitingQueue = Et;
y.isActive = X;
y.update = (t, o = {}) => {
	let e = Ct(t, o);
	if (e) {
		let { props: r, content: a } = e, l = {
			delay: 100,
			...r,
			...o,
			toastId: o.toastId || t,
			updateId: st()
		};
		l.toastId !== t && (l.staleId = t);
		let s = l.render || a;
		delete l.render, H(s, l);
	}
};
y.done = (t) => {
	y.update(t, { progress: 1 });
};
y.onChange = St;
y.play = (t) => it(!0, t);
y.pause = (t) => it(!1, t);
function kt(t) {
	var s;
	let { subscribe: o, getSnapshot: e, setProps: r } = (0, import_react.useRef)(It(t)).current;
	r(t);
	let a = (s = (0, import_react.useSyncExternalStore)(o, e, e)) == null ? void 0 : s.slice();
	function l(f) {
		if (!a) return [];
		let c = /* @__PURE__ */ new Map();
		return t.newestOnTop && a.reverse(), a.forEach((T) => {
			let { position: g } = T.props;
			c.has(g) || c.set(g, []), c.get(g).push(T);
		}), Array.from(c, (T) => f(T[0], T[1]));
	}
	return {
		getToastToRender: l,
		isToastActive: X,
		count: a == null ? void 0 : a.length
	};
}
function Nt(t) {
	let [o, e] = (0, import_react.useState)(!1), [r, a] = (0, import_react.useState)(!1), l = (0, import_react.useRef)(null), s = (0, import_react.useRef)({
		start: 0,
		delta: 0,
		removalDistance: 0,
		canCloseOnClick: !0,
		canDrag: !1,
		didMove: !1
	}).current, { autoClose: f, pauseOnHover: c, closeToast: T, onClick: g, closeOnClick: v } = t;
	Pt({
		id: t.toastId,
		containerId: t.containerId,
		fn: e
	}), (0, import_react.useEffect)(() => {
		if (t.pauseOnFocusLoss) return x(), () => {
			C();
		};
	}, [t.pauseOnFocusLoss]);
	function x() {
		document.hasFocus() || p(), window.addEventListener("focus", d), window.addEventListener("blur", p);
	}
	function C() {
		window.removeEventListener("focus", d), window.removeEventListener("blur", p);
	}
	function S(m) {
		if (t.draggable === !0 || t.draggable === m.pointerType) {
			b();
			let _ = l.current;
			s.canCloseOnClick = !0, s.canDrag = !0, _.style.transition = "none", t.draggableDirection === "x" ? (s.start = m.clientX, s.removalDistance = _.offsetWidth * (t.draggablePercent / 100)) : (s.start = m.clientY, s.removalDistance = _.offsetHeight * (t.draggablePercent === 80 ? t.draggablePercent * 1.5 : t.draggablePercent) / 100);
		}
	}
	function E(m) {
		let { top: _, bottom: k, left: D, right: A } = l.current.getBoundingClientRect();
		m.pointerType === "mouse" && t.pauseOnHover && m.clientX >= D && m.clientX <= A && m.clientY >= _ && m.clientY <= k ? p() : d();
	}
	function d() {
		e(!0);
	}
	function p() {
		e(!1);
	}
	function b() {
		s.didMove = !1, document.addEventListener("pointermove", n), document.addEventListener("pointerup", u);
	}
	function i() {
		document.removeEventListener("pointermove", n), document.removeEventListener("pointerup", u);
	}
	function n(m) {
		let _ = l.current;
		if (s.canDrag && _) {
			s.didMove = !0, o && p(), t.draggableDirection === "x" ? s.delta = m.clientX - s.start : s.delta = m.clientY - s.start, s.start !== m.clientX && (s.canCloseOnClick = !1);
			let k = t.draggableDirection === "x" ? `${s.delta}px, var(--y)` : `0, calc(${s.delta}px + var(--y))`;
			_.style.transform = `translate3d(${k},0)`, _.style.opacity = `${1 - Math.abs(s.delta / s.removalDistance)}`;
		}
	}
	function u() {
		i();
		let m = l.current;
		if (s.canDrag && s.didMove && m) {
			if (s.canDrag = !1, Math.abs(s.delta) > s.removalDistance) {
				a(!0), t.closeToast(!0), t.collapseAll();
				return;
			}
			m.style.transition = "transform 0.2s, opacity 0.2s", m.style.removeProperty("transform"), m.style.removeProperty("opacity");
		}
	}
	let h = {
		onPointerDown: S,
		onPointerUp: E
	};
	return f && c && (h.onMouseEnter = p, t.stacked || (h.onMouseLeave = d)), v && (h.onClick = (m) => {
		g && g(m), s.canCloseOnClick && T(!0);
	}), {
		playToast: d,
		pauseToast: p,
		isRunning: o,
		preventExitTransition: r,
		toastRef: l,
		eventHandlers: h
	};
}
var G = typeof window != "undefined" ? import_react.useLayoutEffect : import_react.useEffect;
var K = ({ theme: t, type: o, isLoading: e, ...r }) => import_react.createElement("svg", {
	viewBox: "0 0 24 24",
	width: "100%",
	height: "100%",
	fill: t === "colored" ? "currentColor" : `var(--toastify-icon-color-${o})`,
	...r
});
function ro(t) {
	return import_react.createElement(K, { ...t }, import_react.createElement("path", { d: "M23.32 17.191L15.438 2.184C14.728.833 13.416 0 11.996 0c-1.42 0-2.733.833-3.443 2.184L.533 17.448a4.744 4.744 0 000 4.368C1.243 23.167 2.555 24 3.975 24h16.05C22.22 24 24 22.044 24 19.632c0-.904-.251-1.746-.68-2.44zm-9.622 1.46c0 1.033-.724 1.823-1.698 1.823s-1.698-.79-1.698-1.822v-.043c0-1.028.724-1.822 1.698-1.822s1.698.79 1.698 1.822v.043zm.039-12.285l-.84 8.06c-.057.581-.408.943-.897.943-.49 0-.84-.367-.896-.942l-.84-8.065c-.057-.624.25-1.095.779-1.095h1.91c.528.005.84.476.784 1.1z" }));
}
function no(t) {
	return import_react.createElement(K, { ...t }, import_react.createElement("path", { d: "M12 0a12 12 0 1012 12A12.013 12.013 0 0012 0zm.25 5a1.5 1.5 0 11-1.5 1.5 1.5 1.5 0 011.5-1.5zm2.25 13.5h-4a1 1 0 010-2h.75a.25.25 0 00.25-.25v-4.5a.25.25 0 00-.25-.25h-.75a1 1 0 010-2h1a2 2 0 012 2v4.75a.25.25 0 00.25.25h.75a1 1 0 110 2z" }));
}
function io(t) {
	return import_react.createElement(K, { ...t }, import_react.createElement("path", { d: "M12 0a12 12 0 1012 12A12.014 12.014 0 0012 0zm6.927 8.2l-6.845 9.289a1.011 1.011 0 01-1.43.188l-4.888-3.908a1 1 0 111.25-1.562l4.076 3.261 6.227-8.451a1 1 0 111.61 1.183z" }));
}
function lo(t) {
	return import_react.createElement(K, { ...t }, import_react.createElement("path", { d: "M11.983 0a12.206 12.206 0 00-8.51 3.653A11.8 11.8 0 000 12.207 11.779 11.779 0 0011.8 24h.214A12.111 12.111 0 0024 11.791 11.766 11.766 0 0011.983 0zM10.5 16.542a1.476 1.476 0 011.449-1.53h.027a1.527 1.527 0 011.523 1.47 1.475 1.475 0 01-1.449 1.53h-.027a1.529 1.529 0 01-1.523-1.47zM11 12.5v-6a1 1 0 012 0v6a1 1 0 11-2 0z" }));
}
function fo() {
	return import_react.createElement("div", { className: "Toastify__spinner" });
}
var q = {
	info: no,
	warning: ro,
	success: io,
	error: lo,
	spinner: fo
}, co = (t) => t in q;
function wt({ theme: t, type: o, isLoading: e, icon: r }) {
	let a = null, l = {
		theme: t,
		type: o
	};
	return r === !1 || (P(r) ? a = r({
		...l,
		isLoading: e
	}) : (0, import_react.isValidElement)(r) ? a = (0, import_react.cloneElement)(r, l) : e ? a = q.spinner() : co(o) && (a = q[o](l))), a;
}
var Dt = (t) => {
	let { isRunning: o, preventExitTransition: e, toastRef: r, eventHandlers: a, playToast: l } = Nt(t), { closeButton: s, children: f, autoClose: c, onClick: T, type: g, hideProgressBar: v, closeToast: x, transition: C, position: S, className: E, style: d, progressClassName: p, updateId: b, role: i, progress: n, rtl: u, toastId: h, deleteToast: m, isIn: _, isLoading: k, closeOnClick: D, theme: A, ariaLabel: B } = t, M = clsx("Toastify__toast", `Toastify__toast-theme--${A}`, `Toastify__toast--${g}`, { ["Toastify__toast--rtl"]: u }, { ["Toastify__toast--close-on-click"]: D }), $ = P(E) ? E({
		rtl: u,
		position: S,
		type: g,
		defaultClassName: M
	}) : clsx(M, E), mt = wt(t), pt = !!n || !c, Z = {
		closeToast: x,
		type: g,
		theme: A
	}, V = null;
	return s === !1 || (P(s) ? V = s(Z) : (0, import_react.isValidElement)(s) ? V = (0, import_react.cloneElement)(s, Z) : V = gt(Z)), import_react.createElement(C, {
		isIn: _,
		done: m,
		position: S,
		preventExitTransition: e,
		nodeRef: r,
		playToast: l
	}, import_react.createElement("div", {
		id: h,
		tabIndex: 0,
		onClick: T,
		"data-in": _,
		className: $,
		...a,
		style: d,
		ref: r,
		..._ && {
			role: i,
			"aria-label": B
		}
	}, mt != null && import_react.createElement("div", { className: clsx("Toastify__toast-icon", { ["Toastify--animate-icon Toastify__zoom-enter"]: !k }) }, mt), ot(f, t, !o), V, !t.customProgressBar && import_react.createElement(bt, {
		...b && !pt ? { key: `p-${b}` } : {},
		rtl: u,
		theme: A,
		delay: c,
		isRunning: o,
		isIn: _,
		closeToast: x,
		hide: v,
		type: g,
		className: p,
		controlledProgress: pt,
		progress: n || 0
	})));
}, j = (t, o = !1) => ({
	enter: `Toastify--animate Toastify__${t}-enter`,
	exit: `Toastify--animate Toastify__${t}-exit`,
	appendPosition: o
}), ft = R(j("bounce", !0));
R(j("slide", !0));
R(j("zoom"));
R(j("flip"));
var vo = {
	position: "top-right",
	transition: ft,
	autoClose: 5e3,
	closeButton: !0,
	pauseOnHover: !0,
	pauseOnFocusLoss: !0,
	draggable: "touch",
	draggablePercent: 80,
	draggableDirection: "x",
	role: "alert",
	theme: "light",
	"aria-label": "Notifications Alt+T",
	hotKeys: (t) => t.altKey && t.code === "KeyT"
};
function dt(t) {
	let o = {
		...vo,
		...t
	}, e = t.stacked, [r, a] = (0, import_react.useState)(!0), l = (0, import_react.useRef)(null), { getToastToRender: s, isToastActive: f, count: c } = kt(o), { className: T, style: g, rtl: v, containerId: x, hotKeys: C } = o;
	function S(d) {
		let p = clsx("Toastify__toast-container", `Toastify__toast-container--${d}`, { ["Toastify__toast-container--rtl"]: v });
		return P(T) ? T({
			position: d,
			rtl: v,
			defaultClassName: p
		}) : clsx(p, z(T));
	}
	function E() {
		e && (a(!0), y.play());
	}
	return G(() => {
		var d;
		if (e) {
			let p = l.current.querySelectorAll("[data-in=\"true\"]"), b = 12, i = (d = o.position) == null ? void 0 : d.includes("top"), n = 0, u = 0;
			Array.from(p).reverse().forEach((h, m) => {
				let _ = h;
				_.classList.add("Toastify__toast--stacked"), m > 0 && (_.dataset.collapsed = `${r}`), _.dataset.pos || (_.dataset.pos = i ? "top" : "bot");
				let k = n * (r ? .2 : 1) + (r ? 0 : b * m), D = Math.max(.5, 1 - (r ? u : 0));
				_.style.setProperty("--y", `${i ? k : k * -1}px`), _.style.setProperty("--g", `${b}`), _.style.setProperty("--s", `${D}`), n += _.offsetHeight, u += .025;
			});
		}
	}, [
		r,
		c,
		e
	]), (0, import_react.useEffect)(() => {
		function d(p) {
			var i;
			let b = l.current;
			C(p) && ((i = b == null ? void 0 : b.querySelector("[tabIndex=\"0\"]")) == null || i.focus(), a(!1), y.pause()), p.key === "Escape" && (document.activeElement === b || b != null && b.contains(document.activeElement)) && (a(!0), y.play());
		}
		return document.addEventListener("keydown", d), () => {
			document.removeEventListener("keydown", d);
		};
	}, [C]), import_react.createElement("section", {
		ref: l,
		className: "Toastify",
		id: x,
		onMouseEnter: () => {
			e && (a(!1), y.pause());
		},
		onMouseLeave: E,
		"aria-live": "polite",
		"aria-atomic": "false",
		"aria-relevant": "additions text",
		"aria-label": o["aria-label"]
	}, s((d, p) => {
		let b = p.length ? { ...g } : {
			...g,
			pointerEvents: "none"
		};
		return import_react.createElement("div", {
			tabIndex: -1,
			className: S(d),
			"data-stacked": e,
			style: b,
			key: `c-${d}`
		}, p.map(({ content: i, props: n }) => import_react.createElement(Dt, {
			...n,
			stacked: e,
			collapseAll: E,
			isIn: f(n.toastId, n.containerId),
			key: `t-${n.key}`
		}, i)));
	}));
}
var Mt = `:root {
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
`;
var $t = /* @__PURE__ */ new Map(), Rt = (t, o) => {
	G(() => {
		if (!t || typeof document == "undefined") return;
		let e = document, r = $t.get(e);
		if (r) {
			o && r.setAttribute("nonce", o);
			return;
		}
		let a = e.createElement("style");
		a.textContent = t, o && a.setAttribute("nonce", o), e.head.appendChild(a), $t.set(e, a);
	}, [o]);
};
function xo(t) {
	return Rt(Mt, t.nonce), import_react.createElement(dt, { ...t });
}
//#endregion
//#region src/components/NSPanelToastContainer.tsx
var NSPanelToastContainer_exports = /* @__PURE__ */ __exportAll({
	CustomToast: () => CustomToast,
	Notify: () => Notify,
	RemoveNotification: () => RemoveNotification,
	default: () => NSPanelToastContainer
});
var import_jsx_runtime = require_jsx_runtime();
var RemoveNotification = (toast_id) => {
	y.dismiss(toast_id);
};
var Notify = ({ message, level, duration, toast_id }) => {
	let toastType = "default";
	if (level === "error") toastType = "error";
	if (level === "warning") toastType = "warning";
	if (level === "info") toastType = "info";
	if (level === "debug") toastType = "default";
	if (level === "success") toastType = "success";
	y(CustomToast, {
		autoClose: duration ?? false,
		type: toastType,
		theme: "colored",
		icon: false,
		data: {
			message,
			level
		},
		toastId: toast_id
	});
};
var CustomToast = ({ data }) => {
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "pl-1 py-2 me-6",
		children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("p", { children: data.message })
	});
};
var contextClass = {
	success: "bg-success text-success-content",
	error: "bg-error text-error-content",
	info: "bg-info text-info-content",
	warning: "bg-warning text-warning-content",
	default: "bg-neutral text-neutral-content"
};
function NSPanelToastContainer() {
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(xo, {
		position: "bottom-right",
		autoClose: 5e3,
		hideProgressBar: false,
		newestOnTop: false,
		closeOnClick: false,
		rtl: false,
		pauseOnFocusLoss: true,
		draggable: true,
		pauseOnHover: true,
		theme: "light",
		transition: ft,
		toastClassName: (context) => contextClass[context?.type || "default"] + " relative flex min-w-84 p-1 min-h-10 rounded-md justify-between overflow-hidden cursor-pointer my-2",
		progressClassName: "progress",
		className: ""
	});
}
//#endregion
export { Notify as n, RemoveNotification as r, NSPanelToastContainer_exports as t };
