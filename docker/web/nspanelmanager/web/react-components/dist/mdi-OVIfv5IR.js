import { i as __commonJSMin, r as require_react } from "./main-D4-XR9wG.js";
//#region node_modules/prop-types/lib/ReactPropTypesSecret.js
/**
* Copyright (c) 2013-present, Facebook, Inc.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/
var require_ReactPropTypesSecret = /* @__PURE__ */ __commonJSMin(((exports, module) => {
	module.exports = "SECRET_DO_NOT_PASS_THIS_OR_YOU_WILL_BE_FIRED";
}));
//#endregion
//#region node_modules/prop-types/factoryWithThrowingShims.js
/**
* Copyright (c) 2013-present, Facebook, Inc.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/
var require_factoryWithThrowingShims = /* @__PURE__ */ __commonJSMin(((exports, module) => {
	var ReactPropTypesSecret = require_ReactPropTypesSecret();
	function emptyFunction() {}
	function emptyFunctionWithReset() {}
	emptyFunctionWithReset.resetWarningCache = emptyFunction;
	module.exports = function() {
		function shim(props, propName, componentName, location, propFullName, secret) {
			if (secret === ReactPropTypesSecret) return;
			var err = /* @__PURE__ */ new Error("Calling PropTypes validators directly is not supported by the `prop-types` package. Use PropTypes.checkPropTypes() to call them. Read more at http://fb.me/use-check-prop-types");
			err.name = "Invariant Violation";
			throw err;
		}
		shim.isRequired = shim;
		function getShim() {
			return shim;
		}
		var ReactPropTypes = {
			array: shim,
			bigint: shim,
			bool: shim,
			func: shim,
			number: shim,
			object: shim,
			string: shim,
			symbol: shim,
			any: shim,
			arrayOf: getShim,
			element: shim,
			elementType: shim,
			instanceOf: getShim,
			node: shim,
			objectOf: getShim,
			oneOf: getShim,
			oneOfType: getShim,
			shape: getShim,
			exact: getShim,
			checkPropTypes: emptyFunctionWithReset,
			resetWarningCache: emptyFunction
		};
		ReactPropTypes.PropTypes = ReactPropTypes;
		return ReactPropTypes;
	};
}));
//#endregion
//#region node_modules/prop-types/index.js
var require_prop_types = /* @__PURE__ */ __commonJSMin(((exports, module) => {
	module.exports = require_factoryWithThrowingShims()();
}));
//#endregion
//#region node_modules/@mdi/react/Icon.js
var require_Icon = /* @__PURE__ */ __commonJSMin(((exports, module) => {
	module.exports = function(e) {
		var t = {};
		function r(n) {
			if (t[n]) return t[n].exports;
			var o = t[n] = {
				i: n,
				l: !1,
				exports: {}
			};
			return e[n].call(o.exports, o, o.exports, r), o.l = !0, o.exports;
		}
		return r.m = e, r.c = t, r.d = function(e, t, n) {
			r.o(e, t) || Object.defineProperty(e, t, {
				enumerable: !0,
				get: n
			});
		}, r.r = function(e) {
			"undefined" != typeof Symbol && Symbol.toStringTag && Object.defineProperty(e, Symbol.toStringTag, { value: "Module" }), Object.defineProperty(e, "__esModule", { value: !0 });
		}, r.t = function(e, t) {
			if (1 & t && (e = r(e)), 8 & t) return e;
			if (4 & t && "object" == typeof e && e && e.__esModule) return e;
			var n = Object.create(null);
			if (r.r(n), Object.defineProperty(n, "default", {
				enumerable: !0,
				value: e
			}), 2 & t && "string" != typeof e) for (var o in e) r.d(n, o, function(t) {
				return e[t];
			}.bind(null, o));
			return n;
		}, r.n = function(e) {
			var t = e && e.__esModule ? function() {
				return e.default;
			} : function() {
				return e;
			};
			return r.d(t, "a", t), t;
		}, r.o = function(e, t) {
			return Object.prototype.hasOwnProperty.call(e, t);
		}, r.p = "", r(r.s = 2);
	}([
		function(e, t) {
			e.exports = require_prop_types();
		},
		function(e, t) {
			e.exports = require_react();
		},
		function(e, t, r) {
			"use strict";
			r.r(t);
			var n = r(1), o = r(0), l = function() {
				return (l = Object.assign || function(e) {
					for (var t, r = 1, n = arguments.length; r < n; r++) for (var o in t = arguments[r]) Object.prototype.hasOwnProperty.call(t, o) && (e[o] = t[o]);
					return e;
				}).apply(this, arguments);
			}, i = function(e, t) {
				var r = {};
				for (var n in e) Object.prototype.hasOwnProperty.call(e, n) && t.indexOf(n) < 0 && (r[n] = e[n]);
				if (null != e && "function" == typeof Object.getOwnPropertySymbols) {
					var o = 0;
					for (n = Object.getOwnPropertySymbols(e); o < n.length; o++) t.indexOf(n[o]) < 0 && Object.prototype.propertyIsEnumerable.call(e, n[o]) && (r[n[o]] = e[n[o]]);
				}
				return r;
			}, a = 0, s = n.forwardRef((function(e, t) {
				var r = e.title, o = void 0 === r ? null : r, s = e.description, c = void 0 === s ? null : s, u = e.size, p = void 0 === u ? null : u, f = e.color, d = void 0 === f ? "currentColor" : f, y = e.horizontal, v = void 0 === y ? null : y, b = e.vertical, m = void 0 === b ? null : b, h = e.rotate, g = void 0 === h ? null : h, O = e.spin, w = void 0 === O ? null : O, j = e.style, z = void 0 === j ? {} : j, E = e.children, P = i(e, [
					"title",
					"description",
					"size",
					"color",
					"horizontal",
					"vertical",
					"rotate",
					"spin",
					"style",
					"children"
				]);
				a++;
				var S, x = null !== w && w, _ = n.Children.map(E, (function(e) {
					var t = e;
					!0 !== x && (x = !0 === (null === w ? t.props.spin : w));
					var r = t.props.size;
					"number" == typeof p && "number" == typeof t.props.size && (r = t.props.size / p);
					var o = {
						size: r,
						color: null === d ? t.props.color : d,
						horizontal: null === v ? t.props.horizontal : v,
						vertical: null === m ? t.props.vertical : m,
						rotate: null === g ? t.props.rotate : g,
						spin: null === w ? t.props.spin : w,
						inStack: !0
					};
					return n.cloneElement(t, o);
				}));
				null !== p && (z.width = "string" == typeof p ? p : 1.5 * p + "rem");
				var k, T = "stack_labelledby_" + a, q = "stack_describedby_" + a;
				if (o) S = c ? T + " " + q : T;
				else if (k = "presentation", c) throw new Error("title attribute required when description is set");
				return n.createElement("svg", l({
					ref: t,
					viewBox: "0 0 24 24",
					style: z,
					role: k,
					"aria-labelledby": S
				}, P), o && n.createElement("title", { id: T }, o), c && n.createElement("desc", { id: q }, c), x && n.createElement("style", null, "@keyframes spin { from { transform: rotate(0deg) } to { transform: rotate(360deg) } }", "@keyframes spin-inverse { from { transform: rotate(0deg) } to { transform: rotate(-360deg) } }"), _);
			}));
			s.displayName = "Stack", s.propTypes = {
				size: o.oneOfType([o.number, o.string]),
				color: o.string,
				horizontal: o.bool,
				vertical: o.bool,
				rotate: o.number,
				spin: o.oneOfType([o.bool, o.number]),
				children: o.oneOfType([o.arrayOf(o.node), o.node]).isRequired,
				className: o.string,
				style: o.object
			}, s.defaultProps = {
				size: null,
				color: null,
				horizontal: null,
				vertical: null,
				rotate: null,
				spin: null
			};
			var c = s;
			r.d(t, "Icon", (function() {
				return d;
			})), r.d(t, "Stack", (function() {
				return c;
			}));
			var u = function() {
				return (u = Object.assign || function(e) {
					for (var t, r = 1, n = arguments.length; r < n; r++) for (var o in t = arguments[r]) Object.prototype.hasOwnProperty.call(t, o) && (e[o] = t[o]);
					return e;
				}).apply(this, arguments);
			}, p = function(e, t) {
				var r = {};
				for (var n in e) Object.prototype.hasOwnProperty.call(e, n) && t.indexOf(n) < 0 && (r[n] = e[n]);
				if (null != e && "function" == typeof Object.getOwnPropertySymbols) {
					var o = 0;
					for (n = Object.getOwnPropertySymbols(e); o < n.length; o++) t.indexOf(n[o]) < 0 && Object.prototype.propertyIsEnumerable.call(e, n[o]) && (r[n[o]] = e[n[o]]);
				}
				return r;
			}, f = 0, d = n.forwardRef((function(e, t) {
				var r = e.path, o = e.id, l = void 0 === o ? ++f : o, i = e.title, a = void 0 === i ? null : i, s = e.description, c = void 0 === s ? null : s, d = e.size, y = void 0 === d ? null : d, v = e.color, b = void 0 === v ? "currentColor" : v, m = e.horizontal, h = void 0 !== m && m, g = e.vertical, O = void 0 !== g && g, w = e.rotate, j = void 0 === w ? 0 : w, z = e.spin, E = void 0 !== z && z, P = e.style, S = void 0 === P ? {} : P, x = e.inStack, _ = void 0 !== x && x, k = p(e, [
					"path",
					"id",
					"title",
					"description",
					"size",
					"color",
					"horizontal",
					"vertical",
					"rotate",
					"spin",
					"style",
					"inStack"
				]), T = {}, q = [];
				null !== y && (_ ? q.push("scale(" + y + ")") : (S.width = "string" == typeof y ? y : 1.5 * y + "rem", S.height = S.width)), h && q.push("scaleX(-1)"), O && q.push("scaleY(-1)"), 0 !== j && q.push("rotate(" + j + "deg)"), null !== b && (T.fill = b);
				var M = n.createElement("path", u({
					d: r,
					style: T
				}, _ ? k : {})), C = M;
				q.length > 0 && (S.transform = q.join(" "), S.transformOrigin = "center", _ && (C = n.createElement("g", { style: S }, M, n.createElement("rect", {
					width: "24",
					height: "24",
					fill: "transparent"
				}))));
				var I, N = C, R = !0 === E || "number" != typeof E ? 2 : E, B = !_ && (h || O);
				if (R < 0 && (B = !B), E && (N = n.createElement("g", { style: {
					animation: "spin" + (B ? "-inverse" : "") + " linear " + Math.abs(R) + "s infinite",
					transformOrigin: "center"
				} }, C, !(h || O || 0 !== j) && n.createElement("rect", {
					width: "24",
					height: "24",
					fill: "transparent"
				}))), _) return N;
				var X, Y = "icon_labelledby_" + l, A = "icon_describedby_" + l;
				if (a) I = c ? Y + " " + A : Y;
				else if (X = "presentation", c) throw new Error("title attribute required when description is set");
				return n.createElement("svg", u({
					ref: t,
					viewBox: "0 0 24 24",
					style: S,
					role: X,
					"aria-labelledby": I
				}, k), a && n.createElement("title", { id: Y }, a), c && n.createElement("desc", { id: A }, c), !_ && E && (B ? n.createElement("style", null, "@keyframes spin-inverse { from { transform: rotate(0deg) } to { transform: rotate(-360deg) } }") : n.createElement("style", null, "@keyframes spin { from { transform: rotate(0deg) } to { transform: rotate(360deg) } }")), N);
			}));
			d.displayName = "Icon", d.propTypes = {
				path: o.string.isRequired,
				size: o.oneOfType([o.number, o.string]),
				color: o.string,
				horizontal: o.bool,
				vertical: o.bool,
				rotate: o.number,
				spin: o.oneOfType([o.bool, o.number]),
				style: o.object,
				inStack: o.bool,
				className: o.string
			}, d.defaultProps = {
				size: null,
				color: "currentColor",
				horizontal: !1,
				vertical: !1,
				rotate: 0,
				spin: !1
			};
			t.default = d;
		}
	]);
}));
//#endregion
//#region node_modules/@mdi/js/mdi.js
var mdiAlertCircle = "M13,13H11V7H13M13,17H11V15H13M12,2A10,10 0 0,0 2,12A10,10 0 0,0 12,22A10,10 0 0,0 22,12A10,10 0 0,0 12,2Z";
var mdiCog = "M12,15.5A3.5,3.5 0 0,1 8.5,12A3.5,3.5 0 0,1 12,8.5A3.5,3.5 0 0,1 15.5,12A3.5,3.5 0 0,1 12,15.5M19.43,12.97C19.47,12.65 19.5,12.33 19.5,12C19.5,11.67 19.47,11.34 19.43,11L21.54,9.37C21.73,9.22 21.78,8.95 21.66,8.73L19.66,5.27C19.54,5.05 19.27,4.96 19.05,5.05L16.56,6.05C16.04,5.66 15.5,5.32 14.87,5.07L14.5,2.42C14.46,2.18 14.25,2 14,2H10C9.75,2 9.54,2.18 9.5,2.42L9.13,5.07C8.5,5.32 7.96,5.66 7.44,6.05L4.95,5.05C4.73,4.96 4.46,5.05 4.34,5.27L2.34,8.73C2.21,8.95 2.27,9.22 2.46,9.37L4.57,11C4.53,11.34 4.5,11.67 4.5,12C4.5,12.33 4.53,12.65 4.57,12.97L2.46,14.63C2.27,14.78 2.21,15.05 2.34,15.27L4.34,18.73C4.46,18.95 4.73,19.03 4.95,18.95L7.44,17.94C7.96,18.34 8.5,18.68 9.13,18.93L9.5,21.58C9.54,21.82 9.75,22 10,22H14C14.25,22 14.46,21.82 14.5,21.58L14.87,18.93C15.5,18.67 16.04,18.34 16.56,17.94L19.05,18.95C19.27,19.03 19.54,18.95 19.66,18.73L21.66,15.27C21.78,15.05 21.73,14.78 21.54,14.63L19.43,12.97Z";
var mdiGauge = "M12,2A10,10 0 0,0 2,12A10,10 0 0,0 12,22A10,10 0 0,0 22,12A10,10 0 0,0 12,2M12,4A8,8 0 0,1 20,12C20,14.4 19,16.5 17.3,18C15.9,16.7 14,16 12,16C10,16 8.2,16.7 6.7,18C5,16.5 4,14.4 4,12A8,8 0 0,1 12,4M14,5.89C13.62,5.9 13.26,6.15 13.1,6.54L11.81,9.77L11.71,10C11,10.13 10.41,10.6 10.14,11.26C9.73,12.29 10.23,13.45 11.26,13.86C12.29,14.27 13.45,13.77 13.86,12.74C14.12,12.08 14,11.32 13.57,10.76L13.67,10.5L14.96,7.29L14.97,7.26C15.17,6.75 14.92,6.17 14.41,5.96C14.28,5.91 14.15,5.89 14,5.89M10,6A1,1 0 0,0 9,7A1,1 0 0,0 10,8A1,1 0 0,0 11,7A1,1 0 0,0 10,6M7,9A1,1 0 0,0 6,10A1,1 0 0,0 7,11A1,1 0 0,0 8,10A1,1 0 0,0 7,9M17,9A1,1 0 0,0 16,10A1,1 0 0,0 17,11A1,1 0 0,0 18,10A1,1 0 0,0 17,9Z";
var mdiInformationOutline = "M11,9H13V7H11M12,20C7.59,20 4,16.41 4,12C4,7.59 7.59,4 12,4C16.41,4 20,7.59 20,12C20,16.41 16.41,20 12,20M12,2A10,10 0 0,0 2,12A10,10 0 0,0 12,22A10,10 0 0,0 22,12A10,10 0 0,0 12,2M11,17H13V11H11V17Z";
var mdiMemory = "M17,17H7V7H17M21,11V9H19V7C19,5.89 18.1,5 17,5H15V3H13V5H11V3H9V5H7C5.89,5 5,5.89 5,7V9H3V11H5V13H3V15H5V17A2,2 0 0,0 7,19H9V21H11V19H13V21H15V19H17A2,2 0 0,0 19,17V15H21V13H19V11M13,13H11V11H13M15,9H9V15H15V9Z";
var mdiMenuDownOutline = "M18,9V10.5L12,16.5L6,10.5V9H18M12,13.67L14.67,11H9.33L12,13.67Z";
var mdiMinusCircle = "M17,13H7V11H17M12,2A10,10 0 0,0 2,12A10,10 0 0,0 12,22A10,10 0 0,0 22,12A10,10 0 0,0 12,2Z";
var mdiRestart = "M12,4C14.1,4 16.1,4.8 17.6,6.3C20.7,9.4 20.7,14.5 17.6,17.6C15.8,19.5 13.3,20.2 10.9,19.9L11.4,17.9C13.1,18.1 14.9,17.5 16.2,16.2C18.5,13.9 18.5,10.1 16.2,7.7C15.1,6.6 13.5,6 12,6V10.6L7,5.6L12,0.6V4M6.3,17.6C3.7,15 3.3,11 5.1,7.9L6.6,9.4C5.5,11.6 5.9,14.4 7.8,16.2C8.3,16.7 8.9,17.1 9.6,17.4L9,19.4C8,19 7.1,18.4 6.3,17.6Z";
var mdiTableArrowUp = "M4 3H18C19.11 3 20 3.9 20 5V12.08C18.45 11.82 16.92 12.18 15.68 13H12V17H13.08C12.97 17.68 12.97 18.35 13.08 19H4C2.9 19 2 18.11 2 17V5C2 3.9 2.9 3 4 3M4 7V11H10V7H4M12 7V11H18V7H12M4 13V17H10V13H4M21.94 17.5H19.94V21.5H17.94V17.5H15.94L18.94 14.5L21.94 17.5";
var mdiThermometer = "M15 13V5A3 3 0 0 0 9 5V13A5 5 0 1 0 15 13M12 4A1 1 0 0 1 13 5V8H11V5A1 1 0 0 1 12 4Z";
var mdiUpload = "M9,16V10H5L12,3L19,10H15V16H9M5,20V18H19V20H5Z";
var mdiWaterPercent = "M12,3.25C12,3.25 6,10 6,14C6,17.32 8.69,20 12,20A6,6 0 0,0 18,14C18,10 12,3.25 12,3.25M14.47,9.97L15.53,11.03L9.53,17.03L8.47,15.97M9.75,10A1.25,1.25 0 0,1 11,11.25A1.25,1.25 0 0,1 9.75,12.5A1.25,1.25 0 0,1 8.5,11.25A1.25,1.25 0 0,1 9.75,10M14.25,14.5A1.25,1.25 0 0,1 15.5,15.75A1.25,1.25 0 0,1 14.25,17A1.25,1.25 0 0,1 13,15.75A1.25,1.25 0 0,1 14.25,14.5Z";
var mdiWeb = "M16.36,14C16.44,13.34 16.5,12.68 16.5,12C16.5,11.32 16.44,10.66 16.36,10H19.74C19.9,10.64 20,11.31 20,12C20,12.69 19.9,13.36 19.74,14M14.59,19.56C15.19,18.45 15.65,17.25 15.97,16H18.92C17.96,17.65 16.43,18.93 14.59,19.56M14.34,14H9.66C9.56,13.34 9.5,12.68 9.5,12C9.5,11.32 9.56,10.65 9.66,10H14.34C14.43,10.65 14.5,11.32 14.5,12C14.5,12.68 14.43,13.34 14.34,14M12,19.96C11.17,18.76 10.5,17.43 10.09,16H13.91C13.5,17.43 12.83,18.76 12,19.96M8,8H5.08C6.03,6.34 7.57,5.06 9.4,4.44C8.8,5.55 8.35,6.75 8,8M5.08,16H8C8.35,17.25 8.8,18.45 9.4,19.56C7.57,18.93 6.03,17.65 5.08,16M4.26,14C4.1,13.36 4,12.69 4,12C4,11.31 4.1,10.64 4.26,10H7.64C7.56,10.66 7.5,11.32 7.5,12C7.5,12.68 7.56,13.34 7.64,14M12,4.03C12.83,5.23 13.5,6.57 13.91,8H10.09C10.5,6.57 11.17,5.23 12,4.03M18.92,8H15.97C15.65,6.75 15.19,5.55 14.59,4.44C16.43,5.07 17.96,6.34 18.92,8M12,2C6.47,2 2,6.5 2,12A10,10 0 0,0 12,22A10,10 0 0,0 22,12A10,10 0 0,0 12,2Z";
var mdiWifiStrength1Alert = "M12 3C7.8 3 3.7 4.4 .4 7C4.4 12.1 7.9 16.4 12 21.5C14.4 18.5 16.7 15.7 19 12.8V9.6L15.6 13.8C14.5 13.3 13.2 13 12 13S9.5 13.3 8.4 13.8L3.3 7.4C5.9 5.8 8.9 5 12 5S18.1 5.9 20.7 7.4L20.3 8H22.9C23.1 7.7 23.5 7.3 23.7 7C20.3 4.4 16.2 3 12 3M21 10V16H23V10M21 18V20H23V18";
var mdiWifiStrength2 = "M12,3C7.79,3 3.7,4.41 0.38,7C4.41,12.06 7.89,16.37 12,21.5C16.08,16.42 20.24,11.24 23.65,7C20.32,4.41 16.22,3 12,3M12,5C15.07,5 18.09,5.86 20.71,7.45L17.5,11.43C16.26,10.74 14.37,10 12,10C9.62,10 7.74,10.75 6.5,11.43L3.27,7.44C5.91,5.85 8.93,5 12,5Z";
var mdiWifiStrength3 = "M12,3C7.79,3 3.7,4.41 0.38,7C4.41,12.06 7.89,16.37 12,21.5C16.08,16.42 20.24,11.24 23.65,7C20.32,4.41 16.22,3 12,3M12,5C15.07,5 18.09,5.86 20.71,7.45L18.77,9.88C17.26,9 14.88,8 12,8C9,8 6.68,9 5.21,9.84L3.27,7.44C5.91,5.85 8.93,5 12,5Z";
var mdiWifiStrength4 = "M12,3C7.79,3 3.7,4.41 0.38,7C4.41,12.06 7.89,16.37 12,21.5C16.08,16.42 20.24,11.24 23.65,7C20.32,4.41 16.22,3 12,3Z";
var mdiWifiStrengthAlertOutline = "M12 3C7.8 3 3.7 4.4 .4 7C4.3 11.8 8.2 16.7 12 21.5C14.3 18.6 16.7 15.7 19 12.8V9.6L12 18.3L3.3 7.4C5.9 5.8 8.9 5 12 5C15.1 5 18.1 5.9 20.7 7.4L20.3 8H22.9C23.2 7.7 23.4 7.3 23.7 7C20.3 4.4 16.2 3 12 3M21 10V16H23V10M21 18V20H23V18";
//#endregion
export { mdiWifiStrength4 as _, mdiMemory as a, mdiRestart as c, mdiUpload as d, mdiWaterPercent as f, mdiWifiStrength3 as g, mdiWifiStrength2 as h, mdiInformationOutline as i, mdiTableArrowUp as l, mdiWifiStrength1Alert as m, mdiCog as n, mdiMenuDownOutline as o, mdiWeb as p, mdiGauge as r, mdiMinusCircle as s, mdiAlertCircle as t, mdiThermometer as u, mdiWifiStrengthAlertOutline as v, require_Icon as y };
