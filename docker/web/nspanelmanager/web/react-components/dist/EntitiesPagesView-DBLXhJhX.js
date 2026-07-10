import { c as __toESM, n as require_react_dom, r as require_react, t as require_jsx_runtime } from "./main-DCuO7i06.js";
import { t as create } from "./react-5B1MJaM9.js";
import { n as MultiStep_AddOrEditEntity, r as useEntitiesPagesStore } from "./MultiStep_AddEditEntity-8C_6PiGG.js";
//#region node_modules/@preact/signals-core/dist/signals-core.module.js
var import_react = /* @__PURE__ */ __toESM(require_react(), 1);
var i = Symbol.for("preact-signals");
function t() {
	if (!(s > 1)) {
		var i, t = !1;
		(function() {
			var i = c;
			c = void 0;
			while (void 0 !== i) {
				if (i.S.v === i.v) i.S.i = i.i;
				i = i.o;
			}
		})();
		while (void 0 !== h) {
			var n = h;
			h = void 0;
			v++;
			while (void 0 !== n) {
				var r = n.u;
				n.u = void 0;
				n.f &= -3;
				if (!(8 & n.f) && w(n)) try {
					n.c();
				} catch (n) {
					if (!t) {
						i = n;
						t = !0;
					}
				}
				n = r;
			}
		}
		v = 0;
		s--;
		if (t) throw i;
	} else s--;
}
function n(i) {
	if (s > 0) return i();
	e = ++u;
	s++;
	try {
		return i();
	} finally {
		t();
	}
}
var r = void 0;
function o(i) {
	var t = r;
	r = void 0;
	try {
		return i();
	} finally {
		r = t;
	}
}
var f, h = void 0, s = 0, v = 0, u = 0, e = 0, c = void 0, d = 0;
function a(i) {
	if (void 0 !== r) {
		var t = i.n;
		if (void 0 === t || t.t !== r) {
			t = {
				i: 0,
				S: i,
				p: r.s,
				n: void 0,
				t: r,
				e: void 0,
				x: void 0,
				r: t
			};
			if (void 0 !== r.s) r.s.n = t;
			r.s = t;
			i.n = t;
			if (32 & r.f) i.S(t);
			return t;
		} else if (-1 === t.i) {
			t.i = 0;
			if (void 0 !== t.n) {
				t.n.p = t.p;
				if (void 0 !== t.p) t.p.n = t.n;
				t.p = r.s;
				t.n = void 0;
				r.s.n = t;
				r.s = t;
			}
			return t;
		}
	}
}
function l(i, t) {
	this.v = i;
	this.i = 0;
	this.n = void 0;
	this.t = void 0;
	this.l = 0;
	this.W = null == t ? void 0 : t.watched;
	this.Z = null == t ? void 0 : t.unwatched;
	this.name = null == t ? void 0 : t.name;
}
l.prototype.brand = i;
l.prototype.h = function() {
	return !0;
};
l.prototype.S = function(i) {
	var t = this, n = this.t;
	if (n !== i && void 0 === i.e) {
		i.x = n;
		this.t = i;
		if (void 0 !== n) n.e = i;
		else o(function() {
			var i;
			null == (i = t.W) || i.call(t);
		});
	}
};
l.prototype.U = function(i) {
	var t = this;
	if (void 0 !== this.t) {
		var n = i.e, r = i.x;
		if (void 0 !== n) {
			n.x = r;
			i.e = void 0;
		}
		if (void 0 !== r) {
			r.e = n;
			i.x = void 0;
		}
		if (i === this.t) {
			this.t = r;
			if (void 0 === r) o(function() {
				var i;
				null == (i = t.Z) || i.call(t);
			});
		}
	}
};
l.prototype.subscribe = function(i) {
	var t = this;
	return j(function() {
		var n = t.value, o = r;
		r = void 0;
		try {
			i(n);
		} finally {
			r = o;
		}
	}, { name: "sub" });
};
l.prototype.valueOf = function() {
	return this.value;
};
l.prototype.toString = function() {
	return this.value + "";
};
l.prototype.toJSON = function() {
	return this.value;
};
l.prototype.peek = function() {
	var i = r;
	r = void 0;
	try {
		return this.value;
	} finally {
		r = i;
	}
};
Object.defineProperty(l.prototype, "value", {
	get: function() {
		var i = a(this);
		if (void 0 !== i) i.i = this.i;
		return this.v;
	},
	set: function(i) {
		if (i !== this.v) {
			if (v > 100) throw new Error("Cycle detected");
			(function(i) {
				if (0 !== s && 0 === v) {
					if (i.l !== e) {
						i.l = e;
						c = {
							S: i,
							v: i.v,
							i: i.i,
							o: c
						};
					}
				}
			})(this);
			this.v = i;
			this.i++;
			d++;
			s++;
			try {
				for (var n = this.t; void 0 !== n; n = n.x) n.t.N();
			} finally {
				t();
			}
		}
	}
});
function y(i, t) {
	return new l(i, t);
}
function w(i) {
	for (var t = i.s; void 0 !== t; t = t.n) if (t.S.i !== t.i || !t.S.h() || t.S.i !== t.i) return !0;
	return !1;
}
function _(i) {
	for (var t = i.s; void 0 !== t; t = t.n) {
		var n = t.S.n;
		if (void 0 !== n) t.r = n;
		t.S.n = t;
		t.i = -1;
		if (void 0 === t.n) {
			i.s = t;
			break;
		}
	}
}
function b(i) {
	var t = i.s, n = void 0;
	while (void 0 !== t) {
		var r = t.p;
		if (-1 === t.i) {
			t.S.U(t);
			if (void 0 !== r) r.n = t.n;
			if (void 0 !== t.n) t.n.p = r;
		} else n = t;
		t.S.n = t.r;
		if (void 0 !== t.r) t.r = void 0;
		t = r;
	}
	i.s = n;
}
function p(i, t) {
	l.call(this, void 0);
	this.x = i;
	this.s = void 0;
	this.g = d - 1;
	this.f = 4;
	this.W = null == t ? void 0 : t.watched;
	this.Z = null == t ? void 0 : t.unwatched;
	this.name = null == t ? void 0 : t.name;
}
p.prototype = new l();
p.prototype.h = function() {
	this.f &= -3;
	if (1 & this.f) return !1;
	if (32 == (36 & this.f)) return !0;
	this.f &= -5;
	if (this.g === d) return !0;
	this.g = d;
	this.f |= 1;
	if (this.i > 0 && !w(this)) {
		this.f &= -2;
		return !0;
	}
	var i = r;
	try {
		_(this);
		r = this;
		var t = this.x();
		if (16 & this.f || this.v !== t || 0 === this.i) {
			this.v = t;
			this.f &= -17;
			this.i++;
		}
	} catch (i) {
		this.v = i;
		this.f |= 16;
		this.i++;
	}
	r = i;
	b(this);
	this.f &= -2;
	return !0;
};
p.prototype.S = function(i) {
	if (void 0 === this.t) {
		this.f |= 36;
		for (var t = this.s; void 0 !== t; t = t.n) t.S.S(t);
	}
	l.prototype.S.call(this, i);
};
p.prototype.U = function(i) {
	if (void 0 !== this.t) {
		l.prototype.U.call(this, i);
		if (void 0 === this.t) {
			this.f &= -33;
			for (var t = this.s; void 0 !== t; t = t.n) t.S.U(t);
		}
	}
};
p.prototype.N = function() {
	if (!(2 & this.f)) {
		this.f |= 6;
		for (var i = this.t; void 0 !== i; i = i.x) i.t.N();
	}
};
Object.defineProperty(p.prototype, "value", { get: function() {
	if (1 & this.f) throw new Error("Cycle detected");
	var i = a(this);
	this.h();
	if (void 0 !== i) i.i = this.i;
	if (16 & this.f) throw this.v;
	return this.v;
} });
function g(i, t) {
	return new p(i, t);
}
function S(i) {
	var n = i.m;
	i.m = void 0;
	if ("function" == typeof n) {
		s++;
		var o = r;
		r = void 0;
		try {
			n();
		} catch (t) {
			i.f &= -2;
			i.f |= 8;
			m(i);
			throw t;
		} finally {
			r = o;
			t();
		}
	}
}
function m(i) {
	for (var t = i.s; void 0 !== t; t = t.n) t.S.U(t);
	i.x = void 0;
	i.s = void 0;
	S(i);
}
function x(i) {
	if (r !== this) throw new Error("Out-of-order effect");
	b(this);
	r = i;
	this.f &= -2;
	if (8 & this.f) m(this);
	t();
}
function E(i, t) {
	this.x = i;
	this.m = void 0;
	this.s = void 0;
	this.u = void 0;
	this.f = 32;
	this.name = null == t ? void 0 : t.name;
	if (f) f.push(this);
}
E.prototype.c = function() {
	var i = this.S();
	try {
		if (8 & this.f) return;
		if (void 0 === this.x) return;
		var t = this.x();
		if ("function" == typeof t) this.m = t;
	} finally {
		i();
	}
};
E.prototype.S = function() {
	if (1 & this.f) throw new Error("Cycle detected");
	this.f |= 1;
	this.f &= -9;
	S(this);
	_(this);
	s++;
	var i = r;
	r = this;
	return x.bind(this, i);
};
E.prototype.N = function() {
	if (!(2 & this.f)) {
		this.f |= 2;
		this.u = h;
		h = this;
	}
};
E.prototype.d = function() {
	this.f |= 8;
	if (!(1 & this.f)) m(this);
};
E.prototype.dispose = function() {
	this.d();
};
function j(i, t) {
	var n = new E(i, t);
	try {
		n.c();
	} catch (i) {
		n.d();
		throw i;
	}
	var r = n.d.bind(n);
	r[Symbol.dispose] = r;
	return r;
}
//#endregion
//#region node_modules/@dnd-kit/state/dist/index.mjs
var __create$5 = Object.create;
var __defProp$6 = Object.defineProperty;
var __defProps$5 = Object.defineProperties;
var __getOwnPropDesc$5 = Object.getOwnPropertyDescriptor;
var __getOwnPropDescs$5 = Object.getOwnPropertyDescriptors;
var __getOwnPropSymbols$6 = Object.getOwnPropertySymbols;
var __hasOwnProp$6 = Object.prototype.hasOwnProperty;
var __propIsEnum$6 = Object.prototype.propertyIsEnumerable;
var __knownSymbol$5 = (name, symbol) => (symbol = Symbol[name]) ? symbol : Symbol.for("Symbol." + name);
var __typeError$6 = (msg) => {
	throw TypeError(msg);
};
var __defNormalProp$6 = (obj, key, value) => key in obj ? __defProp$6(obj, key, {
	enumerable: true,
	configurable: true,
	writable: true,
	value
}) : obj[key] = value;
var __spreadValues$6 = (a, b) => {
	for (var prop in b || (b = {})) if (__hasOwnProp$6.call(b, prop)) __defNormalProp$6(a, prop, b[prop]);
	if (__getOwnPropSymbols$6) {
		for (var prop of __getOwnPropSymbols$6(b)) if (__propIsEnum$6.call(b, prop)) __defNormalProp$6(a, prop, b[prop]);
	}
	return a;
};
var __spreadProps$5 = (a, b) => __defProps$5(a, __getOwnPropDescs$5(b));
var __name$3 = (target, value) => __defProp$6(target, "name", {
	value,
	configurable: true
});
var __decoratorStart$5 = (base) => {
	var _a2;
	return [
		,
		,
		,
		__create$5((_a2 = base == null ? void 0 : base[__knownSymbol$5("metadata")]) != null ? _a2 : null)
	];
};
var __decoratorStrings$5 = [
	"class",
	"method",
	"getter",
	"setter",
	"accessor",
	"field",
	"value",
	"get",
	"set"
];
var __expectFn$5 = (fn) => fn !== void 0 && typeof fn !== "function" ? __typeError$6("Function expected") : fn;
var __decoratorContext$5 = (kind, name, done, metadata, fns) => ({
	kind: __decoratorStrings$5[kind],
	name,
	metadata,
	addInitializer: (fn) => done._ ? __typeError$6("Already initialized") : fns.push(__expectFn$5(fn || null))
});
var __decoratorMetadata$5 = (array, target) => __defNormalProp$6(target, __knownSymbol$5("metadata"), array[3]);
var __runInitializers$5 = (array, flags, self, value) => {
	for (var i = 0, fns = array[flags >> 1], n = fns && fns.length; i < n; i++) flags & 1 ? fns[i].call(self) : value = fns[i].call(self, value);
	return value;
};
var __decorateElement$5 = (array, flags, name, decorators, target, extra) => {
	var fn, it, done, ctx, access, k = flags & 7, s = !!(flags & 8), p = !!(flags & 16);
	var j = k > 3 ? array.length + 1 : k ? s ? 1 : 2 : 0, key = __decoratorStrings$5[k + 5];
	var initializers = k > 3 && (array[j - 1] = []), extraInitializers = array[j] || (array[j] = []);
	var desc = k && (!p && !s && (target = target.prototype), k < 5 && (k > 3 || !p) && __getOwnPropDesc$5(k < 4 ? target : {
		get [name]() {
			return __privateGet$6(this, extra);
		},
		set [name](x) {
			return __privateSet$6(this, extra, x);
		}
	}, name));
	k ? p && k < 4 && __name$3(extra, (k > 2 ? "set " : k > 1 ? "get " : "") + name) : __name$3(target, name);
	for (var i = decorators.length - 1; i >= 0; i--) {
		ctx = __decoratorContext$5(k, name, done = {}, array[3], extraInitializers);
		if (k) {
			ctx.static = s, ctx.private = p, access = ctx.access = { has: p ? (x) => __privateIn$3(target, x) : (x) => name in x };
			if (k ^ 3) access.get = p ? (x) => (k ^ 1 ? __privateGet$6 : __privateMethod$4)(x, target, k ^ 4 ? extra : desc.get) : (x) => x[name];
			if (k > 2) access.set = p ? (x, y) => __privateSet$6(x, target, y, k ^ 4 ? extra : desc.set) : (x, y) => x[name] = y;
		}
		it = (0, decorators[i])(k ? k < 4 ? p ? extra : desc[key] : k > 4 ? void 0 : {
			get: desc.get,
			set: desc.set
		} : target, ctx), done._ = 1;
		if (k ^ 4 || it === void 0) __expectFn$5(it) && (k > 4 ? initializers.unshift(it) : k ? p ? extra = it : desc[key] = it : target = it);
		else if (typeof it !== "object" || it === null) __typeError$6("Object expected");
		else __expectFn$5(fn = it.get) && (desc.get = fn), __expectFn$5(fn = it.set) && (desc.set = fn), __expectFn$5(fn = it.init) && initializers.unshift(fn);
	}
	return k || __decoratorMetadata$5(array, target), desc && __defProp$6(target, name, desc), p ? k ^ 4 ? extra : desc : target;
};
var __accessCheck$6 = (obj, member, msg) => member.has(obj) || __typeError$6("Cannot " + msg);
var __privateIn$3 = (member, obj) => Object(obj) !== obj ? __typeError$6("Cannot use the \"in\" operator on this value") : member.has(obj);
var __privateGet$6 = (obj, member, getter) => (__accessCheck$6(obj, member, "read from private field"), getter ? getter.call(obj) : member.get(obj));
var __privateAdd$6 = (obj, member, value) => member.has(obj) ? __typeError$6("Cannot add the same private member more than once") : member instanceof WeakSet ? member.add(obj) : member.set(obj, value);
var __privateSet$6 = (obj, member, value, setter) => (__accessCheck$6(obj, member, "write to private field"), setter ? setter.call(obj, value) : member.set(obj, value), value);
var __privateMethod$4 = (obj, member, method) => (__accessCheck$6(obj, member, "access private method"), method);
function computed(compute, comparator) {
	if (comparator) {
		let previousValue;
		return g(() => {
			const value = compute();
			if (value && previousValue && comparator(previousValue, value)) return previousValue;
			previousValue = value;
			return value;
		});
	}
	return g(compute);
}
function deepEqual(a, b) {
	if (Object.is(a, b)) return true;
	if (a === null || b === null) return false;
	if (typeof a === "function" && typeof b === "function") return a === b;
	if (a instanceof Set && b instanceof Set) {
		if (a.size !== b.size) return false;
		for (const value of a) if (!b.has(value)) return false;
		return true;
	}
	if (Array.isArray(a)) {
		if (!Array.isArray(b) || a.length !== b.length) return false;
		return !a.some((value, index) => !deepEqual(value, b[index]));
	}
	if (typeof a === "object" && typeof b === "object") {
		const aKeys = Object.keys(a);
		const bKeys = Object.keys(b);
		if (aKeys.length !== bKeys.length) return false;
		return !aKeys.some((key) => !deepEqual(a[key], b[key]));
	}
	return false;
}
function reactive({ get }, _) {
	return {
		init(value) {
			return y(value);
		},
		get() {
			return get.call(this).value;
		},
		set(newValue) {
			const current = get.call(this);
			if (current.peek() === newValue) return;
			current.value = newValue;
		}
	};
}
function derived(target, _) {
	const map = /* @__PURE__ */ new WeakMap();
	return function() {
		let result = map.get(this);
		if (!result) {
			result = computed(target.bind(this));
			map.set(this, result);
		}
		return result.value;
	};
}
function enumerable(enumerable2 = true) {
	return function(_value, context) {
		context.addInitializer(function() {
			const host = context.kind === "field" ? this : context.static ? this : Object.getPrototypeOf(this);
			const descriptor = Object.getOwnPropertyDescriptor(host, context.name);
			if (descriptor) Object.defineProperty(host, context.name, __spreadProps$5(__spreadValues$6({}, descriptor), { enumerable: enumerable2 }));
		});
	};
}
function effects(...entries) {
	const effects2 = entries.map((fn) => j(fn));
	return () => effects2.forEach((cleanup) => cleanup());
}
var _previous_dec, _initial_dec, _current_dec$1, _current_dec2, _previous_dec2, _initial_dec2 = [reactive], _init$5, _initial, _a$3, initial_get, initial_set, _ValueHistory_instances, _previous, _b$1, previous_get, previous_set, _current, _c$2, current_get, current_set;
_previous_dec2 = [reactive], _current_dec2 = [reactive], _current_dec$1 = [enumerable()], _initial_dec = [enumerable()], _previous_dec = [enumerable()];
var ValueHistory = class {
	constructor(defaultValue, equals = Object.is) {
		this.defaultValue = defaultValue;
		this.equals = equals;
		__runInitializers$5(_init$5, 5, this);
		__privateAdd$6(this, _ValueHistory_instances);
		__privateAdd$6(this, _initial, __runInitializers$5(_init$5, 8, this)), __runInitializers$5(_init$5, 11, this);
		__privateAdd$6(this, _previous, __runInitializers$5(_init$5, 12, this)), __runInitializers$5(_init$5, 15, this);
		__privateAdd$6(this, _current, __runInitializers$5(_init$5, 16, this)), __runInitializers$5(_init$5, 19, this);
		this.reset = this.reset.bind(this);
		this.reset();
	}
	get current() {
		return __privateGet$6(this, _ValueHistory_instances, current_get);
	}
	get initial() {
		return __privateGet$6(this, _ValueHistory_instances, initial_get);
	}
	get previous() {
		return __privateGet$6(this, _ValueHistory_instances, previous_get);
	}
	/** Set the current value */
	set current(value) {
		const current = o(() => __privateGet$6(this, _ValueHistory_instances, current_get));
		if (value && current && this.equals(current, value)) return;
		n(() => {
			if (!__privateGet$6(this, _ValueHistory_instances, initial_get)) __privateSet$6(this, _ValueHistory_instances, value, initial_set);
			__privateSet$6(this, _ValueHistory_instances, current, previous_set);
			__privateSet$6(this, _ValueHistory_instances, value, current_set);
		});
	}
	/** Reset the state to the initial value */
	reset(value = this.defaultValue) {
		n(() => {
			__privateSet$6(this, _ValueHistory_instances, void 0, previous_set);
			__privateSet$6(this, _ValueHistory_instances, value, initial_set);
			__privateSet$6(this, _ValueHistory_instances, value, current_set);
		});
	}
};
_init$5 = __decoratorStart$5(null);
_initial = /* @__PURE__ */ new WeakMap();
_ValueHistory_instances = /* @__PURE__ */ new WeakSet();
_previous = /* @__PURE__ */ new WeakMap();
_current = /* @__PURE__ */ new WeakMap();
_a$3 = __decorateElement$5(_init$5, 20, "#initial", _initial_dec2, _ValueHistory_instances, _initial), initial_get = _a$3.get, initial_set = _a$3.set;
_b$1 = __decorateElement$5(_init$5, 20, "#previous", _previous_dec2, _ValueHistory_instances, _previous), previous_get = _b$1.get, previous_set = _b$1.set;
_c$2 = __decorateElement$5(_init$5, 20, "#current", _current_dec2, _ValueHistory_instances, _current), current_get = _c$2.get, current_set = _c$2.set;
__decorateElement$5(_init$5, 2, "current", _current_dec$1, ValueHistory);
__decorateElement$5(_init$5, 2, "initial", _initial_dec, ValueHistory);
__decorateElement$5(_init$5, 2, "previous", _previous_dec, ValueHistory);
__decoratorMetadata$5(_init$5, ValueHistory);
function snapshot(value) {
	return o(() => {
		const output = {};
		for (const key in value) output[key] = value[key];
		return output;
	});
}
var _store;
var WeakStore = class {
	constructor() {
		__privateAdd$6(this, _store, /* @__PURE__ */ new WeakMap());
	}
	get(key, id) {
		var _a2;
		return key ? (_a2 = __privateGet$6(this, _store).get(key)) == null ? void 0 : _a2.get(id) : void 0;
	}
	set(key, id, value) {
		var _a2;
		if (!key) return;
		if (!__privateGet$6(this, _store).has(key)) __privateGet$6(this, _store).set(key, /* @__PURE__ */ new Map());
		return (_a2 = __privateGet$6(this, _store).get(key)) == null ? void 0 : _a2.set(id, value);
	}
	clear(key) {
		var _a2;
		return key ? (_a2 = __privateGet$6(this, _store).get(key)) == null ? void 0 : _a2.clear() : void 0;
	}
};
_store = /* @__PURE__ */ new WeakMap();
//#endregion
//#region node_modules/@dnd-kit/geometry/dist/index.mjs
var __create$4 = Object.create;
var __defProp$5 = Object.defineProperty;
var __getOwnPropDesc$4 = Object.getOwnPropertyDescriptor;
var __getOwnPropSymbols$5 = Object.getOwnPropertySymbols;
var __hasOwnProp$5 = Object.prototype.hasOwnProperty;
var __propIsEnum$5 = Object.prototype.propertyIsEnumerable;
var __knownSymbol$4 = (name, symbol) => (symbol = Symbol[name]) ? symbol : Symbol.for("Symbol." + name);
var __typeError$5 = (msg) => {
	throw TypeError(msg);
};
var __pow = Math.pow;
var __defNormalProp$5 = (obj, key, value) => key in obj ? __defProp$5(obj, key, {
	enumerable: true,
	configurable: true,
	writable: true,
	value
}) : obj[key] = value;
var __spreadValues$5 = (a, b) => {
	for (var prop in b || (b = {})) if (__hasOwnProp$5.call(b, prop)) __defNormalProp$5(a, prop, b[prop]);
	if (__getOwnPropSymbols$5) {
		for (var prop of __getOwnPropSymbols$5(b)) if (__propIsEnum$5.call(b, prop)) __defNormalProp$5(a, prop, b[prop]);
	}
	return a;
};
var __name$2 = (target, value) => __defProp$5(target, "name", {
	value,
	configurable: true
});
var __decoratorStart$4 = (base) => {
	var _a2;
	return [
		,
		,
		,
		__create$4((_a2 = base == null ? void 0 : base[__knownSymbol$4("metadata")]) != null ? _a2 : null)
	];
};
var __decoratorStrings$4 = [
	"class",
	"method",
	"getter",
	"setter",
	"accessor",
	"field",
	"value",
	"get",
	"set"
];
var __expectFn$4 = (fn) => fn !== void 0 && typeof fn !== "function" ? __typeError$5("Function expected") : fn;
var __decoratorContext$4 = (kind, name, done, metadata, fns) => ({
	kind: __decoratorStrings$4[kind],
	name,
	metadata,
	addInitializer: (fn) => done._ ? __typeError$5("Already initialized") : fns.push(__expectFn$4(fn || null))
});
var __decoratorMetadata$4 = (array, target) => __defNormalProp$5(target, __knownSymbol$4("metadata"), array[3]);
var __runInitializers$4 = (array, flags, self, value) => {
	for (var i = 0, fns = array[flags >> 1], n = fns && fns.length; i < n; i++) flags & 1 ? fns[i].call(self) : value = fns[i].call(self, value);
	return value;
};
var __decorateElement$4 = (array, flags, name, decorators, target, extra) => {
	var fn, it, done, ctx, access, k = flags & 7, s = !!(flags & 8), p = !!(flags & 16);
	var j = k > 3 ? array.length + 1 : k ? s ? 1 : 2 : 0, key = __decoratorStrings$4[k + 5];
	var initializers = k > 3 && (array[j - 1] = []), extraInitializers = array[j] || (array[j] = []);
	var desc = k && (!p && !s && (target = target.prototype), k < 5 && (k > 3 || !p) && __getOwnPropDesc$4(k < 4 ? target : {
		get [name]() {
			return __privateGet$5(this, extra);
		},
		set [name](x) {
			return __privateSet$5(this, extra, x);
		}
	}, name));
	k ? p && k < 4 && __name$2(extra, (k > 2 ? "set " : k > 1 ? "get " : "") + name) : __name$2(target, name);
	for (var i = decorators.length - 1; i >= 0; i--) {
		ctx = __decoratorContext$4(k, name, done = {}, array[3], extraInitializers);
		if (k) {
			ctx.static = s, ctx.private = p, access = ctx.access = { has: p ? (x) => __privateIn$2(target, x) : (x) => name in x };
			if (k ^ 3) access.get = p ? (x) => (k ^ 1 ? __privateGet$5 : __privateMethod$3)(x, target, k ^ 4 ? extra : desc.get) : (x) => x[name];
			if (k > 2) access.set = p ? (x, y) => __privateSet$5(x, target, y, k ^ 4 ? extra : desc.set) : (x, y) => x[name] = y;
		}
		it = (0, decorators[i])(k ? k < 4 ? p ? extra : desc[key] : k > 4 ? void 0 : {
			get: desc.get,
			set: desc.set
		} : target, ctx), done._ = 1;
		if (k ^ 4 || it === void 0) __expectFn$4(it) && (k > 4 ? initializers.unshift(it) : k ? p ? extra = it : desc[key] = it : target = it);
		else if (typeof it !== "object" || it === null) __typeError$5("Object expected");
		else __expectFn$4(fn = it.get) && (desc.get = fn), __expectFn$4(fn = it.set) && (desc.set = fn), __expectFn$4(fn = it.init) && initializers.unshift(fn);
	}
	return k || __decoratorMetadata$4(array, target), desc && __defProp$5(target, name, desc), p ? k ^ 4 ? extra : desc : target;
};
var __accessCheck$5 = (obj, member, msg) => member.has(obj) || __typeError$5("Cannot " + msg);
var __privateIn$2 = (member, obj) => Object(obj) !== obj ? __typeError$5("Cannot use the \"in\" operator on this value") : member.has(obj);
var __privateGet$5 = (obj, member, getter) => (__accessCheck$5(obj, member, "read from private field"), getter ? getter.call(obj) : member.get(obj));
var __privateAdd$5 = (obj, member, value) => member.has(obj) ? __typeError$5("Cannot add the same private member more than once") : member instanceof WeakSet ? member.add(obj) : member.set(obj, value);
var __privateSet$5 = (obj, member, value, setter) => (__accessCheck$5(obj, member, "write to private field"), setter ? setter.call(obj, value) : member.set(obj, value), value);
var __privateMethod$3 = (obj, member, method) => (__accessCheck$5(obj, member, "access private method"), method);
var Point$1 = class _Point {
	/**
	* @param {number} Coordinate of the point on the horizontal axis
	* @param {number} Coordinate of the point on the vertical axis
	*/
	constructor(x, y) {
		this.x = x;
		this.y = y;
	}
	/**
	* Returns the delta between this point and another point.
	*
	* @param {Point} a - A point
	* @param {Point} b - Another point
	*/
	static delta(a, b) {
		return new _Point(a.x - b.x, a.y - b.y);
	}
	/**
	* Returns the distance (hypotenuse) between this point and another point.
	*
	* @param {Point} a - A point
	* @param {Point} b - Another point
	*/
	static distance(a, b) {
		return Math.hypot(a.x - b.x, a.y - b.y);
	}
	/**
	* Returns true if both points are equal.
	*
	* @param {Point} a - A point
	* @param {Point} b - Another point
	*/
	static equals(a, b) {
		return a.x === b.x && a.y === b.y;
	}
	static from({ x, y }) {
		return new _Point(x, y);
	}
};
var Rectangle = class _Rectangle {
	constructor(left, top, width, height) {
		this.left = left;
		this.top = top;
		this.width = width;
		this.height = height;
		this.scale = {
			x: 1,
			y: 1
		};
	}
	get inverseScale() {
		return {
			x: 1 / this.scale.x,
			y: 1 / this.scale.y
		};
	}
	translate(x, y) {
		const { top, left, width, height, scale } = this;
		const newShape = new _Rectangle(left + x, top + y, width, height);
		newShape.scale = __spreadValues$5({}, scale);
		return newShape;
	}
	get boundingRectangle() {
		const { width, height, left, top, right, bottom } = this;
		return {
			width,
			height,
			left,
			top,
			right,
			bottom
		};
	}
	get center() {
		const { left, top, right, bottom } = this;
		return new Point$1((left + right) / 2, (top + bottom) / 2);
	}
	get area() {
		const { width, height } = this;
		return width * height;
	}
	equals(shape) {
		if (!(shape instanceof _Rectangle)) return false;
		const { left, top, width, height } = this;
		return left === shape.left && top === shape.top && width === shape.width && height === shape.height;
	}
	containsPoint(point) {
		const { top, left, bottom, right } = this;
		return top <= point.y && point.y <= bottom && left <= point.x && point.x <= right;
	}
	intersectionArea(shape) {
		if (shape instanceof _Rectangle) return rectangleRectangleIntersection(this, shape);
		return 0;
	}
	intersectionRatio(shape) {
		const { area } = this;
		const intersectionArea = this.intersectionArea(shape);
		return intersectionArea / (shape.area + area - intersectionArea);
	}
	get bottom() {
		const { top, height } = this;
		return top + height;
	}
	get right() {
		const { left, width } = this;
		return left + width;
	}
	get aspectRatio() {
		const { width, height } = this;
		return width / height;
	}
	get corners() {
		return [
			{
				x: this.left,
				y: this.top
			},
			{
				x: this.right,
				y: this.top
			},
			{
				x: this.left,
				y: this.bottom
			},
			{
				x: this.right,
				y: this.bottom
			}
		];
	}
	static from({ top, left, width, height }) {
		return new _Rectangle(left, top, width, height);
	}
	static delta(a, b, alignment = {
		x: "center",
		y: "center"
	}) {
		const getCoordinate = (rect, axis) => {
			const align = alignment[axis];
			const start = axis === "x" ? rect.left : rect.top;
			const size = axis === "x" ? rect.width : rect.height;
			if (align == "start") return start;
			if (align == "end") return start + size;
			return start + size / 2;
		};
		return Point$1.delta({
			x: getCoordinate(a, "x"),
			y: getCoordinate(a, "y")
		}, {
			x: getCoordinate(b, "x"),
			y: getCoordinate(b, "y")
		});
	}
	static intersectionRatio(a, b) {
		return _Rectangle.from(a).intersectionRatio(_Rectangle.from(b));
	}
};
function rectangleRectangleIntersection(a, b) {
	const top = Math.max(b.top, a.top);
	const left = Math.max(b.left, a.left);
	const right = Math.min(b.left + b.width, a.left + a.width);
	const bottom = Math.min(b.top + b.height, a.top + a.height);
	const width = right - left;
	const height = bottom - top;
	if (left < right && top < bottom) return width * height;
	return 0;
}
var _direction_dec$1, _delta_dec$1, _a$2, _timestamp$1, _init$4;
var Position$1 = class extends (_a$2 = ValueHistory, _delta_dec$1 = [derived], _direction_dec$1 = [derived], _a$2) {
	constructor(initialValue) {
		const point = Point$1.from(initialValue);
		super(point, (a, b) => Point$1.equals(a, b));
		__runInitializers$4(_init$4, 5, this);
		__privateAdd$5(this, _timestamp$1, 0);
		this.velocity = {
			x: 0,
			y: 0
		};
	}
	get delta() {
		return Point$1.delta(this.current, this.initial);
	}
	get direction() {
		const { current, previous } = this;
		if (!previous) return null;
		const delta = {
			x: current.x - previous.x,
			y: current.y - previous.y
		};
		if (!delta.x && !delta.y) return null;
		if (Math.abs(delta.x) > Math.abs(delta.y)) return delta.x > 0 ? "right" : "left";
		return delta.y > 0 ? "down" : "up";
	}
	get current() {
		return super.current;
	}
	set current(coordinates) {
		const { current } = this;
		const point = Point$1.from(coordinates);
		const delta = {
			x: point.x - current.x,
			y: point.y - current.y
		};
		const timestamp = Date.now();
		const timeDelta = timestamp - __privateGet$5(this, _timestamp$1);
		const velocity = (delta2) => Math.round(delta2 / timeDelta * 100);
		n(() => {
			__privateSet$5(this, _timestamp$1, timestamp);
			this.velocity = {
				x: velocity(delta.x),
				y: velocity(delta.y)
			};
			super.current = point;
		});
	}
	reset(coordinates = this.defaultValue) {
		super.reset(Point$1.from(coordinates));
		this.velocity = {
			x: 0,
			y: 0
		};
	}
};
_init$4 = __decoratorStart$4(_a$2);
_timestamp$1 = /* @__PURE__ */ new WeakMap();
__decorateElement$4(_init$4, 2, "delta", _delta_dec$1, Position$1);
__decorateElement$4(_init$4, 2, "direction", _direction_dec$1, Position$1);
__decoratorMetadata$4(_init$4, Position$1);
function exceedsDistance({ x, y }, distance) {
	const dx = Math.abs(x);
	const dy = Math.abs(y);
	if (typeof distance === "number") return Math.sqrt(__pow(dx, 2) + __pow(dy, 2)) > distance;
	if ("x" in distance && "y" in distance) return dx > distance.x && dy > distance.y;
	if ("x" in distance) return dx > distance.x;
	if ("y" in distance) return dy > distance.y;
	return false;
}
var Axis$1 = /* @__PURE__ */ ((Axis2) => {
	Axis2["Horizontal"] = "x";
	Axis2["Vertical"] = "y";
	return Axis2;
})(Axis$1 || {});
var Axes = Object.values(Axis$1);
//#endregion
//#region node_modules/@dnd-kit/abstract/index.js
var __create$3 = Object.create;
var __defProp$4 = Object.defineProperty;
var __defProps$4 = Object.defineProperties;
var __getOwnPropDesc$3 = Object.getOwnPropertyDescriptor;
var __getOwnPropDescs$4 = Object.getOwnPropertyDescriptors;
var __getOwnPropSymbols$4 = Object.getOwnPropertySymbols;
var __hasOwnProp$4 = Object.prototype.hasOwnProperty;
var __propIsEnum$4 = Object.prototype.propertyIsEnumerable;
var __knownSymbol$3 = (name, symbol) => (symbol = Symbol[name]) ? symbol : Symbol.for("Symbol." + name);
var __typeError$4 = (msg) => {
	throw TypeError(msg);
};
var __defNormalProp$4 = (obj, key, value) => key in obj ? __defProp$4(obj, key, {
	enumerable: true,
	configurable: true,
	writable: true,
	value
}) : obj[key] = value;
var __spreadValues$4 = (a, b) => {
	for (var prop in b || (b = {})) if (__hasOwnProp$4.call(b, prop)) __defNormalProp$4(a, prop, b[prop]);
	if (__getOwnPropSymbols$4) {
		for (var prop of __getOwnPropSymbols$4(b)) if (__propIsEnum$4.call(b, prop)) __defNormalProp$4(a, prop, b[prop]);
	}
	return a;
};
var __spreadProps$4 = (a, b) => __defProps$4(a, __getOwnPropDescs$4(b));
var __name$1 = (target, value) => __defProp$4(target, "name", {
	value,
	configurable: true
});
var __objRest$3 = (source, exclude) => {
	var target = {};
	for (var prop in source) if (__hasOwnProp$4.call(source, prop) && exclude.indexOf(prop) < 0) target[prop] = source[prop];
	if (source != null && __getOwnPropSymbols$4) {
		for (var prop of __getOwnPropSymbols$4(source)) if (exclude.indexOf(prop) < 0 && __propIsEnum$4.call(source, prop)) target[prop] = source[prop];
	}
	return target;
};
var __decoratorStart$3 = (base) => {
	var _a;
	return [
		,
		,
		,
		__create$3((_a = base == null ? void 0 : base[__knownSymbol$3("metadata")]) != null ? _a : null)
	];
};
var __decoratorStrings$3 = [
	"class",
	"method",
	"getter",
	"setter",
	"accessor",
	"field",
	"value",
	"get",
	"set"
];
var __expectFn$3 = (fn) => fn !== void 0 && typeof fn !== "function" ? __typeError$4("Function expected") : fn;
var __decoratorContext$3 = (kind, name, done, metadata, fns) => ({
	kind: __decoratorStrings$3[kind],
	name,
	metadata,
	addInitializer: (fn) => done._ ? __typeError$4("Already initialized") : fns.push(__expectFn$3(fn || null))
});
var __decoratorMetadata$3 = (array, target) => __defNormalProp$4(target, __knownSymbol$3("metadata"), array[3]);
var __runInitializers$3 = (array, flags, self, value) => {
	for (var i = 0, fns = array[flags >> 1], n = fns && fns.length; i < n; i++) flags & 1 ? fns[i].call(self) : value = fns[i].call(self, value);
	return value;
};
var __decorateElement$3 = (array, flags, name, decorators, target, extra) => {
	var fn, it, done, ctx, access, k = flags & 7, s = !!(flags & 8), p = !!(flags & 16);
	var j = k > 3 ? array.length + 1 : k ? s ? 1 : 2 : 0, key = __decoratorStrings$3[k + 5];
	var initializers = k > 3 && (array[j - 1] = []), extraInitializers = array[j] || (array[j] = []);
	var desc = k && (!p && !s && (target = target.prototype), k < 5 && (k > 3 || !p) && __getOwnPropDesc$3(k < 4 ? target : {
		get [name]() {
			return __privateGet$4(this, extra);
		},
		set [name](x) {
			return __privateSet$4(this, extra, x);
		}
	}, name));
	k ? p && k < 4 && __name$1(extra, (k > 2 ? "set " : k > 1 ? "get " : "") + name) : __name$1(target, name);
	for (var i = decorators.length - 1; i >= 0; i--) {
		ctx = __decoratorContext$3(k, name, done = {}, array[3], extraInitializers);
		if (k) {
			ctx.static = s, ctx.private = p, access = ctx.access = { has: p ? (x) => __privateIn$1(target, x) : (x) => name in x };
			if (k ^ 3) access.get = p ? (x) => (k ^ 1 ? __privateGet$4 : __privateMethod$2)(x, target, k ^ 4 ? extra : desc.get) : (x) => x[name];
			if (k > 2) access.set = p ? (x, y) => __privateSet$4(x, target, y, k ^ 4 ? extra : desc.set) : (x, y) => x[name] = y;
		}
		it = (0, decorators[i])(k ? k < 4 ? p ? extra : desc[key] : k > 4 ? void 0 : {
			get: desc.get,
			set: desc.set
		} : target, ctx), done._ = 1;
		if (k ^ 4 || it === void 0) __expectFn$3(it) && (k > 4 ? initializers.unshift(it) : k ? p ? extra = it : desc[key] = it : target = it);
		else if (typeof it !== "object" || it === null) __typeError$4("Object expected");
		else __expectFn$3(fn = it.get) && (desc.get = fn), __expectFn$3(fn = it.set) && (desc.set = fn), __expectFn$3(fn = it.init) && initializers.unshift(fn);
	}
	return k || __decoratorMetadata$3(array, target), desc && __defProp$4(target, name, desc), p ? k ^ 4 ? extra : desc : target;
};
var __accessCheck$4 = (obj, member, msg) => member.has(obj) || __typeError$4("Cannot " + msg);
var __privateIn$1 = (member, obj) => Object(obj) !== obj ? __typeError$4("Cannot use the \"in\" operator on this value") : member.has(obj);
var __privateGet$4 = (obj, member, getter) => (__accessCheck$4(obj, member, "read from private field"), getter ? getter.call(obj) : member.get(obj));
var __privateAdd$4 = (obj, member, value) => member.has(obj) ? __typeError$4("Cannot add the same private member more than once") : member instanceof WeakSet ? member.add(obj) : member.set(obj, value);
var __privateSet$4 = (obj, member, value, setter) => (__accessCheck$4(obj, member, "write to private field"), setter ? setter.call(obj, value) : member.set(obj, value), value);
var __privateMethod$2 = (obj, member, method) => (__accessCheck$4(obj, member, "access private method"), method);
function configure(plugin, options) {
	return {
		plugin,
		options
	};
}
function configurator(plugin) {
	return (options) => {
		return configure(plugin, options);
	};
}
function descriptor(plugin) {
	if (typeof plugin === "function") return {
		plugin,
		options: void 0
	};
	return plugin;
}
var _disabled_dec = [reactive], _init$3, _disabled, _cleanupFunctions$1;
var Plugin = class {
	/**
	* Creates a new plugin instance.
	*
	* @param manager - The drag and drop manager that owns this plugin
	* @param options - Optional configuration for the plugin
	*/
	constructor(manager, options) {
		this.manager = manager;
		this.options = options;
		__privateAdd$4(this, _disabled, __runInitializers$3(_init$3, 8, this, false)), __runInitializers$3(_init$3, 11, this);
		__privateAdd$4(this, _cleanupFunctions$1, /* @__PURE__ */ new Set());
	}
	/**
	* Enables a disabled plugin instance.
	*
	* @remarks
	* This method triggers effects when called.
	*/
	enable() {
		this.disabled = false;
	}
	/**
	* Disables an enabled plugin instance.
	*
	* @remarks
	* This method triggers effects when called.
	*/
	disable() {
		this.disabled = true;
	}
	/**
	* Checks if the plugin instance is disabled.
	*
	* @returns true if the plugin is disabled
	* @remarks
	* This method does not trigger effects when accessed.
	*/
	isDisabled() {
		return o(() => {
			return this.disabled;
		});
	}
	/**
	* Configures a plugin instance with new options.
	*
	* @param options - The new options to apply
	*/
	configure(options) {
		this.options = options;
	}
	/**
	* Registers an effect that will be cleaned up when the plugin is destroyed.
	*
	* @param callback - The effect callback to register
	* @returns A function to dispose of the effect
	*/
	registerEffect(callback) {
		const dispose = j(callback.bind(this));
		__privateGet$4(this, _cleanupFunctions$1).add(dispose);
		return dispose;
	}
	/**
	* Destroys a plugin instance and cleans up its resources.
	*
	* @remarks
	* This method:
	* - Calls all registered cleanup functions
	* - Should be overridden by subclasses to clean up additional resources
	*/
	destroy() {
		__privateGet$4(this, _cleanupFunctions$1).forEach((cleanup) => cleanup());
	}
	/**
	* Configures a plugin constructor with options.
	*
	* @param options - The options to configure the constructor with
	* @returns The configured plugin constructor
	*
	* @remarks
	* This method is used to configure the options that the
	* plugin constructor will use to create plugin instances.
	*/
	static configure(options) {
		return configure(this, options);
	}
};
_init$3 = __decoratorStart$3(null);
_disabled = /* @__PURE__ */ new WeakMap();
_cleanupFunctions$1 = /* @__PURE__ */ new WeakMap();
__decorateElement$3(_init$3, 4, "disabled", _disabled_dec, Plugin, _disabled);
__decoratorMetadata$3(_init$3, Plugin);
var CorePlugin = class extends Plugin {};
var _previousValues;
var PluginRegistry = class {
	/**
	* Creates a new plugin registry.
	*
	* @param manager - The drag and drop manager that owns this registry
	*/
	constructor(manager) {
		this.manager = manager;
		this.instances = /* @__PURE__ */ new Map();
		__privateAdd$4(this, _previousValues, []);
	}
	/**
	* Gets all registered plugin instances.
	*
	* @returns An array of all active plugin instances
	*/
	get values() {
		return Array.from(this.instances.values());
	}
	/**
	* Sets the list of plugins to be used by the registry.
	*
	* @param entries - Array of plugin constructors or descriptors
	* @remarks
	* This method:
	* - Filters out duplicate plugins
	* - Unregisters plugins that are no longer in use
	* - Registers new plugins with their options
	*/
	set values(entries) {
		const descriptors = entries.map(descriptor).reduce((acc, descriptor2) => {
			const existing = acc.find(({ plugin }) => plugin === descriptor2.plugin);
			if (existing) {
				existing.options = descriptor2.options;
				return acc;
			}
			return [...acc, descriptor2];
		}, []);
		const constructors = descriptors.map(({ plugin }) => plugin);
		for (const plugin of __privateGet$4(this, _previousValues)) if (!constructors.includes(plugin)) {
			if (plugin.prototype instanceof CorePlugin) continue;
			this.unregister(plugin);
		}
		for (const { plugin, options } of descriptors) this.register(plugin, options);
		__privateSet$4(this, _previousValues, constructors);
	}
	/**
	* Gets a plugin instance by its constructor.
	*
	* @param plugin - The plugin constructor to look up
	* @returns The plugin instance or undefined if not found
	*/
	get(plugin) {
		return this.instances.get(plugin);
	}
	/**
	* Registers a new plugin instance.
	*
	* @param plugin - The plugin constructor to register
	* @param options - Optional configuration for the plugin
	* @returns The registered plugin instance
	* @remarks
	* If the plugin is already registered, its options will be updated
	* and the existing instance will be returned.
	*/
	register(plugin, options) {
		const existingInstance = this.instances.get(plugin);
		if (existingInstance) {
			if (existingInstance.options !== options) existingInstance.options = options;
			return existingInstance;
		}
		const instance = new plugin(this.manager, options);
		this.instances.set(plugin, instance);
		return instance;
	}
	/**
	* Unregisters a plugin instance.
	*
	* @param plugin - The plugin constructor to unregister
	* @remarks
	* This method:
	* - Destroys the plugin instance
	* - Removes it from the registry
	*/
	unregister(plugin) {
		const instance = this.instances.get(plugin);
		if (instance) {
			instance.destroy();
			this.instances.delete(plugin);
		}
	}
	/**
	* Destroys all registered plugin instances.
	*
	* @remarks
	* This method:
	* - Calls destroy() on all plugin instances
	* - Clears the registry
	*/
	destroy() {
		for (const plugin of this.instances.values()) plugin.destroy();
		this.instances.clear();
	}
};
_previousValues = /* @__PURE__ */ new WeakMap();
function sortCollisions(a, b) {
	if (a.priority === b.priority) {
		if (a.type === b.type) return b.value - a.value;
		return b.type - a.type;
	}
	return b.priority - a.priority;
}
var DEFAULT_VALUE = [];
var _previousCoordinates, _collisions;
var CollisionObserver = class extends Plugin {
	/**
	* Creates a new CollisionObserver instance.
	*
	* @param manager - The drag drop manager instance
	*/
	constructor(manager) {
		super(manager);
		__privateAdd$4(this, _previousCoordinates);
		__privateAdd$4(this, _collisions);
		this.computeCollisions = this.computeCollisions.bind(this);
		__privateSet$4(this, _collisions, y(DEFAULT_VALUE));
		this.destroy = effects(() => {
			const collisions = this.computeCollisions();
			const coordinates = o(() => this.manager.dragOperation.position.current);
			if (collisions !== DEFAULT_VALUE) {
				const previousCoordinates = __privateGet$4(this, _previousCoordinates);
				__privateSet$4(this, _previousCoordinates, coordinates);
				if (previousCoordinates && coordinates.x == previousCoordinates.x && coordinates.y == previousCoordinates.y) return;
			} else __privateSet$4(this, _previousCoordinates, void 0);
			__privateGet$4(this, _collisions).value = collisions;
		}, () => {
			const { dragOperation } = this.manager;
			if (dragOperation.status.initialized) this.forceUpdate();
		});
	}
	/**
	* Forces an immediate update of collision detection.
	*
	* @param immediate - If true, updates collisions immediately. If false, resets previous coordinates.
	*/
	forceUpdate(immediate = true) {
		o(() => {
			if (immediate) __privateGet$4(this, _collisions).value = this.computeCollisions();
			else __privateSet$4(this, _previousCoordinates, void 0);
		});
	}
	/**
	* Computes collisions between draggable and droppable elements.
	*
	* @param entries - Optional array of droppable elements to check. If not provided, uses all registered droppables.
	* @param collisionDetector - Optional custom collision detector function
	* @returns Array of detected collisions, sorted by priority
	*/
	computeCollisions(entries, collisionDetector) {
		const { registry, dragOperation } = this.manager;
		const { source, shape, status } = dragOperation;
		if (!status.initialized || !shape) return DEFAULT_VALUE;
		const collisions = [];
		const potentialTargets = [];
		for (const entry of entries != null ? entries : registry.droppables) {
			if (entry.disabled) continue;
			if (source && !entry.accepts(source)) continue;
			const detectCollision = collisionDetector != null ? collisionDetector : entry.collisionDetector;
			if (!detectCollision) continue;
			potentialTargets.push(entry);
			entry.shape;
			const collision = o(() => detectCollision({
				droppable: entry,
				dragOperation
			}));
			if (collision) {
				if (entry.collisionPriority != null) collision.priority = entry.collisionPriority;
				collisions.push(collision);
			}
		}
		if (potentialTargets.length === 0) return DEFAULT_VALUE;
		collisions.sort(sortCollisions);
		return collisions;
	}
	/**
	* Gets the current collisions signal value.
	*/
	get collisions() {
		return __privateGet$4(this, _collisions).value;
	}
};
_previousCoordinates = /* @__PURE__ */ new WeakMap();
_collisions = /* @__PURE__ */ new WeakMap();
var _disabled_dec2, _data_dec, _manager_dec = [reactive], _Entity_static, flushIdChanges_fn, _init2$1, _manager, _idSignal, _data, _disabled2;
_data_dec = [reactive], _disabled_dec2 = [reactive];
var _Entity = class _Entity {
	/**
	* Creates a new instance of the `Entity` class.
	*
	* @param input - An object containing the initial properties of the entity.
	* @param manager - The manager that controls the drag and drop operations.
	*/
	constructor(input, manager) {
		__privateAdd$4(this, _manager, __runInitializers$3(_init2$1, 8, this)), __runInitializers$3(_init2$1, 11, this);
		/**
		* The unique identifier of the entity.
		*
		* Setting this property defers the signal update to a microtask,
		* batching multiple id changes together atomically. This ensures
		* that when entities swap ids (e.g. during sorting with virtualization),
		* all registry updates happen in a single transaction.
		*/
		__privateAdd$4(this, _idSignal);
		__privateAdd$4(this, _data, __runInitializers$3(_init2$1, 12, this)), __runInitializers$3(_init2$1, 15, this);
		__privateAdd$4(this, _disabled2, __runInitializers$3(_init2$1, 16, this)), __runInitializers$3(_init2$1, 19, this);
		const { effects: effects6, id, data = {}, disabled = false, register = true } = input;
		let previousId = id;
		__privateSet$4(this, _idSignal, y(id));
		this.manager = manager;
		this.data = data;
		this.disabled = disabled;
		this.effects = () => {
			var _a;
			return [() => {
				const { id: id2, manager: manager2 } = this;
				if (id2 === previousId) return;
				previousId = id2;
				manager2?.registry.register(this);
				return () => manager2 == null ? void 0 : manager2.registry.unregister(this);
			}, ...(_a = effects6 == null ? void 0 : effects6()) != null ? _a : []];
		};
		this.register = this.register.bind(this);
		this.unregister = this.unregister.bind(this);
		this.destroy = this.destroy.bind(this);
		if (manager && register) queueMicrotask(this.register);
	}
	get id() {
		var _a, _b;
		const signalValue = __privateGet$4(this, _idSignal).value;
		return (_b = (_a = _Entity.pendingIdChanges) == null ? void 0 : _a.get(this)) != null ? _b : signalValue;
	}
	set id(value) {
		var _a, _b;
		if (value === ((_b = (_a = _Entity.pendingIdChanges) == null ? void 0 : _a.get(this)) != null ? _b : __privateGet$4(this, _idSignal).peek())) return;
		if (!_Entity.pendingIdChanges) {
			_Entity.pendingIdChanges = /* @__PURE__ */ new Map();
			queueMicrotask(() => {
				var _a2;
				return __privateMethod$2(_a2 = _Entity, _Entity_static, flushIdChanges_fn).call(_a2);
			});
		}
		_Entity.pendingIdChanges.set(this, value);
	}
	/**
	* A method that registers the entity with the manager.
	* @returns CleanupFunction | void
	*/
	register() {
		var _a;
		return (_a = this.manager) == null ? void 0 : _a.registry.register(this);
	}
	/**
	* A method that unregisters the entity from the manager.
	* @returns void
	*/
	unregister() {
		var _a;
		(_a = this.manager) == null || _a.registry.unregister(this);
	}
	/**
	* A method that cleans up the entity when it is no longer needed.
	* @returns void
	*/
	destroy() {
		var _a;
		(_a = this.manager) == null || _a.registry.unregister(this);
	}
};
_init2$1 = __decoratorStart$3(null);
_Entity_static = /* @__PURE__ */ new WeakSet();
flushIdChanges_fn = function() {
	const changes = _Entity.pendingIdChanges;
	_Entity.pendingIdChanges = null;
	if (changes) n(() => {
		for (const [entity, id] of changes) __privateGet$4(entity, _idSignal).value = id;
	});
};
_manager = /* @__PURE__ */ new WeakMap();
_idSignal = /* @__PURE__ */ new WeakMap();
_data = /* @__PURE__ */ new WeakMap();
_disabled2 = /* @__PURE__ */ new WeakMap();
__decorateElement$3(_init2$1, 4, "manager", _manager_dec, _Entity, _manager);
__decorateElement$3(_init2$1, 4, "data", _data_dec, _Entity, _data);
__decorateElement$3(_init2$1, 4, "disabled", _disabled_dec2, _Entity, _disabled2);
__privateAdd$4(_Entity, _Entity_static);
__decoratorMetadata$3(_init2$1, _Entity);
_Entity.pendingIdChanges = null;
var Entity = _Entity;
var EntityRegistry = class {
	constructor() {
		this.map = y(/* @__PURE__ */ new Map());
		this.cleanupFunctions = /* @__PURE__ */ new WeakMap();
		/**
		* Registers a entity in the registry.
		* @param key - The unique identifier of the entity.
		* @param value - The entity to register.
		* @returns A function that unregisters the entity.
		*/
		this.register = (key, value) => {
			const current = this.map.peek();
			const currentValue = current.get(key);
			const unregister = () => this.unregister(key, value);
			if (currentValue === value) return unregister;
			if (currentValue) {
				if (currentValue.id === key) {
					this.cleanupFunctions.get(currentValue)?.();
					this.cleanupFunctions.delete(currentValue);
				}
			}
			const updatedMap = new Map(current);
			for (const [existingKey, existingValue] of current) if (existingValue === value && existingKey !== key) {
				updatedMap.delete(existingKey);
				break;
			}
			updatedMap.set(key, value);
			this.map.value = updatedMap;
			const cleanup = effects(...value.effects());
			this.cleanupFunctions.set(value, cleanup);
			return unregister;
		};
		/**
		* Unregisters an entity from the registry.
		* @param key - The unique identifier of the entity.
		* @param value - The entity instance to unregister.
		*/
		this.unregister = (key, value) => {
			const current = this.map.peek();
			if (current.get(key) !== value) return;
			this.cleanupFunctions.get(value)?.();
			this.cleanupFunctions.delete(value);
			const updatedMap = new Map(current);
			updatedMap.delete(key);
			this.map.value = updatedMap;
		};
	}
	/**
	* Iterator for the EntityRegistry class.
	* @returns An iterator for the values in the map.
	*/
	[Symbol.iterator]() {
		return this.map.peek().values();
	}
	get value() {
		return this.map.value.values();
	}
	/**
	* Checks if a entity with the given identifier exists in the registry.
	* @param identifier - The unique identifier of the entity.
	* @returns True if the entity exists, false otherwise.
	*/
	has(identifier) {
		return this.map.value.has(identifier);
	}
	/**
	* Retrieves a entity from the registry using its identifier.
	* @param identifier - The unique identifier of the entity.
	* @returns The entity if it exists, undefined otherwise.
	*/
	get(identifier) {
		return this.map.value.get(identifier);
	}
	/**
	* Destroys all entries in the registry and clears the registry.
	*/
	destroy() {
		for (const entry of this) {
			this.cleanupFunctions.get(entry)?.();
			entry.destroy();
		}
		this.map.value = /* @__PURE__ */ new Map();
	}
};
var _isDragSource_dec, _isDragging_dec, _isDropping_dec, _status_dec, _modifiers_dec, _type_dec, _c$1, _init3$1, _type, _modifiers, _status;
var Draggable$1 = class extends (_c$1 = Entity, _type_dec = [reactive], _modifiers_dec = [reactive], _status_dec = [reactive], _isDropping_dec = [derived], _isDragging_dec = [derived], _isDragSource_dec = [derived], _c$1) {
	constructor(_a, manager) {
		var _b = _a, { modifiers, type, sensors, plugins, effects: effects6 } = _b, input = __objRest$3(_b, [
			"modifiers",
			"type",
			"sensors",
			"plugins",
			"effects"
		]);
		super(__spreadProps$4(__spreadValues$4({}, input), { effects: () => {
			var _a2;
			return [...(_a2 = effects6 == null ? void 0 : effects6()) != null ? _a2 : [], () => {
				const { manager: manager2, plugins: plugins2 } = this;
				if (!manager2 || !plugins2) return;
				for (const entry of plugins2) {
					const { plugin } = descriptor(entry);
					manager2.registry.plugins.register(plugin);
				}
			}];
		} }), manager);
		__runInitializers$3(_init3$1, 5, this);
		__privateAdd$4(this, _type, __runInitializers$3(_init3$1, 8, this)), __runInitializers$3(_init3$1, 11, this);
		__privateAdd$4(this, _modifiers, __runInitializers$3(_init3$1, 12, this)), __runInitializers$3(_init3$1, 15, this);
		__privateAdd$4(this, _status, __runInitializers$3(_init3$1, 16, this, this.isDragSource ? "dragging" : "idle")), __runInitializers$3(_init3$1, 19, this);
		this.type = type;
		this.sensors = sensors;
		this.modifiers = modifiers;
		this.alignment = input.alignment;
		this.plugins = plugins;
	}
	/**
	* Look up per-entity options for a given plugin constructor.
	*/
	pluginConfig(plugin) {
		if (!this.plugins) return void 0;
		for (const entry of this.plugins) {
			const desc = descriptor(entry);
			if (desc.plugin === plugin) return desc.options;
		}
	}
	get isDropping() {
		return this.status === "dropping" && this.isDragSource;
	}
	get isDragging() {
		return this.status === "dragging" && this.isDragSource;
	}
	get isDragSource() {
		var _a, _b;
		return ((_b = (_a = this.manager) == null ? void 0 : _a.dragOperation.source) == null ? void 0 : _b.id) === this.id;
	}
};
_init3$1 = __decoratorStart$3(_c$1);
_type = /* @__PURE__ */ new WeakMap();
_modifiers = /* @__PURE__ */ new WeakMap();
_status = /* @__PURE__ */ new WeakMap();
__decorateElement$3(_init3$1, 4, "type", _type_dec, Draggable$1, _type);
__decorateElement$3(_init3$1, 4, "modifiers", _modifiers_dec, Draggable$1, _modifiers);
__decorateElement$3(_init3$1, 4, "status", _status_dec, Draggable$1, _status);
__decorateElement$3(_init3$1, 2, "isDropping", _isDropping_dec, Draggable$1);
__decorateElement$3(_init3$1, 2, "isDragging", _isDragging_dec, Draggable$1);
__decorateElement$3(_init3$1, 2, "isDragSource", _isDragSource_dec, Draggable$1);
__decoratorMetadata$3(_init3$1, Draggable$1);
var _isDropTarget_dec, _shape_dec, _collisionPriority_dec, _collisionDetector_dec, _type_dec2, _accept_dec, _c2$1, _init4$1, _accept, _type2, _collisionDetector, _collisionPriority, _shape;
var Droppable$1 = class extends (_c2$1 = Entity, _accept_dec = [reactive], _type_dec2 = [reactive], _collisionDetector_dec = [reactive], _collisionPriority_dec = [reactive], _shape_dec = [reactive], _isDropTarget_dec = [derived], _c2$1) {
	constructor(_a, manager) {
		var _b = _a, { accept, collisionDetector, collisionPriority, type } = _b, input = __objRest$3(_b, [
			"accept",
			"collisionDetector",
			"collisionPriority",
			"type"
		]);
		super(input, manager);
		__runInitializers$3(_init4$1, 5, this);
		__privateAdd$4(this, _accept, __runInitializers$3(_init4$1, 8, this)), __runInitializers$3(_init4$1, 11, this);
		__privateAdd$4(this, _type2, __runInitializers$3(_init4$1, 12, this)), __runInitializers$3(_init4$1, 15, this);
		__privateAdd$4(this, _collisionDetector, __runInitializers$3(_init4$1, 16, this)), __runInitializers$3(_init4$1, 19, this);
		__privateAdd$4(this, _collisionPriority, __runInitializers$3(_init4$1, 20, this)), __runInitializers$3(_init4$1, 23, this);
		__privateAdd$4(this, _shape, __runInitializers$3(_init4$1, 24, this)), __runInitializers$3(_init4$1, 27, this);
		this.accept = accept;
		this.collisionDetector = collisionDetector;
		this.collisionPriority = collisionPriority;
		this.type = type;
	}
	/**
	* Checks whether or not the droppable accepts a given draggable.
	*
	* @param draggable - The draggable to check
	* @returns true if the draggable can be dropped here
	*/
	accepts(draggable) {
		const { accept } = this;
		if (!accept) return true;
		if (typeof accept === "function") return accept(draggable);
		if (!draggable.type) return false;
		if (Array.isArray(accept)) return accept.includes(draggable.type);
		return draggable.type === accept;
	}
	get isDropTarget() {
		var _a, _b;
		return ((_b = (_a = this.manager) == null ? void 0 : _a.dragOperation.target) == null ? void 0 : _b.id) === this.id;
	}
};
_init4$1 = __decoratorStart$3(_c2$1);
_accept = /* @__PURE__ */ new WeakMap();
_type2 = /* @__PURE__ */ new WeakMap();
_collisionDetector = /* @__PURE__ */ new WeakMap();
_collisionPriority = /* @__PURE__ */ new WeakMap();
_shape = /* @__PURE__ */ new WeakMap();
__decorateElement$3(_init4$1, 4, "accept", _accept_dec, Droppable$1, _accept);
__decorateElement$3(_init4$1, 4, "type", _type_dec2, Droppable$1, _type2);
__decorateElement$3(_init4$1, 4, "collisionDetector", _collisionDetector_dec, Droppable$1, _collisionDetector);
__decorateElement$3(_init4$1, 4, "collisionPriority", _collisionPriority_dec, Droppable$1, _collisionPriority);
__decorateElement$3(_init4$1, 4, "shape", _shape_dec, Droppable$1, _shape);
__decorateElement$3(_init4$1, 2, "isDropTarget", _isDropTarget_dec, Droppable$1);
__decoratorMetadata$3(_init4$1, Droppable$1);
var Monitor = class {
	constructor() {
		this.registry = /* @__PURE__ */ new Map();
	}
	/**
	* Adds an event listener for the specified event type.
	*
	* @param name - The name of the event to listen for
	* @param handler - The function to call when the event occurs
	* @returns A function to remove the event listener
	*/
	addEventListener(name, handler) {
		const { registry } = this;
		const listeners = new Set(registry.get(name));
		listeners.add(handler);
		registry.set(name, listeners);
		return () => this.removeEventListener(name, handler);
	}
	/**
	* Removes an event listener for the specified event type.
	*
	* @param name - The name of the event
	* @param handler - The function to remove
	*/
	removeEventListener(name, handler) {
		const { registry } = this;
		const listeners = new Set(registry.get(name));
		listeners.delete(handler);
		registry.set(name, listeners);
	}
	/**
	* Dispatches an event to all registered listeners.
	*
	* @param name - The name of the event to dispatch
	* @param args - Arguments to pass to the event handlers
	*/
	dispatch(name, ...args) {
		const { registry } = this;
		const listeners = registry.get(name);
		if (!listeners) return;
		for (const listener of listeners) listener(...args);
	}
};
var DragDropMonitor = class extends Monitor {
	/**
	* Creates a new drag and drop monitor.
	*
	* @param manager - The drag and drop manager to monitor
	*/
	constructor(manager) {
		super();
		this.manager = manager;
	}
	/**
	* Dispatches a drag and drop event.
	*
	* @param type - The type of event to dispatch
	* @param event - The event data to dispatch
	*/
	dispatch(type, event) {
		const args = [event, this.manager];
		super.dispatch(type, ...args);
	}
};
function defaultPreventable(event, cancelable = true) {
	let defaultPrevented = false;
	return __spreadProps$4(__spreadValues$4({}, event), {
		cancelable,
		get defaultPrevented() {
			return defaultPrevented;
		},
		preventDefault() {
			if (!cancelable) return;
			defaultPrevented = true;
		}
	});
}
var CollisionNotifier = class extends CorePlugin {
	constructor(manager) {
		super(manager);
		const isEqual = (a, b) => a.map(({ id }) => id).join("") === b.map(({ id }) => id).join("");
		let previousCollisions = [];
		this.destroy = effects(() => {
			const { dragOperation, collisionObserver } = manager;
			if (dragOperation.status.initializing) {
				previousCollisions = [];
				collisionObserver.enable();
			}
		}, () => {
			const { collisionObserver, monitor } = manager;
			const { collisions } = collisionObserver;
			if (collisionObserver.isDisabled()) return;
			if (Entity.pendingIdChanges) return;
			const event = defaultPreventable({ collisions });
			monitor.dispatch("collision", event);
			if (event.defaultPrevented) return;
			if (isEqual(collisions, previousCollisions)) return;
			else previousCollisions = collisions;
			const [firstCollision] = collisions;
			o(() => {
				var _a;
				if ((firstCollision == null ? void 0 : firstCollision.id) !== ((_a = manager.dragOperation.target) == null ? void 0 : _a.id)) {
					collisionObserver.disable();
					manager.actions.setDropTarget(firstCollision == null ? void 0 : firstCollision.id).then(() => {
						collisionObserver.enable();
					});
				}
			});
		});
	}
};
var CollisionPriority = /* @__PURE__ */ ((CollisionPriority2) => {
	CollisionPriority2[CollisionPriority2["Lowest"] = 0] = "Lowest";
	CollisionPriority2[CollisionPriority2["Low"] = 1] = "Low";
	CollisionPriority2[CollisionPriority2["Normal"] = 2] = "Normal";
	CollisionPriority2[CollisionPriority2["High"] = 3] = "High";
	CollisionPriority2[CollisionPriority2["Highest"] = 4] = "Highest";
	return CollisionPriority2;
})(CollisionPriority || {});
var CollisionType = /* @__PURE__ */ ((CollisionType2) => {
	CollisionType2[CollisionType2["Collision"] = 0] = "Collision";
	CollisionType2[CollisionType2["ShapeIntersection"] = 1] = "ShapeIntersection";
	CollisionType2[CollisionType2["PointerIntersection"] = 2] = "PointerIntersection";
	return CollisionType2;
})(CollisionType || {});
var _dropped_dec, _dragging_dec, _initialized_dec, _initializing_dec, _idle_dec, _current_dec, _value_dec = [reactive], _init5$1, _value;
_current_dec = [derived], _idle_dec = [derived], _initializing_dec = [derived], _initialized_dec = [derived], _dragging_dec = [derived], _dropped_dec = [derived];
var Status = class {
	constructor() {
		__runInitializers$3(_init5$1, 5, this);
		__privateAdd$4(this, _value, __runInitializers$3(_init5$1, 8, this, "idle")), __runInitializers$3(_init5$1, 11, this);
	}
	get current() {
		return this.value;
	}
	get idle() {
		return this.value === "idle";
	}
	get initializing() {
		return this.value === "initializing";
	}
	get initialized() {
		const { value } = this;
		return value !== "idle" && value !== "initialization-pending";
	}
	get dragging() {
		return this.value === "dragging";
	}
	get dropped() {
		return this.value === "dropped";
	}
	/**
	* Sets the current status value.
	*
	* @param value - The new status value
	*/
	set(value) {
		this.value = value;
	}
};
_init5$1 = __decoratorStart$3(null);
_value = /* @__PURE__ */ new WeakMap();
__decorateElement$3(_init5$1, 4, "value", _value_dec, Status, _value);
__decorateElement$3(_init5$1, 2, "current", _current_dec, Status);
__decorateElement$3(_init5$1, 2, "idle", _idle_dec, Status);
__decorateElement$3(_init5$1, 2, "initializing", _initializing_dec, Status);
__decorateElement$3(_init5$1, 2, "initialized", _initialized_dec, Status);
__decorateElement$3(_init5$1, 2, "dragging", _dragging_dec, Status);
__decorateElement$3(_init5$1, 2, "dropped", _dropped_dec, Status);
__decoratorMetadata$3(_init5$1, Status);
var DragActions = class {
	/**
	* Creates a new instance of drag actions.
	*
	* @param manager - The drag and drop manager instance
	*/
	constructor(manager) {
		this.manager = manager;
	}
	/**
	* Sets the source of the drag operation.
	*
	* @param source - The draggable entity or its unique identifier
	*/
	setDragSource(source) {
		const { dragOperation } = this.manager;
		dragOperation.sourceIdentifier = typeof source === "string" || typeof source === "number" ? source : source.id;
	}
	/**
	* Sets the target of the drop operation.
	*
	* @param identifier - The unique identifier of the droppable entity or null/undefined
	* @returns A promise that resolves to true if the drop was prevented
	*/
	setDropTarget(identifier) {
		return o(() => {
			const { dragOperation } = this.manager;
			const id = identifier != null ? identifier : null;
			if (dragOperation.targetIdentifier === id) return Promise.resolve(false);
			dragOperation.targetIdentifier = id;
			const event = defaultPreventable({ operation: dragOperation.snapshot() });
			if (dragOperation.status.dragging) this.manager.monitor.dispatch("dragover", event);
			return this.manager.renderer.rendering.then(() => event.defaultPrevented);
		});
	}
	/**
	* Starts a new drag operation.
	*
	* @param args - Configuration for the drag operation
	* @param args.event - The event that initiated the drag
	* @param args.source - The source draggable entity or its identifier
	* @param args.coordinates - The initial coordinates of the drag
	* @returns true if the drag operation started successfully
	* @throws {Error} If there is no drag source or another operation is active
	*/
	start(args) {
		return o(() => {
			const { dragOperation } = this.manager;
			if (args.source != null) this.setDragSource(args.source);
			if (!dragOperation.source) throw new Error("Cannot start a drag operation without a drag source");
			if (!dragOperation.status.idle) throw new Error("Cannot start a drag operation while another is active");
			const controller = new AbortController();
			const { event: nativeEvent, coordinates } = args;
			n(() => {
				dragOperation.status.set("initialization-pending");
				dragOperation.shape = null;
				dragOperation.canceled = false;
				dragOperation.activatorEvent = nativeEvent != null ? nativeEvent : null;
				dragOperation.position.reset(coordinates);
			});
			const beforeStartEvent = defaultPreventable({ operation: dragOperation.snapshot() });
			this.manager.monitor.dispatch("beforedragstart", beforeStartEvent);
			if (beforeStartEvent.defaultPrevented) {
				dragOperation.reset();
				controller.abort();
				return controller;
			}
			dragOperation.status.set("initializing");
			dragOperation.controller = controller;
			this.manager.renderer.rendering.then(() => {
				if (controller.signal.aborted) return;
				const { status } = dragOperation;
				if (status.current !== "initializing") return;
				n(() => {
					dragOperation.status.set("dragging");
					this.manager.monitor.dispatch("dragstart", {
						nativeEvent,
						operation: dragOperation.snapshot(),
						cancelable: false
					});
				});
			});
			return controller;
		});
	}
	/**
	* Moves the dragged entity to a new position.
	*
	* @param args - Configuration for the move operation
	* @param args.by - Relative coordinates to move by
	* @param args.to - Absolute coordinates to move to
	* @param args.event - The event that triggered the move
	* @param args.cancelable - Whether the move can be canceled
	* @param args.propagate - Whether to dispatch dragmove events
	*/
	move(args) {
		return o(() => {
			var _a, _b;
			const { dragOperation } = this.manager;
			const { status, controller } = dragOperation;
			if (!status.dragging || !controller || controller.signal.aborted) return;
			const event = defaultPreventable({
				nativeEvent: args.event,
				operation: dragOperation.snapshot(),
				by: args.by,
				to: args.to
			}, (_a = args.cancelable) != null ? _a : true);
			if ((_b = args.propagate) != null ? _b : true) this.manager.monitor.dispatch("dragmove", event);
			queueMicrotask(() => {
				var _a2, _b2, _c3, _d, _e;
				if (event.defaultPrevented) return;
				const coordinates = (_e = args.to) != null ? _e : {
					x: dragOperation.position.current.x + ((_b2 = (_a2 = args.by) == null ? void 0 : _a2.x) != null ? _b2 : 0),
					y: dragOperation.position.current.y + ((_d = (_c3 = args.by) == null ? void 0 : _c3.y) != null ? _d : 0)
				};
				dragOperation.position.current = coordinates;
			});
		});
	}
	/**
	* Stops the current drag operation.
	*
	* @param args - Configuration for stopping the operation
	* @param args.event - The event that triggered the stop
	* @param args.canceled - Whether the operation was canceled
	* @remarks
	* This method:
	* - Dispatches a dragend event
	* - Allows suspension of the operation
	* - Handles cleanup of the operation state
	*/
	stop(args = {}) {
		return o(() => {
			var _a, _b;
			const { dragOperation } = this.manager;
			const { controller } = dragOperation;
			if (!controller || controller.signal.aborted) return;
			let promise;
			const suspend = () => {
				const output = {
					resume: () => {},
					abort: () => {}
				};
				promise = new Promise((resolve, reject) => {
					output.resume = resolve;
					output.abort = reject;
				});
				return output;
			};
			controller.abort();
			const end = () => {
				this.manager.renderer.rendering.then(() => {
					dragOperation.status.set("dropped");
					const dropping = o(() => {
						var _a2;
						return ((_a2 = dragOperation.source) == null ? void 0 : _a2.status) === "dropping";
					});
					const cleanup = () => {
						if (dragOperation.controller === controller) dragOperation.controller = void 0;
						dragOperation.reset();
					};
					if (dropping) {
						const { source } = dragOperation;
						const dispose = j(() => {
							if ((source == null ? void 0 : source.status) === "idle") {
								dispose();
								cleanup();
							}
						});
					} else this.manager.renderer.rendering.then(cleanup);
				});
			};
			dragOperation.canceled = (_a = args.canceled) != null ? _a : false;
			this.manager.monitor.dispatch("dragend", {
				nativeEvent: args.event,
				operation: dragOperation.snapshot(),
				canceled: (_b = args.canceled) != null ? _b : false,
				suspend
			});
			if (promise) promise.then(end).catch(() => dragOperation.reset());
			else end();
		});
	}
};
var Sensor = class extends Plugin {
	/**
	* Creates a new sensor instance.
	*
	* @param manager - The drag drop manager instance
	* @param options - Optional sensor configuration
	*/
	constructor(manager, options) {
		super(manager, options);
		this.manager = manager;
		this.options = options;
	}
};
var ActivationController = class extends AbortController {
	constructor(constraints, onActivate) {
		super();
		this.constraints = constraints;
		this.onActivate = onActivate;
		this.activated = false;
		for (const constraint of constraints != null ? constraints : []) constraint.controller = this;
	}
	onEvent(event) {
		var _a;
		if (this.activated) return;
		if ((_a = this.constraints) == null ? void 0 : _a.length) for (const constraint of this.constraints) constraint.onEvent(event);
		else this.activate(event);
	}
	activate(event) {
		if (this.activated) return;
		this.activated = true;
		this.onActivate(event);
	}
	abort(event) {
		this.activated = false;
		super.abort(event);
	}
};
var _controller;
var ActivationConstraint = class {
	constructor(options) {
		this.options = options;
		__privateAdd$4(this, _controller);
	}
	set controller(controller) {
		__privateSet$4(this, _controller, controller);
		controller.signal.addEventListener("abort", () => this.abort());
	}
	/**
	* Called when the activation is triggered.
	*/
	activate(event) {
		var _a;
		(_a = __privateGet$4(this, _controller)) == null || _a.activate(event);
	}
};
_controller = /* @__PURE__ */ new WeakMap();
var Modifier = class extends Plugin {
	/**
	* Creates a new modifier instance.
	*
	* @param manager - The drag and drop manager that owns this modifier
	* @param options - Optional configuration for the modifier
	*/
	constructor(manager, options) {
		super(manager, options);
		this.manager = manager;
		this.options = options;
	}
	/**
	* Applies the modifier to the current drag operation.
	*
	* @param operation - The current state of the drag operation
	* @returns The transformed coordinates
	*
	* @remarks
	* Override this method to implement custom transformation logic.
	* The default implementation returns the original transform unchanged.
	*/
	apply(operation) {
		return operation.transform;
	}
};
var DragDropRegistry = class {
	/**
	* Creates a new registry instance.
	*
	* @param manager - The drag and drop manager that owns this registry
	*/
	constructor(manager) {
		/** Registry for draggable entities */
		this.draggables = new EntityRegistry();
		/** Registry for droppable entities */
		this.droppables = new EntityRegistry();
		this.plugins = new PluginRegistry(manager);
		this.sensors = new PluginRegistry(manager);
		this.modifiers = new PluginRegistry(manager);
	}
	register(input, options) {
		if (input instanceof Draggable$1) return this.draggables.register(input.id, input);
		if (input instanceof Droppable$1) return this.droppables.register(input.id, input);
		if (input.prototype instanceof Modifier) return this.modifiers.register(input, options);
		if (input.prototype instanceof Sensor) return this.sensors.register(input, options);
		if (input.prototype instanceof Plugin) return this.plugins.register(input, options);
		throw new Error("Invalid instance type");
	}
	unregister(input) {
		if (input instanceof Entity) {
			if (input instanceof Draggable$1) return this.draggables.unregister(input.id, input);
			if (input instanceof Droppable$1) return this.droppables.unregister(input.id, input);
			return () => {};
		}
		if (input.prototype instanceof Modifier) return this.modifiers.unregister(input);
		if (input.prototype instanceof Sensor) return this.sensors.unregister(input);
		if (input.prototype instanceof Plugin) return this.plugins.unregister(input);
		throw new Error("Invalid instance type");
	}
	/**
	* Destroys all registered entities and cleans up resources.
	*
	* @remarks
	* This method:
	* - Destroys all draggable and droppable entities
	* - Destroys all plugins, sensors, and modifiers
	* - Cleans up any associated resources
	*/
	destroy() {
		this.draggables.destroy();
		this.droppables.destroy();
		this.plugins.destroy();
		this.sensors.destroy();
		this.modifiers.destroy();
	}
};
var _transform_dec, _target_dec, _source_dec, _modifiers_dec2, _targetIdentifier_dec, _sourceIdentifier_dec, _activatorEvent_dec, _canceled_dec, _shape_dec2 = [derived], _manager2, _previousSource, _shape2, _init6$1, _canceled, _activatorEvent, _sourceIdentifier, _targetIdentifier, _modifiers2, _transform;
_canceled_dec = [reactive], _activatorEvent_dec = [reactive], _sourceIdentifier_dec = [reactive], _targetIdentifier_dec = [reactive], _modifiers_dec2 = [reactive], _source_dec = [derived], _target_dec = [derived], _transform_dec = [derived];
var DragOperation = class {
	/**
	* Creates a new drag operation instance.
	*
	* @param manager - The drag and drop manager that owns this operation
	*/
	constructor(manager) {
		__runInitializers$3(_init6$1, 5, this);
		__privateAdd$4(this, _manager2);
		__privateAdd$4(this, _previousSource);
		__privateAdd$4(this, _shape2, new ValueHistory(void 0, (a, b) => a && b ? a.equals(b) : a === b));
		/** Current status of the drag operation */
		this.status = new Status();
		__privateAdd$4(this, _canceled, __runInitializers$3(_init6$1, 8, this, false)), __runInitializers$3(_init6$1, 11, this);
		__privateAdd$4(this, _activatorEvent, __runInitializers$3(_init6$1, 12, this, null)), __runInitializers$3(_init6$1, 15, this);
		__privateAdd$4(this, _sourceIdentifier, __runInitializers$3(_init6$1, 16, this, null)), __runInitializers$3(_init6$1, 19, this);
		__privateAdd$4(this, _targetIdentifier, __runInitializers$3(_init6$1, 20, this, null)), __runInitializers$3(_init6$1, 23, this);
		__privateAdd$4(this, _modifiers2, __runInitializers$3(_init6$1, 24, this, [])), __runInitializers$3(_init6$1, 27, this);
		/** Current position of the dragged entity */
		this.position = new Position$1({
			x: 0,
			y: 0
		});
		__privateAdd$4(this, _transform, {
			x: 0,
			y: 0
		});
		__privateSet$4(this, _manager2, manager);
	}
	get shape() {
		const { current, initial, previous } = __privateGet$4(this, _shape2);
		if (!current || !initial) return null;
		return {
			current,
			initial,
			previous
		};
	}
	/**
	* Sets the shape of the dragged entity.
	*
	* @param value - The new shape or null to reset
	*/
	set shape(value) {
		if (!value) __privateGet$4(this, _shape2).reset();
		else __privateGet$4(this, _shape2).current = value;
	}
	get source() {
		var _a;
		const identifier = this.sourceIdentifier;
		if (identifier == null) return null;
		const value = __privateGet$4(this, _manager2).registry.draggables.get(identifier);
		if (value) __privateSet$4(this, _previousSource, value);
		return (_a = value != null ? value : __privateGet$4(this, _previousSource)) != null ? _a : null;
	}
	get target() {
		var _a;
		const identifier = this.targetIdentifier;
		return identifier != null ? (_a = __privateGet$4(this, _manager2).registry.droppables.get(identifier)) != null ? _a : null : null;
	}
	get transform() {
		const { x, y } = this.position.delta;
		let transform = {
			x,
			y
		};
		for (const modifier of this.modifiers) transform = modifier.apply(__spreadProps$4(__spreadValues$4({}, this.snapshot()), { transform }));
		__privateSet$4(this, _transform, transform);
		return transform;
	}
	/**
	* Creates a snapshot of the current drag operation state.
	*
	* @returns An immutable snapshot of the current operation state
	*/
	snapshot() {
		return o(() => ({
			source: this.source,
			target: this.target,
			activatorEvent: this.activatorEvent,
			transform: __privateGet$4(this, _transform),
			shape: this.shape ? snapshot(this.shape) : null,
			position: snapshot(this.position),
			status: snapshot(this.status),
			canceled: this.canceled
		}));
	}
	/**
	* Resets the drag operation to its initial state.
	*
	* @remarks
	* This method:
	* - Sets status to idle
	* - Clears source and target identifiers
	* - Resets shape history
	* - Resets position and transform
	* - Clears modifiers
	*/
	reset() {
		n(() => {
			this.status.set("idle");
			this.sourceIdentifier = null;
			this.targetIdentifier = null;
			__privateGet$4(this, _shape2).reset();
			this.position.reset({
				x: 0,
				y: 0
			});
			__privateSet$4(this, _transform, {
				x: 0,
				y: 0
			});
			this.modifiers = [];
		});
	}
};
_init6$1 = __decoratorStart$3(null);
_manager2 = /* @__PURE__ */ new WeakMap();
_previousSource = /* @__PURE__ */ new WeakMap();
_shape2 = /* @__PURE__ */ new WeakMap();
_canceled = /* @__PURE__ */ new WeakMap();
_activatorEvent = /* @__PURE__ */ new WeakMap();
_sourceIdentifier = /* @__PURE__ */ new WeakMap();
_targetIdentifier = /* @__PURE__ */ new WeakMap();
_modifiers2 = /* @__PURE__ */ new WeakMap();
_transform = /* @__PURE__ */ new WeakMap();
__decorateElement$3(_init6$1, 2, "shape", _shape_dec2, DragOperation);
__decorateElement$3(_init6$1, 4, "canceled", _canceled_dec, DragOperation, _canceled);
__decorateElement$3(_init6$1, 4, "activatorEvent", _activatorEvent_dec, DragOperation, _activatorEvent);
__decorateElement$3(_init6$1, 4, "sourceIdentifier", _sourceIdentifier_dec, DragOperation, _sourceIdentifier);
__decorateElement$3(_init6$1, 4, "targetIdentifier", _targetIdentifier_dec, DragOperation, _targetIdentifier);
__decorateElement$3(_init6$1, 4, "modifiers", _modifiers_dec2, DragOperation, _modifiers2);
__decorateElement$3(_init6$1, 2, "source", _source_dec, DragOperation);
__decorateElement$3(_init6$1, 2, "target", _target_dec, DragOperation);
__decorateElement$3(_init6$1, 2, "transform", _transform_dec, DragOperation);
__decoratorMetadata$3(_init6$1, DragOperation);
var defaultRenderer = { get rendering() {
	return Promise.resolve();
} };
function resolveCustomizable(value, defaults) {
	if (typeof value === "function") return value(defaults);
	return value != null ? value : defaults;
}
var DragDropManager$1 = class {
	/**
	* Creates a new drag and drop manager instance.
	*
	* @param config - Optional configuration for plugins, sensors, modifiers, and renderer
	*/
	constructor(config) {
		/**
		* Cleans up resources and stops any active drag operations.
		*/
		this.destroy = () => {
			if (!this.dragOperation.status.idle) this.actions.stop({ canceled: true });
			this.dragOperation.modifiers.forEach((modifier) => modifier.destroy());
			this.registry.destroy();
			this.collisionObserver.destroy();
		};
		var _a;
		const raw = config != null ? config : {};
		const plugins = resolveCustomizable(raw.plugins, []);
		const sensors = resolveCustomizable(raw.sensors, []);
		const modifiers = resolveCustomizable(raw.modifiers, []);
		const renderer = (_a = raw.renderer) != null ? _a : defaultRenderer;
		const monitor = new DragDropMonitor(this);
		const registry = new DragDropRegistry(this);
		this.registry = registry;
		this.monitor = monitor;
		this.renderer = renderer;
		this.actions = new DragActions(this);
		this.dragOperation = new DragOperation(this);
		this.collisionObserver = new CollisionObserver(this);
		this.plugins = [CollisionNotifier, ...plugins];
		this.modifiers = modifiers;
		this.sensors = sensors;
		const { destroy } = this;
		const cleanup = effects(() => {
			var _a2, _b, _c3;
			const currentModifiers = o(() => this.dragOperation.modifiers);
			const managerModifiers = this.modifiers;
			for (const modifier of currentModifiers) if (!managerModifiers.includes(modifier)) modifier.destroy();
			this.dragOperation.modifiers = (_c3 = (_b = (_a2 = this.dragOperation.source) == null ? void 0 : _a2.modifiers) == null ? void 0 : _b.map((modifier) => {
				const { plugin, options } = descriptor(modifier);
				return new plugin(this, options);
			})) != null ? _c3 : managerModifiers;
		});
		this.destroy = () => {
			cleanup();
			destroy();
		};
	}
	/**
	* Gets the list of active plugins.
	*
	* @returns Array of active plugin instances
	*/
	get plugins() {
		return this.registry.plugins.values;
	}
	/**
	* Sets the list of plugins to be used by the manager.
	*
	* @param plugins - Array of plugin constructors or instances
	*/
	set plugins(plugins) {
		this.registry.plugins.values = plugins;
	}
	/**
	* Gets the list of active modifiers.
	*
	* @returns Array of active modifier instances
	*/
	get modifiers() {
		return this.registry.modifiers.values;
	}
	/**
	* Sets the list of modifiers to be used by the manager.
	*
	* @param modifiers - Array of modifier constructors or instances
	*/
	set modifiers(modifiers) {
		this.registry.modifiers.values = modifiers;
	}
	/**
	* Gets the list of active sensors.
	*
	* @returns Array of active sensor instances
	*/
	get sensors() {
		return this.registry.sensors.values;
	}
	/**
	* Sets the list of sensors to be used by the manager.
	*
	* @param sensors - Array of sensor constructors or instances
	*/
	set sensors(sensors) {
		this.registry.sensors.values = sensors;
	}
};
//#endregion
//#region node_modules/@dnd-kit/dom/utilities.js
var __typeError$3 = (msg) => {
	throw TypeError(msg);
};
var __accessCheck$3 = (obj, member, msg) => member.has(obj) || __typeError$3("Cannot " + msg);
var __privateGet$3 = (obj, member, getter) => (__accessCheck$3(obj, member, "read from private field"), member.get(obj));
var __privateAdd$3 = (obj, member, value) => member.has(obj) ? __typeError$3("Cannot add the same private member more than once") : member instanceof WeakSet ? member.add(obj) : member.set(obj, value);
var __privateSet$3 = (obj, member, value, setter) => (__accessCheck$3(obj, member, "write to private field"), member.set(obj, value), value);
var __privateMethod$1 = (obj, member, method) => (__accessCheck$3(obj, member, "access private method"), method);
function isKeyframeEffect(effect) {
	if (!effect) return false;
	if (effect instanceof KeyframeEffect) return true;
	return "getKeyframes" in effect && typeof effect.getKeyframes === "function";
}
function getFinalKeyframe(element, match) {
	const animations2 = element.getAnimations();
	let result = null;
	for (const animation of animations2) {
		if (animation.playState !== "running") continue;
		const { effect } = animation;
		const matchedKeyframes = (isKeyframeEffect(effect) ? effect.getKeyframes() : []).filter(match);
		if (matchedKeyframes.length > 0) result = [matchedKeyframes[matchedKeyframes.length - 1], animation];
	}
	return result;
}
function getBoundingRectangle(element) {
	const { width, height, top, left, bottom, right } = element.getBoundingClientRect();
	return {
		width,
		height,
		top,
		left,
		bottom,
		right
	};
}
function isWindow(element) {
	const elementString = Object.prototype.toString.call(element);
	return elementString === "[object Window]" || elementString === "[object global]";
}
function isNode(node) {
	return "nodeType" in node;
}
function getWindow(target) {
	var _a, _b, _c;
	if (!target) return window;
	if (isWindow(target)) return target;
	if (!isNode(target)) return window;
	if ("defaultView" in target) return (_a = target.defaultView) != null ? _a : window;
	return (_c = (_b = target.ownerDocument) == null ? void 0 : _b.defaultView) != null ? _c : window;
}
function isDocument(node) {
	const { Document: Document2 } = getWindow(node);
	return node instanceof Document2 || "nodeType" in node && node.nodeType === Node.DOCUMENT_NODE;
}
function isHTMLElement(node) {
	if (!node || isWindow(node)) return false;
	return node instanceof getWindow(node).HTMLElement || "namespaceURI" in node && typeof node.namespaceURI === "string" && node.namespaceURI.endsWith("html");
}
function isSVGElement(node) {
	return node instanceof getWindow(node).SVGElement || "namespaceURI" in node && typeof node.namespaceURI === "string" && node.namespaceURI.endsWith("svg");
}
function getDocument(target) {
	if (!target) return document;
	if (isWindow(target)) return target.document;
	if (!isNode(target)) return document;
	if (isDocument(target)) return target;
	if (isHTMLElement(target) || isSVGElement(target)) return target.ownerDocument;
	return document;
}
function getViewportBoundingRectangle(element) {
	var _a, _b, _c, _d;
	const { documentElement } = getDocument(element);
	const vv = getWindow(element).visualViewport;
	const width = (_a = vv == null ? void 0 : vv.width) != null ? _a : documentElement.clientWidth;
	const height = (_b = vv == null ? void 0 : vv.height) != null ? _b : documentElement.clientHeight;
	const top = (_c = vv == null ? void 0 : vv.offsetTop) != null ? _c : 0;
	const left = (_d = vv == null ? void 0 : vv.offsetLeft) != null ? _d : 0;
	return {
		top,
		left,
		right: left + width,
		bottom: top + height,
		width,
		height
	};
}
function isOverflowVisible(element, style) {
	if (isDetailsElement(element) && element.open === false) return false;
	const { overflow, overflowX, overflowY } = getComputedStyle(element);
	return overflow === "visible" && overflowX === "visible" && overflowY === "visible";
}
function isDetailsElement(element) {
	return element.tagName === "DETAILS";
}
function getVisibleBoundingRectangle(element, boundingClientRect = element.getBoundingClientRect(), margin = 0) {
	var _a, _b, _c, _d, _e;
	let rect = boundingClientRect;
	const { ownerDocument } = element;
	const ownerWindow = (_a = ownerDocument.defaultView) != null ? _a : window;
	let ancestor = element.parentElement;
	while (ancestor && ancestor !== ownerDocument.documentElement) {
		if (!isOverflowVisible(ancestor)) {
			const ancestorRect = ancestor.getBoundingClientRect();
			const marginTop = margin * (ancestorRect.bottom - ancestorRect.top);
			const marginRight = margin * (ancestorRect.right - ancestorRect.left);
			const marginBottom = margin * (ancestorRect.bottom - ancestorRect.top);
			const marginLeft = margin * (ancestorRect.right - ancestorRect.left);
			rect = {
				top: Math.max(rect.top, ancestorRect.top - marginTop),
				right: Math.min(rect.right, ancestorRect.right + marginRight),
				bottom: Math.min(rect.bottom, ancestorRect.bottom + marginBottom),
				left: Math.max(rect.left, ancestorRect.left - marginLeft),
				width: 0,
				height: 0
			};
			rect.width = rect.right - rect.left;
			rect.height = rect.bottom - rect.top;
		}
		ancestor = ancestor.parentElement;
	}
	const vv = ownerWindow.visualViewport;
	const viewportTop = (_b = vv == null ? void 0 : vv.offsetTop) != null ? _b : 0;
	const viewportLeft = (_c = vv == null ? void 0 : vv.offsetLeft) != null ? _c : 0;
	const viewportWidth = (_d = vv == null ? void 0 : vv.width) != null ? _d : ownerWindow.innerWidth;
	const viewportHeight = (_e = vv == null ? void 0 : vv.height) != null ? _e : ownerWindow.innerHeight;
	const viewportMarginY = margin * viewportHeight;
	const viewportMarginX = margin * viewportWidth;
	rect = {
		top: Math.max(rect.top, viewportTop - viewportMarginY),
		right: Math.min(rect.right, viewportLeft + viewportWidth + viewportMarginX),
		bottom: Math.min(rect.bottom, viewportTop + viewportHeight + viewportMarginY),
		left: Math.max(rect.left, viewportLeft - viewportMarginX),
		width: 0,
		height: 0
	};
	rect.width = rect.right - rect.left;
	rect.height = rect.bottom - rect.top;
	if (rect.width < 0) rect.width = 0;
	if (rect.height < 0) rect.height = 0;
	return rect;
}
function getEventCoordinates(event) {
	return {
		x: event.clientX,
		y: event.clientY
	};
}
var canUseDOM = typeof window !== "undefined" && typeof window.document !== "undefined" && typeof window.document.createElement !== "undefined";
function getDocuments(rootDoc = document, seen = /* @__PURE__ */ new Set()) {
	if (seen.has(rootDoc)) return [];
	seen.add(rootDoc);
	const docs = [rootDoc];
	for (const frame of Array.from(rootDoc.querySelectorAll("iframe, frame"))) try {
		const childDoc = frame.contentDocument;
		if (childDoc && !seen.has(childDoc)) docs.push(...getDocuments(childDoc, seen));
	} catch (e) {}
	try {
		const win = rootDoc.defaultView;
		if (win && win !== window.top) {
			const parentWin = win.parent;
			if (parentWin && parentWin.document && parentWin.document !== rootDoc) docs.push(...getDocuments(parentWin.document, seen));
		}
	} catch (e) {}
	return docs;
}
function isSafari() {
	return /^((?!chrome|android).)*safari/i.test(navigator.userAgent);
}
function getFixedPositionOffset() {
	var _a, _b;
	const vv = isSafari() ? window.visualViewport : null;
	return {
		x: (_a = vv == null ? void 0 : vv.offsetLeft) != null ? _a : 0,
		y: (_b = vv == null ? void 0 : vv.offsetTop) != null ? _b : 0
	};
}
function isShadowRoot(target) {
	if (!target || !isNode(target)) return false;
	return target instanceof getWindow(target).ShadowRoot;
}
function getRoot(target) {
	if (target && isNode(target)) {
		let root = target.getRootNode();
		if (isShadowRoot(root)) return root;
		else if (root instanceof Document) return root;
	}
	return getDocument(target);
}
function prefersReducedMotion(window2) {
	return window2.matchMedia("(prefers-reduced-motion: reduce)").matches;
}
function cloneElement(element) {
	const selector = "input, textarea, select, canvas, [contenteditable]";
	const clonedElement = element.cloneNode(true);
	const fields = Array.from(element.querySelectorAll(selector));
	Array.from(clonedElement.querySelectorAll(selector)).forEach((field, index) => {
		const originalField = fields[index];
		if (isField(field) && isField(originalField)) {
			if (field.type !== "file") field.value = originalField.value;
			if (field.type === "radio" && field.name) field.name = `Cloned__${field.name}`;
		}
		if (isCanvasElement(field) && isCanvasElement(originalField) && originalField.width > 0 && originalField.height > 0) field.getContext("2d")?.drawImage(originalField, 0, 0);
	});
	return clonedElement;
}
function isField(element) {
	return "value" in element;
}
function isCanvasElement(element) {
	return element.tagName === "CANVAS";
}
function getElementFromPoint(root, { x, y }) {
	const element = root.elementFromPoint(x, y);
	if (isIFrameElement(element)) {
		const { contentDocument } = element;
		if (contentDocument) {
			const { left, top } = element.getBoundingClientRect();
			return getElementFromPoint(contentDocument, {
				x: x - left,
				y: y - top
			});
		}
	}
	return element;
}
function isIFrameElement(element) {
	return (element == null ? void 0 : element.tagName) === "IFRAME";
}
var ProxiedElements = /* @__PURE__ */ new WeakMap();
function isInteractiveElement(element) {
	return Boolean(element.closest(`
      input:not([disabled]),
      select:not([disabled]),
      textarea:not([disabled]),
      button:not([disabled]),
      a[href],
      [contenteditable]:not([contenteditable="false"])
    `));
}
var Listeners = class {
	constructor() {
		this.entries = /* @__PURE__ */ new Set();
		this.clear = () => {
			for (const entry of this.entries) {
				const [target, { type, listener, options }] = entry;
				target.removeEventListener(type, listener, options);
			}
			this.entries.clear();
		};
	}
	bind(target, input) {
		const eventTargets = Array.isArray(target) ? target : [target];
		const listeners = Array.isArray(input) ? input : [input];
		const entries = [];
		for (const target2 of eventTargets) for (const descriptor of listeners) {
			const { type, listener, options } = descriptor;
			const entry = [target2, descriptor];
			target2.addEventListener(type, listener, options);
			this.entries.add(entry);
			entries.push(entry);
		}
		const allEntries = this.entries;
		return function cleanup() {
			for (const entry of entries) {
				const [target2, { type, listener, options }] = entry;
				target2.removeEventListener(type, listener, options);
				allEntries.delete(entry);
			}
		};
	}
};
function getFrameElement(el) {
	const refWindow = el == null ? void 0 : el.ownerDocument.defaultView;
	if (refWindow && refWindow.self !== refWindow.parent) return refWindow.frameElement;
}
function getFrameElements(el) {
	const frames = /* @__PURE__ */ new Set();
	let frame = getFrameElement(el);
	while (frame) {
		frames.add(frame);
		frame = getFrameElement(frame);
	}
	return frames;
}
function timeout(callback, duration) {
	const id = setTimeout(callback, duration);
	return () => clearTimeout(id);
}
function throttle(func, limit) {
	const time = () => performance.now();
	let cancel;
	let lastRan;
	return function(...args) {
		const context = this;
		if (!lastRan) {
			func.apply(context, args);
			lastRan = time();
		} else {
			cancel?.();
			cancel = timeout(() => {
				func.apply(context, args);
				lastRan = time();
			}, limit - (time() - lastRan));
		}
	};
}
function isRectEqual(a, b) {
	if (a === b) return true;
	if (!a || !b) return false;
	return a.top == b.top && a.left == b.left && a.right == b.right && a.bottom == b.bottom;
}
function isVisible(element, boundingClientRect = element.getBoundingClientRect()) {
	const { width, height } = getVisibleBoundingRectangle(element, boundingClientRect);
	return width > 0 && height > 0;
}
var Observer = canUseDOM ? ResizeObserver : class MockResizeObserver {
	observe() {}
	unobserve() {}
	disconnect() {}
};
var _initialized;
var ResizeNotifier = class extends Observer {
	constructor(callback) {
		super((entries) => {
			if (!__privateGet$3(this, _initialized)) {
				__privateSet$3(this, _initialized, true);
				return;
			}
			callback(entries, this);
		});
		__privateAdd$3(this, _initialized, false);
	}
};
_initialized = /* @__PURE__ */ new WeakMap();
var threshold = Array.from({ length: 100 }, (_, index) => index / 100);
var THROTTLE_INTERVAL = 75;
var _visible, _previousBoundingClientRect, _resizeObserver, _positionObserver, _visibilityObserver, _debug, _disconnected, _observePosition, _PositionObserver_instances, notify_fn, updateDebug_fn;
var PositionObserver = class {
	constructor(element, callback, options = {
		debug: false,
		skipInitial: false
	}) {
		this.element = element;
		this.callback = callback;
		__privateAdd$3(this, _PositionObserver_instances);
		this.disconnect = () => {
			var _a, _b, _c;
			__privateSet$3(this, _disconnected, true);
			(_a = __privateGet$3(this, _resizeObserver)) == null || _a.disconnect();
			(_b = __privateGet$3(this, _positionObserver)) == null || _b.disconnect();
			__privateGet$3(this, _visibilityObserver).disconnect();
			(_c = __privateGet$3(this, _debug)) == null || _c.remove();
		};
		__privateAdd$3(this, _visible, true);
		__privateAdd$3(this, _previousBoundingClientRect);
		__privateAdd$3(this, _resizeObserver);
		__privateAdd$3(this, _positionObserver);
		__privateAdd$3(this, _visibilityObserver);
		__privateAdd$3(this, _debug);
		__privateAdd$3(this, _disconnected, false);
		__privateAdd$3(this, _observePosition, throttle(() => {
			var _a, _b, _c;
			const { element } = this;
			(_a = __privateGet$3(this, _positionObserver)) == null || _a.disconnect();
			if (__privateGet$3(this, _disconnected) || !__privateGet$3(this, _visible) || !element.isConnected) return;
			const root = (_b = element.ownerDocument) != null ? _b : document;
			const { innerHeight, innerWidth } = (_c = root.defaultView) != null ? _c : window;
			const clientRect = element.getBoundingClientRect();
			const { top, left, bottom, right } = getVisibleBoundingRectangle(element, clientRect);
			const insetTop = -Math.floor(top);
			const insetLeft = -Math.floor(left);
			const rootMargin = `${insetTop}px ${-Math.floor(innerWidth - right)}px ${-Math.floor(innerHeight - bottom)}px ${insetLeft}px`;
			this.boundingClientRect = clientRect;
			__privateSet$3(this, _positionObserver, new IntersectionObserver((entries) => {
				const [entry] = entries;
				const { intersectionRect } = entry;
				if ((entry.intersectionRatio !== 1 ? entry.intersectionRatio : Rectangle.intersectionRatio(intersectionRect, getVisibleBoundingRectangle(element))) !== 1) __privateGet$3(this, _observePosition).call(this);
			}, {
				threshold,
				rootMargin,
				root
			}));
			__privateGet$3(this, _positionObserver).observe(element);
			__privateMethod$1(this, _PositionObserver_instances, notify_fn).call(this);
		}, THROTTLE_INTERVAL));
		this.boundingClientRect = element.getBoundingClientRect();
		__privateSet$3(this, _visible, isVisible(element, this.boundingClientRect));
		let initial = true;
		this.callback = (boundingClientRect) => {
			if (initial) {
				initial = false;
				if (options.skipInitial) return;
			}
			callback(boundingClientRect);
		};
		const root = element.ownerDocument;
		if (options == null ? void 0 : options.debug) {
			__privateSet$3(this, _debug, document.createElement("div"));
			__privateGet$3(this, _debug).style.background = "rgba(0,0,0,0.15)";
			__privateGet$3(this, _debug).style.position = "fixed";
			__privateGet$3(this, _debug).style.pointerEvents = "none";
			root.body.appendChild(__privateGet$3(this, _debug));
		}
		__privateSet$3(this, _visibilityObserver, new IntersectionObserver((entries) => {
			var _a, _b;
			const { boundingClientRect, isIntersecting: visible } = entries[entries.length - 1];
			const { width, height } = boundingClientRect;
			const previousVisible = __privateGet$3(this, _visible);
			__privateSet$3(this, _visible, visible);
			if (!width && !height) return;
			if (previousVisible && !visible) {
				(_a = __privateGet$3(this, _positionObserver)) == null || _a.disconnect();
				this.callback(null);
				(_b = __privateGet$3(this, _resizeObserver)) == null || _b.disconnect();
				__privateSet$3(this, _resizeObserver, void 0);
				if (__privateGet$3(this, _debug)) __privateGet$3(this, _debug).style.visibility = "hidden";
			} else __privateGet$3(this, _observePosition).call(this);
			if (visible && !__privateGet$3(this, _resizeObserver)) {
				__privateSet$3(this, _resizeObserver, new ResizeNotifier(__privateGet$3(this, _observePosition)));
				__privateGet$3(this, _resizeObserver).observe(element);
			}
		}, {
			threshold,
			root
		}));
		if (__privateGet$3(this, _visible) && !options.skipInitial) this.callback(this.boundingClientRect);
		__privateGet$3(this, _visibilityObserver).observe(element);
	}
};
_visible = /* @__PURE__ */ new WeakMap();
_previousBoundingClientRect = /* @__PURE__ */ new WeakMap();
_resizeObserver = /* @__PURE__ */ new WeakMap();
_positionObserver = /* @__PURE__ */ new WeakMap();
_visibilityObserver = /* @__PURE__ */ new WeakMap();
_debug = /* @__PURE__ */ new WeakMap();
_disconnected = /* @__PURE__ */ new WeakMap();
_observePosition = /* @__PURE__ */ new WeakMap();
_PositionObserver_instances = /* @__PURE__ */ new WeakSet();
notify_fn = function() {
	if (__privateGet$3(this, _disconnected)) return;
	__privateMethod$1(this, _PositionObserver_instances, updateDebug_fn).call(this);
	if (isRectEqual(this.boundingClientRect, __privateGet$3(this, _previousBoundingClientRect))) return;
	this.callback(this.boundingClientRect);
	__privateSet$3(this, _previousBoundingClientRect, this.boundingClientRect);
};
updateDebug_fn = function() {
	if (__privateGet$3(this, _debug)) {
		const { top, left, width, height } = getVisibleBoundingRectangle(this.element);
		__privateGet$3(this, _debug).style.overflow = "hidden";
		__privateGet$3(this, _debug).style.visibility = "visible";
		__privateGet$3(this, _debug).style.top = `${Math.floor(top)}px`;
		__privateGet$3(this, _debug).style.left = `${Math.floor(left)}px`;
		__privateGet$3(this, _debug).style.width = `${Math.floor(width)}px`;
		__privateGet$3(this, _debug).style.height = `${Math.floor(height)}px`;
	}
};
var framePositionObservers = /* @__PURE__ */ new WeakMap();
var scrollListeners = /* @__PURE__ */ new WeakMap();
function addFrameListener(frame, callback) {
	let cached = framePositionObservers.get(frame);
	if (!cached) cached = {
		disconnect: new PositionObserver(frame, (boundingClientRect) => {
			const cached2 = framePositionObservers.get(frame);
			if (!cached2) return;
			cached2.callbacks.forEach((callback2) => callback2(boundingClientRect));
		}, { skipInitial: true }).disconnect,
		callbacks: /* @__PURE__ */ new Set()
	};
	cached.callbacks.add(callback);
	framePositionObservers.set(frame, cached);
	return () => {
		cached.callbacks.delete(callback);
		if (cached.callbacks.size === 0) {
			framePositionObservers.delete(frame);
			cached.disconnect();
		}
	};
}
function observeParentFrames(frames, callback) {
	const cleanup = /* @__PURE__ */ new Set();
	for (const frame of frames) {
		const remove = addFrameListener(frame, callback);
		cleanup.add(remove);
	}
	return () => cleanup.forEach((remove) => remove());
}
function addScrollListener(element, callback) {
	var _a;
	const doc = element.ownerDocument;
	if (!scrollListeners.has(doc)) {
		const controller = new AbortController();
		const listeners2 = /* @__PURE__ */ new Set();
		document.addEventListener("scroll", (event) => listeners2.forEach((listener) => listener(event)), {
			capture: true,
			passive: true,
			signal: controller.signal
		});
		scrollListeners.set(doc, {
			disconnect: () => controller.abort(),
			listeners: listeners2
		});
	}
	const { listeners, disconnect } = (_a = scrollListeners.get(doc)) != null ? _a : {};
	if (!listeners || !disconnect) return () => {};
	listeners.add(callback);
	return () => {
		listeners.delete(callback);
		if (listeners.size === 0) {
			disconnect();
			scrollListeners.delete(doc);
		}
	};
}
var _elementObserver, _disconnected2, _frames, _handleScroll;
var FrameObserver = class {
	constructor(element, callback, options) {
		this.callback = callback;
		__privateAdd$3(this, _elementObserver);
		__privateAdd$3(this, _disconnected2, false);
		__privateAdd$3(this, _frames);
		__privateAdd$3(this, _handleScroll, throttle((event) => {
			if (__privateGet$3(this, _disconnected2)) return;
			if (!event.target) return;
			if ("contains" in event.target && typeof event.target.contains === "function") {
				for (const frame of __privateGet$3(this, _frames)) if (event.target.contains(frame)) {
					this.callback(__privateGet$3(this, _elementObserver).boundingClientRect);
					break;
				}
			}
		}, THROTTLE_INTERVAL));
		const frames = getFrameElements(element);
		const unobserveParentFrames = observeParentFrames(frames, callback);
		const removeScrollListener = addScrollListener(element, __privateGet$3(this, _handleScroll));
		__privateSet$3(this, _frames, frames);
		__privateSet$3(this, _elementObserver, new PositionObserver(element, callback, options));
		this.disconnect = () => {
			if (__privateGet$3(this, _disconnected2)) return;
			__privateSet$3(this, _disconnected2, true);
			unobserveParentFrames();
			removeScrollListener();
			__privateGet$3(this, _elementObserver).disconnect();
		};
	}
};
_elementObserver = /* @__PURE__ */ new WeakMap();
_disconnected2 = /* @__PURE__ */ new WeakMap();
_frames = /* @__PURE__ */ new WeakMap();
_handleScroll = /* @__PURE__ */ new WeakMap();
function supportsPopover(element) {
	return "showPopover" in element && "hidePopover" in element && typeof element.showPopover === "function" && typeof element.hidePopover === "function";
}
function showPopover(element) {
	try {
		if (supportsPopover(element) && element.isConnected && element.hasAttribute("popover") && !element.matches(":popover-open")) element.showPopover();
	} catch (error) {}
}
function isDocumentScrollingElement(element) {
	if (!canUseDOM || !element) return false;
	return element === getDocument(element).scrollingElement;
}
function getScrollPosition(scrollableElement) {
	var _a, _b;
	const window2 = getWindow(scrollableElement);
	const rect = isDocumentScrollingElement(scrollableElement) ? getViewportBoundingRectangle(scrollableElement) : getBoundingRectangle(scrollableElement);
	const vv = window2.visualViewport;
	const dimensions = isDocumentScrollingElement(scrollableElement) ? {
		height: (_a = vv == null ? void 0 : vv.height) != null ? _a : window2.innerHeight,
		width: (_b = vv == null ? void 0 : vv.width) != null ? _b : window2.innerWidth
	} : {
		height: scrollableElement.clientHeight,
		width: scrollableElement.clientWidth
	};
	const position = {
		current: {
			x: scrollableElement.scrollLeft,
			y: scrollableElement.scrollTop
		},
		max: {
			x: scrollableElement.scrollWidth - dimensions.width,
			y: scrollableElement.scrollHeight - dimensions.height
		}
	};
	return {
		rect,
		position,
		isTop: position.current.y <= 0,
		isLeft: position.current.x <= 0,
		isBottom: position.current.y >= position.max.y,
		isRight: position.current.x >= position.max.x
	};
}
function canScroll(scrollableElement, by) {
	const { isTop, isBottom, isLeft, isRight, position } = getScrollPosition(scrollableElement);
	const { x, y } = by != null ? by : {
		x: 0,
		y: 0
	};
	const top = !isTop && position.current.y + y > 0;
	const bottom = !isBottom && position.current.y + y < position.max.y;
	const left = !isLeft && position.current.x + x > 0;
	const right = !isRight && position.current.x + x < position.max.x;
	return {
		top,
		bottom,
		left,
		right,
		x: left || right,
		y: top || bottom
	};
}
var Scheduler$1 = class {
	constructor(scheduler4) {
		this.scheduler = scheduler4;
		this.pending = false;
		this.tasks = /* @__PURE__ */ new Set();
		this.resolvers = /* @__PURE__ */ new Set();
		this.flush = () => {
			const { tasks, resolvers } = this;
			this.pending = false;
			this.tasks = /* @__PURE__ */ new Set();
			this.resolvers = /* @__PURE__ */ new Set();
			for (const task of tasks) task();
			for (const resolve of resolvers) resolve();
		};
	}
	schedule(task) {
		this.tasks.add(task);
		if (!this.pending) {
			this.pending = true;
			this.scheduler(this.flush);
		}
		return new Promise((resolve) => this.resolvers.add(resolve));
	}
};
var scheduler = new Scheduler$1((callback) => {
	if (typeof requestAnimationFrame === "function") requestAnimationFrame(callback);
	else callback();
});
var scheduler2 = new Scheduler$1((callback) => setTimeout(callback, 50));
var cachedStyles = /* @__PURE__ */ new Map();
var clear = cachedStyles.clear.bind(cachedStyles);
function getComputedStyles(element, cached = false) {
	if (!cached) return computeStyles(element);
	let styles = cachedStyles.get(element);
	if (styles) return styles;
	styles = computeStyles(element);
	cachedStyles.set(element, styles);
	scheduler2.schedule(clear);
	return styles;
}
function computeStyles(element) {
	return getWindow(element).getComputedStyle(element);
}
function isFixed(node, computedStyle = getComputedStyles(node, true)) {
	return computedStyle.position === "fixed" || computedStyle.position === "sticky";
}
function isScrollable(element, computedStyle = getComputedStyles(element, true)) {
	const overflowRegex = /(auto|scroll|overlay)/;
	return [
		"overflow",
		"overflowX",
		"overflowY"
	].some((property) => {
		const value = computedStyle[property];
		return typeof value === "string" ? overflowRegex.test(value) : false;
	});
}
var defaultOptions = {
	excludeElement: true,
	escapeShadowDOM: true
};
function getScrollableAncestors(element, options = defaultOptions) {
	const { limit, excludeElement, escapeShadowDOM } = options;
	const scrollParents = /* @__PURE__ */ new Set();
	function findScrollableAncestors(node) {
		if (limit != null && scrollParents.size >= limit) return scrollParents;
		if (!node) return scrollParents;
		if (isDocument(node) && node.scrollingElement != null && !scrollParents.has(node.scrollingElement)) {
			scrollParents.add(node.scrollingElement);
			return scrollParents;
		}
		if (escapeShadowDOM && isShadowRoot(node)) return findScrollableAncestors(node.host);
		if (!isHTMLElement(node)) {
			if (isSVGElement(node)) return findScrollableAncestors(node.parentElement);
			return scrollParents;
		}
		if (scrollParents.has(node)) return scrollParents;
		const computedStyle = getComputedStyles(node, true);
		if (excludeElement && node === element);
		else if (isScrollable(node, computedStyle)) scrollParents.add(node);
		if (isFixed(node, computedStyle)) {
			const { scrollingElement } = node.ownerDocument;
			if (scrollingElement) scrollParents.add(scrollingElement);
			return scrollParents;
		}
		return findScrollableAncestors(node.parentNode);
	}
	if (!element) return scrollParents;
	return findScrollableAncestors(element);
}
function getFrameTransform(el, boundary = window.frameElement) {
	const transform = {
		x: 0,
		y: 0,
		scaleX: 1,
		scaleY: 1
	};
	if (!el) return transform;
	let frame = getFrameElement(el);
	while (frame) {
		if (frame === boundary) return transform;
		const rect = getBoundingRectangle(frame);
		const { x: scaleX, y: scaleY } = getScale(frame, rect);
		transform.x = transform.x + rect.left;
		transform.y = transform.y + rect.top;
		transform.scaleX = transform.scaleX * scaleX;
		transform.scaleY = transform.scaleY * scaleY;
		frame = getFrameElement(frame);
	}
	return transform;
}
function getScale(element, boundingRectangle = getBoundingRectangle(element)) {
	const width = Math.round(boundingRectangle.width);
	const height = Math.round(boundingRectangle.height);
	if (isHTMLElement(element)) return {
		x: width / element.offsetWidth,
		y: height / element.offsetHeight
	};
	const styles = getComputedStyles(element, true);
	return {
		x: (parseFloat(styles.width) || width) / width,
		y: (parseFloat(styles.height) || height) / height
	};
}
function parseScale(scale) {
	if (scale === "none") return null;
	const values = scale.split(" ");
	const x = parseFloat(values[0]);
	const y = parseFloat(values[1]);
	if (isNaN(x) && isNaN(y)) return null;
	return {
		x: isNaN(x) ? y : x,
		y: isNaN(y) ? x : y
	};
}
function parseTranslate(translate) {
	if (translate === "none") return null;
	const [x, y, z = "0"] = translate.split(" ");
	const output = {
		x: parseFloat(x),
		y: parseFloat(y),
		z: parseInt(z, 10)
	};
	if (isNaN(output.x) && isNaN(output.y)) return null;
	return {
		x: isNaN(output.x) ? 0 : output.x,
		y: isNaN(output.y) ? 0 : output.y,
		z: isNaN(output.z) ? 0 : output.z
	};
}
function parseTransform(computedStyles) {
	var _a, _b, _c, _d, _e, _f, _g, _h, _i;
	const { scale, transform, translate } = computedStyles;
	const parsedScale = parseScale(scale);
	const parsedTranslate = parseTranslate(translate);
	const parsedMatrix = parseTransformMatrix(transform);
	if (!parsedMatrix && !parsedScale && !parsedTranslate) return null;
	const normalizedScale = {
		x: (_a = parsedScale == null ? void 0 : parsedScale.x) != null ? _a : 1,
		y: (_b = parsedScale == null ? void 0 : parsedScale.y) != null ? _b : 1
	};
	const normalizedTranslate = {
		x: (_c = parsedTranslate == null ? void 0 : parsedTranslate.x) != null ? _c : 0,
		y: (_d = parsedTranslate == null ? void 0 : parsedTranslate.y) != null ? _d : 0
	};
	const normalizedMatrix = {
		x: (_e = parsedMatrix == null ? void 0 : parsedMatrix.x) != null ? _e : 0,
		y: (_f = parsedMatrix == null ? void 0 : parsedMatrix.y) != null ? _f : 0,
		scaleX: (_g = parsedMatrix == null ? void 0 : parsedMatrix.scaleX) != null ? _g : 1,
		scaleY: (_h = parsedMatrix == null ? void 0 : parsedMatrix.scaleY) != null ? _h : 1
	};
	return {
		x: normalizedTranslate.x + normalizedMatrix.x,
		y: normalizedTranslate.y + normalizedMatrix.y,
		z: (_i = parsedTranslate == null ? void 0 : parsedTranslate.z) != null ? _i : 0,
		scaleX: normalizedScale.x * normalizedMatrix.scaleX,
		scaleY: normalizedScale.y * normalizedMatrix.scaleY
	};
}
function parseTransformMatrix(transform) {
	if (transform.startsWith("matrix3d(")) {
		const transformArray = transform.slice(9, -1).split(/, /);
		return {
			x: +transformArray[12],
			y: +transformArray[13],
			scaleX: +transformArray[0],
			scaleY: +transformArray[5]
		};
	} else if (transform.startsWith("matrix(")) {
		const transformArray = transform.slice(7, -1).split(/, /);
		return {
			x: +transformArray[4],
			y: +transformArray[5],
			scaleX: +transformArray[0],
			scaleY: +transformArray[3]
		};
	}
	return null;
}
var ScrollDirection = /* @__PURE__ */ ((ScrollDirection2) => {
	ScrollDirection2[ScrollDirection2["Idle"] = 0] = "Idle";
	ScrollDirection2[ScrollDirection2["Forward"] = 1] = "Forward";
	ScrollDirection2[ScrollDirection2["Reverse"] = -1] = "Reverse";
	return ScrollDirection2;
})(ScrollDirection || {});
var defaultThreshold = {
	x: .2,
	y: .2
};
var defaultTolerance = {
	x: 10,
	y: 10
};
function detectScrollIntent(scrollableElement, coordinates, intent, acceleration = 25, thresholdPercentage = defaultThreshold, tolerance = defaultTolerance) {
	const { x, y } = coordinates;
	const { rect, isTop, isBottom, isLeft, isRight } = getScrollPosition(scrollableElement);
	const frameTransform = getFrameTransform(scrollableElement);
	const parsedTransform = parseTransform(getComputedStyles(scrollableElement, true));
	const isXAxisInverted = parsedTransform !== null ? (parsedTransform == null ? void 0 : parsedTransform.scaleX) < 0 : false;
	const isYAxisInverted = parsedTransform !== null ? (parsedTransform == null ? void 0 : parsedTransform.scaleY) < 0 : false;
	const scrollContainerRect = new Rectangle(rect.left * frameTransform.scaleX + frameTransform.x, rect.top * frameTransform.scaleY + frameTransform.y, rect.width * frameTransform.scaleX, rect.height * frameTransform.scaleY);
	const direction = {
		x: 0,
		y: 0
	};
	const speed = {
		x: 0,
		y: 0
	};
	const threshold2 = {
		height: scrollContainerRect.height * thresholdPercentage.y,
		width: scrollContainerRect.width * thresholdPercentage.x
	};
	if (threshold2.height > 0 && (!isTop || isYAxisInverted && !isBottom) && y <= scrollContainerRect.top + threshold2.height && (intent == null ? void 0 : intent.y) !== 1 && x >= scrollContainerRect.left - tolerance.x && x <= scrollContainerRect.right + tolerance.x) {
		direction.y = isYAxisInverted ? 1 : -1;
		speed.y = acceleration * Math.abs((scrollContainerRect.top + threshold2.height - y) / threshold2.height);
	} else if (threshold2.height > 0 && (!isBottom || isYAxisInverted && !isTop) && y >= scrollContainerRect.bottom - threshold2.height && (intent == null ? void 0 : intent.y) !== -1 && x >= scrollContainerRect.left - tolerance.x && x <= scrollContainerRect.right + tolerance.x) {
		direction.y = isYAxisInverted ? -1 : 1;
		speed.y = acceleration * Math.abs((scrollContainerRect.bottom - threshold2.height - y) / threshold2.height);
	}
	if (threshold2.width > 0 && (!isRight || isXAxisInverted && !isLeft) && x >= scrollContainerRect.right - threshold2.width && (intent == null ? void 0 : intent.x) !== -1 && y >= scrollContainerRect.top - tolerance.y && y <= scrollContainerRect.bottom + tolerance.y) {
		direction.x = isXAxisInverted ? -1 : 1;
		speed.x = acceleration * Math.abs((scrollContainerRect.right - threshold2.width - x) / threshold2.width);
	} else if (threshold2.width > 0 && (!isLeft || isXAxisInverted && !isRight) && x <= scrollContainerRect.left + threshold2.width && (intent == null ? void 0 : intent.x) !== 1 && y >= scrollContainerRect.top - tolerance.y && y <= scrollContainerRect.bottom + tolerance.y) {
		direction.x = isXAxisInverted ? 1 : -1;
		speed.x = acceleration * Math.abs((scrollContainerRect.left + threshold2.width - x) / threshold2.width);
	}
	return {
		direction,
		speed
	};
}
function scrollIntoViewIfNeeded(el, { block = "nearest", inline = "nearest" } = {}) {
	if (!isHTMLElement(el)) return;
	const scrollableAncestors = getScrollableAncestors(el);
	const processedAncestors = [];
	for (const ancestor of scrollableAncestors) {
		if (!isHTMLElement(ancestor)) continue;
		const { top, left } = getOffsetRelativeTo(el, ancestor);
		let adjustedTop = top;
		let adjustedLeft = left;
		for (const inner of processedAncestors) {
			adjustedTop -= inner.scrollTop;
			adjustedLeft -= inner.scrollLeft;
		}
		if (block !== "none") {
			const overTop = adjustedTop < ancestor.scrollTop;
			if (overTop !== adjustedTop + el.offsetHeight > ancestor.scrollTop + ancestor.clientHeight) if (block === "center") ancestor.scrollTop = adjustedTop - ancestor.clientHeight / 2 + el.offsetHeight / 2;
			else if (overTop) ancestor.scrollTop = adjustedTop;
			else ancestor.scrollTop = adjustedTop + el.offsetHeight - ancestor.clientHeight;
		}
		if (inline !== "none") {
			const overLeft = adjustedLeft < ancestor.scrollLeft;
			if (overLeft !== adjustedLeft + el.offsetWidth > ancestor.scrollLeft + ancestor.clientWidth) if (inline === "center") ancestor.scrollLeft = adjustedLeft - ancestor.clientWidth / 2 + el.offsetWidth / 2;
			else if (overLeft) ancestor.scrollLeft = adjustedLeft;
			else ancestor.scrollLeft = adjustedLeft + el.offsetWidth - ancestor.clientWidth;
		}
		processedAncestors.push(ancestor);
	}
}
function getDocumentOffset(element) {
	let top = 0;
	let left = 0;
	let current = element;
	while (current) {
		top += current.offsetTop;
		left += current.offsetLeft;
		const offsetParent = current.offsetParent;
		if (!isHTMLElement(offsetParent)) break;
		top += offsetParent.clientTop;
		left += offsetParent.clientLeft;
		current = offsetParent;
	}
	return {
		top,
		left
	};
}
function getOffsetRelativeTo(element, ancestor) {
	const elOffset = getDocumentOffset(element);
	const ancestorOffset = getDocumentOffset(ancestor);
	return {
		top: elOffset.top - ancestorOffset.top - ancestor.clientTop,
		left: elOffset.left - ancestorOffset.left - ancestor.clientLeft
	};
}
function applyTransform(rect, parsedTransform, transformOrigin) {
	const { scaleX, scaleY, x: translateX, y: translateY } = parsedTransform;
	const x = rect.left + translateX + (1 - scaleX) * parseFloat(transformOrigin);
	const y = rect.top + translateY + (1 - scaleY) * parseFloat(transformOrigin.slice(transformOrigin.indexOf(" ") + 1));
	const w = scaleX ? rect.width * scaleX : rect.width;
	const h = scaleY ? rect.height * scaleY : rect.height;
	return {
		width: w,
		height: h,
		top: y,
		right: x + w,
		bottom: y + h,
		left: x
	};
}
function inverseTransform(rect, parsedTransform, transformOrigin) {
	const { scaleX, scaleY, x: translateX, y: translateY } = parsedTransform;
	const x = rect.left - translateX - (1 - scaleX) * parseFloat(transformOrigin);
	const y = rect.top - translateY - (1 - scaleY) * parseFloat(transformOrigin.slice(transformOrigin.indexOf(" ") + 1));
	const w = scaleX ? rect.width / scaleX : rect.width;
	const h = scaleY ? rect.height / scaleY : rect.height;
	return {
		width: w,
		height: h,
		top: y,
		right: x + w,
		bottom: y + h,
		left: x
	};
}
function animateTransform({ element, keyframes, options }) {
	return element.animate(keyframes, options).finished;
}
function computeTranslate(element, translate = getComputedStyles(element).translate, projected = true) {
	if (projected) {
		const keyframe = getFinalKeyframe(element, (keyframe2) => "translate" in keyframe2);
		if (keyframe) {
			const { translate: translate2 = "" } = keyframe[0];
			if (typeof translate2 === "string") {
				const finalTranslate = parseTranslate(translate2);
				if (finalTranslate) return finalTranslate;
			}
		}
	}
	if (translate) {
		const finalTranslate = parseTranslate(translate);
		if (finalTranslate) return finalTranslate;
	}
	return {
		x: 0,
		y: 0,
		z: 0
	};
}
var scheduler3$1 = new Scheduler$1((callback) => setTimeout(callback, 0));
var animations = /* @__PURE__ */ new Map();
var clear2 = animations.clear.bind(animations);
function getDocumentAnimations(element) {
	const document2 = element.ownerDocument;
	let documentAnimations = animations.get(document2);
	if (documentAnimations) return documentAnimations;
	documentAnimations = document2.getAnimations();
	animations.set(document2, documentAnimations);
	scheduler3$1.schedule(clear2);
	const elementAnimations = documentAnimations.filter((animation) => isKeyframeEffect(animation.effect) && animation.effect.target === element);
	animations.set(element, elementAnimations);
	return documentAnimations;
}
function forceFinishAnimations(element, options) {
	const animations2 = getDocumentAnimations(element).filter((animation) => {
		var _a, _b;
		if (isKeyframeEffect(animation.effect)) {
			const { target } = animation.effect;
			if ((_b = target && ((_a = options.isValidTarget) == null ? void 0 : _a.call(options, target))) != null ? _b : true) return animation.effect.getKeyframes().some((keyframe) => {
				for (const property of options.properties) if (keyframe[property]) return true;
			});
		}
	}).map((animation) => {
		const { effect, currentTime } = animation;
		const duration = effect == null ? void 0 : effect.getComputedTiming().duration;
		if (animation.pending || animation.playState === "finished") return;
		if (typeof duration == "number" && typeof currentTime == "number" && currentTime < duration) {
			animation.currentTime = duration;
			return () => {
				animation.currentTime = currentTime;
			};
		}
	});
	if (animations2.length > 0) return () => animations2.forEach((reset) => reset == null ? void 0 : reset());
}
var DOMRectangle = class extends Rectangle {
	constructor(element, options = {}) {
		var _a, _b, _c, _d;
		const { frameTransform = getFrameTransform(element), ignoreTransforms, getBoundingClientRect = getBoundingRectangle } = options;
		const resetAnimations = forceFinishAnimations(element, {
			properties: [
				"transform",
				"translate",
				"scale",
				"width",
				"height"
			],
			isValidTarget: (target) => (target !== element || isSafari()) && target.contains(element)
		});
		const boundingRectangle = getBoundingClientRect(element);
		let { top, left, width, height } = boundingRectangle;
		let updated;
		const computedStyles = getComputedStyles(element);
		const parsedTransform = parseTransform(computedStyles);
		const scale = {
			x: (_a = parsedTransform == null ? void 0 : parsedTransform.scaleX) != null ? _a : 1,
			y: (_b = parsedTransform == null ? void 0 : parsedTransform.scaleY) != null ? _b : 1
		};
		const projectedTransform = getProjectedTransform(element, computedStyles);
		resetAnimations?.();
		if (parsedTransform) {
			updated = inverseTransform(boundingRectangle, parsedTransform, computedStyles.transformOrigin);
			if (ignoreTransforms || projectedTransform) {
				top = updated.top;
				left = updated.left;
				width = updated.width;
				height = updated.height;
			}
		}
		const intrinsic = {
			width: (_c = updated == null ? void 0 : updated.width) != null ? _c : width,
			height: (_d = updated == null ? void 0 : updated.height) != null ? _d : height
		};
		if (projectedTransform && !ignoreTransforms && updated) {
			const projected = applyTransform(updated, projectedTransform, computedStyles.transformOrigin);
			top = projected.top;
			left = projected.left;
			width = projected.width;
			height = projected.height;
			scale.x = projectedTransform.scaleX;
			scale.y = projectedTransform.scaleY;
		}
		if (frameTransform) {
			if (!ignoreTransforms) {
				left *= frameTransform.scaleX;
				width *= frameTransform.scaleX;
				top *= frameTransform.scaleY;
				height *= frameTransform.scaleY;
			}
			left += frameTransform.x;
			top += frameTransform.y;
		}
		super(left, top, width, height);
		this.scale = scale;
		this.intrinsicWidth = intrinsic.width;
		this.intrinsicHeight = intrinsic.height;
	}
};
function getProjectedTransform(element, computedStyles) {
	const animations2 = element.getAnimations();
	if (!animations2.length) return null;
	let latestTransform;
	let latestTranslate;
	let latestScale;
	let hasAnimatedProperty = false;
	for (const animation of animations2) {
		if (animation.playState !== "running") continue;
		const keyframes = isKeyframeEffect(animation.effect) ? animation.effect.getKeyframes() : [];
		const keyframe = keyframes[keyframes.length - 1];
		if (!keyframe) continue;
		const { transform, translate, scale } = keyframe;
		if (typeof transform === "string" && transform) {
			latestTransform = transform;
			hasAnimatedProperty = true;
		}
		if (typeof translate === "string" && translate) {
			latestTranslate = translate;
			hasAnimatedProperty = true;
		}
		if (typeof scale === "string" && scale) {
			latestScale = scale;
			hasAnimatedProperty = true;
		}
	}
	if (!hasAnimatedProperty) return null;
	return parseTransform({
		transform: latestTransform != null ? latestTransform : computedStyles.transform,
		translate: latestTranslate != null ? latestTranslate : computedStyles.translate,
		scale: latestScale != null ? latestScale : computedStyles.scale
	});
}
function supportsStyle(element) {
	return "style" in element && typeof element.style === "object" && element.style !== null && "setProperty" in element.style && "removeProperty" in element.style && typeof element.style.setProperty === "function" && typeof element.style.removeProperty === "function";
}
var Styles = class {
	constructor(element) {
		this.element = element;
		this.initial = /* @__PURE__ */ new Map();
	}
	set(properties, prefix = "") {
		const { element } = this;
		if (!supportsStyle(element)) return;
		for (const [key, value] of Object.entries(properties)) {
			const property = `${prefix}${key}`;
			if (!this.initial.has(property)) this.initial.set(property, element.style.getPropertyValue(property));
			element.style.setProperty(property, typeof value === "string" ? value : `${value}px`);
		}
	}
	remove(properties, prefix = "") {
		const { element } = this;
		if (!supportsStyle(element)) return;
		for (const key of properties) {
			const property = `${prefix}${key}`;
			element.style.removeProperty(property);
		}
	}
	reset() {
		const { element } = this;
		if (!supportsStyle(element)) return;
		for (const [key, value] of this.initial) element.style.setProperty(key, value);
		if (element.getAttribute("style") === "") element.removeAttribute("style");
	}
};
function isElement(target) {
	if (!target) return false;
	return target instanceof getWindow(target).Element || isNode(target) && target.nodeType === Node.ELEMENT_NODE;
}
function isKeyboardEvent(event) {
	if (!event) return false;
	const { KeyboardEvent } = getWindow(event.target);
	return event instanceof KeyboardEvent;
}
function isPointerEvent(event) {
	if (!event) return false;
	const { PointerEvent } = getWindow(event.target);
	return event instanceof PointerEvent;
}
function isTextInput(target) {
	if (!isElement(target)) return false;
	const { tagName } = target;
	return tagName === "INPUT" || tagName === "TEXTAREA" || isContentEditable(target);
}
function isContentEditable(element) {
	return element.hasAttribute("contenteditable") && element.getAttribute("contenteditable") !== "false";
}
var ids = {};
function generateUniqueId(prefix) {
	const id = ids[prefix] == null ? 0 : ids[prefix] + 1;
	ids[prefix] = id;
	return `${prefix}-${id}`;
}
//#endregion
//#region node_modules/@dnd-kit/collision/dist/index.js
var pointerIntersection$1 = ({ dragOperation, droppable }) => {
	const pointerCoordinates = dragOperation.position.current;
	if (!pointerCoordinates) return null;
	const { id } = droppable;
	if (!droppable.shape) return null;
	if (droppable.shape.containsPoint(pointerCoordinates)) return {
		id,
		value: 1 / Point$1.distance(droppable.shape.center, pointerCoordinates),
		type: CollisionType.PointerIntersection,
		priority: CollisionPriority.High
	};
	return null;
};
var shapeIntersection$1 = ({ dragOperation, droppable }) => {
	const { shape } = dragOperation;
	if (!droppable.shape || !(shape == null ? void 0 : shape.current)) return null;
	const intersectionArea = shape.current.intersectionArea(droppable.shape);
	if (intersectionArea) {
		const { position } = dragOperation;
		const distance = Point$1.distance(droppable.shape.center, position.current);
		const value = intersectionArea / (shape.current.area + droppable.shape.area - intersectionArea) / distance;
		return {
			id: droppable.id,
			value,
			type: CollisionType.ShapeIntersection,
			priority: CollisionPriority.Normal
		};
	}
	return null;
};
var defaultCollisionDetection$1 = (args) => {
	var _a;
	return (_a = pointerIntersection$1(args)) != null ? _a : shapeIntersection$1(args);
};
var closestCorners = (input) => {
	const { dragOperation, droppable } = input;
	const { shape, position } = dragOperation;
	if (!droppable.shape) return null;
	const shapeCorners = shape ? Rectangle.from(shape.current.boundingRectangle).corners : void 0;
	const value = Rectangle.from(droppable.shape.boundingRectangle).corners.reduce((acc, corner, index) => {
		var _a;
		return acc + Point$1.distance(Point$1.from(corner), (_a = shapeCorners == null ? void 0 : shapeCorners[index]) != null ? _a : position.current);
	}, 0) / 4;
	return {
		id: droppable.id,
		value: 1 / value,
		type: CollisionType.Collision,
		priority: CollisionPriority.Normal
	};
};
//#endregion
//#region node_modules/@dnd-kit/dom/index.js
var __create$2 = Object.create;
var __defProp$3 = Object.defineProperty;
var __defProps$3 = Object.defineProperties;
var __getOwnPropDesc$2 = Object.getOwnPropertyDescriptor;
var __getOwnPropDescs$3 = Object.getOwnPropertyDescriptors;
var __getOwnPropSymbols$3 = Object.getOwnPropertySymbols;
var __hasOwnProp$3 = Object.prototype.hasOwnProperty;
var __propIsEnum$3 = Object.prototype.propertyIsEnumerable;
var __knownSymbol$2 = (name, symbol) => (symbol = Symbol[name]) ? symbol : Symbol.for("Symbol." + name);
var __typeError$2 = (msg) => {
	throw TypeError(msg);
};
var __defNormalProp$3 = (obj, key, value) => key in obj ? __defProp$3(obj, key, {
	enumerable: true,
	configurable: true,
	writable: true,
	value
}) : obj[key] = value;
var __spreadValues$3 = (a, b) => {
	for (var prop in b || (b = {})) if (__hasOwnProp$3.call(b, prop)) __defNormalProp$3(a, prop, b[prop]);
	if (__getOwnPropSymbols$3) {
		for (var prop of __getOwnPropSymbols$3(b)) if (__propIsEnum$3.call(b, prop)) __defNormalProp$3(a, prop, b[prop]);
	}
	return a;
};
var __spreadProps$3 = (a, b) => __defProps$3(a, __getOwnPropDescs$3(b));
var __name = (target, value) => __defProp$3(target, "name", {
	value,
	configurable: true
});
var __objRest$2 = (source, exclude) => {
	var target = {};
	for (var prop in source) if (__hasOwnProp$3.call(source, prop) && exclude.indexOf(prop) < 0) target[prop] = source[prop];
	if (source != null && __getOwnPropSymbols$3) {
		for (var prop of __getOwnPropSymbols$3(source)) if (exclude.indexOf(prop) < 0 && __propIsEnum$3.call(source, prop)) target[prop] = source[prop];
	}
	return target;
};
var __decoratorStart$2 = (base) => {
	var _a5;
	return [
		,
		,
		,
		__create$2((_a5 = base == null ? void 0 : base[__knownSymbol$2("metadata")]) != null ? _a5 : null)
	];
};
var __decoratorStrings$2 = [
	"class",
	"method",
	"getter",
	"setter",
	"accessor",
	"field",
	"value",
	"get",
	"set"
];
var __expectFn$2 = (fn) => fn !== void 0 && typeof fn !== "function" ? __typeError$2("Function expected") : fn;
var __decoratorContext$2 = (kind, name, done, metadata, fns) => ({
	kind: __decoratorStrings$2[kind],
	name,
	metadata,
	addInitializer: (fn) => done._ ? __typeError$2("Already initialized") : fns.push(__expectFn$2(fn || null))
});
var __decoratorMetadata$2 = (array, target) => __defNormalProp$3(target, __knownSymbol$2("metadata"), array[3]);
var __runInitializers$2 = (array, flags, self, value) => {
	for (var i = 0, fns = array[flags >> 1], n = fns && fns.length; i < n; i++) flags & 1 ? fns[i].call(self) : value = fns[i].call(self, value);
	return value;
};
var __decorateElement$2 = (array, flags, name, decorators, target, extra) => {
	var fn, it, done, ctx, access, k = flags & 7, s = !!(flags & 8), p = !!(flags & 16);
	var j = k > 3 ? array.length + 1 : k ? s ? 1 : 2 : 0, key = __decoratorStrings$2[k + 5];
	var initializers = k > 3 && (array[j - 1] = []), extraInitializers = array[j] || (array[j] = []);
	var desc = k && (!p && !s && (target = target.prototype), k < 5 && (k > 3 || !p) && __getOwnPropDesc$2(k < 4 ? target : {
		get [name]() {
			return __privateGet$2(this, extra);
		},
		set [name](x) {
			return __privateSet$2(this, extra, x);
		}
	}, name));
	k ? p && k < 4 && __name(extra, (k > 2 ? "set " : k > 1 ? "get " : "") + name) : __name(target, name);
	for (var i = decorators.length - 1; i >= 0; i--) {
		ctx = __decoratorContext$2(k, name, done = {}, array[3], extraInitializers);
		if (k) {
			ctx.static = s, ctx.private = p, access = ctx.access = { has: p ? (x) => __privateIn(target, x) : (x) => name in x };
			if (k ^ 3) access.get = p ? (x) => (k ^ 1 ? __privateGet$2 : __privateMethod)(x, target, k ^ 4 ? extra : desc.get) : (x) => x[name];
			if (k > 2) access.set = p ? (x, y) => __privateSet$2(x, target, y, k ^ 4 ? extra : desc.set) : (x, y) => x[name] = y;
		}
		it = (0, decorators[i])(k ? k < 4 ? p ? extra : desc[key] : k > 4 ? void 0 : {
			get: desc.get,
			set: desc.set
		} : target, ctx), done._ = 1;
		if (k ^ 4 || it === void 0) __expectFn$2(it) && (k > 4 ? initializers.unshift(it) : k ? p ? extra = it : desc[key] = it : target = it);
		else if (typeof it !== "object" || it === null) __typeError$2("Object expected");
		else __expectFn$2(fn = it.get) && (desc.get = fn), __expectFn$2(fn = it.set) && (desc.set = fn), __expectFn$2(fn = it.init) && initializers.unshift(fn);
	}
	return k || __decoratorMetadata$2(array, target), desc && __defProp$3(target, name, desc), p ? k ^ 4 ? extra : desc : target;
};
var __accessCheck$2 = (obj, member, msg) => member.has(obj) || __typeError$2("Cannot " + msg);
var __privateIn = (member, obj) => Object(obj) !== obj ? __typeError$2("Cannot use the \"in\" operator on this value") : member.has(obj);
var __privateGet$2 = (obj, member, getter) => (__accessCheck$2(obj, member, "read from private field"), getter ? getter.call(obj) : member.get(obj));
var __privateAdd$2 = (obj, member, value) => member.has(obj) ? __typeError$2("Cannot add the same private member more than once") : member instanceof WeakSet ? member.add(obj) : member.set(obj, value);
var __privateSet$2 = (obj, member, value, setter) => (__accessCheck$2(obj, member, "write to private field"), setter ? setter.call(obj, value) : member.set(obj, value), value);
var __privateMethod = (obj, member, method) => (__accessCheck$2(obj, member, "access private method"), method);
var defaultAttributes = {
	role: "button",
	roleDescription: "draggable"
};
var defaultDescriptionIdPrefix = `dnd-kit-description`;
var defaultAnnouncementIdPrefix = `dnd-kit-announcement`;
var defaultScreenReaderInstructions = { draggable: `To pick up a draggable item, press the space bar. While dragging, use the arrow keys to move the item in a given direction. Press space again to drop the item in its new position, or press escape to cancel.` };
var defaultAnnouncements = {
	dragstart({ operation: { source } }) {
		if (!source) return;
		return `Picked up draggable item ${source.id}.`;
	},
	dragover({ operation: { source, target } }) {
		if (!source || source.id === (target == null ? void 0 : target.id)) return;
		if (target) return `Draggable item ${source.id} was moved over droppable target ${target.id}.`;
		return `Draggable item ${source.id} is no longer over a droppable target.`;
	},
	dragend({ operation: { source, target }, canceled }) {
		if (!source) return;
		if (canceled) return `Dragging was cancelled. Draggable item ${source.id} was dropped.`;
		if (target) return `Draggable item ${source.id} was dropped over droppable target ${target.id}`;
		return `Draggable item ${source.id} was dropped.`;
	}
};
function isFocusable(element) {
	const tagName = element.tagName.toLowerCase();
	return [
		"input",
		"select",
		"textarea",
		"a",
		"button"
	].includes(tagName);
}
function createHiddenText(id, value) {
	const element = document.createElement("div");
	element.id = id;
	element.style.setProperty("display", "none");
	element.textContent = value;
	return element;
}
function createLiveRegion(id) {
	const element = document.createElement("div");
	element.id = id;
	element.setAttribute("role", "status");
	element.setAttribute("aria-live", "polite");
	element.setAttribute("aria-atomic", "true");
	element.style.setProperty("position", "fixed");
	element.style.setProperty("width", "1px");
	element.style.setProperty("height", "1px");
	element.style.setProperty("margin", "-1px");
	element.style.setProperty("border", "0");
	element.style.setProperty("padding", "0");
	element.style.setProperty("overflow", "hidden");
	element.style.setProperty("clip", "rect(0 0 0 0)");
	element.style.setProperty("clip-path", "inset(100%)");
	element.style.setProperty("white-space", "nowrap");
	return element;
}
var debouncedEvents = ["dragover", "dragmove"];
var Accessibility = class extends Plugin {
	constructor(manager, options) {
		super(manager);
		const { id, idPrefix: { description: descriptionPrefix = defaultDescriptionIdPrefix, announcement: announcementPrefix = defaultAnnouncementIdPrefix } = {}, announcements = defaultAnnouncements, screenReaderInstructions = defaultScreenReaderInstructions, debounce: debounceMs = 500 } = options != null ? options : {};
		const descriptionId = id ? `${descriptionPrefix}-${id}` : generateUniqueId(descriptionPrefix);
		const announcementId = id ? `${announcementPrefix}-${id}` : generateUniqueId(announcementPrefix);
		let hiddenTextElement;
		let liveRegionElement;
		let liveRegionTextNode;
		let latestAnnouncement;
		const updateAnnouncement = (value = latestAnnouncement) => {
			if (!liveRegionTextNode || !value) return;
			if ((liveRegionTextNode == null ? void 0 : liveRegionTextNode.nodeValue) !== value) liveRegionTextNode.nodeValue = value;
		};
		const scheduleUpdateAnnouncement = () => scheduler.schedule(updateAnnouncement);
		const debouncedUpdateAnnouncement = debounce(scheduleUpdateAnnouncement, debounceMs);
		const eventListeners = Object.entries(announcements).map(([eventName, getAnnouncement]) => {
			return this.manager.monitor.addEventListener(eventName, (event, manager2) => {
				const element = liveRegionTextNode;
				if (!element) return;
				const announcement = getAnnouncement == null ? void 0 : getAnnouncement(event, manager2);
				if (announcement && element.nodeValue !== announcement) {
					latestAnnouncement = announcement;
					if (debouncedEvents.includes(eventName)) debouncedUpdateAnnouncement();
					else {
						scheduleUpdateAnnouncement();
						debouncedUpdateAnnouncement.cancel();
					}
				}
			});
		});
		const initialize = () => {
			let elements = [];
			if (!(hiddenTextElement == null ? void 0 : hiddenTextElement.isConnected)) {
				hiddenTextElement = createHiddenText(descriptionId, screenReaderInstructions.draggable);
				elements.push(hiddenTextElement);
			}
			if (!(liveRegionElement == null ? void 0 : liveRegionElement.isConnected)) {
				liveRegionElement = createLiveRegion(announcementId);
				liveRegionTextNode = document.createTextNode("");
				liveRegionElement.appendChild(liveRegionTextNode);
				elements.push(liveRegionElement);
			}
			if (elements.length > 0) document.body.append(...elements);
		};
		const mutations = /* @__PURE__ */ new Set();
		function executeMutations() {
			for (const operation of mutations) operation();
		}
		this.registerEffect(() => {
			var _a5;
			mutations.clear();
			for (const draggable of this.manager.registry.draggables.value) {
				const activator = (_a5 = draggable.handle) != null ? _a5 : draggable.element;
				if (activator) {
					if (!hiddenTextElement || !liveRegionElement) mutations.add(initialize);
					if ((!isFocusable(activator) || isSafari()) && !activator.hasAttribute("tabindex")) mutations.add(() => activator.setAttribute("tabindex", "0"));
					if (!activator.hasAttribute("role") && !(activator.tagName.toLowerCase() === "button")) mutations.add(() => activator.setAttribute("role", defaultAttributes.role));
					if (!activator.hasAttribute("aria-roledescription")) mutations.add(() => activator.setAttribute("aria-roledescription", defaultAttributes.roleDescription));
					if (!activator.hasAttribute("aria-describedby")) mutations.add(() => activator.setAttribute("aria-describedby", descriptionId));
					for (const key of ["aria-pressed", "aria-grabbed"]) {
						const value = String(draggable.isDragging);
						if (activator.getAttribute(key) !== value) mutations.add(() => activator.setAttribute(key, value));
					}
					const disabled = String(draggable.disabled);
					if (activator.getAttribute("aria-disabled") !== disabled) mutations.add(() => activator.setAttribute("aria-disabled", disabled));
				}
			}
			if (mutations.size > 0) scheduler.schedule(executeMutations);
		});
		this.destroy = () => {
			super.destroy();
			hiddenTextElement?.remove();
			liveRegionElement?.remove();
			eventListeners.forEach((unsubscribe) => unsubscribe());
		};
	}
};
function debounce(fn, wait) {
	let timeout;
	const debounced = () => {
		clearTimeout(timeout);
		timeout = setTimeout(fn, wait);
	};
	debounced.cancel = () => clearTimeout(timeout);
	return debounced;
}
var styleRegistry = /* @__PURE__ */ new Map();
var _roots_dec, _targetRoot_dec, _sourceRoot_dec, _additionalRoots_dec, _a$1, _registeredRules, _init$2, _additionalRoots, _StyleInjector_instances, syncStyles_fn, inject_fn, injectStyleElement_fn, injectAdoptedSheet_fn;
var _StyleInjector = class _StyleInjector extends (_a$1 = CorePlugin, _additionalRoots_dec = [reactive], _sourceRoot_dec = [derived], _targetRoot_dec = [derived], _roots_dec = [derived], _a$1) {
	constructor(manager, options) {
		super(manager, options);
		__runInitializers$2(_init$2, 5, this);
		__privateAdd$2(this, _StyleInjector_instances);
		__privateAdd$2(this, _registeredRules, /* @__PURE__ */ new Set());
		__privateAdd$2(this, _additionalRoots, __runInitializers$2(_init$2, 8, this, /* @__PURE__ */ new Set())), __runInitializers$2(_init$2, 11, this);
		this.registerEffect(__privateMethod(this, _StyleInjector_instances, syncStyles_fn));
	}
	/**
	* Registers CSS rules to be injected into the active drag operation's
	* document and shadow roots. The StyleInjector handles tracking
	* which roots need the styles and cleaning up when they're no longer needed.
	*
	* Returns a cleanup function that unregisters the rules.
	*/
	register(cssRules) {
		__privateGet$2(this, _registeredRules).add(cssRules);
		return () => {
			__privateGet$2(this, _registeredRules).delete(cssRules);
		};
	}
	/**
	* Adds an additional root to track for style injection.
	* Returns a cleanup function that removes the root.
	*/
	addRoot(root) {
		o(() => {
			const roots = new Set(this.additionalRoots);
			roots.add(root);
			this.additionalRoots = roots;
		});
		return () => {
			o(() => {
				const roots = new Set(this.additionalRoots);
				roots.delete(root);
				this.additionalRoots = roots;
			});
		};
	}
	get sourceRoot() {
		var _a5;
		const { source } = this.manager.dragOperation;
		return getRoot((_a5 = source == null ? void 0 : source.element) != null ? _a5 : null);
	}
	get targetRoot() {
		var _a5;
		const { target } = this.manager.dragOperation;
		return getRoot((_a5 = target == null ? void 0 : target.element) != null ? _a5 : null);
	}
	get roots() {
		const { status } = this.manager.dragOperation;
		if (status.initializing || status.initialized) {
			const roots = [this.sourceRoot, this.targetRoot].filter((root) => root != null);
			return /* @__PURE__ */ new Set([...roots, ...this.additionalRoots]);
		}
		return /* @__PURE__ */ new Set();
	}
};
_init$2 = __decoratorStart$2(_a$1);
_registeredRules = /* @__PURE__ */ new WeakMap();
_additionalRoots = /* @__PURE__ */ new WeakMap();
_StyleInjector_instances = /* @__PURE__ */ new WeakSet();
syncStyles_fn = function() {
	const { roots } = this;
	const cleanups = [];
	for (const root of roots) for (const cssRules of __privateGet$2(this, _registeredRules)) cleanups.push(__privateMethod(this, _StyleInjector_instances, inject_fn).call(this, root, cssRules));
	return () => {
		for (const cleanup of cleanups) cleanup();
	};
};
inject_fn = function(root, cssRules) {
	let rootStyles = styleRegistry.get(root);
	if (!rootStyles) {
		rootStyles = /* @__PURE__ */ new Map();
		styleRegistry.set(root, rootStyles);
	}
	let registration = rootStyles.get(cssRules);
	if (!registration) {
		const created = isDocument(root) ? __privateMethod(this, _StyleInjector_instances, injectStyleElement_fn).call(this, root, rootStyles, cssRules) : __privateMethod(this, _StyleInjector_instances, injectAdoptedSheet_fn).call(this, root, rootStyles, cssRules);
		if (!created) return () => {};
		registration = created;
		rootStyles.set(cssRules, registration);
	}
	registration.refCount++;
	let disposed = false;
	return () => {
		if (disposed) return;
		disposed = true;
		registration.refCount--;
		if (registration.refCount === 0) registration.cleanup();
	};
};
/**
* For Document roots, prepend a <style> element to <head> so that any
* @layer declarations appear before layers from regular stylesheets,
* giving them the lowest cascade priority.
*/
injectStyleElement_fn = function(root, rootStyles, cssRules) {
	var _a5;
	const style = root.createElement("style");
	const { nonce } = (_a5 = this.options) != null ? _a5 : {};
	if (nonce) style.setAttribute("nonce", nonce);
	style.textContent = cssRules;
	root.head.prepend(style);
	const observer = new MutationObserver((entries) => {
		for (const entry of entries) for (const node of Array.from(entry.removedNodes)) if (node === style) {
			root.head.prepend(style);
			return;
		}
	});
	observer.observe(root.head, { childList: true });
	return {
		refCount: 0,
		cleanup: () => {
			observer.disconnect();
			style.remove();
			rootStyles.delete(cssRules);
			if (rootStyles.size === 0) styleRegistry.delete(root);
		}
	};
};
/**
* For ShadowRoot roots, use adoptedStyleSheets to avoid DOM side effects
* like interfering with :first-child or :nth-child selectors.
*/
injectAdoptedSheet_fn = function(root, rootStyles, cssRules) {
	if (!("adoptedStyleSheets" in root && Array.isArray(root.adoptedStyleSheets)) && false);
	const targetWindow = root.ownerDocument.defaultView;
	const { CSSStyleSheet } = targetWindow != null ? targetWindow : {};
	if (!CSSStyleSheet) return null;
	const sheet = new CSSStyleSheet();
	sheet.replaceSync(cssRules);
	root.adoptedStyleSheets.push(sheet);
	return {
		refCount: 0,
		cleanup: () => {
			var _a5;
			if (isShadowRoot(root) && ((_a5 = root.host) == null ? void 0 : _a5.isConnected)) {
				const index = root.adoptedStyleSheets.indexOf(sheet);
				if (index !== -1) root.adoptedStyleSheets.splice(index, 1);
			}
			rootStyles.delete(cssRules);
			if (rootStyles.size === 0) styleRegistry.delete(root);
		}
	};
};
__decorateElement$2(_init$2, 4, "additionalRoots", _additionalRoots_dec, _StyleInjector, _additionalRoots);
__decorateElement$2(_init$2, 2, "sourceRoot", _sourceRoot_dec, _StyleInjector);
__decorateElement$2(_init$2, 2, "targetRoot", _targetRoot_dec, _StyleInjector);
__decorateElement$2(_init$2, 2, "roots", _roots_dec, _StyleInjector);
__decoratorMetadata$2(_init$2, _StyleInjector);
_StyleInjector.configure = configurator(_StyleInjector);
var StyleInjector = _StyleInjector;
var Cursor = class extends Plugin {
	constructor(manager, options) {
		super(manager, options);
		this.manager = manager;
		const { cursor = "grabbing" } = options != null ? options : {};
		const styleInjector = manager.registry.plugins.get(StyleInjector);
		const unregisterStyles = styleInjector == null ? void 0 : styleInjector.register(`* { cursor: ${cursor} !important; }`);
		if (unregisterStyles) {
			const originalDestroy = this.destroy.bind(this);
			this.destroy = () => {
				unregisterStyles();
				originalDestroy();
			};
		}
	}
};
var ATTR_PREFIX = "data-dnd-";
var DROPPING_ATTRIBUTE = `${ATTR_PREFIX}dropping`;
var CSS_PREFIX = "--dnd-";
var ATTRIBUTE = `${ATTR_PREFIX}dragging`;
var PLACEHOLDER_ATTRIBUTE = `${ATTR_PREFIX}placeholder`;
var IGNORED_ATTRIBUTES = [
	ATTRIBUTE,
	PLACEHOLDER_ATTRIBUTE,
	"popover",
	"aria-pressed",
	"aria-grabbing"
];
var IGNORED_STYLES = ["view-transition-name"];
var CSS_RULES = `
  :is(:root,:host) [${ATTRIBUTE}] {
    position: fixed !important;
    pointer-events: none !important;
    touch-action: none;
    z-index: calc(infinity);
    will-change: translate;
    top: var(${CSS_PREFIX}top, 0px) !important;
    left: var(${CSS_PREFIX}left, 0px) !important;
    right: unset !important;
    bottom: unset !important;
    width: var(${CSS_PREFIX}width, auto);
    max-width: var(${CSS_PREFIX}width, auto);
    height: var(${CSS_PREFIX}height, auto);
    max-height: var(${CSS_PREFIX}height, auto);
    transform: var(${CSS_PREFIX}transform, none) !important;
    transition: var(${CSS_PREFIX}transition) !important;
  }

  :is(:root,:host) [${PLACEHOLDER_ATTRIBUTE}] {
    transition: none;
  }

  :is(:root,:host) [${PLACEHOLDER_ATTRIBUTE}='hidden'] {
    visibility: hidden;
  }

  [${ATTRIBUTE}] * {
    pointer-events: none !important;
  }

  [${ATTRIBUTE}]:not([${DROPPING_ATTRIBUTE}]) {
    translate: var(${CSS_PREFIX}translate) !important;
  }

  [${ATTRIBUTE}][style*='${CSS_PREFIX}scale'] {
    scale: var(${CSS_PREFIX}scale) !important;
    transform-origin: var(${CSS_PREFIX}transform-origin) !important;
  }

  @layer dnd-kit {
    :where([${ATTRIBUTE}][popover]) {
      overflow: visible;
      background: unset;
      border: unset;
      margin: unset;
      padding: unset;
      color: inherit;

      &:is(input, button) {
        border: revert;
        background: revert;
      }
    }
  }
  [${ATTRIBUTE}]::backdrop, [${ATTR_PREFIX}overlay]:not([${ATTRIBUTE}]) {
    display: none;
    visibility: hidden;
  }
`.replace(/\n+/g, " ").replace(/\s+/g, " ").trim();
function createPlaceholder(source, type = "hidden") {
	return o(() => {
		const { element, manager } = source;
		if (!element || !manager) return;
		const containedDroppables = findContainedDroppables(element, manager.registry.droppables);
		const cleanup = [];
		const placeholder = cloneElement(element);
		const { remove } = placeholder;
		proxyDroppableElements(containedDroppables, placeholder, cleanup);
		configurePlaceholder(placeholder, type);
		placeholder.remove = () => {
			cleanup.forEach((fn) => fn());
			remove.call(placeholder);
		};
		return placeholder;
	});
}
function findContainedDroppables(element, droppables) {
	const containedDroppables = /* @__PURE__ */ new Map();
	for (const droppable of droppables) {
		if (!droppable.element) continue;
		if (element === droppable.element || element.contains(droppable.element)) {
			const identifierAttribute = `${ATTR_PREFIX}${generateUniqueId("dom-id")}`;
			droppable.element.setAttribute(identifierAttribute, "");
			containedDroppables.set(droppable, identifierAttribute);
		}
	}
	return containedDroppables;
}
function proxyDroppableElements(containedDroppables, placeholder, cleanup) {
	for (const [droppable, identifierAttribute] of containedDroppables) {
		if (!droppable.element) continue;
		const selector = `[${identifierAttribute}]`;
		const clonedElement = placeholder.matches(selector) ? placeholder : placeholder.querySelector(selector);
		droppable.element.removeAttribute(identifierAttribute);
		if (!clonedElement) continue;
		const originalElement = droppable.element;
		droppable.proxy = clonedElement;
		clonedElement.removeAttribute(identifierAttribute);
		ProxiedElements.set(originalElement, clonedElement);
		cleanup.push(() => {
			ProxiedElements.delete(originalElement);
			droppable.proxy = void 0;
		});
	}
}
function configurePlaceholder(placeholder, type = "hidden") {
	placeholder.setAttribute("inert", "true");
	placeholder.setAttribute("tab-index", "-1");
	placeholder.setAttribute("aria-hidden", "true");
	placeholder.setAttribute(PLACEHOLDER_ATTRIBUTE, type);
}
function isSameFrame(element, target) {
	if (element === target) return true;
	return getFrameElement(element) === getFrameElement(target);
}
function preventPopoverClose(event) {
	const { target } = event;
	if ("newState" in event && event.newState === "closed" && isElement(target) && target.hasAttribute("popover")) requestAnimationFrame(() => showPopover(target));
}
function isTableRow(element) {
	return element.tagName === "TR";
}
function createElementMutationObserver(element, placeholder, clone) {
	const observer = new MutationObserver((mutations) => {
		let hasChildrenMutations = false;
		for (const mutation of mutations) {
			if (mutation.target !== element) {
				hasChildrenMutations = true;
				continue;
			}
			if (mutation.type !== "attributes") continue;
			const attributeName = mutation.attributeName;
			if (attributeName.startsWith("aria-") || IGNORED_ATTRIBUTES.includes(attributeName)) continue;
			const attributeValue = element.getAttribute(attributeName);
			if (attributeName === "style") {
				if (supportsStyle(element) && supportsStyle(placeholder)) {
					const styles = element.style;
					for (const key of Array.from(placeholder.style)) if (styles.getPropertyValue(key) === "") placeholder.style.removeProperty(key);
					for (const key of Array.from(styles)) {
						if (IGNORED_STYLES.includes(key) || key.startsWith(CSS_PREFIX)) continue;
						const value = styles.getPropertyValue(key);
						placeholder.style.setProperty(key, value);
					}
				}
			} else if (attributeValue !== null) placeholder.setAttribute(attributeName, attributeValue);
			else placeholder.removeAttribute(attributeName);
		}
		if (hasChildrenMutations && clone) placeholder.replaceChildren(...element.cloneNode(true).childNodes);
	});
	observer.observe(element, {
		attributes: true,
		subtree: true,
		childList: true
	});
	return observer;
}
function createDocumentMutationObserver(element, placeholder, feedbackElement) {
	const observer = new MutationObserver((entries) => {
		for (const entry of entries) {
			if (entry.addedNodes.length === 0) continue;
			for (const node of Array.from(entry.addedNodes)) {
				if (node.contains(element) && element.nextElementSibling !== placeholder) {
					element.insertAdjacentElement("afterend", placeholder);
					showPopover(feedbackElement);
					return;
				}
				if (node.contains(placeholder) && placeholder.previousElementSibling !== element) {
					placeholder.insertAdjacentElement("beforebegin", element);
					showPopover(feedbackElement);
					return;
				}
			}
		}
		if (element.isConnected && placeholder.isConnected && element.nextElementSibling !== placeholder) {
			element.insertAdjacentElement("afterend", placeholder);
			showPopover(feedbackElement);
		}
	});
	observer.observe(element.ownerDocument.body, {
		childList: true,
		subtree: true
	});
	return observer;
}
function createResizeObserver(ctx) {
	return new ResizeObserver(() => {
		var _a5, _b2, _c3;
		const placeholderShape = new DOMRectangle(ctx.placeholder, {
			frameTransform: ctx.frameTransform,
			ignoreTransforms: true
		});
		const origin = (_a5 = ctx.transformOrigin) != null ? _a5 : {
			x: 1,
			y: 1
		};
		const dX = (ctx.width - placeholderShape.width) * origin.x + ctx.delta.x;
		const dY = (ctx.height - placeholderShape.height) * origin.y + ctx.delta.y;
		const fixedOffset = getFixedPositionOffset();
		ctx.styles.set({
			width: placeholderShape.width - ctx.widthOffset,
			height: placeholderShape.height - ctx.heightOffset,
			top: ctx.top + dY + fixedOffset.y,
			left: ctx.left + dX + fixedOffset.x
		}, CSS_PREFIX);
		(_b2 = ctx.getElementMutationObserver()) == null || _b2.takeRecords();
		if (isTableRow(ctx.element) && isTableRow(ctx.placeholder)) {
			const cells = Array.from(ctx.element.cells);
			const placeholderCells = Array.from(ctx.placeholder.cells);
			if (!ctx.getSavedCellWidths()) ctx.setSavedCellWidths(cells.map((cell) => cell.style.width));
			for (const [index, cell] of cells.entries()) {
				const placeholderCell = placeholderCells[index];
				cell.style.width = `${placeholderCell.getBoundingClientRect().width}px`;
			}
		}
		const translate = (_c3 = ctx.getTranslate()) != null ? _c3 : {
			x: 0,
			y: 0
		};
		const shapeLeft = ctx.left + dX + fixedOffset.x + translate.x;
		const shapeTop = ctx.top + dY + fixedOffset.y + translate.y;
		const shapeWidth = placeholderShape.width - ctx.widthOffset;
		const shapeHeight = placeholderShape.height - ctx.heightOffset;
		const ft = ctx.frameTransform;
		ctx.dragOperation.shape = new Rectangle(shapeLeft * ft.scaleX + ft.x, shapeTop * ft.scaleY + ft.y, shapeWidth * ft.scaleX, shapeHeight * ft.scaleY);
	});
}
var DEFAULT_DURATION = 250;
var DEFAULT_EASING = "ease";
function runDropAnimation(ctx) {
	var _a5, _b2, _c3, _d2;
	const { animation } = ctx;
	if (typeof animation === "function") {
		const result = animation({
			source: ctx.source,
			element: ctx.element,
			feedbackElement: ctx.feedbackElement,
			placeholder: ctx.placeholder,
			translate: ctx.translate,
			moved: ctx.moved
		});
		Promise.resolve(result).then(() => {
			ctx.cleanup();
			requestAnimationFrame(ctx.restoreFocus);
		});
		return;
	}
	const { duration = DEFAULT_DURATION, easing = DEFAULT_EASING } = animation != null ? animation : {};
	showPopover(ctx.feedbackElement);
	const [, runningAnimation] = (_a5 = getFinalKeyframe(ctx.feedbackElement, (keyframe) => "translate" in keyframe)) != null ? _a5 : [];
	runningAnimation?.pause();
	const target = (_b2 = ctx.placeholder) != null ? _b2 : ctx.element;
	const options = { frameTransform: isSameFrame(ctx.feedbackElement, target) ? null : void 0 };
	const current = new DOMRectangle(ctx.feedbackElement, options);
	const currentTranslate = (_c3 = parseTranslate(getComputedStyles(ctx.feedbackElement).translate)) != null ? _c3 : ctx.translate;
	const final = new DOMRectangle(target, options);
	const delta = Rectangle.delta(current, final, ctx.alignment);
	const finalTranslate = {
		x: currentTranslate.x - delta.x,
		y: currentTranslate.y - delta.y
	};
	const heightKeyframes = Math.round(current.intrinsicHeight) !== Math.round(final.intrinsicHeight) ? {
		minHeight: [`${current.intrinsicHeight}px`, `${final.intrinsicHeight}px`],
		maxHeight: [`${current.intrinsicHeight}px`, `${final.intrinsicHeight}px`]
	} : {};
	const widthKeyframes = Math.round(current.intrinsicWidth) !== Math.round(final.intrinsicWidth) ? {
		minWidth: [`${current.intrinsicWidth}px`, `${final.intrinsicWidth}px`],
		maxWidth: [`${current.intrinsicWidth}px`, `${final.intrinsicWidth}px`]
	} : {};
	ctx.styles.set({ transition: ctx.transition }, CSS_PREFIX);
	ctx.feedbackElement.setAttribute(DROPPING_ATTRIBUTE, "");
	(_d2 = ctx.getElementMutationObserver()) == null || _d2.takeRecords();
	animateTransform({
		element: ctx.feedbackElement,
		keyframes: __spreadProps$3(__spreadValues$3(__spreadValues$3({}, heightKeyframes), widthKeyframes), { translate: [`${currentTranslate.x}px ${currentTranslate.y}px 0`, `${finalTranslate.x}px ${finalTranslate.y}px 0`] }),
		options: {
			duration: prefersReducedMotion(getWindow(ctx.feedbackElement)) ? 0 : ctx.moved || ctx.feedbackElement !== ctx.element ? duration : 0,
			easing
		}
	}).then(() => {
		ctx.feedbackElement.removeAttribute(DROPPING_ATTRIBUTE);
		runningAnimation?.finish();
		ctx.cleanup();
		requestAnimationFrame(ctx.restoreFocus);
	});
}
var _overlay_dec, _a2, _init2, _overlay, _Feedback_instances, trackOverlayRoot_fn, render_fn;
var _Feedback = class _Feedback extends (_a2 = Plugin, _overlay_dec = [reactive], _a2) {
	constructor(manager, options) {
		super(manager, options);
		__privateAdd$2(this, _Feedback_instances);
		__privateAdd$2(this, _overlay, __runInitializers$2(_init2, 8, this)), __runInitializers$2(_init2, 11, this);
		this.state = {
			initial: {},
			current: {}
		};
		const styleInjector = manager.registry.plugins.get(StyleInjector);
		const unregisterStyles = styleInjector == null ? void 0 : styleInjector.register(CSS_RULES);
		if (unregisterStyles) {
			const originalDestroy = this.destroy.bind(this);
			this.destroy = () => {
				unregisterStyles();
				originalDestroy();
			};
		}
		this.registerEffect(__privateMethod(this, _Feedback_instances, trackOverlayRoot_fn).bind(this, styleInjector));
		this.registerEffect(__privateMethod(this, _Feedback_instances, render_fn));
	}
};
_init2 = __decoratorStart$2(_a2);
_overlay = /* @__PURE__ */ new WeakMap();
_Feedback_instances = /* @__PURE__ */ new WeakSet();
trackOverlayRoot_fn = function(styleInjector) {
	const { overlay } = this;
	if (!overlay || !styleInjector) return;
	const root = getRoot(overlay);
	if (!root) return;
	return styleInjector.addRoot(root);
};
render_fn = function() {
	var _a5, _b2, _c3, _d2, _e, _f, _g;
	const { state, manager, options } = this;
	const { dragOperation } = manager;
	const { position, source, status } = dragOperation;
	if (status.idle) {
		state.current = {};
		state.initial = {};
		return;
	}
	if (!source) return;
	const { element } = source;
	const entityOptions = source.pluginConfig(_Feedback);
	const feedbackOption = (_b2 = (_a5 = entityOptions == null ? void 0 : entityOptions.feedback) != null ? _a5 : options == null ? void 0 : options.feedback) != null ? _b2 : "default";
	const feedback = typeof feedbackOption === "function" ? feedbackOption(source, manager) : feedbackOption;
	if (!element || feedback === "none" || !status.initialized || status.initializing) return;
	const { initial } = state;
	const feedbackElement = (_c3 = this.overlay) != null ? _c3 : element;
	const frameTransform = getFrameTransform(feedbackElement);
	const elementFrameTransform = getFrameTransform(element);
	const crossFrame = !isSameFrame(element, feedbackElement);
	const shape = new DOMRectangle(element, {
		frameTransform: crossFrame ? elementFrameTransform : null,
		ignoreTransforms: !crossFrame
	});
	const scaleDelta = {
		x: elementFrameTransform.scaleX / frameTransform.scaleX,
		y: elementFrameTransform.scaleY / frameTransform.scaleY
	};
	let { width, height, top, left } = shape;
	if (crossFrame) {
		width = width / scaleDelta.x;
		height = height / scaleDelta.y;
	}
	const styles = new Styles(feedbackElement);
	const elementStyles = getComputedStyles(element);
	const { transition, translate, boxSizing, paddingBlockStart, paddingBlockEnd, paddingInlineStart, paddingInlineEnd, borderInlineStartWidth, borderInlineEndWidth, borderBlockStartWidth, borderBlockEndWidth } = elementStyles;
	const feedbackTransition = transition.split(",").filter((t) => !/^\s*(transform|translate|scale)\b/.test(t)).join(",");
	const parsedTransform = parseTransform(elementStyles);
	const initialTransformStyle = elementStyles.transform;
	const clone = feedback === "clone";
	const contentBox = boxSizing === "content-box";
	const widthOffset = contentBox ? parseInt(paddingInlineStart) + parseInt(paddingInlineEnd) + parseInt(borderInlineStartWidth) + parseInt(borderInlineEndWidth) : 0;
	const heightOffset = contentBox ? parseInt(paddingBlockStart) + parseInt(paddingBlockEnd) + parseInt(borderBlockStartWidth) + parseInt(borderBlockEndWidth) : 0;
	const placeholder = feedback !== "move" && !this.overlay ? createPlaceholder(source, clone ? "clone" : "hidden") : null;
	const isKeyboardOperation = o(() => isKeyboardEvent(manager.dragOperation.activatorEvent));
	if (!initial.translate) {
		if (this.overlay && parsedTransform) initial.translate = {
			x: parsedTransform.x,
			y: parsedTransform.y
		};
		else if (translate !== "none") {
			const parsedTranslate = parseTranslate(translate);
			if (parsedTranslate) initial.translate = parsedTranslate;
		}
	}
	if (!initial.transformOrigin) {
		const current = o(() => position.current);
		const visualLeft = left + ((_d2 = parsedTransform == null ? void 0 : parsedTransform.x) != null ? _d2 : 0);
		const visualTop = top + ((_e = parsedTransform == null ? void 0 : parsedTransform.y) != null ? _e : 0);
		initial.transformOrigin = {
			x: (current.x - visualLeft * frameTransform.scaleX - frameTransform.x) / (width * frameTransform.scaleX),
			y: (current.y - visualTop * frameTransform.scaleY - frameTransform.y) / (height * frameTransform.scaleY)
		};
	}
	const { transformOrigin } = initial;
	const relativeTop = top * frameTransform.scaleY + frameTransform.y;
	const relativeLeft = left * frameTransform.scaleX + frameTransform.x;
	if (!initial.coordinates) {
		initial.coordinates = {
			x: relativeLeft,
			y: relativeTop
		};
		if (scaleDelta.x !== 1 || scaleDelta.y !== 1) {
			const { scaleX, scaleY } = elementFrameTransform;
			const { x: tX2, y: tY2 } = transformOrigin;
			initial.coordinates.x += (width * scaleX - width) * tX2;
			initial.coordinates.y += (height * scaleY - height) * tY2;
		}
	}
	if (!initial.dimensions) initial.dimensions = {
		width,
		height
	};
	if (!initial.frameTransform) initial.frameTransform = frameTransform;
	const coordinatesDelta = {
		x: initial.coordinates.x - relativeLeft,
		y: initial.coordinates.y - relativeTop
	};
	const sizeDelta = {
		width: (initial.dimensions.width * initial.frameTransform.scaleX - width * frameTransform.scaleX) * transformOrigin.x,
		height: (initial.dimensions.height * initial.frameTransform.scaleY - height * frameTransform.scaleY) * transformOrigin.y
	};
	const delta = {
		x: coordinatesDelta.x / frameTransform.scaleX + sizeDelta.width,
		y: coordinatesDelta.y / frameTransform.scaleY + sizeDelta.height
	};
	const projected = {
		left: left + delta.x,
		top: top + delta.y
	};
	feedbackElement.setAttribute(ATTRIBUTE, "true");
	const transform = o(() => dragOperation.transform);
	const initialTranslate = (_f = initial.translate) != null ? _f : {
		x: 0,
		y: 0
	};
	const tX = transform.x * frameTransform.scaleX + initialTranslate.x;
	const tY = transform.y * frameTransform.scaleY + initialTranslate.y;
	const fixedOffset = getFixedPositionOffset();
	styles.set({
		width: width - widthOffset,
		height: height - heightOffset,
		top: projected.top + fixedOffset.y,
		left: projected.left + fixedOffset.x,
		translate: `${tX}px ${tY}px 0`,
		transform: this.overlay ? "none" : initialTransformStyle,
		transition: feedbackTransition ? `${feedbackTransition}, translate 0ms linear` : "translate 0ms linear",
		scale: crossFrame ? `${scaleDelta.x} ${scaleDelta.y}` : "",
		"transform-origin": `${transformOrigin.x * 100}% ${transformOrigin.y * 100}%`
	}, CSS_PREFIX);
	if (placeholder) {
		element.insertAdjacentElement("afterend", placeholder);
		if (options == null ? void 0 : options.rootElement) (typeof options.rootElement === "function" ? options.rootElement(source) : options.rootElement).appendChild(element);
	}
	if (supportsPopover(feedbackElement)) {
		if (!feedbackElement.hasAttribute("popover")) feedbackElement.setAttribute("popover", "manual");
		showPopover(feedbackElement);
		feedbackElement.addEventListener("beforetoggle", preventPopoverClose);
	}
	let elementMutationObserver;
	let documentMutationObserver;
	let savedCellWidths;
	const resizeObserver = createResizeObserver({
		placeholder,
		element,
		feedbackElement,
		frameTransform,
		transformOrigin,
		width,
		height,
		top,
		left,
		widthOffset,
		heightOffset,
		delta,
		styles,
		dragOperation,
		getTranslate: () => state.current.translate,
		getElementMutationObserver: () => elementMutationObserver,
		getSavedCellWidths: () => savedCellWidths,
		setSavedCellWidths: (widths) => {
			savedCellWidths = widths;
		}
	});
	const initialShape = new DOMRectangle(feedbackElement);
	o(() => dragOperation.shape = initialShape);
	const feedbackWindow = getWindow(feedbackElement);
	const handleWindowResize = (event) => {
		this.manager.actions.stop({ event });
	};
	const reducedMotion = prefersReducedMotion(feedbackWindow);
	if (isKeyboardOperation) feedbackWindow.addEventListener("resize", handleWindowResize);
	if (o(() => source.status) === "idle") requestAnimationFrame(() => source.status = "dragging");
	if (placeholder) {
		resizeObserver.observe(placeholder);
		elementMutationObserver = createElementMutationObserver(element, placeholder, clone);
		documentMutationObserver = createDocumentMutationObserver(element, placeholder, feedbackElement);
	}
	const id = (_g = manager.dragOperation.source) == null ? void 0 : _g.id;
	const restoreFocus = () => {
		var _a6;
		if (!isKeyboardOperation || id == null) return;
		const draggable = manager.registry.draggables.get(id);
		const focusTarget = (_a6 = draggable == null ? void 0 : draggable.handle) != null ? _a6 : draggable == null ? void 0 : draggable.element;
		if (isHTMLElement(focusTarget)) focusTarget.focus();
	};
	const cleanup = () => {
		var _a6;
		elementMutationObserver?.disconnect();
		documentMutationObserver?.disconnect();
		resizeObserver.disconnect();
		feedbackWindow.removeEventListener("resize", handleWindowResize);
		if (supportsPopover(feedbackElement)) {
			feedbackElement.removeEventListener("beforetoggle", preventPopoverClose);
			feedbackElement.removeAttribute("popover");
		}
		feedbackElement.removeAttribute(ATTRIBUTE);
		styles.reset();
		if (savedCellWidths && isTableRow(element)) {
			const cells = Array.from(element.cells);
			for (const [index, cell] of cells.entries()) cell.style.width = (_a6 = savedCellWidths[index]) != null ? _a6 : "";
		}
		source.status = "idle";
		const moved = state.current.translate != null;
		const isDragging = dragOperation.status.dragging;
		if (placeholder && (!isDragging && moved || placeholder.parentElement !== feedbackElement.parentElement) && feedbackElement.isConnected) placeholder.replaceWith(feedbackElement);
		placeholder?.remove();
	};
	const optionsDropAnimation = options == null ? void 0 : options.dropAnimation;
	const feedbackPlugin = this;
	const cleanupEffects = effects(() => {
		var _a6, _b3, _c4;
		const { transform: transform2, status: status2 } = dragOperation;
		if (!transform2.x && !transform2.y && !state.current.translate) return;
		if (status2.dragging) {
			const initialTranslate2 = (_a6 = initial.translate) != null ? _a6 : {
				x: 0,
				y: 0
			};
			const translate2 = {
				x: transform2.x / frameTransform.scaleX + initialTranslate2.x,
				y: transform2.y / frameTransform.scaleY + initialTranslate2.y
			};
			const previousTranslate = state.current.translate;
			const modifiers = o(() => dragOperation.modifiers);
			const currentShape = o(() => {
				var _a7;
				return (_a7 = dragOperation.shape) == null ? void 0 : _a7.current;
			});
			const keyboardTransition = options == null ? void 0 : options.keyboardTransition;
			const translateTransition = isKeyboardOperation && !reducedMotion && keyboardTransition !== null ? `${(_b3 = keyboardTransition == null ? void 0 : keyboardTransition.duration) != null ? _b3 : 250}ms ${(_c4 = keyboardTransition == null ? void 0 : keyboardTransition.easing) != null ? _c4 : "cubic-bezier(0.25, 1, 0.5, 1)"}` : "0ms linear";
			styles.set({
				transition: feedbackTransition ? `${feedbackTransition}, translate ${translateTransition}` : `translate ${translateTransition}`,
				translate: `${translate2.x}px ${translate2.y}px 0`
			}, CSS_PREFIX);
			elementMutationObserver?.takeRecords();
			if (currentShape && currentShape !== initialShape && previousTranslate && !modifiers.length) {
				const delta2 = Point$1.delta(translate2, previousTranslate);
				dragOperation.shape = Rectangle.from(currentShape.boundingRectangle).translate(delta2.x * frameTransform.scaleX, delta2.y * frameTransform.scaleY);
			} else dragOperation.shape = new DOMRectangle(feedbackElement);
			state.current.translate = translate2;
		}
	}, function() {
		if (dragOperation.status.dropped) {
			this.dispose();
			source.status = "dropping";
			const dropAnimationConfig = (entityOptions == null ? void 0 : entityOptions.dropAnimation) !== void 0 ? entityOptions.dropAnimation : feedbackPlugin.dropAnimation !== void 0 ? feedbackPlugin.dropAnimation : optionsDropAnimation;
			let translate2 = state.current.translate;
			const moved = translate2 != null;
			if (!translate2 && element !== feedbackElement) translate2 = {
				x: 0,
				y: 0
			};
			if (!translate2 || dropAnimationConfig === null) {
				cleanup();
				return;
			}
			manager.renderer.rendering.then(() => {
				runDropAnimation({
					source,
					element,
					feedbackElement,
					placeholder,
					translate: translate2,
					moved,
					transition,
					alignment: source.alignment,
					styles,
					animation: dropAnimationConfig != null ? dropAnimationConfig : void 0,
					getElementMutationObserver: () => elementMutationObserver,
					cleanup,
					restoreFocus
				});
			});
		}
	});
	return () => {
		cleanup();
		cleanupEffects();
	};
};
__decorateElement$2(_init2, 4, "overlay", _overlay_dec, _Feedback, _overlay);
__decoratorMetadata$2(_init2, _Feedback);
_Feedback.configure = configurator(_Feedback);
var Feedback = _Feedback;
var LOCKED = true;
var UNLOCKED = false;
var _dec, _a3, _dec2, _b = (_dec2 = [reactive], ScrollDirection.Forward), _init3, __b, __a;
_a3 = (_dec = [reactive], ScrollDirection.Reverse);
var ScrollLock = class {
	constructor() {
		__privateAdd$2(this, __b, __runInitializers$2(_init3, 8, this, LOCKED)), __runInitializers$2(_init3, 11, this);
		__privateAdd$2(this, __a, __runInitializers$2(_init3, 12, this, LOCKED)), __runInitializers$2(_init3, 15, this);
	}
	isLocked(direction) {
		if (direction === ScrollDirection.Idle) return false;
		if (direction == null) return this[ScrollDirection.Forward] === LOCKED && this[ScrollDirection.Reverse] === LOCKED;
		return this[direction] === LOCKED;
	}
	unlock(direction) {
		if (direction === ScrollDirection.Idle) return;
		this[direction] = UNLOCKED;
	}
};
_init3 = __decoratorStart$2(null);
__b = /* @__PURE__ */ new WeakMap();
__a = /* @__PURE__ */ new WeakMap();
__decorateElement$2(_init3, 4, _b, _dec2, ScrollLock, __b);
__decorateElement$2(_init3, 4, _a3, _dec, ScrollLock, __a);
__decoratorMetadata$2(_init3, ScrollLock);
var DIRECTIONS = [ScrollDirection.Forward, ScrollDirection.Reverse];
var ScrollIntent = class {
	constructor() {
		this.x = new ScrollLock();
		this.y = new ScrollLock();
	}
	isLocked() {
		return this.x.isLocked() && this.y.isLocked();
	}
};
var ScrollIntentTracker = class extends Plugin {
	constructor(manager) {
		super(manager);
		const scrollIntent = y(new ScrollIntent());
		let previousDelta = null;
		this.signal = scrollIntent;
		j(() => {
			const { status } = manager.dragOperation;
			if (!status.initialized) {
				previousDelta = null;
				scrollIntent.value = new ScrollIntent();
				return;
			}
			const { delta } = manager.dragOperation.position;
			if (previousDelta) {
				const directions = {
					x: getDirection$1(delta.x, previousDelta.x),
					y: getDirection$1(delta.y, previousDelta.y)
				};
				const intent = scrollIntent.peek();
				n(() => {
					for (const axis of Axes) for (const direction of DIRECTIONS) if (directions[axis] === direction) intent[axis].unlock(direction);
					scrollIntent.value = intent;
				});
			}
			previousDelta = delta;
		});
	}
	get current() {
		return this.signal.peek();
	}
};
function getDirection$1(a, b) {
	return Math.sign(a - b);
}
var _autoScrolling_dec, _a4, _init4, _autoScrolling, _meta, _scroll;
var Scroller = class extends (_a4 = CorePlugin, _autoScrolling_dec = [reactive], _a4) {
	constructor(manager) {
		super(manager);
		__privateAdd$2(this, _autoScrolling, __runInitializers$2(_init4, 8, this, false)), __runInitializers$2(_init4, 11, this);
		__privateAdd$2(this, _meta);
		__privateAdd$2(this, _scroll, () => {
			if (!__privateGet$2(this, _meta)) return;
			const { element, by } = __privateGet$2(this, _meta);
			if (by.y) element.scrollTop += by.y;
			if (by.x) element.scrollLeft += by.x;
		});
		this.scroll = (options, scrollOptions) => {
			var _a5;
			if (this.disabled) return false;
			const elements = this.getScrollableElements();
			if (!elements) {
				__privateSet$2(this, _meta, void 0);
				return false;
			}
			const { position } = this.manager.dragOperation;
			const currentPosition = position == null ? void 0 : position.current;
			if (currentPosition) {
				const { by } = options != null ? options : {};
				const intent = by ? {
					x: getScrollIntent(by.x),
					y: getScrollIntent(by.y)
				} : void 0;
				const scrollIntent = intent ? void 0 : this.scrollIntentTracker.current;
				if (scrollIntent == null ? void 0 : scrollIntent.isLocked()) return false;
				for (const scrollableElement of elements) {
					const elementCanScroll = canScroll(scrollableElement, by);
					if (elementCanScroll.x || elementCanScroll.y) {
						const { speed, direction } = detectScrollIntent(scrollableElement, currentPosition, intent, scrollOptions == null ? void 0 : scrollOptions.acceleration, scrollOptions == null ? void 0 : scrollOptions.threshold);
						if (scrollIntent) {
							for (const axis of Axes) if (scrollIntent[axis].isLocked(direction[axis])) {
								speed[axis] = 0;
								direction[axis] = 0;
							}
						}
						if (direction.x || direction.y) {
							const { x, y } = by != null ? by : direction;
							const scrollLeftBy = x * speed.x;
							const scrollTopBy = y * speed.y;
							if (scrollLeftBy || scrollTopBy) {
								const previousScrollBy = (_a5 = __privateGet$2(this, _meta)) == null ? void 0 : _a5.by;
								if (this.autoScrolling && previousScrollBy) {
									if (previousScrollBy.x && !scrollLeftBy || previousScrollBy.y && !scrollTopBy) continue;
								}
								__privateSet$2(this, _meta, {
									element: scrollableElement,
									by: {
										x: scrollLeftBy,
										y: scrollTopBy
									}
								});
								scheduler.schedule(__privateGet$2(this, _scroll));
								return true;
							}
						}
					}
				}
			}
			__privateSet$2(this, _meta, void 0);
			return false;
		};
		let previousElementFromPoint = null;
		let previousScrollableElements = null;
		const elementFromPoint = computed(() => {
			const { position, source } = manager.dragOperation;
			if (!position) return null;
			const element = getElementFromPoint(getRoot(source == null ? void 0 : source.element), position.current);
			if (element) previousElementFromPoint = element;
			return element != null ? element : previousElementFromPoint;
		});
		const scrollableElements = computed(() => {
			const element = elementFromPoint.value;
			const { documentElement } = getDocument(element);
			if (!element || element === documentElement) {
				const { target } = manager.dragOperation;
				const targetElement = target == null ? void 0 : target.element;
				if (targetElement) {
					const elements = getScrollableAncestors(targetElement, { excludeElement: false });
					previousScrollableElements = elements;
					return elements;
				}
			}
			if (element) {
				const elements = getScrollableAncestors(element, { excludeElement: false });
				if (this.autoScrolling && previousScrollableElements && elements.size < (previousScrollableElements == null ? void 0 : previousScrollableElements.size)) return previousScrollableElements;
				previousScrollableElements = elements;
				return elements;
			}
			previousScrollableElements = null;
			return null;
		}, deepEqual);
		this.getScrollableElements = () => {
			return scrollableElements.value;
		};
		this.scrollIntentTracker = new ScrollIntentTracker(manager);
		this.destroy = manager.monitor.addEventListener("dragmove", (event) => {
			if (this.disabled || event.defaultPrevented || !isKeyboardEvent(manager.dragOperation.activatorEvent) || !event.by) return;
			if (this.scroll({ by: event.by })) event.preventDefault();
		});
	}
};
_init4 = __decoratorStart$2(_a4);
_autoScrolling = /* @__PURE__ */ new WeakMap();
_meta = /* @__PURE__ */ new WeakMap();
_scroll = /* @__PURE__ */ new WeakMap();
__decorateElement$2(_init4, 4, "autoScrolling", _autoScrolling_dec, Scroller, _autoScrolling);
__decoratorMetadata$2(_init4, Scroller);
function getScrollIntent(value) {
	if (value > 0) return ScrollDirection.Forward;
	if (value < 0) return ScrollDirection.Reverse;
	return ScrollDirection.Idle;
}
var Scheduler = class {
	constructor(scheduler5) {
		this.scheduler = scheduler5;
		this.pending = false;
		this.tasks = /* @__PURE__ */ new Set();
		this.resolvers = /* @__PURE__ */ new Set();
		this.flush = () => {
			const { tasks, resolvers } = this;
			this.pending = false;
			this.tasks = /* @__PURE__ */ new Set();
			this.resolvers = /* @__PURE__ */ new Set();
			for (const task of tasks) task();
			for (const resolve of resolvers) resolve();
		};
	}
	schedule(task) {
		this.tasks.add(task);
		if (!this.pending) {
			this.pending = true;
			this.scheduler(this.flush);
		}
		return new Promise((resolve) => this.resolvers.add(resolve));
	}
};
var scheduler3 = new Scheduler((callback) => {
	if (typeof requestAnimationFrame === "function") requestAnimationFrame(callback);
	else callback();
});
var AUTOSCROLL_INTERVAL = 10;
var _AutoScroller = class _AutoScroller extends Plugin {
	constructor(manager, options) {
		super(manager, options);
		const scroller = manager.registry.plugins.get(Scroller);
		if (!scroller) throw new Error("AutoScroller plugin depends on Scroller plugin");
		this.destroy = j(() => {
			var _a5, _b2, _c3;
			if (this.disabled) return;
			const { position: _, status } = manager.dragOperation;
			if (status.dragging) {
				const scrollOptions = {
					acceleration: (_a5 = this.options) == null ? void 0 : _a5.acceleration,
					threshold: typeof ((_b2 = this.options) == null ? void 0 : _b2.threshold) === "number" ? {
						x: this.options.threshold,
						y: this.options.threshold
					} : (_c3 = this.options) == null ? void 0 : _c3.threshold
				};
				if (scroller.scroll(void 0, scrollOptions)) {
					scroller.autoScrolling = true;
					const interval = setInterval(() => scheduler3.schedule(() => scroller.scroll(void 0, scrollOptions)), AUTOSCROLL_INTERVAL);
					return () => {
						clearInterval(interval);
					};
				} else scroller.autoScrolling = false;
			}
		});
	}
};
_AutoScroller.configure = configurator(_AutoScroller);
var AutoScroller = _AutoScroller;
var listenerOptions = {
	capture: true,
	passive: true
};
var _timeout;
var ScrollListener = class extends CorePlugin {
	constructor(manager) {
		super(manager);
		__privateAdd$2(this, _timeout);
		this.handleScroll = () => {
			if (__privateGet$2(this, _timeout) == null) __privateSet$2(this, _timeout, setTimeout(() => {
				this.manager.collisionObserver.forceUpdate(false);
				__privateSet$2(this, _timeout, void 0);
			}, 50));
		};
		const { dragOperation } = this.manager;
		this.destroy = j(() => {
			var _a5, _b2, _c3;
			if (dragOperation.status.dragging) {
				const root = (_c3 = (_b2 = (_a5 = dragOperation.source) == null ? void 0 : _a5.element) == null ? void 0 : _b2.ownerDocument) != null ? _c3 : document;
				root.addEventListener("scroll", this.handleScroll, listenerOptions);
				return () => {
					root.removeEventListener("scroll", this.handleScroll, listenerOptions);
				};
			}
		});
	}
};
_timeout = /* @__PURE__ */ new WeakMap();
var CSS_RULES2 = "* { user-select: none !important; -webkit-user-select: none !important; }";
var PreventSelection = class extends Plugin {
	constructor(manager) {
		super(manager);
		this.manager = manager;
		const styleInjector = manager.registry.plugins.get(StyleInjector);
		const unregisterStyles = styleInjector == null ? void 0 : styleInjector.register(CSS_RULES2);
		this.destroy = j(() => {
			const { dragOperation } = this.manager;
			if (dragOperation.status.initialized) {
				removeSelection();
				document.addEventListener("selectionchange", removeSelection, { capture: true });
				return () => {
					document.removeEventListener("selectionchange", removeSelection, { capture: true });
				};
			}
		});
		if (unregisterStyles) {
			const originalDestroy = this.destroy.bind(this);
			this.destroy = () => {
				unregisterStyles();
				originalDestroy();
			};
		}
	}
};
function removeSelection() {
	var _a5;
	(_a5 = document.getSelection()) == null || _a5.removeAllRanges();
}
var defaults = Object.freeze({
	offset: 10,
	keyboardCodes: {
		start: ["Space", "Enter"],
		cancel: ["Escape"],
		end: [
			"Space",
			"Enter",
			"Tab"
		],
		up: ["ArrowUp"],
		down: ["ArrowDown"],
		left: ["ArrowLeft"],
		right: ["ArrowRight"]
	},
	preventActivation(event, source) {
		var _a5;
		const target = (_a5 = source.handle) != null ? _a5 : source.element;
		return event.target !== target;
	}
});
var _cleanupFunctions;
var _KeyboardSensor = class _KeyboardSensor extends Sensor {
	constructor(manager, options) {
		super(manager);
		this.manager = manager;
		this.options = options;
		__privateAdd$2(this, _cleanupFunctions, []);
		this.listeners = new Listeners();
		this.handleSourceKeyDown = (event, source, options) => {
			if (this.disabled || event.defaultPrevented) return;
			if (!isElement(event.target)) return;
			if (source.disabled) return;
			const { keyboardCodes = defaults.keyboardCodes, preventActivation = defaults.preventActivation } = options != null ? options : {};
			if (!keyboardCodes.start.includes(event.code)) return;
			if (!this.manager.dragOperation.status.idle) return;
			if (preventActivation == null ? void 0 : preventActivation(event, source)) return;
			this.handleStart(event, source, options);
		};
	}
	bind(source, options = this.options) {
		return j(() => {
			var _a5;
			const target = (_a5 = source.handle) != null ? _a5 : source.element;
			const listener = (event) => {
				if (isKeyboardEvent(event)) this.handleSourceKeyDown(event, source, options);
			};
			if (target) {
				target.addEventListener("keydown", listener);
				return () => {
					target.removeEventListener("keydown", listener);
				};
			}
		});
	}
	handleStart(event, source, options) {
		const { element } = source;
		if (!element) throw new Error("Source draggable does not have an associated element");
		event.preventDefault();
		event.stopImmediatePropagation();
		scrollIntoViewIfNeeded(element);
		const { center } = new DOMRectangle(element);
		if (this.manager.actions.start({
			event,
			coordinates: {
				x: center.x,
				y: center.y
			},
			source
		}).signal.aborted) return this.cleanup();
		this.sideEffects();
		const sourceDocument = getDocument(element);
		const listeners = [this.listeners.bind(sourceDocument, [{
			type: "keydown",
			listener: (event2) => this.handleKeyDown(event2, source, options),
			options: { capture: true }
		}])];
		__privateGet$2(this, _cleanupFunctions).push(...listeners);
	}
	handleKeyDown(event, _source, options) {
		const { keyboardCodes = defaults.keyboardCodes } = options != null ? options : {};
		if (isKeycode(event, [...keyboardCodes.end, ...keyboardCodes.cancel])) {
			event.preventDefault();
			const canceled = isKeycode(event, keyboardCodes.cancel);
			this.handleEnd(event, canceled);
			return;
		}
		if (isKeycode(event, keyboardCodes.up)) this.handleMove("up", event);
		else if (isKeycode(event, keyboardCodes.down)) this.handleMove("down", event);
		if (isKeycode(event, keyboardCodes.left)) this.handleMove("left", event);
		else if (isKeycode(event, keyboardCodes.right)) this.handleMove("right", event);
	}
	handleEnd(event, canceled) {
		this.manager.actions.stop({
			event,
			canceled
		});
		this.cleanup();
	}
	handleMove(direction, event) {
		var _a5, _b2;
		const { shape } = this.manager.dragOperation;
		const factor = event.shiftKey ? 5 : 1;
		let by = {
			x: 0,
			y: 0
		};
		let offset = (_b2 = (_a5 = this.options) == null ? void 0 : _a5.offset) != null ? _b2 : defaults.offset;
		if (typeof offset === "number") offset = {
			x: offset,
			y: offset
		};
		if (!shape) return;
		switch (direction) {
			case "up":
				by = {
					x: 0,
					y: -offset.y * factor
				};
				break;
			case "down":
				by = {
					x: 0,
					y: offset.y * factor
				};
				break;
			case "left":
				by = {
					x: -offset.x * factor,
					y: 0
				};
				break;
			case "right":
				by = {
					x: offset.x * factor,
					y: 0
				};
				break;
		}
		if (by.x || by.y) {
			event.preventDefault();
			this.manager.actions.move({
				event,
				by
			});
		}
	}
	sideEffects() {
		const autoScroller = this.manager.registry.plugins.get(AutoScroller);
		if ((autoScroller == null ? void 0 : autoScroller.disabled) === false) {
			autoScroller.disable();
			__privateGet$2(this, _cleanupFunctions).push(() => {
				autoScroller.enable();
			});
		}
	}
	cleanup() {
		__privateGet$2(this, _cleanupFunctions).forEach((cleanup) => cleanup());
		__privateSet$2(this, _cleanupFunctions, []);
	}
	destroy() {
		this.cleanup();
		this.listeners.clear();
	}
};
_cleanupFunctions = /* @__PURE__ */ new WeakMap();
_KeyboardSensor.configure = configurator(_KeyboardSensor);
_KeyboardSensor.defaults = defaults;
var KeyboardSensor = _KeyboardSensor;
function isKeycode(event, codes) {
	return codes.includes(event.code);
}
var _coordinates;
var DistanceConstraint = class extends ActivationConstraint {
	constructor() {
		super(...arguments);
		__privateAdd$2(this, _coordinates);
	}
	onEvent(event) {
		switch (event.type) {
			case "pointerdown":
				__privateSet$2(this, _coordinates, getEventCoordinates(event));
				break;
			case "pointermove":
				if (!__privateGet$2(this, _coordinates)) return;
				const { x, y } = getEventCoordinates(event);
				const delta = {
					x: x - __privateGet$2(this, _coordinates).x,
					y: y - __privateGet$2(this, _coordinates).y
				};
				const { tolerance } = this.options;
				if (tolerance && exceedsDistance(delta, tolerance)) {
					this.abort();
					return;
				}
				if (exceedsDistance(delta, this.options.value)) this.activate(event);
				break;
			case "pointerup":
				this.abort();
				break;
		}
	}
	abort() {
		__privateSet$2(this, _coordinates, void 0);
	}
};
_coordinates = /* @__PURE__ */ new WeakMap();
var _timeout2, _coordinates2;
var DelayConstraint = class extends ActivationConstraint {
	constructor() {
		super(...arguments);
		__privateAdd$2(this, _timeout2);
		__privateAdd$2(this, _coordinates2);
	}
	onEvent(event) {
		switch (event.type) {
			case "pointerdown":
				__privateSet$2(this, _coordinates2, getEventCoordinates(event));
				__privateSet$2(this, _timeout2, setTimeout(() => this.activate(event), this.options.value));
				break;
			case "pointermove":
				if (!__privateGet$2(this, _coordinates2)) return;
				const { x, y } = getEventCoordinates(event);
				if (exceedsDistance({
					x: x - __privateGet$2(this, _coordinates2).x,
					y: y - __privateGet$2(this, _coordinates2).y
				}, this.options.tolerance)) this.abort();
				break;
			case "pointerup":
				this.abort();
				break;
		}
	}
	abort() {
		if (__privateGet$2(this, _timeout2)) {
			clearTimeout(__privateGet$2(this, _timeout2));
			__privateSet$2(this, _coordinates2, void 0);
			__privateSet$2(this, _timeout2, void 0);
		}
	}
};
_timeout2 = /* @__PURE__ */ new WeakMap();
_coordinates2 = /* @__PURE__ */ new WeakMap();
var PointerActivationConstraints = class {};
PointerActivationConstraints.Delay = DelayConstraint;
PointerActivationConstraints.Distance = DistanceConstraint;
var defaults2 = Object.freeze({
	activationConstraints(event, source) {
		var _a5;
		const { pointerType, target } = event;
		if (pointerType === "mouse" && isElement(target) && (source.handle === target || ((_a5 = source.handle) == null ? void 0 : _a5.contains(target)))) return;
		if (pointerType === "touch") return [new PointerActivationConstraints.Delay({
			value: 250,
			tolerance: 5
		})];
		if (isTextInput(target) && !event.defaultPrevented) return [new PointerActivationConstraints.Delay({
			value: 200,
			tolerance: 0
		})];
		return [new PointerActivationConstraints.Delay({
			value: 200,
			tolerance: 10
		}), new PointerActivationConstraints.Distance({ value: 5 })];
	},
	preventActivation(event, source) {
		var _a5;
		const { target } = event;
		if (target === source.element) return false;
		if (target === source.handle) return false;
		if (!isElement(target)) return false;
		if ((_a5 = source.handle) == null ? void 0 : _a5.contains(target)) return false;
		return isInteractiveElement(target);
	}
});
var _cleanup;
var _PointerSensor = class _PointerSensor extends Sensor {
	constructor(manager, options) {
		super(manager);
		this.manager = manager;
		this.options = options;
		__privateAdd$2(this, _cleanup, /* @__PURE__ */ new Set());
		this.listeners = new Listeners();
		this.latest = {
			event: void 0,
			coordinates: void 0
		};
		this.handleMove = () => {
			const { event, coordinates: to } = this.latest;
			if (!event || !to) return;
			this.manager.actions.move({
				event,
				to
			});
		};
		this.handleCancel = this.handleCancel.bind(this);
		this.handlePointerUp = this.handlePointerUp.bind(this);
		this.handleKeyDown = this.handleKeyDown.bind(this);
	}
	activationConstraints(event, source, options = this.options) {
		const { activationConstraints = defaults2.activationConstraints } = options != null ? options : {};
		return typeof activationConstraints === "function" ? activationConstraints(event, source) : activationConstraints;
	}
	bind(source, options = this.options) {
		return j(() => {
			var _a5;
			const controller = new AbortController();
			const { signal: signal3 } = controller;
			const listener = (event) => {
				if (isPointerEvent(event)) this.handlePointerDown(event, source, options);
			};
			let targets = [(_a5 = source.handle) != null ? _a5 : source.element];
			if (options == null ? void 0 : options.activatorElements) if (Array.isArray(options.activatorElements)) targets = options.activatorElements;
			else targets = options.activatorElements(source);
			for (const target of targets) {
				if (!target) continue;
				patchWindow(target.ownerDocument.defaultView);
				target.addEventListener("pointerdown", listener, { signal: signal3 });
			}
			return () => controller.abort();
		});
	}
	handlePointerDown(event, source, options) {
		if (this.disabled || !event.isPrimary || event.button !== 0 || !isElement(event.target) || source.disabled || isCapturedBySensor(event) || !this.manager.dragOperation.status.idle) return;
		const { preventActivation = defaults2.preventActivation } = options != null ? options : {};
		if (preventActivation == null ? void 0 : preventActivation(event, source)) return;
		const { target } = event;
		const isNativeDraggable = isHTMLElement(target) && target.draggable && target.getAttribute("draggable") === "true";
		const offset = getFrameTransform(source.element);
		const { x, y } = getEventCoordinates(event);
		this.initialCoordinates = {
			x: x * offset.scaleX + offset.x,
			y: y * offset.scaleY + offset.y
		};
		const constraints = this.activationConstraints(event, source, options);
		event.sensor = this;
		const controller = new ActivationController(constraints, (event2) => this.handleStart(source, event2));
		controller.signal.onabort = () => this.handleCancel(event);
		controller.onEvent(event);
		this.controller = controller;
		const documents = getDocuments();
		const unbindListeners = this.listeners.bind(documents, [
			{
				type: "pointermove",
				listener: (event2) => this.handlePointerMove(event2, source)
			},
			{
				type: "pointerup",
				listener: this.handlePointerUp,
				options: { capture: true }
			},
			{
				type: "pointercancel",
				listener: this.handleCancel
			},
			{
				type: "dragstart",
				listener: isNativeDraggable ? this.handleCancel : preventDefault,
				options: { capture: true }
			}
		]);
		const cleanup = () => {
			unbindListeners();
			this.initialCoordinates = void 0;
		};
		__privateGet$2(this, _cleanup).add(cleanup);
	}
	handlePointerMove(event, source) {
		var _a5, _b2;
		if (((_a5 = this.controller) == null ? void 0 : _a5.activated) === false) {
			(_b2 = this.controller) == null || _b2.onEvent(event);
			return;
		}
		if (this.manager.dragOperation.status.dragging) {
			const coordinates = getEventCoordinates(event);
			const offset = getFrameTransform(source.element);
			coordinates.x = coordinates.x * offset.scaleX + offset.x;
			coordinates.y = coordinates.y * offset.scaleY + offset.y;
			event.preventDefault();
			event.stopPropagation();
			this.latest.event = event;
			this.latest.coordinates = coordinates;
			scheduler.schedule(this.handleMove);
		}
	}
	handlePointerUp(event) {
		const { status } = this.manager.dragOperation;
		if (!status.idle) {
			event.preventDefault();
			event.stopPropagation();
			const canceled = !status.initialized;
			this.manager.actions.stop({
				event,
				canceled
			});
		}
		this.cleanup();
	}
	handleKeyDown(event) {
		if (event.key === "Escape") {
			event.preventDefault();
			this.handleCancel(event);
		}
	}
	handleStart(source, event) {
		const { manager, initialCoordinates } = this;
		if (!initialCoordinates || !manager.dragOperation.status.idle) return;
		if (event.defaultPrevented) return;
		if (manager.actions.start({
			coordinates: initialCoordinates,
			event,
			source
		}).signal.aborted) return this.cleanup();
		event.preventDefault();
		const pointerCaptureTarget = getDocument(event.target).body;
		try {
			pointerCaptureTarget.setPointerCapture(event.pointerId);
		} catch (e) {
			this.handleCancel(event);
			return;
		}
		const listenerTargets = isElement(event.target) ? [event.target, pointerCaptureTarget] : pointerCaptureTarget;
		const unbind = this.listeners.bind(listenerTargets, [
			{
				type: "touchmove",
				listener: preventDefault,
				options: { passive: false }
			},
			{
				type: "click",
				listener: preventDefault
			},
			{
				type: "contextmenu",
				listener: preventDefault
			},
			{
				type: "keydown",
				listener: this.handleKeyDown
			}
		]);
		__privateGet$2(this, _cleanup).add(unbind);
	}
	handleCancel(event) {
		const { dragOperation } = this.manager;
		if (dragOperation.status.initialized) this.manager.actions.stop({
			event,
			canceled: true
		});
		this.cleanup();
	}
	cleanup() {
		const { controller } = this;
		this.controller = void 0;
		if (controller && !controller.signal.aborted) controller.abort();
		this.latest = {
			event: void 0,
			coordinates: void 0
		};
		__privateGet$2(this, _cleanup).forEach((cleanup) => cleanup());
		__privateGet$2(this, _cleanup).clear();
	}
	destroy() {
		this.cleanup();
		this.listeners.clear();
	}
};
_cleanup = /* @__PURE__ */ new WeakMap();
_PointerSensor.configure = configurator(_PointerSensor);
_PointerSensor.defaults = defaults2;
var PointerSensor = _PointerSensor;
function isCapturedBySensor(event) {
	return "sensor" in event;
}
function preventDefault(event) {
	event.preventDefault();
}
function noop() {}
var windows = /* @__PURE__ */ new WeakSet();
function patchWindow(window) {
	if (!window || windows.has(window)) return;
	window.addEventListener("touchmove", noop, {
		capture: false,
		passive: false
	});
	windows.add(window);
}
var defaultPreset = {
	modifiers: [],
	plugins: [
		Accessibility,
		AutoScroller,
		Cursor,
		Feedback,
		PreventSelection
	],
	sensors: [PointerSensor, KeyboardSensor]
};
var DragDropManager = class extends DragDropManager$1 {
	constructor(input = {}) {
		const plugins = resolveCustomizable(input.plugins, defaultPreset.plugins);
		const sensors = resolveCustomizable(input.sensors, defaultPreset.sensors);
		const modifiers = resolveCustomizable(input.modifiers, defaultPreset.modifiers);
		super(__spreadProps$3(__spreadValues$3({}, input), {
			plugins: [
				ScrollListener,
				Scroller,
				StyleInjector,
				...plugins
			],
			sensors,
			modifiers
		}));
	}
};
var _element_dec, _handle_dec, _c, _init5, _handle, _element$1;
var Draggable = class extends (_c = Draggable$1, _handle_dec = [reactive], _element_dec = [reactive], _c) {
	constructor(_a5, manager) {
		var _b2 = _a5, { element, effects: effects2 = () => [], handle } = _b2, input = __objRest$2(_b2, [
			"element",
			"effects",
			"handle"
		]);
		super(__spreadValues$3({ effects: () => [...effects2(), () => {
			var _a6, _b3;
			const { manager: manager2 } = this;
			if (!manager2) return;
			const unbindFunctions = ((_b3 = (_a6 = this.sensors) == null ? void 0 : _a6.map(descriptor)) != null ? _b3 : [...manager2.sensors]).map((entry) => {
				const sensorInstance = entry instanceof Sensor ? entry : manager2.registry.register(entry.plugin);
				const options = entry instanceof Sensor ? void 0 : entry.options;
				return sensorInstance.bind(this, options);
			});
			return function cleanup() {
				unbindFunctions.forEach((unbind) => unbind());
			};
		}] }, input), manager);
		__privateAdd$2(this, _handle, __runInitializers$2(_init5, 8, this)), __runInitializers$2(_init5, 11, this);
		__privateAdd$2(this, _element$1, __runInitializers$2(_init5, 12, this)), __runInitializers$2(_init5, 15, this);
		this.element = element;
		this.handle = handle;
	}
};
_init5 = __decoratorStart$2(_c);
_handle = /* @__PURE__ */ new WeakMap();
_element$1 = /* @__PURE__ */ new WeakMap();
__decorateElement$2(_init5, 4, "handle", _handle_dec, Draggable, _handle);
__decorateElement$2(_init5, 4, "element", _element_dec, Draggable, _element$1);
__decoratorMetadata$2(_init5, Draggable);
var _proxy_dec, _element_dec2, _c2, _init6, _element2, _d, element_get, element_set, _Droppable_instances, _proxy;
var Droppable = class extends (_c2 = Droppable$1, _element_dec2 = [reactive], _proxy_dec = [reactive], _c2) {
	constructor(_a5, manager) {
		var _b2 = _a5, { element, effects: effects2 = () => [] } = _b2, input = __objRest$2(_b2, ["element", "effects"]);
		const { collisionDetector = defaultCollisionDetection$1 } = input;
		const updateShape = (boundingClientRect) => {
			const { manager: manager2, element: element2 } = this;
			if (!element2 || boundingClientRect === null) {
				this.shape = void 0;
				return;
			}
			if (!manager2) return;
			const updatedShape = new DOMRectangle(element2);
			const shape = o(() => this.shape);
			if (updatedShape && (shape == null ? void 0 : shape.equals(updatedShape))) return shape;
			this.shape = updatedShape;
			return updatedShape;
		};
		const observePosition = y(false);
		super(__spreadProps$3(__spreadValues$3({}, input), {
			collisionDetector,
			effects: () => [
				...effects2(),
				() => {
					const { element: element2, manager: manager2 } = this;
					if (!manager2) return;
					const { dragOperation } = manager2;
					const { source } = dragOperation;
					observePosition.value = Boolean(source && dragOperation.status.initialized && element2 && !this.disabled && this.accepts(source));
				},
				() => {
					const { element: element2 } = this;
					if (observePosition.value && element2) {
						const positionObserver = new FrameObserver(element2, updateShape);
						return () => {
							positionObserver.disconnect();
							this.shape = void 0;
						};
					}
				},
				() => {
					var _a6;
					if ((_a6 = this.manager) == null ? void 0 : _a6.dragOperation.status.initialized) return () => {
						this.shape = void 0;
					};
				}
			]
		}), manager);
		__privateAdd$2(this, _Droppable_instances);
		__privateAdd$2(this, _element2, __runInitializers$2(_init6, 8, this)), __runInitializers$2(_init6, 11, this);
		__privateAdd$2(this, _proxy, __runInitializers$2(_init6, 12, this)), __runInitializers$2(_init6, 15, this);
		this.element = element;
		this.refreshShape = () => updateShape();
	}
	set element(element) {
		__privateSet$2(this, _Droppable_instances, element, element_set);
	}
	get element() {
		var _a5;
		return (_a5 = this.proxy) != null ? _a5 : __privateGet$2(this, _Droppable_instances, element_get);
	}
};
_init6 = __decoratorStart$2(_c2);
_element2 = /* @__PURE__ */ new WeakMap();
_Droppable_instances = /* @__PURE__ */ new WeakSet();
_proxy = /* @__PURE__ */ new WeakMap();
_d = __decorateElement$2(_init6, 20, "#element", _element_dec2, _Droppable_instances, _element2), element_get = _d.get, element_set = _d.set;
__decorateElement$2(_init6, 4, "proxy", _proxy_dec, Droppable, _proxy);
__decoratorMetadata$2(_init6, Droppable);
//#endregion
//#region node_modules/@dnd-kit/react/utilities.js
var import_react_dom = require_react_dom();
function isRef(value) {
	return value != null && typeof value === "object" && "current" in value;
}
function currentValue(value) {
	var _a;
	if (value == null) return;
	if (isRef(value)) return (_a = value.current) != null ? _a : void 0;
	return value;
}
//#endregion
//#region node_modules/@dnd-kit/react/hooks.js
var useIsomorphicLayoutEffect = typeof window !== "undefined" && typeof window.document !== "undefined" && typeof window.document.createElement !== "undefined" ? import_react.useLayoutEffect : import_react.useEffect;
function useForceUpdate() {
	const setState = (0, import_react.useState)(0)[1];
	return (0, import_react.useCallback)(() => {
		setState((value) => value + 1);
	}, [setState]);
}
function useDeepSignal(target, synchronous) {
	const tracked = (0, import_react.useRef)(/* @__PURE__ */ new Map());
	const forceUpdate = useForceUpdate();
	useIsomorphicLayoutEffect(() => {
		if (!target) {
			tracked.current.clear();
			return;
		}
		return j(() => {
			var _a;
			let stale = false;
			let sync = false;
			for (const entry of tracked.current) {
				const [key] = entry;
				const value = o(() => entry[1]);
				const latestValue = target[key];
				if (value !== latestValue) {
					stale = true;
					tracked.current.set(key, latestValue);
					sync = (_a = synchronous == null ? void 0 : synchronous(key, value, latestValue)) != null ? _a : false;
				}
			}
			if (stale) if (sync) queueMicrotask(() => (0, import_react_dom.flushSync)(forceUpdate));
			else forceUpdate();
		});
	}, [target]);
	return (0, import_react.useMemo)(() => target ? new Proxy(target, { get(target2, key) {
		const value = target2[key];
		tracked.current.set(key, value);
		return value;
	} }) : target, [target]);
}
function useImmediateEffect(callback, _) {
	callback();
}
function useLatest(value) {
	const valueRef = (0, import_react.useRef)(value);
	useIsomorphicLayoutEffect(() => {
		valueRef.current = value;
	}, [value]);
	return valueRef;
}
function useOnValueChange(value, onChange, effect3 = import_react.useEffect, compare = Object.is) {
	const tracked = (0, import_react.useRef)(value);
	effect3(() => {
		const oldValue = tracked.current;
		if (!compare(value, oldValue)) {
			tracked.current = value;
			onChange(value, oldValue);
		}
	}, [onChange, value]);
}
function useOnElementChange(value, onChange) {
	const previous = (0, import_react.useRef)(currentValue(value));
	useIsomorphicLayoutEffect(() => {
		const current = currentValue(value);
		if (current !== previous.current) {
			previous.current = current;
			onChange(current);
		}
	});
}
//#endregion
//#region node_modules/@dnd-kit/react/index.js
var import_jsx_runtime = require_jsx_runtime();
var __defProp$2 = Object.defineProperty;
var __defProps$2 = Object.defineProperties;
var __getOwnPropDescs$2 = Object.getOwnPropertyDescriptors;
var __getOwnPropSymbols$2 = Object.getOwnPropertySymbols;
var __hasOwnProp$2 = Object.prototype.hasOwnProperty;
var __propIsEnum$2 = Object.prototype.propertyIsEnumerable;
var __defNormalProp$2 = (obj, key, value) => key in obj ? __defProp$2(obj, key, {
	enumerable: true,
	configurable: true,
	writable: true,
	value
}) : obj[key] = value;
var __spreadValues$2 = (a, b) => {
	for (var prop in b || (b = {})) if (__hasOwnProp$2.call(b, prop)) __defNormalProp$2(a, prop, b[prop]);
	if (__getOwnPropSymbols$2) {
		for (var prop of __getOwnPropSymbols$2(b)) if (__propIsEnum$2.call(b, prop)) __defNormalProp$2(a, prop, b[prop]);
	}
	return a;
};
var __spreadProps$2 = (a, b) => __defProps$2(a, __getOwnPropDescs$2(b));
var __objRest$1 = (source, exclude) => {
	var target = {};
	for (var prop in source) if (__hasOwnProp$2.call(source, prop) && exclude.indexOf(prop) < 0) target[prop] = source[prop];
	if (source != null && __getOwnPropSymbols$2) {
		for (var prop of __getOwnPropSymbols$2(source)) if (exclude.indexOf(prop) < 0 && __propIsEnum$2.call(source, prop)) target[prop] = source[prop];
	}
	return target;
};
var DragDropContext = (0, import_react.createContext)(new DragDropManager());
var Renderer = (0, import_react.memo)((0, import_react.forwardRef)(({ children }, ref) => {
	const [transitionCount, setTransitionCount] = (0, import_react.useState)(0);
	const rendering = (0, import_react.useRef)(null);
	const resolver = (0, import_react.useRef)(null);
	const renderer = (0, import_react.useMemo)(() => ({
		renderer: { get rendering() {
			var _a2;
			return (_a2 = rendering.current) != null ? _a2 : Promise.resolve();
		} },
		trackRendering(callback) {
			if (!rendering.current) rendering.current = new Promise((resolve) => {
				resolver.current = resolve;
			});
			(0, import_react.startTransition)(() => {
				callback();
				setTransitionCount((count) => count + 1);
			});
		}
	}), []);
	useIsomorphicLayoutEffect(() => {
		var _a2;
		(_a2 = resolver.current) == null || _a2.call(resolver);
		rendering.current = null;
	}, [children, transitionCount]);
	(0, import_react.useImperativeHandle)(ref, () => renderer);
	return null;
}));
var options = [void 0, deepEqual];
function DragDropProvider(_a2) {
	var _b = _a2, { children, onCollision, onBeforeDragStart, onDragStart, onDragMove, onDragOver, onDragEnd } = _b, input = __objRest$1(_b, [
		"children",
		"onCollision",
		"onBeforeDragStart",
		"onDragStart",
		"onDragMove",
		"onDragOver",
		"onDragEnd"
	]);
	const rendererRef = (0, import_react.useRef)(null);
	const { plugins: pluginsInput, modifiers: modifiersInput, sensors: sensorsInput } = input;
	const plugins = resolveCustomizable(pluginsInput, defaultPreset.plugins);
	const sensors = resolveCustomizable(sensorsInput, defaultPreset.sensors);
	const modifiers = resolveCustomizable(modifiersInput, defaultPreset.modifiers);
	const handleBeforeDragStart = useLatest(onBeforeDragStart);
	const handleDragStart = useLatest(onDragStart);
	const handleDragOver = useLatest(onDragOver);
	const handleDragMove = useLatest(onDragMove);
	const handleDragEnd = useLatest(onDragEnd);
	const handleCollision = useLatest(onCollision);
	const manager = useStableInstance(() => {
		var _a3;
		return (_a3 = input.manager) != null ? _a3 : new DragDropManager(input);
	});
	(0, import_react.useEffect)(() => {
		if (!rendererRef.current) throw new Error("Renderer not found");
		const { renderer, trackRendering } = rendererRef.current;
		const { monitor } = manager;
		manager.renderer = renderer;
		const listeners = [
			monitor.addEventListener("beforedragstart", (event) => {
				const callback = handleBeforeDragStart.current;
				if (callback) trackRendering(() => callback(event, manager));
			}),
			monitor.addEventListener("dragstart", (event) => {
				var _a3;
				return (_a3 = handleDragStart.current) == null ? void 0 : _a3.call(handleDragStart, event, manager);
			}),
			monitor.addEventListener("dragover", (event) => {
				const callback = handleDragOver.current;
				if (callback) trackRendering(() => callback(event, manager));
			}),
			monitor.addEventListener("dragmove", (event) => {
				const callback = handleDragMove.current;
				if (callback) trackRendering(() => callback(event, manager));
			}),
			monitor.addEventListener("dragend", (event) => {
				const callback = handleDragEnd.current;
				if (callback) trackRendering(() => callback(event, manager));
			}),
			monitor.addEventListener("collision", (event) => {
				var _a3;
				return (_a3 = handleCollision.current) == null ? void 0 : _a3.call(handleCollision, event, manager);
			})
		];
		return () => listeners.forEach((dispose) => dispose());
	}, [manager]);
	useOnValueChange(plugins, () => manager && (manager.plugins = plugins), ...options);
	useOnValueChange(sensors, () => manager && (manager.sensors = sensors), ...options);
	useOnValueChange(modifiers, () => manager && (manager.modifiers = modifiers), ...options);
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(DragDropContext.Provider, {
		value: manager,
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)(Renderer, {
			ref: rendererRef,
			children
		}), children]
	});
}
function useStableInstance(create) {
	const ref = (0, import_react.useRef)(null);
	if (!ref.current) ref.current = create();
	(0, import_react.useInsertionEffect)(() => {
		return () => {
			var _a2;
			return (_a2 = ref.current) == null ? void 0 : _a2.destroy();
		};
	}, []);
	return ref.current;
}
function useDragDropManager() {
	return (0, import_react.useContext)(DragDropContext);
}
function useInstance(initializer) {
	var _a2;
	const manager = (_a2 = useDragDropManager()) != null ? _a2 : void 0;
	const [instance] = (0, import_react.useState)(() => initializer(manager));
	if (instance.manager !== manager) instance.manager = manager;
	useIsomorphicLayoutEffect(instance.register, [manager, instance]);
	return instance;
}
function useDraggable(input) {
	const { disabled, data, element, handle, id, modifiers, sensors, plugins } = input;
	const draggable = useInstance((manager) => new Draggable(__spreadProps$2(__spreadValues$2({}, input), {
		register: false,
		handle: currentValue(handle),
		element: currentValue(element)
	}), manager));
	const trackedDraggable = useDeepSignal(draggable, shouldUpdateSynchronously$1);
	useOnValueChange(id, () => draggable.id = id);
	useOnElementChange(handle, (handle2) => draggable.handle = handle2);
	useOnElementChange(element, (element2) => draggable.element = element2);
	useOnValueChange(data, () => data && (draggable.data = data));
	useOnValueChange(disabled, () => draggable.disabled = disabled === true);
	useOnValueChange(sensors, () => draggable.sensors = sensors);
	useOnValueChange(modifiers, () => draggable.modifiers = modifiers, void 0, deepEqual);
	useOnValueChange(plugins, () => draggable.plugins = plugins, void 0, deepEqual);
	useOnValueChange(input.alignment, () => draggable.alignment = input.alignment);
	return {
		draggable: trackedDraggable,
		get isDragging() {
			return trackedDraggable.isDragging;
		},
		get isDropping() {
			return trackedDraggable.isDropping;
		},
		get isDragSource() {
			return trackedDraggable.isDragSource;
		},
		handleRef: (0, import_react.useCallback)((element2) => {
			draggable.handle = element2 != null ? element2 : void 0;
		}, [draggable]),
		ref: (0, import_react.useCallback)((element2) => {
			var _a2, _b;
			if (!element2 && ((_a2 = draggable.element) == null ? void 0 : _a2.isConnected) && !((_b = draggable.manager) == null ? void 0 : _b.dragOperation.status.idle)) return;
			draggable.element = element2 != null ? element2 : void 0;
		}, [draggable])
	};
}
function shouldUpdateSynchronously$1(key, oldValue, newValue) {
	if (key === "isDragSource" && !newValue && oldValue) return true;
	return false;
}
var __create$1 = Object.create;
var __defProp2$1 = Object.defineProperty;
var __getOwnPropDesc$1 = Object.getOwnPropertyDescriptor;
var __knownSymbol$1 = (name, symbol) => (symbol = Symbol[name]) ? symbol : Symbol.for("Symbol." + name);
var __typeError$1 = (msg) => {
	throw TypeError(msg);
};
var __defNormalProp2$1 = (obj, key, value) => key in obj ? __defProp2$1(obj, key, {
	enumerable: true,
	configurable: true,
	writable: true,
	value
}) : obj[key] = value;
var __decoratorStart$1 = (base) => {
	var _a2;
	return [
		,
		,
		,
		__create$1((_a2 = base == null ? void 0 : base[__knownSymbol$1("metadata")]) != null ? _a2 : null)
	];
};
var __decoratorStrings$1 = [
	"class",
	"method",
	"getter",
	"setter",
	"accessor",
	"field",
	"value",
	"get",
	"set"
];
var __expectFn$1 = (fn) => fn !== void 0 && typeof fn !== "function" ? __typeError$1("Function expected") : fn;
var __decoratorContext$1 = (kind, name, done, metadata, fns) => ({
	kind: __decoratorStrings$1[kind],
	name,
	metadata,
	addInitializer: (fn) => done._ ? __typeError$1("Already initialized") : fns.push(__expectFn$1(fn || null))
});
var __decoratorMetadata$1 = (array, target) => __defNormalProp2$1(target, __knownSymbol$1("metadata"), array[3]);
var __runInitializers$1 = (array, flags, self, value) => {
	for (var i = 0, fns = array[flags >> 1], n = fns && fns.length; i < n; i++) fns[i].call(self);
	return value;
};
var __decorateElement$1 = (array, flags, name, decorators, target, extra) => {
	var it, done, ctx, access, k = flags & 7, s = false, p = false;
	var j = 2, key = __decoratorStrings$1[k + 5];
	var extraInitializers = array[j] || (array[j] = []);
	var desc = (target = target.prototype, __getOwnPropDesc$1(target, name));
	for (var i = decorators.length - 1; i >= 0; i--) {
		ctx = __decoratorContext$1(k, name, done = {}, array[3], extraInitializers);
		ctx.static = s, ctx.private = p, access = ctx.access = { has: (x) => name in x };
		access.get = (x) => x[name];
		it = (0, decorators[i])(desc[key], ctx), done._ = 1;
		__expectFn$1(it) && (desc[key] = it);
	}
	return desc && __defProp2$1(target, name, desc), target;
};
var __accessCheck$1 = (obj, member, msg) => member.has(obj) || __typeError$1("Cannot " + msg);
var __privateGet$1 = (obj, member, getter) => (__accessCheck$1(obj, member, "read from private field"), member.get(obj));
var __privateAdd$1 = (obj, member, value) => member.has(obj) ? __typeError$1("Cannot add the same private member more than once") : member instanceof WeakSet ? member.add(obj) : member.set(obj, value);
var __privateSet$1 = (obj, member, value, setter) => (__accessCheck$1(obj, member, "write to private field"), member.set(obj, value), value);
var Point = class _Point {
	/**
	* @param {number} Coordinate of the point on the horizontal axis
	* @param {number} Coordinate of the point on the vertical axis
	*/
	constructor(x, y) {
		this.x = x;
		this.y = y;
	}
	/**
	* Returns the delta between this point and another point.
	*
	* @param {Point} a - A point
	* @param {Point} b - Another point
	*/
	static delta(a, b) {
		return new _Point(a.x - b.x, a.y - b.y);
	}
	/**
	* Returns the distance (hypotenuse) between this point and another point.
	*
	* @param {Point} a - A point
	* @param {Point} b - Another point
	*/
	static distance(a, b) {
		return Math.hypot(a.x - b.x, a.y - b.y);
	}
	/**
	* Returns true if both points are equal.
	*
	* @param {Point} a - A point
	* @param {Point} b - Another point
	*/
	static equals(a, b) {
		return a.x === b.x && a.y === b.y;
	}
	static from({ x, y }) {
		return new _Point(x, y);
	}
};
var _direction_dec;
var _delta_dec;
var _a;
var _timestamp;
var _init$1;
var Position = class extends (_a = ValueHistory, _delta_dec = [derived], _direction_dec = [derived], _a) {
	constructor(initialValue) {
		const point = Point.from(initialValue);
		super(point, (a, b) => Point.equals(a, b));
		__runInitializers$1(_init$1, 5, this);
		__privateAdd$1(this, _timestamp, 0);
		this.velocity = {
			x: 0,
			y: 0
		};
	}
	get delta() {
		return Point.delta(this.current, this.initial);
	}
	get direction() {
		const { current, previous } = this;
		if (!previous) return null;
		const delta = {
			x: current.x - previous.x,
			y: current.y - previous.y
		};
		if (!delta.x && !delta.y) return null;
		if (Math.abs(delta.x) > Math.abs(delta.y)) return delta.x > 0 ? "right" : "left";
		return delta.y > 0 ? "down" : "up";
	}
	get current() {
		return super.current;
	}
	set current(coordinates) {
		const { current } = this;
		const point = Point.from(coordinates);
		const delta = {
			x: point.x - current.x,
			y: point.y - current.y
		};
		const timestamp = Date.now();
		const timeDelta = timestamp - __privateGet$1(this, _timestamp);
		const velocity = (delta2) => Math.round(delta2 / timeDelta * 100);
		n(() => {
			__privateSet$1(this, _timestamp, timestamp);
			this.velocity = {
				x: velocity(delta.x),
				y: velocity(delta.y)
			};
			super.current = point;
		});
	}
	reset(coordinates = this.defaultValue) {
		super.reset(Point.from(coordinates));
		this.velocity = {
			x: 0,
			y: 0
		};
	}
};
_init$1 = __decoratorStart$1(_a);
_timestamp = /* @__PURE__ */ new WeakMap();
__decorateElement$1(_init$1, 2, "delta", _delta_dec, Position);
__decorateElement$1(_init$1, 2, "direction", _direction_dec, Position);
__decoratorMetadata$1(_init$1, Position);
var Axis = /* @__PURE__ */ ((Axis2) => {
	Axis2["Horizontal"] = "x";
	Axis2["Vertical"] = "y";
	return Axis2;
})(Axis || {});
Object.values(Axis);
var pointerIntersection = ({ dragOperation, droppable }) => {
	const pointerCoordinates = dragOperation.position.current;
	if (!pointerCoordinates) return null;
	const { id } = droppable;
	if (!droppable.shape) return null;
	if (droppable.shape.containsPoint(pointerCoordinates)) return {
		id,
		value: 1 / Point.distance(droppable.shape.center, pointerCoordinates),
		type: CollisionType.PointerIntersection,
		priority: CollisionPriority.High
	};
	return null;
};
var shapeIntersection = ({ dragOperation, droppable }) => {
	const { shape } = dragOperation;
	if (!droppable.shape || !(shape == null ? void 0 : shape.current)) return null;
	const intersectionArea = shape.current.intersectionArea(droppable.shape);
	if (intersectionArea) {
		const { position } = dragOperation;
		const distance = Point.distance(droppable.shape.center, position.current);
		const value = intersectionArea / (shape.current.area + droppable.shape.area - intersectionArea) / distance;
		return {
			id: droppable.id,
			value,
			type: CollisionType.ShapeIntersection,
			priority: CollisionPriority.Normal
		};
	}
	return null;
};
var defaultCollisionDetection = (args) => {
	var _a2;
	return (_a2 = pointerIntersection(args)) != null ? _a2 : shapeIntersection(args);
};
function useDroppable(input) {
	const { collisionDetector, data, disabled, element, id, accept, type } = input;
	const droppable = useInstance((manager) => new Droppable(__spreadProps$2(__spreadValues$2({}, input), {
		register: false,
		element: currentValue(element)
	}), manager));
	const trackedDroppalbe = useDeepSignal(droppable);
	useOnValueChange(id, () => droppable.id = id);
	useOnElementChange(element, (element2) => droppable.element = element2);
	useOnValueChange(accept, () => droppable.accept = accept, void 0, deepEqual);
	useOnValueChange(collisionDetector, () => droppable.collisionDetector = collisionDetector != null ? collisionDetector : defaultCollisionDetection);
	useOnValueChange(data, () => data && (droppable.data = data));
	useOnValueChange(disabled, () => droppable.disabled = disabled === true);
	useOnValueChange(type, () => droppable.type = type);
	return {
		droppable: trackedDroppalbe,
		get isDropTarget() {
			return trackedDroppalbe.isDropTarget;
		},
		ref: (0, import_react.useCallback)((element2) => {
			var _a2, _b;
			if (!element2 && ((_a2 = droppable.element) == null ? void 0 : _a2.isConnected) && !((_b = droppable.manager) == null ? void 0 : _b.dragOperation.status.idle)) return;
			droppable.element = element2 != null ? element2 : void 0;
		}, [droppable])
	};
}
//#endregion
//#region src/components/EntitiesPage/GenericEntityBox.tsx
var GenericEntityBox = ({ entity, draging_item }) => {
	const { ref } = useDraggable({
		id: `entity-${entity.id}`,
		data: {
			type: "entity",
			config: entity
		}
	});
	const { entities_pages, removeEntity, fetchData } = useEntitiesPagesStore();
	const entity_page = entities_pages.find((page) => page.id === entity.entities_page_id);
	const removeEntityDialogRef = (0, import_react.useRef)(null);
	const [editDialogOpened, setEditDialogOpened] = (0, import_react.useState)(false);
	if (entity_page == void 0) return null;
	function getCookie(name) {
		let cookieValue = "";
		if (document.cookie && document.cookie !== "") {
			const cookies = document.cookie.split(";");
			for (let i = 0; i < cookies.length; i++) {
				const cookie = cookies[i].trim();
				if (cookie.substring(0, name.length + 1) === name + "=") {
					cookieValue = decodeURIComponent(cookie.substring(name.length + 1));
					break;
				}
			}
		}
		return cookieValue;
	}
	async function deleteEntity(id) {
		console.log("Deleting entity", id);
		if (entities_pages.find((page) => page.id === entity.entities_page_id)) fetch(`/rest/entities/${id}`, {
			credentials: "same-origin",
			method: "DELETE",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			}
		}).then(async (response) => {
			if (response.ok) {
				console.log("Successfully deleted entity", id);
				removeEntity(id);
			}
		}).catch((error) => {
			console.error("Error deleting entity", error);
		});
	}
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("dialog", {
			ref: removeEntityDialogRef,
			className: "modal",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "modal-box",
				children: [
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("h3", {
						className: "text-lg font-bold",
						children: "Delete entities page"
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("p", {
						className: "py-4",
						children: [
							"Are you sure you want to delete entity \"",
							entity.friendly_name,
							"\"?"
						]
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
						className: "flex justify-end join",
						children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
							onClick: () => removeEntityDialogRef.current?.close(),
							className: "btn btn-neutral join-item",
							children: "Cancel"
						}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
							onClick: () => {
								removeEntityDialogRef.current?.close();
								deleteEntity(entity.id);
							},
							className: "btn btn-error join-item",
							children: "Delete"
						})]
					})
				]
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("form", {
				method: "dialog",
				className: "modal-backdrop",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", { children: "close" })
			})]
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsx)(MultiStep_AddOrEditEntity, {
			type: entity.entity_type,
			room_id: entity_page.room_id,
			entities_page_id: entity.entities_page_id,
			room_view_position: entity.room_view_position,
			id: entity.id,
			opened: editDialogOpened,
			setOpened: setEditDialogOpened,
			onComplete: () => {
				fetchData(entity_page.room_id);
				setEditDialogOpened(false);
			}
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			ref,
			className: `draggable-entity-item bg-neutral/50 text-neutral-content rounded-box flex items-center justify-center indicator w-full h-full ${draging_item?.type != "entity_page" ? "hover:outline-1 hover:outline-accent" : ""} cursor-grab`,
			title: "Drag & drop to move this entity",
			children: [
				(() => {
					if (entity.controlled_by_nspanel_main_page) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "indicator-item badge badge-secondary me-16 w-6 h-6 flex items-center justify-center cursor-default",
						title: "Controlled from main page",
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "mdi mdi-home" })
					});
					return null;
				})(),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
					className: "indicator-item badge badge-info me-8 w-6 h-6 flex items-center justify-center cursor-pointer",
					title: "Edit entity",
					onClick: () => {
						setEditDialogOpened(true);
					},
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "mdi mdi-pencil" })
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
					className: "indicator-item badge badge-warning hover:badge-error w-6 h-6 flex items-center justify-center cursor-pointer",
					title: "Remove entity from page/room",
					onClick: () => {
						removeEntityDialogRef.current?.showModal();
					},
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "mdi mdi-close" })
				}),
				(() => {
					if (entity.controller == "home_assistant") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "status status-info shadow-none absolute top-2 left-2 cursor-default",
						title: "Controlled by Home Assistant"
					});
					else if (entity.controller == "openhab") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "status status-warning shadow-none absolute top-2 left-2 cursor-default",
						title: "Controlled by OpenHAB"
					});
					else if (entity.controller == "nspm_scene" || entity.controller == "nspm") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "status status-accent shadow-none absolute top-2 left-2 cursor-default",
						title: "Controlled by NSPanel Manager"
					});
					else return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "status status-error animate-ping absolute top-2 left-2 cursor-default",
						title: "Unknown controller"
					});
				})(),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
					className: "text-sm m-2",
					children: entity.friendly_name
				})
			]
		})
	] });
};
//#endregion
//#region src/components/EntitiesPage/GenericSceneBox.tsx
var GenericSceneBox = ({ scene, draging_item }) => {
	const { ref } = useDraggable({
		id: `scene-${scene.id}`,
		data: {
			type: "scene",
			config: scene
		}
	});
	const { entities_pages, removeScene, fetchData } = useEntitiesPagesStore();
	const entity_page = entities_pages.find((page) => page.id === scene.entities_page_id);
	const removeSceneDialogRef = (0, import_react.useRef)(null);
	const [editDialogOpened, setEditDialogOpened] = (0, import_react.useState)(false);
	function getCookie(name) {
		let cookieValue = "";
		if (document.cookie && document.cookie !== "") {
			const cookies = document.cookie.split(";");
			for (let i = 0; i < cookies.length; i++) {
				const cookie = cookies[i].trim();
				if (cookie.substring(0, name.length + 1) === name + "=") {
					cookieValue = decodeURIComponent(cookie.substring(name.length + 1));
					break;
				}
			}
		}
		return cookieValue;
	}
	async function deleteScene(id) {
		console.log("Deleting scene", id);
		if (entity_page) fetch(`/rest/scenes/${id}`, {
			credentials: "same-origin",
			method: "DELETE",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			}
		}).then(async (response) => {
			if (response.ok) {
				console.log("Successfully deleted scene", id);
				removeScene(id);
			}
		}).catch((error) => {
			console.error("Error deleting scene", error);
		});
	}
	if (!entity_page) return null;
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("dialog", {
			ref: removeSceneDialogRef,
			className: "modal",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "modal-box",
				children: [
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("h3", {
						className: "text-lg font-bold",
						children: "Delete scene"
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("p", {
						className: "py-4",
						children: [
							"Are you sure you want to delete scene \"",
							scene.friendly_name,
							"\"?"
						]
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
						className: "flex justify-end join",
						children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
							onClick: () => removeSceneDialogRef.current?.close(),
							className: "btn btn-neutral join-item",
							children: "Cancel"
						}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
							onClick: () => {
								removeSceneDialogRef.current?.close();
								deleteScene(scene.id);
							},
							className: "btn btn-error join-item",
							children: "Delete"
						})]
					})
				]
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("form", {
				method: "dialog",
				className: "modal-backdrop",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", { children: "close" })
			})]
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsx)(MultiStep_AddOrEditEntity, {
			type: "scene",
			room_id: entity_page.room_id,
			entities_page_id: scene.entities_page_id,
			room_view_position: scene.room_view_position,
			id: scene.id,
			opened: editDialogOpened,
			setOpened: setEditDialogOpened,
			onComplete: () => {
				fetchData(entity_page.room_id);
				setEditDialogOpened(false);
			}
		}),
		/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			ref,
			className: `draggable-entity-item bg-neutral/50 rounded-box text-neutral-content flex items-center justify-center indicator w-full h-full ${draging_item ? "hover:outline-1 hover:outline-accent" : ""} cursor-grab`,
			title: "Drag & drop to move this entity",
			children: [
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
					className: "indicator-item badge badge-info me-8 w-6 h-6 flex items-center justify-center cursor-pointer",
					title: "Edit scene",
					onClick: () => setEditDialogOpened(true),
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "mdi mdi-pencil" })
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
					className: "indicator-item badge badge-warning hover:badge-error w-6 h-6 flex items-center justify-center cursor-pointer",
					title: "Remove scene from page/room",
					onClick: () => {
						removeSceneDialogRef.current?.showModal();
					},
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "mdi mdi-close" })
				}),
				(() => {
					if (scene.controller == "home_assistant") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "status status-info shadow-none absolute top-2 left-2 cursor-default",
						title: "Controlled by Home Assistant"
					});
					else if (scene.controller == "openhab") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "status status-warning shadow-none absolute top-2 left-2 cursor-default",
						title: "Controlled by OpenHAB"
					});
					else if (scene.controller == "nspm_scene" || scene.controller == "nspm") return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "status status-accent shadow-none absolute top-2 left-2 cursor-default",
						title: "Controlled by NSPanel Manager"
					});
					else return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
						className: "status status-error animate-ping absolute top-2 left-2 cursor-default",
						title: "Unknown controller"
					});
				})(),
				/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
					className: "text-sm m-2",
					children: scene.friendly_name
				})
			]
		})
	] });
};
//#endregion
//#region src/components/EntitiesPage/EntitiesPageDropTarget.tsx
var EntitiesDropTarget = (0, import_react.forwardRef)(({ entities_page_id, room_view_position, type, children, draging_item }) => {
	const [addEditEntityDialogOpened, setAddEditEntityDialogOpened] = (0, import_react.useState)(false);
	const { entities_pages, fetchData } = useEntitiesPagesStore();
	const entities_page = entities_pages.find((page) => page.id === entities_page_id);
	const { ref, isDropTarget } = useDroppable({
		id: `entities_page-${entities_page_id}-room_view_position-${room_view_position}`,
		data: {
			accepts: type,
			type: "entities_page_drop_target",
			config: {
				type,
				entities_page_id,
				room_view_position
			}
		}
	});
	if (entities_page === void 0) return null;
	const isChildDragging = draging_item && draging_item.config && draging_item.config.entities_page_id === entities_page_id && draging_item.config.room_view_position === room_view_position;
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		ref,
		children: (() => {
			if (isChildDragging) return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: `rounded-box p-1 items-center justify-center w-full h-full bg-conic/[from_var(--border-angle)] from-base-200 via-accent/50 to-base-200 animate-rotate-border from-30% to-60%`,
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					className: "flex items-center justify-center rounded-box w-full h-full bg-base-200",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "italic font-thin",
						children: draging_item.config && draging_item.config.friendly_name
					})
				}), children]
			});
			else if (children) return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				className: `rounded-box ${draging_item && draging_item.type != "entity_page" && isDropTarget ? "border-2 border-solid border-accent" : ""} items-center justify-center w-full h-full`,
				children
			});
			return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: `rounded-box ${draging_item && draging_item.type != "entity_page" && isDropTarget ? "border-2 border-solid border-accent" : "border-dashed border-2 border-neutral/50"} flex items-center justify-center indicator w-full h-full`,
				children: [
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)(MultiStep_AddOrEditEntity, {
						type,
						room_id: entities_page.room_id,
						entities_page_id,
						room_view_position,
						onComplete: () => {
							fetchData(entities_page.room_id);
							setAddEditEntityDialogOpened(false);
						},
						opened: addEditEntityDialogOpened,
						setOpened: setAddEditEntityDialogOpened
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
						className: "italic font-thin",
						children: draging_item?.type !== "entity_page" && isDropTarget && draging_item && draging_item.config ? draging_item.config.friendly_name : "No " + (type === "scene" ? "scene" : "entity") + " set"
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
						hidden: draging_item?.type !== "entity_page" && isDropTarget,
						className: "btn btn-neutral hover:btn-success btn-xs btn-circle ms-2 flex items-center justify-center",
						title: `Add ${type === "scene" ? "scene" : "entity"}`,
						onClick: () => setAddEditEntityDialogOpened(true),
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "mdi mdi-plus" })
					})
				]
			});
		})()
	});
});
//#endregion
//#region node_modules/@dnd-kit/dom/sortable.js
var __create = Object.create;
var __defProp$1 = Object.defineProperty;
var __defProps$1 = Object.defineProperties;
var __getOwnPropDesc = Object.getOwnPropertyDescriptor;
var __getOwnPropDescs$1 = Object.getOwnPropertyDescriptors;
var __getOwnPropSymbols$1 = Object.getOwnPropertySymbols;
var __hasOwnProp$1 = Object.prototype.hasOwnProperty;
var __propIsEnum$1 = Object.prototype.propertyIsEnumerable;
var __knownSymbol = (name, symbol) => (symbol = Symbol[name]) ? symbol : Symbol.for("Symbol." + name);
var __typeError = (msg) => {
	throw TypeError(msg);
};
var __defNormalProp$1 = (obj, key, value) => key in obj ? __defProp$1(obj, key, {
	enumerable: true,
	configurable: true,
	writable: true,
	value
}) : obj[key] = value;
var __spreadValues$1 = (a, b) => {
	for (var prop in b || (b = {})) if (__hasOwnProp$1.call(b, prop)) __defNormalProp$1(a, prop, b[prop]);
	if (__getOwnPropSymbols$1) {
		for (var prop of __getOwnPropSymbols$1(b)) if (__propIsEnum$1.call(b, prop)) __defNormalProp$1(a, prop, b[prop]);
	}
	return a;
};
var __spreadProps$1 = (a, b) => __defProps$1(a, __getOwnPropDescs$1(b));
var __objRest = (source, exclude) => {
	var target = {};
	for (var prop in source) if (__hasOwnProp$1.call(source, prop) && exclude.indexOf(prop) < 0) target[prop] = source[prop];
	if (source != null && __getOwnPropSymbols$1) {
		for (var prop of __getOwnPropSymbols$1(source)) if (exclude.indexOf(prop) < 0 && __propIsEnum$1.call(source, prop)) target[prop] = source[prop];
	}
	return target;
};
var __decoratorStart = (base) => {
	var _a;
	return [
		,
		,
		,
		__create((_a = void 0) != null ? _a : null)
	];
};
var __decoratorStrings = [
	"class",
	"method",
	"getter",
	"setter",
	"accessor",
	"field",
	"value",
	"get",
	"set"
];
var __expectFn = (fn) => fn !== void 0 && typeof fn !== "function" ? __typeError("Function expected") : fn;
var __decoratorContext = (kind, name, done, metadata, fns) => ({
	kind: __decoratorStrings[kind],
	name,
	metadata,
	addInitializer: (fn) => done._ ? __typeError("Already initialized") : fns.push(__expectFn(fn || null))
});
var __decoratorMetadata = (array, target) => __defNormalProp$1(target, __knownSymbol("metadata"), array[3]);
var __runInitializers = (array, flags, self, value) => {
	for (var i = 0, fns = array[flags >> 1], n = fns && fns.length; i < n; i++) flags & 1 ? fns[i].call(self) : value = fns[i].call(self, value);
	return value;
};
var __decorateElement = (array, flags, name, decorators, target, extra) => {
	var fn, it, done, ctx, access, k = flags & 7, s = false, p = false;
	var j = array.length + 1, key = __decoratorStrings[k + 5];
	var initializers = array[j - 1] = [], extraInitializers = array[j] || (array[j] = []);
	var desc = (target = target.prototype, __getOwnPropDesc({
		get [name]() {
			return __privateGet(this, extra);
		},
		set [name](x) {
			return __privateSet(this, extra, x);
		}
	}, name));
	for (var i = decorators.length - 1; i >= 0; i--) {
		ctx = __decoratorContext(k, name, done = {}, array[3], extraInitializers);
		ctx.static = s, ctx.private = p, access = ctx.access = { has: (x) => name in x };
		access.get = (x) => x[name];
		access.set = (x, y) => x[name] = y;
		it = (0, decorators[i])({
			get: desc.get,
			set: desc.set
		}, ctx), done._ = 1;
		if (it === void 0) __expectFn(it) && (desc[key] = it);
		else if (typeof it !== "object" || it === null) __typeError("Object expected");
		else __expectFn(fn = it.get) && (desc.get = fn), __expectFn(fn = it.set) && (desc.set = fn), __expectFn(fn = it.init) && initializers.unshift(fn);
	}
	return desc && __defProp$1(target, name, desc), target;
};
var __accessCheck = (obj, member, msg) => member.has(obj) || __typeError("Cannot " + msg);
var __privateGet = (obj, member, getter) => (__accessCheck(obj, member, "read from private field"), member.get(obj));
var __privateAdd = (obj, member, value) => member.has(obj) ? __typeError("Cannot add the same private member more than once") : member instanceof WeakSet ? member.add(obj) : member.set(obj, value);
var __privateSet = (obj, member, value, setter) => (__accessCheck(obj, member, "write to private field"), member.set(obj, value), value);
function isSortable(element) {
	return element instanceof SortableDroppable || element instanceof SortableDraggable;
}
var TOLERANCE = 10;
var SortableKeyboardPlugin = class extends Plugin {
	constructor(manager) {
		super(manager);
		const cleanupEffect = j(() => {
			const { dragOperation } = manager;
			if (!isKeyboardEvent(dragOperation.activatorEvent)) return;
			if (!isSortable(dragOperation.source)) return;
			if (dragOperation.status.initialized) {
				const scroller = manager.registry.plugins.get(Scroller);
				if (scroller) {
					scroller.disable();
					return () => scroller.enable();
				}
			}
		});
		const unsubscribe = manager.monitor.addEventListener("dragmove", (event, manager2) => {
			queueMicrotask(() => {
				if (this.disabled || event.defaultPrevented || !event.nativeEvent) return;
				const { dragOperation } = manager2;
				if (!isKeyboardEvent(event.nativeEvent)) return;
				if (!isSortable(dragOperation.source)) return;
				if (!dragOperation.shape) return;
				const { actions, collisionObserver, registry } = manager2;
				const { by } = event;
				if (!by) return;
				const direction = getDirection(by);
				const { source, target } = dragOperation;
				const { center } = dragOperation.shape.current;
				const potentialTargets = [];
				const cleanup = [];
				n(() => {
					for (const droppable of registry.droppables) {
						const { id: id2 } = droppable;
						if (!droppable.accepts(source) || id2 === (target == null ? void 0 : target.id) && isSortable(droppable) || !droppable.element) continue;
						let previousShape = droppable.shape;
						const shape = new DOMRectangle(droppable.element, { getBoundingClientRect: (element) => getVisibleBoundingRectangle(element, void 0, .2) });
						if (!shape.height || !shape.width) continue;
						if (direction == "down" && center.y + TOLERANCE < shape.center.y || direction == "up" && center.y - TOLERANCE > shape.center.y || direction == "left" && center.x - TOLERANCE > shape.center.x || direction == "right" && center.x + TOLERANCE < shape.center.x) {
							potentialTargets.push(droppable);
							droppable.shape = shape;
							cleanup.push(() => droppable.shape = previousShape);
						}
					}
				});
				event.preventDefault();
				collisionObserver.disable();
				const collisions = collisionObserver.computeCollisions(potentialTargets, closestCorners);
				n(() => cleanup.forEach((clean) => clean()));
				const [firstCollision] = collisions;
				if (!firstCollision) return;
				const { id } = firstCollision;
				const { index, group } = source.sortable;
				actions.setDropTarget(id).then(() => {
					const { source: source2, target: target2, shape } = dragOperation;
					if (!source2 || !isSortable(source2) || !shape) return;
					const { index: newIndex, group: newGroup, target: targetElement } = source2.sortable;
					const updated = index !== newIndex || group !== newGroup;
					const element = updated ? targetElement : target2 == null ? void 0 : target2.element;
					if (!element) return;
					scrollIntoViewIfNeeded(element);
					const updatedShape = new DOMRectangle(element);
					if (!updatedShape) return;
					const delta = Rectangle.delta(updatedShape, Rectangle.from(shape.current.boundingRectangle), source2.alignment);
					actions.move({ by: delta });
					if (updated) actions.setDropTarget(source2.id).then(() => collisionObserver.enable());
					else collisionObserver.enable();
				});
			});
		});
		this.destroy = () => {
			unsubscribe();
			cleanupEffect();
		};
	}
};
function getDirection(delta) {
	const { x, y } = delta;
	if (x > 0) return "right";
	else if (x < 0) return "left";
	else if (y > 0) return "down";
	else if (y < 0) return "up";
}
var __defProp2 = Object.defineProperty;
var __defProps2 = Object.defineProperties;
var __getOwnPropDescs2 = Object.getOwnPropertyDescriptors;
var __getOwnPropSymbols2 = Object.getOwnPropertySymbols;
var __hasOwnProp2 = Object.prototype.hasOwnProperty;
var __propIsEnum2 = Object.prototype.propertyIsEnumerable;
var __defNormalProp2 = (obj, key, value) => key in obj ? __defProp2(obj, key, {
	enumerable: true,
	configurable: true,
	writable: true,
	value
}) : obj[key] = value;
var __spreadValues2 = (a, b) => {
	for (var prop in b || (b = {})) if (__hasOwnProp2.call(b, prop)) __defNormalProp2(a, prop, b[prop]);
	if (__getOwnPropSymbols2) {
		for (var prop of __getOwnPropSymbols2(b)) if (__propIsEnum2.call(b, prop)) __defNormalProp2(a, prop, b[prop]);
	}
	return a;
};
var __spreadProps2 = (a, b) => __defProps2(a, __getOwnPropDescs2(b));
function arrayMove(array, from, to) {
	if (from === to) return array;
	const newArray = array.slice();
	newArray.splice(to, 0, newArray.splice(from, 1)[0]);
	return newArray;
}
function hasSortableIndices(source) {
	return "initialIndex" in source && typeof source.initialIndex === "number" && "index" in source && typeof source.index === "number";
}
function mutate(items, event, mutation) {
	var _a, _b, _c;
	const { source, target, canceled } = event.operation;
	if (!source || !target || canceled) {
		if ("preventDefault" in event) event.preventDefault();
		return items;
	}
	const findIndex = (item, id) => item === id || typeof item === "object" && "id" in item && item.id === id;
	if (Array.isArray(items)) {
		const sourceIndex2 = items.findIndex((item) => findIndex(item, source.id));
		const targetIndex2 = items.findIndex((item) => findIndex(item, target.id));
		if (sourceIndex2 === -1 || targetIndex2 === -1) {
			if (hasSortableIndices(source)) {
				const from = source.initialIndex;
				const to = source.index;
				if (from === to || from < 0 || from >= items.length) {
					if ("preventDefault" in event) event.preventDefault();
					return items;
				}
				return mutation(items, from, to);
			}
			return items;
		}
		if (!canceled && "index" in source && typeof source.index === "number") {
			const projectedSourceIndex = source.index;
			if (projectedSourceIndex !== sourceIndex2) return mutation(items, sourceIndex2, projectedSourceIndex);
		}
		return mutation(items, sourceIndex2, targetIndex2);
	}
	const entries = Object.entries(items);
	let sourceIndex = -1;
	let sourceParent;
	let targetIndex = -1;
	let targetParent;
	for (const [id, children] of entries) {
		if (sourceIndex === -1) {
			sourceIndex = children.findIndex((item) => findIndex(item, source.id));
			if (sourceIndex !== -1) sourceParent = id;
		}
		if (targetIndex === -1) {
			targetIndex = children.findIndex((item) => findIndex(item, target.id));
			if (targetIndex !== -1) targetParent = id;
		}
		if (sourceIndex !== -1 && targetIndex !== -1) break;
	}
	if (sourceIndex === -1 && hasSortableIndices(source)) {
		const srcParent = source.initialGroup;
		const srcIndex = source.initialIndex;
		const tgtParent = source.group;
		const tgtIndex = source.index;
		if (srcParent == null || tgtParent == null || !(srcParent in items) || !(tgtParent in items)) {
			if ("preventDefault" in event) event.preventDefault();
			return items;
		}
		if (srcParent === tgtParent && srcIndex === tgtIndex) {
			if ("preventDefault" in event) event.preventDefault();
			return items;
		}
		if (srcParent === tgtParent) return __spreadProps2(__spreadValues2({}, items), { [srcParent]: mutation(items[srcParent], srcIndex, tgtIndex) });
		const sourceItem2 = items[srcParent][srcIndex];
		return __spreadProps2(__spreadValues2({}, items), {
			[srcParent]: [...items[srcParent].slice(0, srcIndex), ...items[srcParent].slice(srcIndex + 1)],
			[tgtParent]: [
				...items[tgtParent].slice(0, tgtIndex),
				sourceItem2,
				...items[tgtParent].slice(tgtIndex)
			]
		});
	}
	if (!source.manager) return items;
	const { dragOperation } = source.manager;
	const position = (_b = (_a = dragOperation.shape) == null ? void 0 : _a.current.center) != null ? _b : dragOperation.position.current;
	if (targetParent == null) {
		if (target.id in items) {
			const insertionIndex = target.shape && position.y > target.shape.center.y ? items[target.id].length : 0;
			targetParent = target.id;
			targetIndex = insertionIndex;
		}
	}
	if (sourceParent == null || targetParent == null || sourceParent === targetParent && sourceIndex === targetIndex) {
		if (sourceParent != null && sourceParent === targetParent && sourceIndex === targetIndex && hasSortableIndices(source)) {
			const hasGroupChanged = source.group != null && source.group !== sourceParent;
			const hasIndexChanged = source.index !== sourceIndex;
			if (hasGroupChanged || hasIndexChanged) {
				const reconciledTargetParent = (_c = source.group) != null ? _c : sourceParent;
				if (reconciledTargetParent in items) {
					if (sourceParent === reconciledTargetParent) return __spreadProps2(__spreadValues2({}, items), { [sourceParent]: mutation(items[sourceParent], sourceIndex, source.index) });
					const sourceItem2 = items[sourceParent][sourceIndex];
					return __spreadProps2(__spreadValues2({}, items), {
						[sourceParent]: [...items[sourceParent].slice(0, sourceIndex), ...items[sourceParent].slice(sourceIndex + 1)],
						[reconciledTargetParent]: [
							...items[reconciledTargetParent].slice(0, source.index),
							sourceItem2,
							...items[reconciledTargetParent].slice(source.index)
						]
					});
				}
			}
		}
		if ("preventDefault" in event) event.preventDefault();
		return items;
	}
	if (sourceParent === targetParent) return __spreadProps2(__spreadValues2({}, items), { [sourceParent]: mutation(items[sourceParent], sourceIndex, targetIndex) });
	const modifier = target.shape && Math.round(position.y) > Math.round(target.shape.center.y) ? 1 : 0;
	const sourceItem = items[sourceParent][sourceIndex];
	return __spreadProps2(__spreadValues2({}, items), {
		[sourceParent]: [...items[sourceParent].slice(0, sourceIndex), ...items[sourceParent].slice(sourceIndex + 1)],
		[targetParent]: [
			...items[targetParent].slice(0, targetIndex + modifier),
			sourceItem,
			...items[targetParent].slice(targetIndex + modifier)
		]
	});
}
function move(items, event) {
	return mutate(items, event, arrayMove);
}
var defaultGroup = "__default__";
var OptimisticSortingPlugin = class extends Plugin {
	constructor(manager) {
		super(manager);
		const getSortableInstances = () => {
			const sortableInstances = /* @__PURE__ */ new Map();
			for (const droppable of manager.registry.droppables) if (droppable instanceof SortableDroppable) {
				const { sortable } = droppable;
				const { group } = sortable;
				let instances = sortableInstances.get(group);
				if (!instances) {
					instances = /* @__PURE__ */ new Set();
					sortableInstances.set(group, instances);
				}
				instances.add(sortable);
			}
			for (const [group, instances] of sortableInstances) sortableInstances.set(group, new Set(sort(instances)));
			return sortableInstances;
		};
		const unsubscribe = [manager.monitor.addEventListener("dragover", (event, manager2) => {
			if (this.disabled) return;
			const { dragOperation } = manager2;
			const { source, target } = dragOperation;
			if (!isSortable(source) || !isSortable(target)) return;
			if (source.sortable === target.sortable) return;
			const instances = getSortableInstances();
			const sameGroup = source.sortable.group === target.sortable.group;
			const sourceInstances = instances.get(source.sortable.group);
			const targetInstances = sameGroup ? sourceInstances : instances.get(target.sortable.group);
			if (!sourceInstances || !targetInstances) return;
			queueMicrotask(() => {
				if (event.defaultPrevented) return;
				manager2.renderer.rendering.then(() => {
					var _a, _b, _c;
					const newInstances = getSortableInstances();
					for (const [group, sortableInstances] of instances.entries()) {
						const entries = Array.from(sortableInstances).entries();
						for (const [index, sortable] of entries) if (sortable.index !== index || sortable.group !== group || !((_a = newInstances.get(group)) == null ? void 0 : _a.has(sortable))) return;
					}
					const sourceElement = source.sortable.element;
					const targetElement = target.sortable.element;
					if (!targetElement || !sourceElement) return;
					if (!sameGroup && target.id === source.sortable.group) return;
					const orderedSourceSortables = sort(sourceInstances);
					const orderedTargetSortables = sameGroup ? orderedSourceSortables : sort(targetInstances);
					const sourceGroup = (_b = source.sortable.group) != null ? _b : defaultGroup;
					const targetGroup = (_c = target.sortable.group) != null ? _c : defaultGroup;
					const state = {
						[sourceGroup]: orderedSourceSortables,
						[targetGroup]: orderedTargetSortables
					};
					const newState = move(state, event);
					if (state === newState) return;
					const sourceIndex = newState[targetGroup].indexOf(source.sortable);
					const targetIndex = newState[targetGroup].indexOf(target.sortable);
					manager2.collisionObserver.disable();
					reorder(sourceElement, sourceIndex, targetElement, targetIndex);
					n(() => {
						for (const [index, sortable] of newState[sourceGroup].entries()) sortable.index = index;
						if (!sameGroup) for (const [index, sortable] of newState[targetGroup].entries()) {
							sortable.group = target.sortable.group;
							sortable.index = index;
						}
					});
					manager2.actions.setDropTarget(source.id).then(() => manager2.collisionObserver.enable());
				});
			});
		}), manager.monitor.addEventListener("dragend", (event, manager2) => {
			if (!event.canceled) return;
			const { dragOperation } = manager2;
			const { source } = dragOperation;
			if (!isSortable(source)) return;
			if (source.sortable.initialIndex === source.sortable.index && source.sortable.initialGroup === source.sortable.group) return;
			queueMicrotask(() => {
				const instances = getSortableInstances();
				const initialGroupInstances = instances.get(source.sortable.initialGroup);
				if (!initialGroupInstances) return;
				manager2.renderer.rendering.then(() => {
					for (const [group, sortableInstances] of instances.entries()) {
						const entries = Array.from(sortableInstances).entries();
						for (const [index, sortable] of entries) if (sortable.index !== index || sortable.group !== group) return;
					}
					const initialGroup = sort(initialGroupInstances);
					const sourceElement = source.sortable.element;
					const target = initialGroup[source.sortable.initialIndex];
					const targetElement = target == null ? void 0 : target.element;
					if (!target || !targetElement || !sourceElement) return;
					reorder(sourceElement, target.index, targetElement, source.index);
					n(() => {
						for (const [_, sortableInstances] of instances.entries()) {
							const entries = Array.from(sortableInstances).values();
							for (const sortable of entries) {
								sortable.index = sortable.initialIndex;
								sortable.group = sortable.initialGroup;
							}
						}
					});
				});
			});
		})];
		this.destroy = () => {
			for (const unsubscribeListener of unsubscribe) unsubscribeListener();
		};
	}
};
function reorder(sourceElement, sourceIndex, targetElement, targetIndex) {
	const position = targetIndex < sourceIndex ? "afterend" : "beforebegin";
	targetElement.insertAdjacentElement(position, sourceElement);
}
function sortByIndex(a, b) {
	return a.index - b.index;
}
function sort(instances) {
	return Array.from(instances).sort(sortByIndex);
}
var defaultPlugins = [SortableKeyboardPlugin, OptimisticSortingPlugin];
var defaultSortableTransition = {
	duration: 250,
	easing: "cubic-bezier(0.25, 1, 0.5, 1)",
	idle: false
};
var store = new WeakStore();
var _group_dec, _index_dec = [reactive], _init, _index, _previousGroup, _previousIndex, _group, _element;
_group_dec = [reactive];
var Sortable2 = class {
	constructor(_a, manager) {
		__privateAdd(this, _index, __runInitializers(_init, 8, this)), __runInitializers(_init, 11, this);
		__privateAdd(this, _previousGroup);
		__privateAdd(this, _previousIndex);
		__privateAdd(this, _group, __runInitializers(_init, 12, this)), __runInitializers(_init, 15, this);
		__privateAdd(this, _element);
		this.register = () => {
			n(() => {
				var _a, _b;
				(_a = this.manager) == null || _a.registry.register(this.droppable);
				(_b = this.manager) == null || _b.registry.register(this.draggable);
			});
			return () => this.unregister();
		};
		this.unregister = () => {
			n(() => {
				var _a, _b;
				(_a = this.manager) == null || _a.registry.unregister(this.droppable);
				(_b = this.manager) == null || _b.registry.unregister(this.draggable);
			});
		};
		this.destroy = () => {
			n(() => {
				this.droppable.destroy();
				this.draggable.destroy();
			});
		};
		var _b = _a, { effects: inputEffects = () => [], group, index, sensors, type, transition = defaultSortableTransition, plugins: pluginsInput } = _b, input = __objRest(_b, [
			"effects",
			"group",
			"index",
			"sensors",
			"type",
			"transition",
			"plugins"
		]);
		const plugins = resolveCustomizable(pluginsInput, defaultPlugins);
		this.droppable = new SortableDroppable(input, manager, this);
		this.draggable = new SortableDraggable(__spreadProps$1(__spreadValues$1({}, input), {
			plugins,
			effects: () => [
				() => {
					var _a2, _b2, _c;
					const status = (_a2 = this.manager) == null ? void 0 : _a2.dragOperation.status;
					if ((status == null ? void 0 : status.initializing) && this.id === ((_c = (_b2 = this.manager) == null ? void 0 : _b2.dragOperation.source) == null ? void 0 : _c.id)) store.clear(this.manager);
					if (status == null ? void 0 : status.dragging) store.set(this.manager, this.id, o(() => ({
						initialIndex: this.index,
						initialGroup: this.group
					})));
				},
				() => {
					const { index: index2, group: group2, manager: _ } = this;
					const previousIndex = __privateGet(this, _previousIndex);
					const previousGroup = __privateGet(this, _previousGroup);
					if (index2 !== previousIndex || group2 !== previousGroup) {
						__privateSet(this, _previousIndex, index2);
						__privateSet(this, _previousGroup, group2);
						this.animate();
					}
				},
				() => {
					var _a2, _b2;
					const { target } = this;
					const { isDragSource } = this.draggable;
					if (((_b2 = (_a2 = this.draggable.pluginConfig(Feedback)) == null ? void 0 : _a2.feedback) != null ? _b2 : "default") === "move" && isDragSource) this.droppable.disabled = !target;
				},
				...inputEffects()
			],
			type,
			sensors
		}), manager, this);
		__privateSet(this, _element, input.element);
		this.manager = manager;
		this.index = index;
		__privateSet(this, _previousIndex, index);
		this.group = group;
		__privateSet(this, _previousGroup, group);
		this.type = type;
		this.transition = transition;
	}
	get initialIndex() {
		var _a, _b;
		return (_b = (_a = store.get(this.manager, this.id)) == null ? void 0 : _a.initialIndex) != null ? _b : this.index;
	}
	get initialGroup() {
		var _a, _b;
		return (_b = (_a = store.get(this.manager, this.id)) == null ? void 0 : _a.initialGroup) != null ? _b : this.group;
	}
	animate() {
		o(() => {
			const { manager, transition } = this;
			const { shape } = this.droppable;
			if (!manager) return;
			const { idle } = manager.dragOperation.status;
			if (!shape || !transition || idle && !transition.idle) return;
			manager.renderer.rendering.then(() => {
				const { element } = this;
				if (!element) return;
				for (const animation of element.getAnimations()) if ("transitionProperty" in animation && (animation.transitionProperty === "transform" || animation.transitionProperty === "translate" || animation.transitionProperty === "scale")) animation.cancel();
				const updatedShape = this.refreshShape();
				if (!updatedShape) return;
				const delta = {
					x: shape.boundingRectangle.left - updatedShape.boundingRectangle.left,
					y: shape.boundingRectangle.top - updatedShape.boundingRectangle.top
				};
				const { translate } = getComputedStyles(element);
				const currentTranslate = computeTranslate(element, translate, false);
				const finalTranslate = computeTranslate(element, translate);
				if (delta.x || delta.y) {
					const resolvedTransition = prefersReducedMotion(getWindow(element)) ? __spreadProps$1(__spreadValues$1({}, transition), { duration: 0 }) : transition;
					animateTransform({
						element,
						keyframes: { translate: [`${currentTranslate.x + delta.x}px ${currentTranslate.y + delta.y}px ${currentTranslate.z}`, `${finalTranslate.x}px ${finalTranslate.y}px ${finalTranslate.z}`] },
						options: resolvedTransition
					}).then(() => {
						if (!manager.dragOperation.status.dragging) this.droppable.shape = void 0;
					});
				}
			});
		});
	}
	get manager() {
		return this.draggable.manager;
	}
	set manager(manager) {
		n(() => {
			this.draggable.manager = manager;
			this.droppable.manager = manager;
		});
	}
	set element(element) {
		n(() => {
			const previousElement = __privateGet(this, _element);
			const droppableElement = this.droppable.element;
			const draggableElement = this.draggable.element;
			if (!droppableElement || droppableElement === previousElement) this.droppable.element = element;
			if (!draggableElement || draggableElement === previousElement) this.draggable.element = element;
			__privateSet(this, _element, element);
		});
	}
	get element() {
		var _a, _b;
		const element = __privateGet(this, _element);
		if (!element) return;
		return (_b = (_a = ProxiedElements.get(element)) != null ? _a : element) != null ? _b : this.droppable.element;
	}
	set target(target) {
		this.droppable.element = target;
	}
	get target() {
		return this.droppable.element;
	}
	set source(source) {
		this.draggable.element = source;
	}
	get source() {
		return this.draggable.element;
	}
	get disabled() {
		return this.draggable.disabled && this.droppable.disabled;
	}
	set plugins(value) {
		this.draggable.plugins = resolveCustomizable(value, defaultPlugins);
	}
	set disabled(value) {
		n(() => {
			this.droppable.disabled = value;
			this.draggable.disabled = value;
		});
	}
	set data(data) {
		n(() => {
			this.droppable.data = data;
			this.draggable.data = data;
		});
	}
	set handle(handle) {
		this.draggable.handle = handle;
	}
	set id(id) {
		this.droppable.id = id;
		this.draggable.id = id;
	}
	get id() {
		return this.droppable.id;
	}
	set sensors(value) {
		this.draggable.sensors = value;
	}
	set modifiers(value) {
		this.draggable.modifiers = value;
	}
	set collisionPriority(value) {
		this.droppable.collisionPriority = value;
	}
	set collisionDetector(value) {
		this.droppable.collisionDetector = value != null ? value : defaultCollisionDetection$1;
	}
	set alignment(value) {
		this.draggable.alignment = value;
	}
	get alignment() {
		return this.draggable.alignment;
	}
	set type(type) {
		n(() => {
			this.droppable.type = type;
			this.draggable.type = type;
		});
	}
	get type() {
		return this.draggable.type;
	}
	set accept(value) {
		this.droppable.accept = value;
	}
	get accept() {
		return this.droppable.accept;
	}
	get isDropTarget() {
		return this.droppable.isDropTarget;
	}
	/**
	* A boolean indicating whether the sortable item is the source of a drag operation.
	*/
	get isDragSource() {
		return this.draggable.isDragSource;
	}
	/**
	* A boolean indicating whether the sortable item is being dragged.
	*/
	get isDragging() {
		return this.draggable.isDragging;
	}
	/**
	* A boolean indicating whether the sortable item is being dropped.
	*/
	get isDropping() {
		return this.draggable.isDropping;
	}
	get status() {
		return this.draggable.status;
	}
	refreshShape() {
		return this.droppable.refreshShape();
	}
	accepts(draggable) {
		return this.droppable.accepts(draggable);
	}
};
_init = __decoratorStart();
_index = /* @__PURE__ */ new WeakMap();
_previousGroup = /* @__PURE__ */ new WeakMap();
_previousIndex = /* @__PURE__ */ new WeakMap();
_group = /* @__PURE__ */ new WeakMap();
_element = /* @__PURE__ */ new WeakMap();
__decorateElement(_init, 4, "index", _index_dec, Sortable2, _index);
__decorateElement(_init, 4, "group", _group_dec, Sortable2, _group);
__decoratorMetadata(_init, Sortable2);
var SortableDraggable = class extends Draggable {
	constructor(input, manager, sortable) {
		super(input, manager);
		this.sortable = sortable;
	}
	get index() {
		return this.sortable.index;
	}
	get initialIndex() {
		return this.sortable.initialIndex;
	}
	get group() {
		return this.sortable.group;
	}
	get initialGroup() {
		return this.sortable.initialGroup;
	}
};
var SortableDroppable = class extends Droppable {
	constructor(input, manager, sortable) {
		super(input, manager);
		this.sortable = sortable;
	}
	get index() {
		return this.sortable.index;
	}
	get group() {
		return this.sortable.group;
	}
};
//#endregion
//#region node_modules/@dnd-kit/react/sortable.js
var __defProp = Object.defineProperty;
var __defProps = Object.defineProperties;
var __getOwnPropDescs = Object.getOwnPropertyDescriptors;
var __getOwnPropSymbols = Object.getOwnPropertySymbols;
var __hasOwnProp = Object.prototype.hasOwnProperty;
var __propIsEnum = Object.prototype.propertyIsEnumerable;
var __defNormalProp = (obj, key, value) => key in obj ? __defProp(obj, key, {
	enumerable: true,
	configurable: true,
	writable: true,
	value
}) : obj[key] = value;
var __spreadValues = (a, b) => {
	for (var prop in b || (b = {})) if (__hasOwnProp.call(b, prop)) __defNormalProp(a, prop, b[prop]);
	if (__getOwnPropSymbols) {
		for (var prop of __getOwnPropSymbols(b)) if (__propIsEnum.call(b, prop)) __defNormalProp(a, prop, b[prop]);
	}
	return a;
};
var __spreadProps = (a, b) => __defProps(a, __getOwnPropDescs(b));
function useSortable(input) {
	const { accept, collisionDetector, collisionPriority, id, data, element, handle, index, group, disabled, modifiers, sensors, target, type, plugins } = input;
	const transition = __spreadValues(__spreadValues({}, defaultSortableTransition), input.transition);
	const sortable = useInstance((manager) => {
		return new Sortable2(__spreadProps(__spreadValues({}, input), {
			transition,
			register: false,
			handle: currentValue(handle),
			element: currentValue(element),
			target: currentValue(target)
		}), manager);
	});
	const trackedSortable = useDeepSignal(sortable, shouldUpdateSynchronously);
	useOnValueChange(id, () => sortable.id = id);
	useIsomorphicLayoutEffect(() => {
		n(() => {
			sortable.group = group;
			sortable.index = index;
		});
	}, [
		sortable,
		group,
		index
	]);
	useOnValueChange(type, () => sortable.type = type);
	useOnValueChange(accept, () => sortable.accept = accept, void 0, deepEqual);
	useOnValueChange(data, () => data && (sortable.data = data));
	useOnValueChange(index, () => {
		var _a;
		if (((_a = sortable.manager) == null ? void 0 : _a.dragOperation.status.idle) && (transition == null ? void 0 : transition.idle)) sortable.refreshShape();
	}, useImmediateEffect);
	useOnElementChange(handle, (handle2) => sortable.handle = handle2);
	useOnElementChange(element, (element2) => sortable.element = element2);
	useOnElementChange(target, (target2) => sortable.target = target2);
	useOnValueChange(disabled, () => sortable.disabled = disabled === true);
	useOnValueChange(sensors, () => sortable.sensors = sensors);
	useOnValueChange(collisionDetector, () => sortable.collisionDetector = collisionDetector);
	useOnValueChange(collisionPriority, () => sortable.collisionPriority = collisionPriority);
	useOnValueChange(plugins, () => sortable.plugins = plugins, void 0, deepEqual);
	useOnValueChange(transition, () => sortable.transition = transition, void 0, deepEqual);
	useOnValueChange(modifiers, () => sortable.modifiers = modifiers, void 0, deepEqual);
	useOnValueChange(input.alignment, () => sortable.alignment = input.alignment);
	return {
		sortable: trackedSortable,
		get isDragging() {
			return trackedSortable.isDragging;
		},
		get isDropping() {
			return trackedSortable.isDropping;
		},
		get isDragSource() {
			return trackedSortable.isDragSource;
		},
		get isDropTarget() {
			return trackedSortable.isDropTarget;
		},
		handleRef: (0, import_react.useCallback)((element2) => {
			sortable.handle = element2 != null ? element2 : void 0;
		}, [sortable]),
		ref: (0, import_react.useCallback)((element2) => {
			var _a, _b;
			if (!element2 && ((_a = sortable.element) == null ? void 0 : _a.isConnected) && !((_b = sortable.manager) == null ? void 0 : _b.dragOperation.status.idle)) return;
			sortable.element = element2 != null ? element2 : void 0;
		}, [sortable]),
		sourceRef: (0, import_react.useCallback)((element2) => {
			var _a, _b;
			if (!element2 && ((_a = sortable.source) == null ? void 0 : _a.isConnected) && !((_b = sortable.manager) == null ? void 0 : _b.dragOperation.status.idle)) return;
			sortable.source = element2 != null ? element2 : void 0;
		}, [sortable]),
		targetRef: (0, import_react.useCallback)((element2) => {
			var _a, _b;
			if (!element2 && ((_a = sortable.target) == null ? void 0 : _a.isConnected) && !((_b = sortable.manager) == null ? void 0 : _b.dragOperation.status.idle)) return;
			sortable.target = element2 != null ? element2 : void 0;
		}, [sortable])
	};
}
function shouldUpdateSynchronously(key, oldValue, newValue) {
	if (key === "isDragSource" && !newValue && oldValue) return true;
	return false;
}
//#endregion
//#region src/components/EntitiesPage/EntitiesPage.tsx
var EntitiesPage = ({ id, draging_item, deleteEntitiesPage }) => {
	const { entities_pages, entities, scenes } = useEntitiesPagesStore();
	const page = entities_pages.find((p) => p.id === id);
	const [pageData, setPageData] = (0, import_react.useState)({
		room_id: page ? page.room_id : -1,
		id,
		can_remove: page ? entities_pages.filter((p) => p.type == page.type).length > 1 : false,
		type: page ? page.type : "unknown",
		number_of_entities: page ? page.number_of_entities : 12
	});
	const [editPageTypeOpen, setEditPageTypeOpen] = (0, import_react.useState)(false);
	const gridRows = pageData.number_of_entities === 12 ? "grid-rows-6" : pageData.number_of_entities === 8 ? "grid-rows-4" : "grid-rows-2";
	const dialogRef = (0, import_react.useRef)(null);
	const entitiesPageNumberOfEntitiesDiv = (0, import_react.useRef)(null);
	const [handle, setHandle] = (0, import_react.useState)(null);
	const { ref } = useSortable({
		id,
		index: page ? page.display_order : 0,
		data: {
			type: "entity_page",
			entity_page_config: page
		},
		handle,
		disabled: !pageData.can_remove
	});
	function getCookie(name) {
		let cookieValue = "";
		if (document.cookie && document.cookie !== "") {
			const cookies = document.cookie.split(";");
			for (let i = 0; i < cookies.length; i++) {
				const cookie = cookies[i].trim();
				if (cookie.substring(0, name.length + 1) === name + "=") {
					cookieValue = decodeURIComponent(cookie.substring(name.length + 1));
					break;
				}
			}
		}
		return cookieValue;
	}
	async function setPageNumberOfEntities(type) {
		setPageData({
			...pageData,
			number_of_entities: type
		});
		setEditPageTypeOpen(false);
		fetch(`/rest/entities_pages/${pageData.id}`, {
			credentials: "same-origin",
			method: "PUT",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			},
			body: JSON.stringify({
				...pageData,
				number_of_entities: type
			})
		}).then(async (response) => {
			const data = await response.json();
			if (!response.ok) {
				const error = data && data.message || response.status;
				return Promise.reject(error);
			}
		}).catch((error) => {
			console.error("There was an error!", error);
		});
	}
	(0, import_react.useEffect)(() => {
		const listener = (event) => {
			if (entitiesPageNumberOfEntitiesDiv.current && !entitiesPageNumberOfEntitiesDiv.current.contains(event.target)) setEditPageTypeOpen(false);
		};
		window.addEventListener("click", listener);
		return () => {
			window.removeEventListener("click", listener);
		};
	}, [entitiesPageNumberOfEntitiesDiv, editPageTypeOpen]);
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("dialog", {
		ref: dialogRef,
		className: "modal",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "modal-box",
			children: [
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("h3", {
					className: "text-lg font-bold",
					children: [
						"Delete ",
						pageData.type,
						" page"
					]
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("p", {
					className: "py-4",
					children: [
						"Are you sure you want to delete this ",
						pageData.type,
						" page and all its entities?"
					]
				}),
				/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
					className: "flex justify-end join",
					children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
						onClick: () => dialogRef.current?.close(),
						className: "btn btn-neutral join-item",
						children: "Cancel"
					}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
						onClick: () => {
							dialogRef.current?.close();
							deleteEntitiesPage(pageData.id);
						},
						className: "btn btn-error join-item",
						children: "Delete"
					})]
				})
			]
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)("form", {
			method: "dialog",
			className: "modal-backdrop",
			children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", { children: "close" })
		})]
	}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("li", {
		ref,
		id: "nspanel_entities_page_{{ page.id }}",
		"hx-swap-oob": "true",
		className: `bg-base-200 float-left m-2 rounded-box p-3 grid grid-cols-2 ${gridRows} gap-4 w-full max-w-[24rem] aspect-square indicator nspanel-entities-page`,
		children: [
			(() => {
				const items = [];
				for (let i = 0; i < pageData.number_of_entities; i++) {
					let item = null;
					if (pageData.type == "entity") {
						for (let j = 0; j < entities.length; j++) if (entities[j].room_view_position === i && entities[j].type === "entity" && entities[j].entities_page_id === pageData.id) {
							item = entities[j];
							break;
						}
					} else if (pageData.type == "scene") {
						for (let j = 0; j < scenes.length; j++) if (scenes[j].room_view_position === i && scenes[j].type === "scene" && scenes[j].entities_page_id === pageData.id) {
							item = scenes[j];
							break;
						}
					} else console.error("Unknown entities page type: ", pageData.type);
					if (item != null) if (item.type === "scene") items.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)(EntitiesDropTarget, {
						entities_page_id: pageData.id,
						room_view_position: i,
						type: pageData.type,
						draging_item,
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)(GenericSceneBox, {
							scene: item,
							draging_item
						}, item.id)
					}, i));
					else items.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)(EntitiesDropTarget, {
						entities_page_id: pageData.id,
						room_view_position: i,
						type: pageData.type,
						draging_item,
						children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)(GenericEntityBox, {
							entity: item,
							draging_item
						}, item.id)
					}, i));
					else items.push(/* @__PURE__ */ (0, import_jsx_runtime.jsx)(EntitiesDropTarget, {
						entities_page_id: pageData.id,
						room_view_position: i,
						type: pageData.type,
						draging_item
					}, i));
				}
				return items;
			})(),
			/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
				disabled: !pageData.can_remove,
				className: "indicator-item indicator-bottom indicator-end badge not-disabled:badge-neutral not-disabled:hover:badge-error w-6 h-6 flex items-center justify-center not-disabled:cursor-pointer",
				title: `${pageData.can_remove ? "Delete page" : "Cannot delete last page"}`,
				onClick: () => dialogRef.current?.showModal(),
				children: "✕"
			}),
			/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				ref: entitiesPageNumberOfEntitiesDiv,
				className: `dropdown dropdown-top dropdown-center ${editPageTypeOpen ? "dropdown-open" : ""} me-8 flex indicator-item indicator-bottom indicator-end`,
				children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
					onClick: () => setEditPageTypeOpen(!editPageTypeOpen),
					className: "badge badge-info w-6 h-6 flex items-center justify-center cursor-pointer",
					children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "mdi mdi-pencil" })
				}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
					className: "dropdown-content bg-base-100 join rounded-box z-1 p-1 shadow-sm",
					children: [
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
							onClick: () => setPageNumberOfEntities(4),
							className: `btn join-item btn-lg btn-square ${pageData.number_of_entities === 4 ? "btn-active btn-accent" : ""}`,
							children: "4"
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
							onClick: () => setPageNumberOfEntities(8),
							className: `btn join-item btn-lg btn-square ${pageData.number_of_entities === 8 ? "btn-active btn-accent" : ""}`,
							children: "8"
						}),
						/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
							onClick: () => setPageNumberOfEntities(12),
							className: `btn join-item btn-lg btn-square ${pageData.number_of_entities === 12 ? "btn-active btn-accent" : ""}`,
							children: "12"
						})
					]
				})]
			}),
			pageData.can_remove && /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", {
				className: "indicator-item indicator-bottom indicator-center badge badge-neutral hover:badge-info w-6 h-6 flex items-center justify-center nspanel-entities-page-move-handle cursor-grab",
				title: "Drag & drop to move this page",
				ref: setHandle,
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "mdi mdi-drag" })
			})
		]
	})] });
};
//#endregion
//#region src/stores/RoomsStore.ts
function getCookie(name) {
	let cookieValue = "";
	if (document.cookie && document.cookie !== "") {
		const cookies = document.cookie.split(";");
		for (let i = 0; i < cookies.length; i++) {
			const cookie = cookies[i].trim();
			if (cookie.substring(0, name.length + 1) === name + "=") {
				cookieValue = decodeURIComponent(cookie.substring(name.length + 1));
				break;
			}
		}
	}
	return cookieValue;
}
var useRoomsStore = create((set) => ({
	status: "none",
	rooms: null,
	fetchData: async () => {
		set({ status: "loading" });
		fetch(`/rest/rooms`, {
			credentials: "same-origin",
			method: "GET",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			}
		}).then((response) => response.json()).then((data) => {
			set((state) => ({
				...state,
				status: "loaded",
				rooms: data.rooms
			}));
		});
	}
}));
//#endregion
//#region src/components/EntitiesPage/EntitiesPagesView.tsx
var EntitiesPagesView = ({ room_id, type }) => {
	const { entities_pages, fetchData, removeEntitiesPage, entities, scenes, setScenePosition, setEntityPosition } = useEntitiesPagesStore();
	const [dragingItem, setDragingItem] = (0, import_react.useState)(void 0);
	const [addNewPageTypeOpen, setAddNewPageTypeOpen] = (0, import_react.useState)(false);
	(0, import_react.useEffect)(() => {
		fetchData(room_id);
	}, [room_id, fetchData]);
	(0, import_react.useEffect)(() => {
		if (useRoomsStore.getState().status == "none") useRoomsStore.getState().fetchData();
	}, []);
	function getCookie(name) {
		let cookieValue = "";
		if (document.cookie && document.cookie !== "") {
			const cookies = document.cookie.split(";");
			for (let i = 0; i < cookies.length; i++) {
				const cookie = cookies[i].trim();
				if (cookie.substring(0, name.length + 1) === name + "=") {
					cookieValue = decodeURIComponent(cookie.substring(name.length + 1));
					break;
				}
			}
		}
		return cookieValue;
	}
	async function deleteEntitiesPage(id) {
		console.log("Deleting entities page", id);
		fetch(`/rest/entities_pages/${id}`, {
			credentials: "same-origin",
			method: "DELETE",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			}
		}).then(async (response) => {
			if (response.ok) {
				console.log("Successfully deleted entities page", id);
				removeEntitiesPage(id);
			}
		}).catch((error) => {
			console.error("Error deleting entities page", error);
		});
	}
	async function createEntitiesPage(number_of_entities) {
		console.log("Creating entities page", type);
		let url = `/rest/global/entities_pages`;
		if (room_id !== void 0) url = `/rest/rooms/${room_id}/entities_pages`;
		fetch(url, {
			credentials: "same-origin",
			method: "PUT",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			},
			body: JSON.stringify({
				type: number_of_entities,
				is_scenes_page: type == "scene"
			})
		}).then(async (response) => {
			if (response.ok) {
				console.log("Successfully created entities page", type, "number of entities", number_of_entities);
				fetchData(room_id);
			}
		}).catch((error) => {
			console.error("Error creating entities page", error);
		});
	}
	async function saveEntitiesOrder(entities, scenes) {
		fetch(`/rest/rooms/${room_id}/entities/order`, {
			credentials: "same-origin",
			method: "PUT",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			},
			body: JSON.stringify({
				entities,
				scenes
			})
		}).then(async (response) => {
			const data = await response.json();
			if (!response.ok) {
				const error = data && data.message || response.status;
				return Promise.reject(error);
			}
		}).catch((error) => {
			console.error("There was an error!", error);
		});
	}
	async function saveEntitiesPagesOrder(entities_pages) {
		fetch(`/rest/entities_pages/order`, {
			credentials: "same-origin",
			method: "PUT",
			mode: "same-origin",
			headers: {
				"Content-Type": "application/json",
				"X-CSRFToken": getCookie("csrftoken")
			},
			body: JSON.stringify({ order: entities_pages.map((page) => [page.id, page.display_order]) })
		}).then(async (response) => {
			const data = await response.json();
			if (!response.ok) {
				const error = data && data.message || response.status;
				return Promise.reject(error);
			}
		}).catch((error) => {
			console.error("There was an error!", error);
		});
	}
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)(import_jsx_runtime.Fragment, { children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
		className: "flex items-center justify-start mb-4",
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("span", {
			className: "text-xl",
			children: [
				useRoomsStore.getState().rooms?.find((r) => r.id == room_id)?.name,
				" ",
				(() => {
					if (type == "entity") return "entities";
					if (type == "scene") return "scenes";
					return "UNKNOWN ENTITY PAGE TYPE";
				})()
			]
		}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: `dropdown dropdown-top dropdown-center ${addNewPageTypeOpen ? "dropdown-open" : ""} me-8 flex indicator-item indicator-bottom indicator-end`,
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
				onClick: () => setAddNewPageTypeOpen(!addNewPageTypeOpen),
				className: "btn btn-xs btn-circle btn-success ml-2",
				children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { className: "mdi mdi-plus" })
			}), /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
				className: "dropdown-content bg-base-100 join rounded-box z-1 p-1 shadow-sm",
				children: [
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
						onClick: () => {
							setAddNewPageTypeOpen(false);
							createEntitiesPage(4);
						},
						className: `btn join-item btn-lg btn-square`,
						children: "4"
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
						onClick: () => {
							setAddNewPageTypeOpen(false);
							createEntitiesPage(8);
						},
						className: `btn join-item btn-lg btn-square`,
						children: "8"
					}),
					/* @__PURE__ */ (0, import_jsx_runtime.jsx)("button", {
						onClick: () => {
							setAddNewPageTypeOpen(false);
							createEntitiesPage(12);
						},
						className: `btn join-item btn-lg btn-square`,
						children: "12"
					})
				]
			})]
		})]
	}), /* @__PURE__ */ (0, import_jsx_runtime.jsx)(DragDropProvider, {
		onDragStart: (event) => {
			const { source } = event.operation;
			if (source == null) return;
			setDragingItem(source.data);
		},
		onDragEnd: (event) => {
			setDragingItem(void 0);
			if (event.canceled) return;
			const { source, target } = event.operation;
			if (source == null || target == null) return;
			const sourceData = source.data;
			const targetData = target.data;
			if (sourceData.config && targetData.config && sourceData.config.entities_page_id == targetData.config.entities_page_id && sourceData.config.room_view_position == targetData.config.room_view_position) return;
			if (sourceData.type == "entity" && sourceData.config) {
				const entity = entities.find((e) => sourceData.config && e.id == sourceData.config.id);
				if (!entity) return;
				let existingEntity = entities.find((e) => targetData.config && e.entities_page_id == targetData.config.entities_page_id && e.room_view_position == targetData.config.room_view_position);
				if (existingEntity != null) {
					existingEntity = JSON.parse(JSON.stringify(existingEntity));
					if (existingEntity) setEntityPosition(existingEntity.id, entity.entities_page_id, entity.room_view_position);
				}
				const updatedEntity = JSON.parse(JSON.stringify(entity));
				if (targetData.config) {
					setEntityPosition(updatedEntity.id, targetData.config.entities_page_id, targetData.config.room_view_position);
					saveEntitiesOrder(useEntitiesPagesStore.getState().entities, []);
				} else console.error("Failed to set new entity position. Target data has no config");
			} else if (sourceData.type == "scene" && sourceData.config) {
				const scene = scenes.find((e) => sourceData.config && e.id == sourceData.config.id);
				if (!scene) return;
				let existingScene = scenes.find((e) => targetData.config && e.entities_page_id == targetData.config.entities_page_id && e.room_view_position == targetData.config.room_view_position);
				if (existingScene != null) {
					existingScene = JSON.parse(JSON.stringify(existingScene));
					if (existingScene) setScenePosition(existingScene.id, scene.entities_page_id, scene.room_view_position);
				}
				const updatedEntity = JSON.parse(JSON.stringify(scene));
				if (targetData.config) {
					setScenePosition(updatedEntity.id, targetData.config.entities_page_id, targetData.config.room_view_position);
					saveEntitiesOrder([], useEntitiesPagesStore.getState().scenes);
				} else console.error("Failed to set new scene position as target as not config");
			} else if (sourceData.type == "entity_page" && sourceData.entity_page_config) {
				if (isSortable(source)) {
					const { initialIndex, index } = source;
					console.log("Initial index: ", initialIndex, " index: ", index);
					if (initialIndex == index) return;
					if (!useEntitiesPagesStore.getState().entities_pages.find((page) => page.display_order == initialIndex)) {
						console.error("Failed to find source page data. Cannot continue.");
						return;
					}
					const newEntitiesPagesOrder = [...useEntitiesPagesStore.getState().entities_pages.filter((page) => page.type == type)];
					const [removed] = newEntitiesPagesOrder.splice(initialIndex, 1);
					newEntitiesPagesOrder.splice(index, 0, removed);
					newEntitiesPagesOrder.forEach((page, i) => {
						page.display_order = i;
					});
					useEntitiesPagesStore.setState({ entities_pages: [...newEntitiesPagesOrder, ...useEntitiesPagesStore.getState().entities_pages.filter((page) => page.type != type)] });
					saveEntitiesPagesOrder(newEntitiesPagesOrder);
				}
			}
		},
		children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("ul", {
			className: "list-none w-full nspanel-entities-pages",
			children: entities_pages.filter((entity_page) => entity_page.type === type).map((entity_page) => /* @__PURE__ */ (0, import_jsx_runtime.jsx)(EntitiesPage, {
				id: entity_page.id,
				draging_item: dragingItem,
				deleteEntitiesPage
			}, `entity_page-${entity_page.id}`))
		})
	})] });
};
//#endregion
export { EntitiesPagesView as default };
